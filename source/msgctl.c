#include <__436F64657252436C69626D696E677733325F657874656E646564.h>
#include <sys/msg.h>
#include <windows.h>
#include <dlfcn.h>
#include <errno.h>
#include <string.h>
#include <time.h>

/*
 * Shared queue table, the same in msgctl.c, msgget.c, msgrcv.c and
 * msgsnd.c (one function per file, like pwd.h)
 *
 * Message queues outlive the process that created them, like System V
 * queues: the table lives in a file in the user's Local AppData folder
 * that every process maps. A named mutex guards it across processes and a
 * named event wakes callers blocked in msgsnd() or msgrcv(). The table
 * is reset when the machine has rebooted since it was written.
 *
 * Every field has a fixed width so 32-bit and 64-bit programs share it.
 */
#define MSG_QUEUES 32
#define MSG_BYTES 16384
#define MSG_COUNT 512
#define MSG_HEADER 8
#define MSG_DATA \
  (MSG_BYTES \
   + MSG_COUNT \
   * MSG_HEADER)

struct msg_queue
{
  int in_use;			/* 1 while the identifier exists */
  int key;			/* key_t passed to msgget() */
  unsigned int seq;		/* Bumped on removal so stale ids fail */
  unsigned int uid, gid, cuid, cgid;
  unsigned int mode;		/* Low 9 bits are the permissions */
  unsigned int qnum;		/* Messages on the queue */
  unsigned int qbytes;		/* Byte limit for message text */
  unsigned int cbytes;		/* Bytes of message text on the queue */
  unsigned int used;		/* Bytes of data[] in use */
  unsigned int lspid, lrpid;
  long long stime, rtime, ctime;
  /* Messages back to back: int mtype, unsigned int size, then text */
  unsigned char data[MSG_DATA];
};

struct msg_table
{
  unsigned int magic;
  unsigned int version;
  unsigned int size;
  unsigned int generation;	/* Bumped on every change */
  long long boot_time;		/* Seconds since 1970, to detect a reboot */
  struct msg_queue queues[MSG_QUEUES];
};

typedef struct msg_table msg_table;
typedef struct msg_queue msg_queue;


#define MSG_MAGIC 0x5147534DU	/* "MSGQ" */
#define MSG_VERSION 1U
#define MSG_FILE_NAME "__436F64657252436C69626D696E677733325F657874656E646564_msg.ipc"
#define MSG_MUTEX_NAME "Local\\__436F64657252436C69626D696E677733325F657874656E646564_msg_mutex"
#define MSG_EVENT_NAME "Local\\__436F64657252436C69626D696E677733325F657874656E646564_msg_event"
#define MSG_WAIT_MS 20		/* Re-check blocked calls at least this often */
#define MSG_BOOT_SLACK 120	/* Seconds of jitter allowed in the boot time */

/* Typedef for SHGetFolderPathA function pointer */
typedef HRESULT (WINAPI SHGetFolderPathAFunc) (HWND hwnd, int csidl,
					       HANDLE token, DWORD flags,
					       LPSTR path);

/* CSIDL_LOCAL_APPDATA from shlobj.h */
#define MSG_CSIDL_LOCAL_APPDATA 0x001c

/* The table lives in the user's Local AppData folder, which does not
   change with TEMP/TMP, so programs started from MSYS2, cmd or
   Explorer all share the same queues. GetTempPath() is the fallback. */
static int
table_path (char *path, size_t size)
{
  void *shell32 = dlopen ("shell32.dll", RTLD_NOW);
  size_t length = 0;

  if (shell32)
    {
      SHGetFolderPathAFunc *_SHGetFolderPathA =
	dlsym (shell32, "SHGetFolderPathA");
      if (_SHGetFolderPathA
	  && _SHGetFolderPathA (NULL, MSG_CSIDL_LOCAL_APPDATA, NULL, 0,
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
  if (!length || length + sizeof (MSG_FILE_NAME) > size)
    {
      return -1;
    }
  memcpy (path + length, MSG_FILE_NAME, sizeof (MSG_FILE_NAME));
  return 0;
}

static long long
boot_time (void)
{
  return (long long) time (NULL) - (long long) (GetTickCount64 () / 1000);
}

static void
close_handles (HANDLE file, HANDLE mapping, HANDLE mutex, HANDLE event,
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

/* Reset a table that is new or from another version. A boot time that
   no longer matches means the machine rebooted, but the wall clock can
   also have been changed, so that only resets the table when no other
   process has it open (first_user). */
static void
check_table (msg_table *table, int first_user)
{
  long long now_boot = boot_time ();
  long long drift = table->boot_time - now_boot;

  if (drift < 0)
    {
      drift = -drift;
    }
  if (table->magic == MSG_MAGIC && table->version == MSG_VERSION
      && table->size == sizeof (msg_table)
      && (drift <= MSG_BOOT_SLACK || !first_user))
    {
      return;
    }

  memset (table, 0, sizeof (msg_table));
  table->magic = MSG_MAGIC;
  table->version = MSG_VERSION;
  table->size = sizeof (msg_table);
  table->boot_time = now_boot;
}

/* Open the file, mapping, mutex and event once per process */
static int
attach (void)
{
  char path[MAX_PATH + 1];
  HANDLE file = INVALID_HANDLE_VALUE;
  HANDLE mapping = NULL;
  HANDLE mutex = NULL;
  HANDLE event = NULL;
  void *view = NULL;
  int first_user;
  DWORD result;

  if (__436F64657252436C69626D696E677733325F657874656E646564.msg_table)
    {
      return 0;
    }

  __436F64657252436C69626D696E677733325F657874656E646564_lock ();

  /* Re-check after acquiring lock */
  if (__436F64657252436C69626D696E677733325F657874656E646564.msg_table)
    {
      __436F64657252436C69626D696E677733325F657874656E646564_unlock ();
      return 0;
    }

  if (table_path (path, sizeof (path)))
    {
      __436F64657252436C69626D696E677733325F657874656E646564_unlock ();
      _set_errno (ENOSPC);
      return -1;
    }

  mutex = CreateMutexA (NULL, FALSE, MSG_MUTEX_NAME);
  first_user = mutex && GetLastError () != ERROR_ALREADY_EXISTS;
  event = CreateEventA (NULL, FALSE, FALSE, MSG_EVENT_NAME);
  file = CreateFileA (path, GENERIC_READ | GENERIC_WRITE,
		      FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS,
		      FILE_ATTRIBUTE_NORMAL, NULL);
  if (mutex && event && file != INVALID_HANDLE_VALUE)
    {
      mapping = CreateFileMappingA (file, NULL, PAGE_READWRITE, 0,
				    sizeof (msg_table), NULL);
    }
  if (mapping)
    {
      view = MapViewOfFile (mapping, FILE_MAP_ALL_ACCESS, 0, 0,
			    sizeof (msg_table));
    }
  if (!view)
    {
      close_handles (file, mapping, mutex, event, view);
      __436F64657252436C69626D696E677733325F657874656E646564_unlock ();
      _set_errno (ENOSPC);
      return -1;
    }

  result = WaitForSingleObject (mutex, INFINITE);
  if (result == WAIT_OBJECT_0 || result == WAIT_ABANDONED)
    {
      check_table ((msg_table *) view, first_user);
      ReleaseMutex (mutex);
    }

  __436F64657252436C69626D696E677733325F657874656E646564.msg_file_handle =
    file;
  __436F64657252436C69626D696E677733325F657874656E646564.msg_mapping_handle =
    mapping;
  __436F64657252436C69626D696E677733325F657874656E646564.msg_mutex_handle =
    mutex;
  __436F64657252436C69626D696E677733325F657874656E646564.msg_event_handle =
    event;
  __436F64657252436C69626D696E677733325F657874656E646564.msg_table = view;

  __436F64657252436C69626D696E677733325F657874656E646564_unlock ();
  return 0;
}

static msg_table *
msg_lock (void)
{
  DWORD result;

  if (attach ())
    {
      return NULL;
    }

  result =
    WaitForSingleObject
    (__436F64657252436C69626D696E677733325F657874656E646564.msg_mutex_handle,
     INFINITE);
  /* WAIT_ABANDONED means a process died holding the mutex; we own it
     now and carry on, as the queue that process was changing is the
     only one at risk. */
  if (result != WAIT_OBJECT_0 && result != WAIT_ABANDONED)
    {
      _set_errno (EINVAL);
      return NULL;
    }

  return __436F64657252436C69626D696E677733325F657874656E646564.msg_table;
}

static void
msg_unlock (int changed)
{
  if (changed)
    {
      ((msg_table *) __436F64657252436C69626D696E677733325F657874656E646564.
       msg_table)->generation++;
    }
  ReleaseMutex (__436F64657252436C69626D696E677733325F657874656E646564.
		msg_mutex_handle);
  if (changed)
    {
      SetEvent (__436F64657252436C69626D696E677733325F657874656E646564.
		msg_event_handle);
    }
}

static msg_queue *
msg_find (msg_table *table, int msqid)
{
  msg_queue *queue;

  if (msqid < 0)
    {
      return NULL;
    }
  queue = &table->queues[msqid % MSG_QUEUES];
  if (!queue->in_use || queue->seq != (unsigned int) (msqid / MSG_QUEUES))
    {
      return NULL;
    }
  return queue;
}

static int
msg_allowed (const msg_queue *queue, int want)
{
  /* Every process runs as uid 0 / gid 0 here (see pwd.h), so the owner
     bits of the mode are the ones that apply. */
  return ((queue->mode >> 6) & (unsigned int) want) == (unsigned int) want;
}

static int
msg_privileged (void)
{
  HANDLE token = NULL;
  TOKEN_ELEVATION elevation = { 0 };
  DWORD size = 0;
  int privileged = 0;

  if (OpenProcessToken (GetCurrentProcess (), TOKEN_QUERY, &token))
    {
      if (GetTokenInformation (token, TokenElevation, &elevation,
			       sizeof (elevation), &size))
	{
	  privileged = elevation.TokenIsElevated != 0;
	}
      CloseHandle (token);
    }
  return privileged;
}

int
msgctl (int __msqid, int __cmd, struct msqid_ds *__buf)
{
  DWORD initErrCode = GetLastError ();
  msg_table *table;
  msg_queue *queue;
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

  table = msg_lock ();
  if (!table)
    {
      SetLastError (initErrCode);
      return -1;
    }

  queue = msg_find (table, __msqid);
  if (!queue)
    {
      error = EINVAL;
    }
  else if (__cmd == IPC_STAT)
    {
      if (!msg_allowed (queue, 4))
	{
	  error = EACCES;
	}
      else
	{
	  memset (__buf, 0, sizeof (*__buf));
	  __buf->msg_perm.uid = (uid_t) queue->uid;
	  __buf->msg_perm.gid = (gid_t) queue->gid;
	  __buf->msg_perm.cuid = (uid_t) queue->cuid;
	  __buf->msg_perm.cgid = (gid_t) queue->cgid;
	  __buf->msg_perm.mode = (mode_t) queue->mode;
	  __buf->msg_perm.seq = (unsigned short) queue->seq;
	  __buf->msg_perm.key = (key_t) queue->key;
	  __buf->msg_qnum = queue->qnum;
	  __buf->msg_qbytes = queue->qbytes;
	  __buf->msg_lspid = (pid_t) queue->lspid;
	  __buf->msg_lrpid = (pid_t) queue->lrpid;
	  __buf->msg_stime = (time_t) queue->stime;
	  __buf->msg_rtime = (time_t) queue->rtime;
	  __buf->msg_ctime = (time_t) queue->ctime;
	}
    }
  else
    {
      /* IPC_SET and IPC_RMID: every process is uid 0, the owner and
         creator (see pwd.h), so the ownership check always passes. */
      if (__cmd == IPC_SET)
	{
	  if (__buf->msg_qbytes > MSGMNB)
	    {
	      /* Above what the shared table can hold */
	      error = EINVAL;
	    }
	  else if (__buf->msg_qbytes > queue->qbytes && !msg_privileged ())
	    {
	      error = EPERM;
	    }
	  else
	    {
	      queue->uid = (unsigned int) __buf->msg_perm.uid;
	      queue->gid = (unsigned int) __buf->msg_perm.gid;
	      queue->mode = (unsigned int) (__buf->msg_perm.mode & 0777);
	      queue->qbytes = (unsigned int) __buf->msg_qbytes;
	      queue->ctime = (long long) time (NULL);
	      changed = 1;
	    }
	}
      else
	{
	  /* IPC_RMID: bump seq so the old identifier fails (EIDRM for
	     blocked callers, EINVAL afterwards) */
	  queue->in_use = 0;
	  queue->seq++;
	  queue->qnum = 0;
	  queue->cbytes = 0;
	  queue->used = 0;
	  changed = 1;
	}
    }

  msg_unlock (changed);
  SetLastError (initErrCode);
  if (error)
    {
      _set_errno (error);
      return -1;
    }
  return 0;
}
