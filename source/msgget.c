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
  unsigned int qbytes;	/* Byte limit for message text */
  unsigned int cbytes;	/* Bytes of message text on the queue */
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
  long long boot_time;	/* Seconds since 1970, to detect a reboot */
  struct msg_queue queues[MSG_QUEUES];
};

typedef struct msg_table msg_table;
typedef struct msg_queue msg_queue;

#define MSG_STATE __436F64657252436C69626D696E677733325F657874656E646564

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
check_table (msg_table * table, int first_user)
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

  if (MSG_STATE.msg_table)
    {
      return 0;
    }

  __436F64657252436C69626D696E677733325F657874656E646564_lock ();

  /* Re-check after acquiring lock */
  if (MSG_STATE.msg_table)
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

  MSG_STATE.msg_file_handle = file;
  MSG_STATE.msg_mapping_handle = mapping;
  MSG_STATE.msg_mutex_handle = mutex;
  MSG_STATE.msg_event_handle = event;
  MSG_STATE.msg_table = view;

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

  result = WaitForSingleObject (MSG_STATE.msg_mutex_handle, INFINITE);
  /* WAIT_ABANDONED means a process died holding the mutex; we own it
     now and carry on, as the queue that process was changing is the
     only one at risk. */
  if (result != WAIT_OBJECT_0 && result != WAIT_ABANDONED)
    {
      _set_errno (EINVAL);
      return NULL;
    }

  return MSG_STATE.msg_table;
}

static void
msg_unlock (int changed)
{
  if (changed)
    {
      ((msg_table *) MSG_STATE.msg_table)->generation++;
    }
  ReleaseMutex (MSG_STATE.msg_mutex_handle);
  if (changed)
    {
      SetEvent (MSG_STATE.msg_event_handle);
    }
}

static int
msg_allowed (const msg_queue * queue, int want)
{
  /* Every process runs as uid 0 / gid 0 here (see pwd.h), so the owner
     bits of the mode are the ones that apply. */
  return ((queue->mode >> 6) & (unsigned int) want) == (unsigned int) want;
}

/* Windows stat always returns 0 for uid/gid (see pwd.h) */
#define STAT_UID 0
#define STAT_GID 0

int
msgget (key_t __key, int __msgflg)
{
  DWORD initErrCode = GetLastError ();
  msg_table *table;
  msg_queue *queue = NULL;
  int slot;
  int msqid;

  table = msg_lock ();
  if (!table)
    {
      SetLastError (initErrCode);
      return -1;
    }

  if (__key != IPC_PRIVATE)
    {
      for (slot = 0; slot < MSG_QUEUES; slot++)
	{
	  if (table->queues[slot].in_use && table->queues[slot].key == __key)
	    {
	      queue = &table->queues[slot];
	      break;
	    }
	}
    }

  if (queue)
    {
      /* An identifier already exists for key */
      int requested = __msgflg & 0777;
      requested = (requested >> 6 | requested >> 3 | requested) & 7;

      if ((__msgflg & IPC_CREAT) && (__msgflg & IPC_EXCL))
	{
	  msg_unlock (0);
	  SetLastError (initErrCode);
	  _set_errno (EEXIST);
	  return -1;
	}
      if (!msg_allowed
	  (queue, requested))
	{
	  msg_unlock (0);
	  SetLastError (initErrCode);
	  _set_errno (EACCES);
	  return -1;
	}
      msqid = (int) (queue->seq * MSG_QUEUES + (queue - table->queues));
      msg_unlock (0);
      SetLastError (initErrCode);
      return msqid;
    }

  if (__key != IPC_PRIVATE && !(__msgflg & IPC_CREAT))
    {
      msg_unlock (0);
      SetLastError (initErrCode);
      _set_errno (ENOENT);
      return -1;
    }

  for (slot = 0; slot < MSG_QUEUES; slot++)
    {
      if (!table->queues[slot].in_use)
	{
	  queue = &table->queues[slot];
	  break;
	}
    }
  if (!queue)
    {
      msg_unlock (0);
      SetLastError (initErrCode);
      _set_errno (ENOSPC);
      return -1;
    }

  /* Keep the identifier non-negative when seq grows large */
  if (queue->seq > (unsigned int) (0x7FFFFFFF / MSG_QUEUES) - 1)
    {
      queue->seq = 0;
    }

  queue->key = __key;
  queue->uid = queue->cuid = STAT_UID;
  queue->gid = queue->cgid = STAT_GID;
  queue->mode = __msgflg & 0777;
  queue->qnum = 0;
  queue->qbytes = MSGMNB;
  queue->cbytes = 0;
  queue->used = 0;
  queue->lspid = 0;
  queue->lrpid = 0;
  queue->stime = 0;
  queue->rtime = 0;
  queue->ctime = (long long) time (NULL);
  queue->in_use = 1;

  msqid = (int) (queue->seq * MSG_QUEUES + slot);
  msg_unlock (1);
  SetLastError (initErrCode);
  return msqid;
}
