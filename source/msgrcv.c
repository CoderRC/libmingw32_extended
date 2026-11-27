#include <__436F64657252436C69626D696E677733325F657874656E646564.h>
#include <sys/msg.h>
#include <windows.h>
#include <errno.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

typedef struct __436F64657252436C69626D696E677733325F657874656E646564_msg_table
  msg_table;
typedef struct __436F64657252436C69626D696E677733325F657874656E646564_msg_queue
  msg_queue;

#define MSG_HEADER __436F64657252436C69626D696E677733325F657874656E646564_MSG_HEADER

/* Offset of the message msgtyp selects, or -1 when there is none */
static long
find_message (const msg_queue * queue, long msgtyp)
{
  unsigned int offset = 0;
  long found = -1;
  int lowest = 0;

  while (offset < queue->used)
    {
      int type;
      unsigned int size;
      memcpy (&type, queue->data + offset, sizeof (type));
      memcpy (&size, queue->data + offset + sizeof (type), sizeof (size));

      if (msgtyp == 0 || type == msgtyp)
	{
	  /* First message, or first of exactly that type */
	  return (long) offset;
	}
      if (msgtyp < 0 && (long) type <= -msgtyp
	  && (found < 0 || type < lowest))
	{
	  /* First message of the lowest type <= |msgtyp| */
	  found = (long) offset;
	  lowest = type;
	}
      offset += MSG_HEADER + size;
    }
  return found;
}

ssize_t
msgrcv (int __msqid, void *__msgp, size_t __msgsz, long __msgtyp,
	int __msgflg)
{
  DWORD initErrCode = GetLastError ();
  msg_table *table;
  msg_queue *queue;
  long offset = -1;
  int type;
  unsigned int size;
  size_t copied;
  long mtype;
  int error = 0;

  if (!__msgp)
    {
      _set_errno (EFAULT);
      return -1;
    }
  if (__msgsz > PTRDIFF_MAX)
    {
      _set_errno (EINVAL);
      return -1;
    }
  /* -LONG_MIN overflows; it selects the same messages as -LONG_MAX */
  if (__msgtyp < -0x7FFFFFFFL)
    {
      __msgtyp = -0x7FFFFFFFL;
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
	   (queue, 4))
    {
      error = EACCES;
    }

  while (!error)
    {
      offset = find_message (queue, __msgtyp);
      if (offset >= 0)
	{
	  break;
	}
      if (__msgflg & IPC_NOWAIT)
	{
	  error = ENOMSG;
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

  if (!error)
    {
      memcpy (&type, queue->data + offset, sizeof (type));
      memcpy (&size, queue->data + offset + sizeof (type), sizeof (size));
      if (size > __msgsz && !(__msgflg & MSG_NOERROR))
	{
	  /* Message stays on the queue */
	  error = E2BIG;
	}
    }

  if (error)
    {
      __436F64657252436C69626D696E677733325F657874656E646564_msg_unlock (0);
      SetLastError (initErrCode);
      _set_errno (error);
      return -1;
    }

  copied = size < __msgsz ? size : __msgsz;
  mtype = type;
  memcpy (__msgp, &mtype, sizeof (mtype));
  memcpy ((char *) __msgp + sizeof (long),
	  queue->data + offset + MSG_HEADER, copied);

  /* Remove the record and close the gap */
  memmove (queue->data + offset,
	   queue->data + offset + MSG_HEADER + size,
	   queue->used - (offset + MSG_HEADER + size));
  queue->used -= MSG_HEADER + size;
  queue->cbytes -= size;
  queue->qnum--;
  queue->lrpid = (unsigned int) GetCurrentProcessId ();
  queue->rtime = (long long) time (NULL);

  __436F64657252436C69626D696E677733325F657874656E646564_msg_unlock (1);
  SetLastError (initErrCode);
  return (ssize_t) copied;
}
