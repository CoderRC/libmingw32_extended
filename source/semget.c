#include <__436F64657252436C69626D696E677733325F657874656E646564.h>
#include <sys/sem.h>
#include <windows.h>
#include <dlfcn.h>
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/*
 * Shared semaphore table, the same in semctl.c, semget.c and semop.c
 * (one function per file, like pwd.h)
 *
 * Semaphore sets outlive the processes that use them, like System V
 * semaphores: the table lives in a file in the user's Local AppData
 * folder that every process maps. A named mutex guards it across
 * processes and a named event wakes callers blocked in semop(). SEM_UNDO
 * adjustments and blocked callers are recorded per process, so the
 * adjustments of a process that exits or is killed are applied, and it
 * stops being counted in semncnt / semzcnt. The table is reset after a
 * reboot.
 *
 * Every field has a fixed width so 32-bit and 64-bit programs share it.
 */
#define SEM_SETS SEMMNI
#define SEM_WAITERS 512		/* Blocked semop() calls across all processes */

struct sem_value
{
  int val;			/* semval */
  unsigned int pid;		/* sempid: process of the last operation */
};

struct sem_set
{
  int in_use;			/* 1 while the identifier exists */
  int key;			/* key_t passed to semget() */
  unsigned int seq;		/* Bumped on removal so stale ids fail */
  unsigned int uid, gid, cuid, cgid;
  unsigned int mode;		/* Low 9 bits are the permissions */
  unsigned int nsems;
  long long otime, ctime;
  struct sem_value sems[SEMMSL];
};

/* SEM_UNDO adjustments of one process for one set */
struct sem_undo
{
  unsigned int pid;		/* 0 when the record is free */
  unsigned int slot;
  unsigned int seq;
  unsigned int unused;
  unsigned long long started;	/* Process creation time, against pid reuse */
  short adj[SEMMSL];		/* semadj */
};

/* One semop() call blocked on one semaphore */
struct sem_waiter
{
  unsigned int pid;		/* 0 when the record is free */
  unsigned int slot;
  unsigned int seq;
  unsigned short semnum;
  unsigned short zero;		/* 1: waits for zero (semzcnt), 0: semncnt */
  unsigned long long started;
};

struct sem_table
{
  unsigned int magic;
  unsigned int version;
  unsigned int size;
  unsigned int generation;	/* Bumped on every change */
  long long boot_time;		/* Seconds since 1970, to detect a reboot */
  struct sem_set sets[SEM_SETS];
  struct sem_undo undos[SEMMNU];
  struct sem_waiter waiters[SEM_WAITERS];
};

typedef struct sem_table sem_table;
typedef struct sem_set sem_set;

#define SEM_MAGIC 0x4D455353U	/* "SSEM" */
#define SEM_VERSION 1U
#define SEM_PREFIX "__436F64657252436C69626D696E677733325F657874656E646564_sem"
#define SEM_MUTEX_NAME "Local\\__436F64657252436C69626D696E677733325F657874656E646564_sem_mutex"
#define SEM_EVENT_NAME "Local\\__436F64657252436C69626D696E677733325F657874656E646564_sem_event"
#define SEM_WAIT_MS 20		/* Re-check blocked calls at least this often */
#define SEM_BOOT_SLACK 120	/* Seconds of jitter allowed in the boot time */

/* Typedef for SHGetFolderPathA function pointer */
typedef HRESULT (WINAPI SHGetFolderPathAFunc) (HWND hwnd, int csidl,
					       HANDLE token, DWORD flags,
					       LPSTR path);

/* CSIDL_LOCAL_APPDATA from shlobj.h */
#define SEM_CSIDL_LOCAL_APPDATA 0x001c

/* Path of the table file. Local AppData does not change with TEMP/TMP,
   so programs started from MSYS2, cmd or Explorer share the same sets. */
/* Creation time of a process, 1 when it exists but cannot be inspected,
   or 0 when it has exited */
/* Non-zero when the process that made a record has exited */
/* Reset a table that is new or from another version. A boot time that
   no longer matches means the machine rebooted, but the wall clock can
   also have been changed, so that only resets the table when no other
   process has it open (first_user). */
/* Open the table file, mapping, mutex and event once per process */
/* Apply the SEM_UNDO adjustments of exited processes and drop their
   waiter records. Returns non-zero when anything changed. */
/* Attach the table, take the cross-process lock and tidy up after
   exited processes */
/* Release the lock, wait until the table changes or a short timeout
   passes, then take the lock again */

static int
sem_table_path (char *path, size_t size)
{
  void *shell32 = dlopen ("shell32.dll", RTLD_NOW);
  size_t length = 0;

  if (shell32)
    {
      SHGetFolderPathAFunc *_SHGetFolderPathA =
	dlsym (shell32, "SHGetFolderPathA");
      if (_SHGetFolderPathA
	  && _SHGetFolderPathA (NULL, SEM_CSIDL_LOCAL_APPDATA, NULL, 0,
				path) == S_OK)
	{
	  length = strlen (path);
	  if (length && path[length - 1] != '\\' && length + 1 < size)
	    {
	      path[length++] = '\\';
	    }
	}
      dlclose (shell32);
    }
  if (!length)
    {
      length = GetTempPathA ((DWORD) size, path);
    }
  if (!length || length + sizeof (SEM_PREFIX ".ipc") > size)
    {
      return -1;
    }
  memcpy (path + length, SEM_PREFIX ".ipc", sizeof (SEM_PREFIX ".ipc"));
  return 0;
}

static long long
sem_boot_time (void)
{
  return (long long) time (NULL) - (long long) (GetTickCount64 () / 1000);
}

static unsigned long long
sem_process_started (DWORD pid)
{
  HANDLE process;
  FILETIME created, exited, kernel, user;
  DWORD code;
  unsigned long long started = 0;

  process = OpenProcess (PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
  if (!process)
    {
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

static int
sem_process_gone (unsigned int pid, unsigned long long started)
{
  unsigned long long now;

  if (pid == GetCurrentProcessId ())
    {
      return 0;
    }
  now = sem_process_started (pid);
  return !now || (now != 1 && now != started);
}

static void
sem_check_table (sem_table *table, int first_user)
{
  long long now_boot = sem_boot_time ();
  long long drift = table->boot_time - now_boot;

  if (drift < 0)
    {
      drift = -drift;
    }
  if (table->magic == SEM_MAGIC && table->version == SEM_VERSION
      && table->size == sizeof (sem_table)
      && (drift <= SEM_BOOT_SLACK || !first_user))
    {
      return;
    }

  memset (table, 0, sizeof (sem_table));
  table->magic = SEM_MAGIC;
  table->version = SEM_VERSION;
  table->size = sizeof (sem_table);
  table->boot_time = now_boot;
}

static void
sem_close_handles (HANDLE file, HANDLE mapping, HANDLE mutex, HANDLE event,
		   void *view)
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
  if (event)
    {
      CloseHandle (event);
    }
}

static int
sem_attach_table (void)
{
  char path[MAX_PATH + 1];
  HANDLE file = INVALID_HANDLE_VALUE;
  HANDLE mapping = NULL;
  HANDLE mutex = NULL;
  HANDLE event = NULL;
  void *view = NULL;
  int first_user;
  DWORD result;

  if (__436F64657252436C69626D696E677733325F657874656E646564.sem_table)
    {
      return 0;
    }

  __436F64657252436C69626D696E677733325F657874656E646564_lock ();

  /* Re-check after acquiring lock */
  if (__436F64657252436C69626D696E677733325F657874656E646564.sem_table)
    {
      __436F64657252436C69626D696E677733325F657874656E646564_unlock ();
      return 0;
    }

  if (sem_table_path (path, sizeof (path)))
    {
      __436F64657252436C69626D696E677733325F657874656E646564_unlock ();
      _set_errno (ENOSPC);
      return -1;
    }

  mutex = CreateMutexA (NULL, FALSE, SEM_MUTEX_NAME);
  first_user = mutex && GetLastError () != ERROR_ALREADY_EXISTS;
  event = CreateEventA (NULL, FALSE, FALSE, SEM_EVENT_NAME);
  file = CreateFileA (path, GENERIC_READ | GENERIC_WRITE,
		      FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS,
		      FILE_ATTRIBUTE_NORMAL, NULL);
  if (mutex && event && file != INVALID_HANDLE_VALUE)
    {
      mapping = CreateFileMappingA (file, NULL, PAGE_READWRITE, 0,
				    sizeof (sem_table), NULL);
    }
  if (mapping)
    {
      view = MapViewOfFile (mapping, FILE_MAP_ALL_ACCESS, 0, 0,
			    sizeof (sem_table));
    }
  if (!view)
    {
      sem_close_handles (file, mapping, mutex, event, view);
      __436F64657252436C69626D696E677733325F657874656E646564_unlock ();
      _set_errno (ENOSPC);
      return -1;
    }

  result = WaitForSingleObject (mutex, INFINITE);
  if (result == WAIT_OBJECT_0 || result == WAIT_ABANDONED)
    {
      sem_check_table ((sem_table *) view, first_user);
      ReleaseMutex (mutex);
    }

  __436F64657252436C69626D696E677733325F657874656E646564.sem_file_handle =
    file;
  __436F64657252436C69626D696E677733325F657874656E646564.sem_mapping_handle
    = mapping;
  __436F64657252436C69626D696E677733325F657874656E646564.sem_mutex_handle =
    mutex;
  __436F64657252436C69626D696E677733325F657874656E646564.sem_event_handle =
    event;
  __436F64657252436C69626D696E677733325F657874656E646564.sem_table = view;

  __436F64657252436C69626D696E677733325F657874656E646564_unlock ();
  return 0;
}

static int
sem_reap (sem_table *table)
{
  int changed = 0;
  int i, n;

  for (i = 0; i < SEMMNU; i++)
    {
      struct sem_undo *undo = &table->undos[i];
      if (!undo->pid || !sem_process_gone (undo->pid, undo->started))
	{
	  continue;
	}
      if (table->sets[undo->slot].in_use
	  && table->sets[undo->slot].seq == undo->seq)
	{
	  sem_set *set = &table->sets[undo->slot];
	  for (n = 0; n < (int) set->nsems; n++)
	    {
	      int val = set->sems[n].val + undo->adj[n];
	      /* Clamp like Linux: the adjustment cannot go below 0 or
	         above SEMVMX */
	      set->sems[n].val = val < 0 ? 0 : val > SEMVMX ? SEMVMX : val;
	    }
	}
      memset (undo, 0, sizeof (*undo));
      changed = 1;
    }
  for (i = 0; i < SEM_WAITERS; i++)
    {
      struct sem_waiter *waiter = &table->waiters[i];
      if (waiter->pid && sem_process_gone (waiter->pid, waiter->started))
	{
	  memset (waiter, 0, sizeof (*waiter));
	  changed = 1;
	}
    }
  return changed;
}

static sem_table *
sem_lock (void)
{
  sem_table *table;
  DWORD result;

  if (sem_attach_table ())
    {
      return NULL;
    }

  result =
    WaitForSingleObject
    (__436F64657252436C69626D696E677733325F657874656E646564.sem_mutex_handle,
     INFINITE);
  /* WAIT_ABANDONED means a process died holding the mutex; we own it
     now and carry on */
  if (result != WAIT_OBJECT_0 && result != WAIT_ABANDONED)
    {
      _set_errno (EINVAL);
      return NULL;
    }
  table = __436F64657252436C69626D696E677733325F657874656E646564.sem_table;
  if (sem_reap (table))
    {
      table->generation++;
      SetEvent
	(__436F64657252436C69626D696E677733325F657874656E646564.sem_event_handle);
    }
  return table;
}

static void
sem_unlock (int changed)
{
  if (changed)
    {
      ((sem_table *)
       __436F64657252436C69626D696E677733325F657874656E646564.
       sem_table)->generation++;
    }
  ReleaseMutex
    (__436F64657252436C69626D696E677733325F657874656E646564.sem_mutex_handle);
  if (changed)
    {
      SetEvent
	(__436F64657252436C69626D696E677733325F657874656E646564.sem_event_handle);
    }
}

/* Non-zero when the caller holds the permission want (4 read, 2 write).
   Every process runs as uid 0 / gid 0 here (see pwd.h), so the owner
   bits of the mode are the ones that apply. */
static int
sem_allowed (const sem_set *set, int want)
{
  return ((set->mode >> 6) & (unsigned int) want) == (unsigned int) want;
}

int
semget (key_t __key, int __nsems, int __semflg)
{
  DWORD initErrCode = GetLastError ();
  sem_table *table;
  sem_set *set = NULL;
  int slot;
  int semid;
  int error;
  int i;

  table = sem_lock ();
  if (!table)
    {
      SetLastError (initErrCode);
      return -1;
    }

  if (__key != IPC_PRIVATE)
    {
      for (slot = 0; slot < SEM_SETS; slot++)
	{
	  if (table->sets[slot].in_use && table->sets[slot].key == __key)
	    {
	      set = &table->sets[slot];
	      break;
	    }
	}
    }

  if (set)
    {
      /* An identifier already exists for key */
      int requested = __semflg & 0777;
      requested = (requested >> 6 | requested >> 3 | requested) & 7;

      error = 0;
      if ((__semflg & IPC_CREAT) && (__semflg & IPC_EXCL))
	{
	  error = EEXIST;
	}
      else if (!sem_allowed (set, requested))
	{
	  error = EACCES;
	}
      else if (__nsems < 0 || (unsigned int) __nsems > set->nsems)
	{
	  error = EINVAL;
	}
      semid = (int) (set->seq * SEM_SETS + (set - table->sets));
      sem_unlock (0);
      SetLastError (initErrCode);
      if (error)
	{
	  _set_errno (error);
	  return -1;
	}
      return semid;
    }

  if (__key != IPC_PRIVATE && !(__semflg & IPC_CREAT))
    {
      sem_unlock (0);
      SetLastError (initErrCode);
      _set_errno (ENOENT);
      return -1;
    }
  if (__nsems <= 0 || __nsems > SEMMSL)
    {
      sem_unlock (0);
      SetLastError (initErrCode);
      _set_errno (EINVAL);
      return -1;
    }

  for (slot = 0; slot < SEM_SETS; slot++)
    {
      if (!table->sets[slot].in_use)
	{
	  set = &table->sets[slot];
	  break;
	}
    }
  if (!set)
    {
      sem_unlock (0);
      SetLastError (initErrCode);
      _set_errno (ENOSPC);
      return -1;
    }

  /* Keep the identifier non-negative when seq grows large */
  if (set->seq > (unsigned int) (0x7FFFFFFF / SEM_SETS) - 1)
    {
      set->seq = 0;
    }

  /* Records left from an earlier set in this slot */
  for (i = 0; i < SEMMNU; i++)
    {
      if (table->undos[i].pid && table->undos[i].slot == (unsigned int) slot)
	{
	  memset (&table->undos[i], 0, sizeof (table->undos[i]));
	}
    }

  set->key = __key;
  set->uid = set->cuid = 0;
  set->gid = set->cgid = 0;
  set->mode = __semflg & 0777;
  set->nsems = (unsigned int) __nsems;
  memset (set->sems, 0, sizeof (set->sems));
  set->otime = 0;
  set->ctime = (long long) time (NULL);
  set->in_use = 1;

  semid = (int) (set->seq * SEM_SETS + slot);
  sem_unlock (1);
  SetLastError (initErrCode);
  return semid;
}
