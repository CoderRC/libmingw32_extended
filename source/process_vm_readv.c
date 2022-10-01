#include <sys/uio.h>
#include <windows.h>

ssize_t
process_vm_readv (pid_t __pid, const struct iovec *__lvec,
		  unsigned long int __liovcnt,
		  const struct iovec *__rvec,
		  unsigned long int __riovcnt, unsigned long int __flags)
{
  size_t number_of_bytes_read = 0;
  size_t iLvec = 0;
  size_t iRvec = 0;
  size_t offLvec = 0;
  size_t offRvec = 0;
  size_t writeLvec = 0;
  HANDLE hProcess = OpenProcess (PROCESS_VM_READ,
				 FALSE,
				 __pid);
  if (hProcess == INVALID_HANDLE_VALUE)
    {
      _set_errno (EPERM);
      return -1;
    }

  writeLvec = __lvec->iov_len;
  if (writeLvec > __rvec->iov_len)
    {
      writeLvec = __rvec->iov_len;
    }

  while (iLvec != __liovcnt && iRvec != __riovcnt && writeLvec != 0)
    {
      ReadProcessMemory (hProcess,
			 (void *) ((size_t) __rvec[iRvec].iov_base + offRvec),
			 (void *) ((size_t) __lvec[iLvec].iov_base + offLvec),
			 writeLvec, NULL);
      offLvec += writeLvec;
      offRvec += writeLvec;
      number_of_bytes_read += writeLvec;
      if (offLvec == __lvec[iLvec].iov_len)
	{
	  offLvec = 0;
	  iLvec++;
	}
      if (offRvec == __rvec[iRvec].iov_len)
	{
	  offRvec = 0;
	  iRvec++;
	}
      writeLvec = __lvec[iLvec].iov_len - offLvec;
      if (writeLvec > __rvec[iRvec].iov_len - offRvec)
	{
	  writeLvec = __rvec[iRvec].iov_len - offRvec;
	}
    }
  CloseHandle (hProcess);
  return number_of_bytes_read;
}
