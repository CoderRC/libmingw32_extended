#include <__436F64657252436C69626D696E677733325F657874656E646564.h>
#include <sys/shm.h>
#include <windows.h>
#include <dlfcn.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/*
 * Shared segment table, the same in shmat.c, shmctl.c, shmdt.c and
 * shmget.c (one function per file, like pwd.h)
 *
 * Segments outlive the processes that use them, like System V shared
 * memory: each segment is a sparse file in the user's Local AppData
 * folder, and a table file there records every segment and who has it
 * attached. A named mutex guards the table across processes. Removing
 * a segment deletes its file; Windows keeps the data alive until the
 * last process unmaps it, which is the POSIX "destroyed after the last
 * detach" rule. The table and the files are cleared after a reboot.
 *
 * Every field has a fixed width so 32-bit and 64-bit programs share it.
 */
#define SHM_SLOTS SHMMNI
#define SHM_ATTACHES 1024	/* Attach records shared by all processes */

struct shm_segment
{
  int in_use;			/* 1 while the identifier exists */
  int key;			/* key_t passed to shmget() */
  unsigned int seq;		/* Bumped on removal so stale ids fail */
  unsigned int uid, gid, cuid, cgid;
  unsigned int mode;		/* Low 9 bits are the permissions */
  unsigned int cpid, lpid;
  unsigned long long size;	/* Bytes */
  long long atime, dtime, ctime;
};

/* One record per process and segment it has attached */
struct shm_attach
{
  unsigned int pid;		/* 0 when the record is free */
  unsigned int slot;
  unsigned int seq;
  unsigned int count;		/* Attaches by that process */
  unsigned long long started;	/* Process creation time, against pid reuse */
};

struct shm_table
{
  unsigned int magic;
  unsigned int version;
  unsigned int size;
  unsigned int generation;	/* Bumped on every change */
  long long boot_time;		/* Seconds since 1970, to detect a reboot */
  struct shm_segment segments[SHM_SLOTS];
  struct shm_attach attaches[SHM_ATTACHES];
};

/* This process's attachments, kept in the shared state struct */
struct shm_mapping
{
  void *addr;			/* NULL when the entry is free */
  unsigned int slot;
  unsigned int seq;
};

typedef struct shm_table shm_table;
typedef struct shm_segment shm_segment;

#define SHM_MAGIC 0x4D485353U	/* "SSHM" */
#define SHM_VERSION 1U
#define SHM_PREFIX "__436F64657252436C69626D696E677733325F657874656E646564_shm"
#define SHM_MUTEX_NAME "Local\\__436F64657252436C69626D696E677733325F657874656E646564_shm_mutex"
#define SHM_BOOT_SLACK 120	/* Seconds of jitter allowed in the boot time */

/* Typedef for SHGetFolderPathA function pointer */
typedef HRESULT (WINAPI SHGetFolderPathAFunc) (HWND hwnd, int csidl,
					       HANDLE token, DWORD flags,
					       LPSTR path);

/* CSIDL_LOCAL_APPDATA from shlobj.h */
#define SHM_CSIDL_LOCAL_APPDATA 0x001c

/* Folder for the table and segment files, with a trailing backslash.
   Local AppData does not change with TEMP/TMP, so programs started from
   MSYS2, cmd or Explorer all share the same segments. */
/* Path of the file that holds one segment */
/* Creation time of a process, or 0 when it has exited */
/* Reset a table that is new or from another version. A boot time that
   no longer matches means the machine rebooted, but the wall clock can
   also have been changed, so that only resets the table when no other
   process has it open (first_user). Segment files left from before the
   reset are deleted. */
/* Open the table file, mapping and mutex once per process */
/* Attach the table and take the cross-process lock */

static int
shm_folder (char *path, size_t size)
{
  void *shell32 = dlopen ("shell32.dll", RTLD_NOW);
  size_t length = 0;

  if (shell32)
    {
      SHGetFolderPathAFunc *_SHGetFolderPathA =
	dlsym (shell32, "SHGetFolderPathA");
      if (_SHGetFolderPathA
	  && _SHGetFolderPathA (NULL, SHM_CSIDL_LOCAL_APPDATA, NULL, 0,
				path) == S_OK)
	{
	  length = strlen (path);
	}
      dlclose (shell32);
    }
  if (!length)
    {
      length = GetTempPathA ((DWORD) size, path);
    }
  if (!length || length + 2 > size)
    {
      return -1;
    }
  if (path[length - 1] != '\\')
    {
      path[length++] = '\\';
      path[length] = '\0';
    }
  return 0;
}

static int
shm_segment_path (char *path, size_t size, unsigned int slot,
		  unsigned int seq)
{
  size_t length;

  if (shm_folder (path, size))
    {
      return -1;
    }
  length = strlen (path);
  if (snprintf (path + length, size - length, "%s_%u_%u.seg", SHM_PREFIX,
		slot, seq) >= (int) (size - length))
    {
      return -1;
    }
  return 0;
}

static long long
shm_boot_time (void)
{
  return (long long) time (NULL) - (long long) (GetTickCount64 () / 1000);
}

static unsigned long long
shm_process_started (DWORD pid)
{
  HANDLE process;
  FILETIME created, exited, kernel, user;
  DWORD code;
  unsigned long long started = 0;

  process = OpenProcess (PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
  if (!process)
    {
      /* Exists but belongs to someone else: count it as alive */
      return GetLastError () == ERROR_ACCESS_DENIED ? 1 : 0;
    }
  if (GetExitCodeProcess (process, &code) && code == STILL_ACTIVE
      && GetProcessTimes (process, &created, &exited, &kernel, &user))
    {
      started = ((unsigned long long) created.dwHighDateTime << 32)
	| created.dwLowDateTime;
    }
  CloseHandle (process);
  return started;
}

static void
shm_check_table (shm_table *table, int first_user)
{
  long long now_boot = shm_boot_time ();
  long long drift = table->boot_time - now_boot;
  char pattern[2 * MAX_PATH];
  char path[MAX_PATH + 1];
  WIN32_FIND_DATAA found;
  HANDLE find;
  size_t length;

  if (drift < 0)
    {
      drift = -drift;
    }
  if (table->magic == SHM_MAGIC && table->version == SHM_VERSION
      && table->size == sizeof (shm_table)
      && (drift <= SHM_BOOT_SLACK || !first_user))
    {
      return;
    }

  memset (table, 0, sizeof (shm_table));
  table->magic = SHM_MAGIC;
  table->version = SHM_VERSION;
  table->size = sizeof (shm_table);
  table->boot_time = now_boot;

  if (shm_folder (path, sizeof (path)))
    {
      return;
    }
  length = strlen (path);
  if (snprintf (pattern, sizeof (pattern), "%s%s_*.seg", path, SHM_PREFIX)
      >= (int) sizeof (pattern))
    {
      return;
    }
  find = FindFirstFileA (pattern, &found);
  if (find == INVALID_HANDLE_VALUE)
    {
      return;
    }
  do
    {
      if (length + strlen (found.cFileName) < sizeof (path))
	{
	  strcpy (path + length, found.cFileName);
	  DeleteFileA (path);
	}
    }
  while (FindNextFileA (find, &found));
  FindClose (find);
}

static void
shm_close_handles (HANDLE file, HANDLE mapping, HANDLE mutex, void *view)
{
  if (view)
    {
      UnmapViewOfFile (view);
    }
  if (mapping)
    {
      CloseHandle (mapping);
    }
  if (file && file != INVALID_HANDLE_VALUE)
    {
      CloseHandle (file);
    }
  if (mutex)
    {
      CloseHandle (mutex);
    }
}

static int
shm_attach_table (void)
{
  char path[MAX_PATH + 1];
  HANDLE file = INVALID_HANDLE_VALUE;
  HANDLE mapping = NULL;
  HANDLE mutex = NULL;
  void *view = NULL;
  void *mappings = NULL;
  int first_user;
  DWORD result;

  if (__436F64657252436C69626D696E677733325F657874656E646564.shm_table)
    {
      return 0;
    }

  __436F64657252436C69626D696E677733325F657874656E646564_lock ();

  /* Re-check after acquiring lock */
  if (__436F64657252436C69626D696E677733325F657874656E646564.shm_table)
    {
      __436F64657252436C69626D696E677733325F657874656E646564_unlock ();
      return 0;
    }

  if (shm_folder (path, sizeof (path))
      || strlen (path) + sizeof (SHM_PREFIX ".ipc") > sizeof (path))
    {
      __436F64657252436C69626D696E677733325F657874656E646564_unlock ();
      _set_errno (ENOSPC);
      return -1;
    }
  strcat (path, SHM_PREFIX ".ipc");

  mutex = CreateMutexA (NULL, FALSE, SHM_MUTEX_NAME);
  first_user = mutex && GetLastError () != ERROR_ALREADY_EXISTS;
  file = CreateFileA (path, GENERIC_READ | GENERIC_WRITE,
		      FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS,
		      FILE_ATTRIBUTE_NORMAL, NULL);
  if (mutex && file != INVALID_HANDLE_VALUE)
    {
      mapping = CreateFileMappingA (file, NULL, PAGE_READWRITE, 0,
				    sizeof (shm_table), NULL);
    }
  if (mapping)
    {
      view = MapViewOfFile (mapping, FILE_MAP_ALL_ACCESS, 0, 0,
			    sizeof (shm_table));
    }
  if (view)
    {
      mappings = calloc (SHMSEG, sizeof (struct shm_mapping));
    }
  if (!mappings)
    {
      shm_close_handles (file, mapping, mutex, view);
      __436F64657252436C69626D696E677733325F657874656E646564_unlock ();
      _set_errno (view ? ENOMEM : ENOSPC);
      return -1;
    }

  result = WaitForSingleObject (mutex, INFINITE);
  if (result == WAIT_OBJECT_0 || result == WAIT_ABANDONED)
    {
      shm_check_table ((shm_table *) view, first_user);
      ReleaseMutex (mutex);
    }

  __436F64657252436C69626D696E677733325F657874656E646564.shm_file_handle =
    file;
  __436F64657252436C69626D696E677733325F657874656E646564.shm_mapping_handle
    = mapping;
  __436F64657252436C69626D696E677733325F657874656E646564.shm_mutex_handle =
    mutex;
  __436F64657252436C69626D696E677733325F657874656E646564.shm_attached =
    mappings;
  __436F64657252436C69626D696E677733325F657874656E646564.shm_table = view;

  __436F64657252436C69626D696E677733325F657874656E646564_unlock ();
  return 0;
}

static shm_table *
shm_lock (void)
{
  DWORD result;

  if (shm_attach_table ())
    {
      return NULL;
    }

  result =
    WaitForSingleObject
    (__436F64657252436C69626D696E677733325F657874656E646564.shm_mutex_handle,
     INFINITE);
  /* WAIT_ABANDONED means a process died holding the mutex; we own it
     now and carry on */
  if (result != WAIT_OBJECT_0 && result != WAIT_ABANDONED)
    {
      _set_errno (EINVAL);
      return NULL;
    }
  return __436F64657252436C69626D696E677733325F657874656E646564.shm_table;
}

static void
shm_unlock (int changed)
{
  if (changed)
    {
      ((shm_table *)
       __436F64657252436C69626D696E677733325F657874656E646564.
       shm_table)->generation++;
    }
  ReleaseMutex
    (__436F64657252436C69626D696E677733325F657874656E646564.shm_mutex_handle);
}

/* Look up a segment by identifier while locked, or NULL */
static shm_segment *
shm_find (shm_table *table, int shmid)
{
  shm_segment *segment;

  if (shmid < 0)
    {
      return NULL;
    }
  segment = &table->segments[shmid % SHM_SLOTS];
  if (!segment->in_use || segment->seq != (unsigned int) (shmid / SHM_SLOTS))
    {
      return NULL;
    }
  return segment;
}

/* Non-zero when the caller holds the permission want (4 read, 2 write).
   Every process runs as uid 0 / gid 0 here (see pwd.h), so the owner
   bits of the mode are the ones that apply. */
static int
shm_allowed (const shm_segment *segment, int want)
{
  return ((segment->mode >> 6) & (unsigned int) want) == (unsigned int) want;
}

/* Attaches of a segment by processes that are still running. Records of
   processes that have exited, and empty records, are freed on the way,
   so a crashed process does not keep a segment counted as attached. */
static unsigned int
shm_live_nattch (shm_table *table, unsigned int slot, unsigned int seq)
{
  DWORD self = GetCurrentProcessId ();
  unsigned int nattch = 0;
  unsigned long long started;
  int i;

  for (i = 0; i < SHM_ATTACHES; i++)
    {
      struct shm_attach *record = &table->attaches[i];
      if (!record->pid)
	{
	  continue;
	}
      if (record->count && record->pid != self)
	{
	  /* 1 means the process exists but cannot be inspected */
	  started = shm_process_started (record->pid);
	  if (!started || (started != 1 && started != record->started))
	    {
	      record->count = 0;
	    }
	}
      if (!record->count)
	{
	  memset (record, 0, sizeof (*record));
	  continue;
	}
      if (record->slot == slot && record->seq == seq)
	{
	  nattch += record->count;
	}
    }
  return nattch;
}

int
shmctl (int __shmid, int __cmd, struct shmid_ds *__buf)
{
  DWORD initErrCode = GetLastError ();
  shm_table *table;
  shm_segment *segment;
  char path[MAX_PATH + 1];
  unsigned int slot;
  int changed = 0;
  int error = 0;

  if (__cmd != IPC_STAT && __cmd != IPC_SET && __cmd != IPC_RMID)
    {
      _set_errno (EINVAL);
      return -1;
    }
  if (__cmd != IPC_RMID && !__buf)
    {
      _set_errno (EFAULT);
      return -1;
    }

  table = shm_lock ();
  if (!table)
    {
      SetLastError (initErrCode);
      return -1;
    }

  segment = shm_find (table, __shmid);
  if (!segment)
    {
      error = EINVAL;
    }
  else if (__cmd == IPC_STAT)
    {
      slot = (unsigned int) (segment - table->segments);
      if (!shm_allowed (segment, 4))
	{
	  error = EACCES;
	}
      else if (segment->size > (unsigned long long) (size_t) -1)
	{
	  /* A 32-bit caller cannot hold the size of this segment */
	  error = EOVERFLOW;
	}
      else
	{
	  memset (__buf, 0, sizeof (*__buf));
	  __buf->shm_perm.uid = (uid_t) segment->uid;
	  __buf->shm_perm.gid = (gid_t) segment->gid;
	  __buf->shm_perm.cuid = (uid_t) segment->cuid;
	  __buf->shm_perm.cgid = (gid_t) segment->cgid;
	  __buf->shm_perm.mode = (mode_t) segment->mode;
	  __buf->shm_perm.seq = (unsigned short) segment->seq;
	  __buf->shm_perm.key = (key_t) segment->key;
	  __buf->shm_segsz = (size_t) segment->size;
	  __buf->shm_lpid = (pid_t) segment->lpid;
	  __buf->shm_cpid = (pid_t) segment->cpid;
	  __buf->shm_nattch = shm_live_nattch (table, slot, segment->seq);
	  __buf->shm_atime = (time_t) segment->atime;
	  __buf->shm_dtime = (time_t) segment->dtime;
	  __buf->shm_ctime = (time_t) segment->ctime;
	  /* shm_live_nattch() may have freed records of exited processes */
	  changed = 1;
	}
    }
  else if (__cmd == IPC_SET)
    {
      /* Every process is uid 0, the owner and creator (see pwd.h), so
         the ownership check always passes. */
      segment->uid = (unsigned int) __buf->shm_perm.uid;
      segment->gid = (unsigned int) __buf->shm_perm.gid;
      segment->mode = (unsigned int) (__buf->shm_perm.mode & 0777);
      segment->ctime = (long long) time (NULL);
      changed = 1;
    }
  else
    {
      /* IPC_RMID: the identifier goes now. Deleting the file only marks
         it; Windows keeps the data until the last view is unmapped. */
      slot = (unsigned int) (segment - table->segments);
      if (!shm_segment_path (path, sizeof (path), slot, segment->seq))
	{
	  DeleteFileA (path);
	}
      segment->in_use = 0;
      segment->seq++;
      changed = 1;
    }

  shm_unlock (changed);
  SetLastError (initErrCode);
  if (error)
    {
      _set_errno (error);
      return -1;
    }
  return 0;
}
