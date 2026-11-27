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

#define MSG_COUNT __436F64657252436C69626D696E677733325F657874656E646564_MSG_COUNT
#define MSG_HEADER __436F64657252436C69626D696E677733325F657874656E646564_MSG_HEADER

int
msgsnd (int __msqid, const void *__msgp, size_t __msgsz, int __msgflg)
{
  DWORD initErrCode = GetLastError ();
  msg_table *table;
  msg_queue *queue;
  long mtype;
  int record_type;
  unsigned int record_size;
  int error = 0;

  if (!__msgp)
    {
      _set_errno (EFAULT);
      return -1;
    }
  if (__msgsz > MSGMAX)
    {
      _set_errno (EINVAL);
      return -1;
    }
  memcpy (&mtype, __msgp, sizeof (mtype));
  if (mtype < 1)
    {
      _set_errno (EINVAL);
      return -1;
    }

  table = __436F64657252436C69626D696E677733325F657874656E646564_msg_lock ();
  if (!table)
    {
      SetLastError (initErrCode);
      return -1;
    }

  queue =
    __436F64657252436C69626D696E677733325F657874656E646564_msg_find (table,
								     __msqid);
  if (!queue)
    {
      error = EINVAL;
    }
  else if (!__436F64657252436C69626D696E677733325F657874656E646564_msg_allowed
	   (queue, 2))
    {
      error = EACCES;
    }

  while (!error)
    {
      if (__msgsz > queue->qbytes)
	{
	  /* Could never fit, even on an empty queue */
	  error = EINVAL;
	  break;
	}
      if (queue->cbytes + __msgsz <= queue->qbytes && queue->qnum < MSG_COUNT)
	{
	  break;
	}
      if (__msgflg & IPC_NOWAIT)
	{
	  error = EAGAIN;
	  break;
	}

      table =
	__436F64657252436C69626D696E677733325F657874656E646564_msg_wait ();
      if (!table)
	{
	  SetLastError (initErrCode);
	  return -1;
	}
      queue =
	__436F64657252436C69626D696E677733325F657874656E646564_msg_find
	(table, __msqid);
      if (!queue)
	{
	  error = EIDRM;
	}
    }

  if (error)
    {
      __436F64657252436C69626D696E677733325F657874656E646564_msg_unlock (0);
      SetLastError (initErrCode);
      _set_errno (error);
      return -1;
    }

  /* Append: int mtype, unsigned int size, then the text */
  record_type = (int) mtype;
  record_size = (unsigned int) __msgsz;
  memcpy (queue->data + queue->used, &record_type, sizeof (record_type));
  memcpy (queue->data + queue->used + sizeof (record_type), &record_size,
	  sizeof (record_size));
  memcpy (queue->data + queue->used + MSG_HEADER,
	  (const char *) __msgp + sizeof (long), __msgsz);
  queue->used += MSG_HEADER + record_size;
  queue->cbytes += record_size;
  queue->qnum++;
  queue->lspid = (unsigned int) GetCurrentProcessId ();
  queue->stime = (long long) time (NULL);

  __436F64657252436C69626D696E677733325F657874656E646564_msg_unlock (1);
  SetLastError (initErrCode);
  return 0;
}
