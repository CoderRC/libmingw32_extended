#include <sys/mman.h>
#include <pthread.h>
#ifdef _WIN32_WINNT
#undef _WIN32_WINNT
#endif
#define _WIN32_WINNT _WIN32_WINNT_WIN8
#include <windows.h>

typedef struct madvise_D_PrefetchVirtualMemory
{
  PWIN32_MEMORY_RANGE_ENTRY VirtualAddresses;
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
	      PrefetchVirtualMemory (GetCurrentProcess (),
				     memoryScanStrideMultipliers[i],
				     madvise_d_PrefetchVirtualMemory->
				     VirtualAddresses +
				     k * memoryScanStrideMultipliers[i], 0);
	    }
	  if (madvise_d_PrefetchVirtualMemory->NumberOfEntries %
	      memoryScanStrideMultipliers[i])
	    {
	      PrefetchVirtualMemory (GetCurrentProcess (),
				     madvise_d_PrefetchVirtualMemory->
				     NumberOfEntries %
				     memoryScanStrideMultipliers[i],
				     madvise_d_PrefetchVirtualMemory->
				     VirtualAddresses +
				     k * memoryScanStrideMultipliers[i], 0);
	    }
	}
    }
  munmap (madvise_d_PrefetchVirtualMemory->VirtualAddresses,
	  madvise_d_PrefetchVirtualMemory->NumberOfEntries *
	  sizeof (WIN32_MEMORY_RANGE_ENTRY));
  munmap (madvise_d_PrefetchVirtualMemory,
	  sizeof (madvise_D_PrefetchVirtualMemory));
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
  PWIN32_MEMORY_RANGE_ENTRY VirtualAddresses;
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
		(PWIN32_MEMORY_RANGE_ENTRY) mmap (NULL,
						  NumberOfEntries *
						  sizeof
						  (WIN32_MEMORY_RANGE_ENTRY),
						  PROT_READ | PROT_WRITE,
						  MAP_PRIVATE | MAP_ANONYMOUS,
						  -1, 0);
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
					  VirtualAddresses[NumberOfEntries].
					    VirtualAddress =
					    (void *) ((size_t) __addr +
						      i * memoryScanStride);
					  VirtualAddresses[NumberOfEntries].
					    NumberOfBytes = memoryScanStride;
					  NumberOfEntries++;
					}
				      if (sizeLeft % memoryScanStride != 0)
					{
					  VirtualAddresses[NumberOfEntries].
					    VirtualAddress =
					    (void *) ((size_t) __addr +
						      i * memoryScanStride);
					  VirtualAddresses[NumberOfEntries].
					    NumberOfBytes =
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
				  sizeof (WIN32_MEMORY_RANGE_ENTRY));
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
			      sizeof (WIN32_MEMORY_RANGE_ENTRY));
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
