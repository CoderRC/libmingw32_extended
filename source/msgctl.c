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
  else if (__cmd == IPC_STAT)
    {
      if (!__436F64657252436C69626D696E677733325F657874656E646564_msg_allowed
	  (queue, 4))
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
	  else if (__buf->msg_qbytes > queue->qbytes
		   &&
		   !__436F64657252436C69626D696E677733325F657874656E646564_msg_privileged
		   ())
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

  __436F64657252436C69626D696E677733325F657874656E646564_msg_unlock (changed);
  SetLastError (initErrCode);
  if (error)
    {
      _set_errno (error);
      return -1;
    }
  return 0;
}
