#include <__436F64657252436C69626D696E677733325F657874656E646564.h>
#include <windows.h>
#include <dlfcn.h>
#include <errno.h>
#include <string.h>
#include <time.h>

/* Shared queue table used by msgget(), msgsnd(), msgrcv() and msgctl() */

typedef struct __436F64657252436C69626D696E677733325F657874656E646564_msg_table
  msg_table;
typedef struct __436F64657252436C69626D696E677733325F657874656E646564_msg_queue
  msg_queue;

#define MSG_STATE __436F64657252436C69626D696E677733325F657874656E646564
#define MSG_QUEUES __436F64657252436C69626D696E677733325F657874656E646564_MSG_QUEUES

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
  MSG_STATE.msg_table = (msg_table *) view;

  __436F64657252436C69626D696E677733325F657874656E646564_unlock ();
  return 0;
}

msg_table *
__436F64657252436C69626D696E677733325F657874656E646564_msg_lock (void)
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

void
__436F64657252436C69626D696E677733325F657874656E646564_msg_unlock (int changed)
{
  if (changed)
    {
      MSG_STATE.msg_table->generation++;
    }
  ReleaseMutex (MSG_STATE.msg_mutex_handle);
  if (changed)
    {
      SetEvent (MSG_STATE.msg_event_handle);
    }
}

msg_table *
__436F64657252436C69626D696E677733325F657874656E646564_msg_wait (void)
{
  ReleaseMutex (MSG_STATE.msg_mutex_handle);
  WaitForSingleObject (MSG_STATE.msg_event_handle, MSG_WAIT_MS);
  return __436F64657252436C69626D696E677733325F657874656E646564_msg_lock ();
}

msg_queue *
__436F64657252436C69626D696E677733325F657874656E646564_msg_find (msg_table *
								 table,
								 int msqid)
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

int
__436F64657252436C69626D696E677733325F657874656E646564_msg_allowed (const
								    msg_queue
								    * queue,
								    int want)
{
  /* Every process runs as uid 0 / gid 0 here (see pwd.h), so the owner
     bits of the mode are the ones that apply. */
  return ((queue->mode >> 6) & (unsigned int) want) == (unsigned int) want;
}

int
__436F64657252436C69626D696E677733325F657874656E646564_msg_privileged (void)
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
