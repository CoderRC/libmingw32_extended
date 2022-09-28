#include <sys/uio.h>
#include <windows.h>

ssize_t
process_vm_writev (pid_t __pid, const struct iovec *__lvec,
		   unsigned long int __liovcnt,
		   const struct iovec *__rvec,
		   unsigned long int __riovcnt, unsigned long int __flags)
{
  size_t number_of_bytes_written = 0;
  size_t iLvec = 0;
  size_t iRvec = 0;
  size_t offLvec = 0;
  size_t offRvec = 0;
  size_t writeRvec = 0;
  HANDLE hProcess = OpenProcess (PROCESS_VM_WRITE | PROCESS_VM_OPERATION,
				 FALSE,
				 __pid);
  if (hProcess == INVALID_HANDLE_VALUE)
    {
      _set_errno (EPERM);
      return -1;
    }

  writeRvec = __lvec->iov_len;
  if (writeRvec > __rvec->iov_len)
    {
      writeRvec = __rvec->iov_len;
    }

  while (iLvec != __liovcnt && iRvec != __riovcnt && writeRvec != 0)
    {
      WriteProcessMemory (hProcess,
			  (void *) ((size_t) __rvec[iRvec].iov_base +
				    offRvec),
			  (void *) ((size_t) __lvec[iLvec].iov_base +
				    offLvec), writeRvec, NULL);
      offLvec += writeRvec;
      offRvec += writeRvec;
      number_of_bytes_written += writeRvec;
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
      writeRvec = __lvec[iLvec].iov_len - offLvec;
      if (writeRvec > __rvec[iRvec].iov_len - offRvec)
	{
	  writeRvec = __rvec[iRvec].iov_len - offRvec;
	}
    }
  CloseHandle (hProcess);
  return number_of_bytes_written;
}
