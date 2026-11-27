#include <__436F64657252436C69626D696E677733325F657874656E646564.h>
#include <sys/msg.h>
#include <windows.h>
#include <errno.h>
#include <string.h>
#include <time.h>

typedef struct __436F64657252436C69626D696E677733325F657874656E646564_msg_table
  msg_table;
typedef struct __436F64657252436C69626D696E677733325F657874656E646564_msg_queue
  msg_queue;

#define MSG_QUEUES __436F64657252436C69626D696E677733325F657874656E646564_MSG_QUEUES

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

  table = __436F64657252436C69626D696E677733325F657874656E646564_msg_lock ();
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
	  __436F64657252436C69626D696E677733325F657874656E646564_msg_unlock (0);
	  SetLastError (initErrCode);
	  _set_errno (EEXIST);
	  return -1;
	}
      if (!__436F64657252436C69626D696E677733325F657874656E646564_msg_allowed
	  (queue, requested))
	{
	  __436F64657252436C69626D696E677733325F657874656E646564_msg_unlock (0);
	  SetLastError (initErrCode);
	  _set_errno (EACCES);
	  return -1;
	}
      msqid = (int) (queue->seq * MSG_QUEUES + (queue - table->queues));
      __436F64657252436C69626D696E677733325F657874656E646564_msg_unlock (0);
      SetLastError (initErrCode);
      return msqid;
    }

  if (__key != IPC_PRIVATE && !(__msgflg & IPC_CREAT))
    {
      __436F64657252436C69626D696E677733325F657874656E646564_msg_unlock (0);
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
      __436F64657252436C69626D696E677733325F657874656E646564_msg_unlock (0);
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
  __436F64657252436C69626D696E677733325F657874656E646564_msg_unlock (1);
  SetLastError (initErrCode);
  return msqid;
}
