#include <sys/mman.h>
#include <dlfcn.h>
#include <pthread.h>
#include <stdio.h>
#include <windows.h>
#include <psapi.h>

typedef struct _KERNEL32_WIN32_MEMORY_RANGE_ENTRY
{
  PVOID VirtualAddress;
  SIZE_T NumberOfBytes;
} KERNEL32_WIN32_MEMORY_RANGE_ENTRY, *PKERNEL32_WIN32_MEMORY_RANGE_ENTRY;

typedef struct madvise_D_PrefetchVirtualMemory
{
  PKERNEL32_WIN32_MEMORY_RANGE_ENTRY VirtualAddresses;
  ULONG_PTR NumberOfEntries;
} madvise_D_PrefetchVirtualMemory;

static int
madvise_PrefetchVirtualMemory (madvise_D_PrefetchVirtualMemory *
			       madvise_d_PrefetchVirtualMemory)
{
  const size_t memoryScanStrideMultipliers[] = { 1024, 16, 1 };
  size_t i;
  size_t j;
  size_t k;
  DWORD dwErrCode = GetLastError ();
  void *Kernel32 = dlopen ("kernel32.dll", RTLD_NOW);
  BOOL (*Kernel32PrefetchVirtualMemory) (HANDLE hProcess,
					 ULONG_PTR NumberOfEntries,
					 PKERNEL32_WIN32_MEMORY_RANGE_ENTRY
					 VirtualAddresses, ULONG Flags) =
    dlsym (Kernel32, "PrefetchVirtualMemory");
  DWORD lpFilenameSize;
  DWORD nSize;
  LPSTR lpFilename;
  DWORD lpFilenameSize1;
  DWORD nSize1;
  LPSTR lpFilename1;
  DWORD lpFilenameSize2;
  DWORD nSize2;
  LPSTR lpFilename2;
  const char *root = "\\\\?\\GLOBALROOT";
  MEMORY_BASIC_INFORMATION lpBuffer;
  size_t baseaddr;
  FILE *fp;
  fpos_t fpos;
  void *readMemLoc;
  size_t readMemSize;
  size_t readMemMaxSize;
  size_t lpos;
  size_t k_size;
  if (Kernel32PrefetchVirtualMemory)
    {
      for (i = 0;
	   i !=
	   sizeof (memoryScanStrideMultipliers) /
	   sizeof (*memoryScanStrideMultipliers); i++)
	{
	  for (j = 0; j != 5; j++)
	    {
	      for (k = 0;
		   k !=
		   madvise_d_PrefetchVirtualMemory->NumberOfEntries /
		   memoryScanStrideMultipliers[i]; k++)
		{
		  Kernel32PrefetchVirtualMemory (GetCurrentProcess (),
						 memoryScanStrideMultipliers
						 [i],
						 madvise_d_PrefetchVirtualMemory->VirtualAddresses
						 +
						 k *
						 memoryScanStrideMultipliers
						 [i], 0);
		}
	      if (madvise_d_PrefetchVirtualMemory->NumberOfEntries %
		  memoryScanStrideMultipliers[i])
		{
		  Kernel32PrefetchVirtualMemory (GetCurrentProcess (),
						 madvise_d_PrefetchVirtualMemory->NumberOfEntries
						 %
						 memoryScanStrideMultipliers
						 [i],
						 madvise_d_PrefetchVirtualMemory->VirtualAddresses
						 +
						 k *
						 memoryScanStrideMultipliers
						 [i], 0);
		}
	    }
	}
      dlclose (Kernel32);
    }
  else
    {
      lpFilenameSize = strlen (root) / sizeof (*lpFilename) + 2;
      nSize = lpFilenameSize;
      lpFilename = malloc (nSize);
      lpFilenameSize2 = lpFilenameSize;
      nSize2 = lpFilenameSize;
      lpFilename2 = malloc (nSize2);
      VirtualQuery (madvise_d_PrefetchVirtualMemory->VirtualAddresses->
		    VirtualAddress, &lpBuffer, sizeof (lpBuffer));
      SetLastError (ERROR_SUCCESS);
      lpFilename += strlen (root) / sizeof (*lpFilename);
      lpFilenameSize =
	GetMappedFileName (GetCurrentProcess (),
			   (LPVOID) lpBuffer.BaseAddress, lpFilename,
			   nSize - strlen (root) - 1) + strlen (root) + 1;
      while (GetLastError () == ERROR_INSUFFICIENT_BUFFER)
	{
	  nSize += 4096;
	  lpFilename -= strlen (root) / sizeof (*lpFilename);
	  free (lpFilename);
	  lpFilename = malloc (nSize);
	  if (lpFilename == 0)
	    {
	      return 0;
	    }
	  lpFilename += strlen (root) / sizeof (*lpFilename);
	  lpFilenameSize =
	    GetMappedFileName (GetCurrentProcess (),
			       (LPVOID) lpBuffer.BaseAddress, lpFilename,
			       nSize - strlen (root) - 1) + strlen (root) + 1;
	}
      if (GetLastError () != ERROR_SUCCESS)
	{
	  return 0;
	}
      lpFilename -= strlen (root) / sizeof (*lpFilename);
      memcpy (lpFilename, root, strlen (root));
      baseaddr = (size_t) lpBuffer.BaseAddress;
      fp = fopen (lpFilename, "rb");
      nSize1 = nSize;
      lpFilename1 = lpFilename;
      lpFilenameSize1 = lpFilenameSize;
      for (i = 0;
	   i !=
	   sizeof (memoryScanStrideMultipliers) /
	   sizeof (*memoryScanStrideMultipliers); i++)
	{
	  readMemSize =
	    madvise_d_PrefetchVirtualMemory->VirtualAddresses->NumberOfBytes *
	    memoryScanStrideMultipliers[i];
	  readMemLoc = malloc (readMemSize);
	  if (readMemLoc)
	    {
	      readMemMaxSize = readMemSize;
	      for (j = 0;
		   j !=
		   madvise_d_PrefetchVirtualMemory->NumberOfEntries /
		   memoryScanStrideMultipliers[i]; j++)
		{
		  lpos =
		    (size_t) madvise_d_PrefetchVirtualMemory->
		    VirtualAddresses[j *
				     memoryScanStrideMultipliers[i]].
		    VirtualAddress;
		  readMemSize = 0;
		  for (k = 0; k != memoryScanStrideMultipliers[i]; k++)
		    {
		      if (readMemSize > readMemMaxSize)
			{
			  fpos = lpos - baseaddr;
			  fsetpos (fp, &fpos);
			  fread (readMemLoc, 1, readMemMaxSize, fp);
			  lpos += readMemMaxSize;
			  readMemSize = readMemMaxSize - readMemSize;
			}
		      readMemSize +=
			(size_t) madvise_d_PrefetchVirtualMemory->
			VirtualAddresses[j * memoryScanStrideMultipliers[i] +
					 k].NumberOfBytes;
		      if (k + 1 != memoryScanStrideMultipliers[i]
			  && (lpos + readMemSize !=
			      ((size_t) madvise_d_PrefetchVirtualMemory->
			       VirtualAddresses[j *
						memoryScanStrideMultipliers[i]
						+ k + 1].VirtualAddress)
			      || lpos + readMemSize >
			      baseaddr + lpBuffer.RegionSize))
			{
			  fpos = lpos - baseaddr;
			  fsetpos (fp, &fpos);
			  fread (readMemLoc, 1, readMemSize, fp);
			  lpos =
			    (size_t) madvise_d_PrefetchVirtualMemory->
			    VirtualAddresses[j *
					     memoryScanStrideMultipliers[i] +
					     k + 1].VirtualAddress;
			  readMemSize = 0;
			  nSize = nSize2;
			  lpFilename = lpFilename2;
			  VirtualQuery ((void *) lpos, &lpBuffer,
					sizeof (lpBuffer));
			  SetLastError (ERROR_SUCCESS);
			  lpFilename += strlen (root) / sizeof (*lpFilename);
			  lpFilenameSize =
			    GetMappedFileName (GetCurrentProcess (),
					       (LPVOID) lpBuffer.BaseAddress,
					       lpFilename,
					       nSize - strlen (root) - 1) +
			    strlen (root) + 1;
			  while (GetLastError () == ERROR_INSUFFICIENT_BUFFER)
			    {
			      nSize += 4096;
			      lpFilename -=
				strlen (root) / sizeof (*lpFilename);
			      free (lpFilename);
			      lpFilename = malloc (nSize);
			      if (lpFilename == 0)
				{
				  return 0;
				}
			      lpFilename +=
				strlen (root) / sizeof (*lpFilename);
			      lpFilenameSize =
				GetMappedFileName (GetCurrentProcess (),
						   (LPVOID) lpBuffer.
						   BaseAddress, lpFilename,
						   nSize - strlen (root) -
						   1) + strlen (root) + 1;
			    }
			  if (GetLastError () != ERROR_SUCCESS)
			    {
			      return 0;
			    }
			  lpFilename -= strlen (root) / sizeof (*lpFilename);
			  memcpy (lpFilename, root, strlen (root));
			  lpFilename[lpFilenameSize - 1] = 0;
			  if (lpFilenameSize != lpFilenameSize1
			      || strncmp (lpFilename, lpFilename1,
					  lpFilenameSize))
			    {
			      baseaddr = (size_t) lpBuffer.BaseAddress;
			      fclose (fp);
			      fp = fopen (lpFilename, "rb");
			      nSize2 = nSize1;
			      lpFilename2 = lpFilename1;
			      lpFilenameSize2 = lpFilenameSize1;
			      nSize1 = nSize;
			      lpFilename1 = lpFilename;
			      lpFilenameSize1 = lpFilenameSize;
			      nSize = nSize2;
			      lpFilename = lpFilename2;
			      lpFilenameSize = lpFilenameSize2;
			    }
			  nSize2 = nSize;
			  lpFilename2 = lpFilename;
			  lpFilenameSize2 = lpFilenameSize;
			}
		    }
		  fpos = lpos - baseaddr;
		  fsetpos (fp, &fpos);
		  fread (readMemLoc, 1, readMemSize, fp);
		}
	      if (madvise_d_PrefetchVirtualMemory->NumberOfEntries %
		  memoryScanStrideMultipliers[i])
		{
		  lpos =
		    (size_t) madvise_d_PrefetchVirtualMemory->
		    VirtualAddresses[j *
				     memoryScanStrideMultipliers[i]].
		    VirtualAddress;
		  readMemSize = 0;
		  k_size =
		    madvise_d_PrefetchVirtualMemory->NumberOfEntries %
		    memoryScanStrideMultipliers[i];
		  for (k = 0; k != k_size; k++)
		    {
		      if (readMemSize > readMemMaxSize)
			{
			  fpos = lpos - baseaddr;
			  fsetpos (fp, &fpos);
			  fread (readMemLoc, 1, readMemMaxSize, fp);
			  lpos += readMemMaxSize;
			  readMemSize = readMemMaxSize - readMemSize;
			}
		      readMemSize +=
			(size_t) madvise_d_PrefetchVirtualMemory->
			VirtualAddresses[j * memoryScanStrideMultipliers[i] +
					 k].NumberOfBytes;
		      if (k + 1 != k_size
			  && (lpos + readMemSize !=
			      ((size_t) madvise_d_PrefetchVirtualMemory->
			       VirtualAddresses[j *
						memoryScanStrideMultipliers[i]
						+ k + 1].VirtualAddress)
			      || lpos + readMemSize >
			      baseaddr + lpBuffer.RegionSize))
			{
			  fpos = lpos - baseaddr;
			  fsetpos (fp, &fpos);
			  fread (readMemLoc, 1, readMemSize, fp);
			  lpos =
			    (size_t) madvise_d_PrefetchVirtualMemory->
			    VirtualAddresses[j *
					     memoryScanStrideMultipliers[i] +
					     k + 1].VirtualAddress;
			  readMemSize = 0;
			  nSize = nSize2;
			  lpFilename = lpFilename2;
			  VirtualQuery ((void *) lpos, &lpBuffer,
					sizeof (lpBuffer));
			  SetLastError (ERROR_SUCCESS);
			  lpFilename += strlen (root) / sizeof (*lpFilename);
			  lpFilenameSize =
			    GetMappedFileName (GetCurrentProcess (),
					       (LPVOID) lpBuffer.BaseAddress,
					       lpFilename,
					       nSize - strlen (root) - 1) +
			    strlen (root) + 1;
			  while (GetLastError () == ERROR_INSUFFICIENT_BUFFER)
			    {
			      nSize += 4096;
			      lpFilename -=
				strlen (root) / sizeof (*lpFilename);
			      free (lpFilename);
			      lpFilename = malloc (nSize);
			      if (lpFilename == 0)
				{
				  return 0;
				}
			      lpFilename +=
				strlen (root) / sizeof (*lpFilename);
			      lpFilenameSize =
				GetMappedFileName (GetCurrentProcess (),
						   (LPVOID) lpBuffer.
						   BaseAddress, lpFilename,
						   nSize - strlen (root) -
						   1) + strlen (root) + 1;
			    }
			  if (GetLastError () != ERROR_SUCCESS)
			    {
			      return 0;
			    }
			  lpFilename -= strlen (root) / sizeof (*lpFilename);
			  memcpy (lpFilename, root, strlen (root));
			  lpFilename[lpFilenameSize - 1] = 0;
			  if (lpFilenameSize != lpFilenameSize1
			      || strncmp (lpFilename, lpFilename1,
					  lpFilenameSize))
			    {
			      baseaddr = (size_t) lpBuffer.BaseAddress;
			      fclose (fp);
			      fp = fopen (lpFilename, "rb");
			      nSize2 = nSize1;
			      lpFilename2 = lpFilename1;
			      lpFilenameSize2 = lpFilenameSize1;
			      nSize1 = nSize;
			      lpFilename1 = lpFilename;
			      lpFilenameSize1 = lpFilenameSize;
			      nSize = nSize2;
			      lpFilename = lpFilename2;
			      lpFilenameSize = lpFilenameSize2;
			    }
			  nSize2 = nSize;
			  lpFilename2 = lpFilename;
			  lpFilenameSize2 = lpFilenameSize;
			}
		    }
		  fpos = lpos - baseaddr;
		  fsetpos (fp, &fpos);
		  fread (readMemLoc, 1, readMemSize, fp);
		}
	      free (readMemLoc);
	      free (lpFilename1);
	      free (lpFilename2);
	      i =
		sizeof (memoryScanStrideMultipliers) /
		sizeof (*memoryScanStrideMultipliers) - 1;
	    }
	}
    }
  munmap (madvise_d_PrefetchVirtualMemory->VirtualAddresses,
	  madvise_d_PrefetchVirtualMemory->NumberOfEntries *
	  sizeof (KERNEL32_WIN32_MEMORY_RANGE_ENTRY));
  munmap (madvise_d_PrefetchVirtualMemory,
	  sizeof (madvise_D_PrefetchVirtualMemory));
  SetLastError (dwErrCode);
  return 1;
}

int
madvise (void *__addr, size_t __len, int __advice)
{
  LPCVOID lpAddress;
  MEMORY_BASIC_INFORMATION lpBuffer;
  size_t sizeLeft;
  ULONG_PTR NumberOfEntries;
  pthread_t threadId;
  PKERNEL32_WIN32_MEMORY_RANGE_ENTRY VirtualAddresses;
  madvise_D_PrefetchVirtualMemory *madvise_d_PrefetchVirtualMemory;
  size_t i;
  const size_t memoryScanStride = (1 << 20);
  if (__advice == MADV_WILLNEED)
    {
      lpAddress = __addr;
      sizeLeft = __len;
      if (VirtualQuery
	  (lpAddress, &lpBuffer, sizeof (MEMORY_BASIC_INFORMATION)))
	{
	  if ((size_t) __addr - (size_t) lpBuffer.BaseAddress +
	      lpBuffer.RegionSize < sizeLeft)
	    {
	      NumberOfEntries =
		((size_t) __addr - (size_t) lpBuffer.BaseAddress +
		 lpBuffer.RegionSize) / memoryScanStride;
	      NumberOfEntries +=
		((size_t) __addr - (size_t) lpBuffer.BaseAddress +
		 lpBuffer.RegionSize) % memoryScanStride != 0;
	      lpAddress =
		(void *) ((size_t) lpBuffer.BaseAddress +
			  lpBuffer.RegionSize);
	      sizeLeft -=
		(size_t) __addr - (size_t) lpBuffer.BaseAddress +
		lpBuffer.RegionSize;
	      while (sizeLeft
		     && VirtualQuery (lpAddress, &lpBuffer,
				      sizeof (MEMORY_BASIC_INFORMATION)))
		{
		  if (lpBuffer.State == MEM_COMMIT)
		    {
		      if (lpBuffer.RegionSize < sizeLeft)
			{
			  NumberOfEntries +=
			    lpBuffer.RegionSize / memoryScanStride;
			  NumberOfEntries +=
			    lpBuffer.RegionSize % memoryScanStride != 0;
			}
		      else
			{
			  NumberOfEntries += sizeLeft / memoryScanStride;
			  NumberOfEntries += sizeLeft % memoryScanStride != 0;
			}
		    }
		  if (lpBuffer.RegionSize < sizeLeft)
		    {
		      lpAddress =
			(void *) ((size_t) lpAddress + lpBuffer.RegionSize);
		      sizeLeft -= lpBuffer.RegionSize;
		    }
		  else
		    {
		      sizeLeft = 0;
		    }
		}
	    }
	  else
	    {
	      NumberOfEntries = sizeLeft / memoryScanStride;
	      NumberOfEntries += sizeLeft % memoryScanStride != 0;
	    }
	  madvise_d_PrefetchVirtualMemory =
	    (madvise_D_PrefetchVirtualMemory *) mmap (NULL,
						      sizeof
						      (madvise_D_PrefetchVirtualMemory),
						      PROT_READ | PROT_WRITE,
						      MAP_PRIVATE |
						      MAP_ANONYMOUS, -1, 0);
	  if (madvise_d_PrefetchVirtualMemory != MAP_FAILED)
	    {
	      VirtualAddresses =
		(PKERNEL32_WIN32_MEMORY_RANGE_ENTRY) mmap (NULL,
							   NumberOfEntries *
							   sizeof
							   (KERNEL32_WIN32_MEMORY_RANGE_ENTRY),
							   PROT_READ |
							   PROT_WRITE,
							   MAP_PRIVATE |
							   MAP_ANONYMOUS, -1,
							   0);
	      if (VirtualAddresses != MAP_FAILED)
		{
		  lpAddress = __addr;
		  sizeLeft = __len;
		  if (VirtualQuery
		      (lpAddress, &lpBuffer,
		       sizeof (MEMORY_BASIC_INFORMATION)))
		    {
		      NumberOfEntries = 0;
		      if ((size_t) __addr - (size_t) lpBuffer.BaseAddress +
			  lpBuffer.RegionSize < sizeLeft)
			{
			  for (i = 0;
			       i !=
			       ((size_t) __addr -
				(size_t) lpBuffer.BaseAddress +
				lpBuffer.RegionSize) / memoryScanStride; i++)
			    {
			      VirtualAddresses[NumberOfEntries].VirtualAddress
				=
				(void *) ((size_t) __addr +
					  i * memoryScanStride);
			      VirtualAddresses[NumberOfEntries].NumberOfBytes
				= memoryScanStride;
			      NumberOfEntries++;
			    }
			  if (((size_t) __addr -
			       (size_t) lpBuffer.BaseAddress +
			       lpBuffer.RegionSize) % memoryScanStride != 0)
			    {
			      VirtualAddresses[NumberOfEntries].VirtualAddress
				=
				(void *) ((size_t) __addr +
					  i * memoryScanStride);
			      VirtualAddresses[NumberOfEntries].NumberOfBytes
				=
				((size_t) __addr -
				 (size_t) lpBuffer.BaseAddress +
				 lpBuffer.RegionSize) % memoryScanStride;
			      NumberOfEntries++;
			    }
			  lpAddress =
			    (void *) ((size_t) lpBuffer.BaseAddress +
				      lpBuffer.RegionSize);
			  sizeLeft -=
			    (size_t) __addr - (size_t) lpBuffer.BaseAddress +
			    lpBuffer.RegionSize;
			  while (sizeLeft
				 && VirtualQuery (lpAddress, &lpBuffer,
						  sizeof
						  (MEMORY_BASIC_INFORMATION)))
			    {
			      if (lpBuffer.State == MEM_COMMIT
				  && lpBuffer.BaseAddress !=
				  madvise_d_PrefetchVirtualMemory
				  && lpBuffer.BaseAddress != VirtualAddresses)
				{
				  if (lpBuffer.RegionSize < sizeLeft)
				    {
				      for (i = 0;
					   i !=
					   lpBuffer.RegionSize /
					   memoryScanStride; i++)
					{
					  VirtualAddresses
					    [NumberOfEntries].VirtualAddress =
					    (void *) ((size_t)
						      lpBuffer.BaseAddress +
						      i * memoryScanStride);
					  VirtualAddresses
					    [NumberOfEntries].NumberOfBytes =
					    memoryScanStride;
					  NumberOfEntries++;
					}
				      if (lpBuffer.RegionSize %
					  memoryScanStride != 0)
					{
					  VirtualAddresses
					    [NumberOfEntries].VirtualAddress =
					    (void *) ((size_t)
						      lpBuffer.BaseAddress +
						      i * memoryScanStride);
					  VirtualAddresses
					    [NumberOfEntries].NumberOfBytes =
					    lpBuffer.RegionSize %
					    memoryScanStride;
					  NumberOfEntries++;
					}
				    }
				  else
				    {
				      for (i = 0;
					   i != sizeLeft / memoryScanStride;
					   i++)
					{
					  VirtualAddresses
					    [NumberOfEntries].VirtualAddress =
					    (void *) ((size_t) __addr +
						      i * memoryScanStride);
					  VirtualAddresses
					    [NumberOfEntries].NumberOfBytes =
					    memoryScanStride;
					  NumberOfEntries++;
					}
				      if (sizeLeft % memoryScanStride != 0)
					{
					  VirtualAddresses
					    [NumberOfEntries].VirtualAddress =
					    (void *) ((size_t) __addr +
						      i * memoryScanStride);
					  VirtualAddresses
					    [NumberOfEntries].NumberOfBytes =
					    ((size_t) __addr -
					     (size_t) lpBuffer.BaseAddress +
					     lpBuffer.RegionSize) %
					    memoryScanStride;
					  NumberOfEntries++;
					}
				    }
				}
			      if (lpBuffer.RegionSize < sizeLeft)
				{
				  lpAddress =
				    (void *) ((size_t) lpAddress +
					      lpBuffer.RegionSize);
				  sizeLeft -= lpBuffer.RegionSize;
				}
			      else
				{
				  sizeLeft = 0;
				}
			    }
			}
		      else
			{
			  for (i = 0; i != sizeLeft / memoryScanStride; i++)
			    {
			      VirtualAddresses[NumberOfEntries].VirtualAddress
				=
				(void *) ((size_t) __addr +
					  i * memoryScanStride);
			      VirtualAddresses[NumberOfEntries].NumberOfBytes
				= memoryScanStride;
			      NumberOfEntries++;
			    }
			  if (sizeLeft % memoryScanStride != 0)
			    {
			      VirtualAddresses[NumberOfEntries].VirtualAddress
				=
				(void *) ((size_t) __addr +
					  i * memoryScanStride);
			      VirtualAddresses[NumberOfEntries].NumberOfBytes
				=
				((size_t) __addr -
				 (size_t) lpBuffer.BaseAddress +
				 lpBuffer.RegionSize) % memoryScanStride;
			      NumberOfEntries++;
			    }
			}
		      madvise_d_PrefetchVirtualMemory->VirtualAddresses =
			VirtualAddresses;
		      madvise_d_PrefetchVirtualMemory->NumberOfEntries =
			NumberOfEntries;
		      if (pthread_create (&threadId, 0,
					  (void
					   *(*)(void
						*))
					  (&madvise_PrefetchVirtualMemory),
					  madvise_d_PrefetchVirtualMemory))
			{
			  munmap (madvise_d_PrefetchVirtualMemory,
				  sizeof (madvise_D_PrefetchVirtualMemory));
			  munmap (VirtualAddresses,
				  NumberOfEntries *
				  sizeof (KERNEL32_WIN32_MEMORY_RANGE_ENTRY));
			}
		      else
			{
			  pthread_detach (threadId);
			}
		    }
		  else
		    {
		      munmap (madvise_d_PrefetchVirtualMemory,
			      sizeof (madvise_D_PrefetchVirtualMemory));
		      munmap (VirtualAddresses,
			      NumberOfEntries *
			      sizeof (KERNEL32_WIN32_MEMORY_RANGE_ENTRY));
		    }
		}
	      else
		{
		  munmap (madvise_d_PrefetchVirtualMemory,
			  sizeof (madvise_D_PrefetchVirtualMemory));
		}
	    }
	}
    }
  return 0;
}
