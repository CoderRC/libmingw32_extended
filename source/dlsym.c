#include <dlfcn.h>
#include <windows.h>

static PIMAGE_NT_HEADERS
ImageNtHeader (PVOID Base)
{
  PIMAGE_DOS_HEADER imageDosHeader = Base;
  return (PIMAGE_NT_HEADERS) ((char *) (imageDosHeader) +
			      imageDosHeader->e_lfanew);
}

void *
dlsym (void *__restrict __handle, const char *__restrict __name)
{
  PIMAGE_NT_HEADERS imageNtHeaders = ImageNtHeader (__handle);
  MEMORY_BASIC_INFORMATION mbi;
  VirtualQuery ((char *) (imageNtHeaders) +
		imageNtHeaders->OptionalHeader.
		DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress,
		&mbi, sizeof (mbi));
  if (mbi.Protect & PAGE_WRITECOPY)
    {
      PIMAGE_IMPORT_DESCRIPTOR imageImportDescriptor;
      imageImportDescriptor =
	(PIMAGE_IMPORT_DESCRIPTOR) ((char *) (__handle) +
				    imageNtHeaders->OptionalHeader.
				    DataDirectory
				    [IMAGE_DIRECTORY_ENTRY_IMPORT].
				    VirtualAddress);
      LPCSTR libFileName;
      HMODULE hModule;
      LPCSTR procName;
      PIMAGE_THUNK_DATA imageThunkData;
      while (imageImportDescriptor->Name != 0)
	{
	  libFileName = (char *) __handle + imageImportDescriptor->Name;
	  hModule = LoadLibraryA (libFileName);
	  if (hModule)
	    {
	      imageThunkData =
		(PIMAGE_THUNK_DATA) ((char *) (__handle) +
				     imageImportDescriptor->FirstThunk);
	      while (imageThunkData->u1.AddressOfData != 0)
		{
		  if (IMAGE_SNAP_BY_ORDINAL (imageThunkData->u1.Ordinal))
		    {
		      procName =
			(LPCSTR) IMAGE_ORDINAL (imageThunkData->u1.Ordinal);
		    }
		  else
		    {
		      procName =
			((PIMAGE_IMPORT_BY_NAME)
			 ((char *) (__handle) +
			  imageThunkData->u1.AddressOfData))->Name;
		    }
		  imageThunkData->u1.Function =
		    (DWORD_PTR) GetProcAddress (hModule, procName);
		  ++imageThunkData;
		}
	    }
	  imageImportDescriptor++;
	}
      BOOL WINAPI (*DllEntry) (HINSTANCE hinstDLL, DWORD fdwReason,
			       LPVOID lpvReserved) =
	(BOOL
	 WINAPI (*)(HINSTANCE hinstDLL, DWORD fdwReason,
		    LPVOID lpvReserved)) ((char *) (__handle) +
					  imageNtHeaders->OptionalHeader.
					  AddressOfEntryPoint);
      (*DllEntry) ((HINSTANCE) __handle, DLL_PROCESS_ATTACH, 0);
    }
  return GetProcAddress (__handle, __name);
}
