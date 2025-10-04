/* dlfcn.h
 */

#ifndef _DLFCN_H
#define _DLFCN_H

#include <sys/cdefs.h>
#include <limits.h>

__BEGIN_DECLS

/// @brief Loads a DLL module into the process's address space.
///
/// This function mimics the POSIX dlopen interface on Windows by using LoadLibraryExA.
///
/// @param __file The path to the DLL file to load.
/// @param __mode Mode flags controlling symbol resolution behavior. Supports:
/// - RTLD_LAZY: Load the DLL without resolving references immediately (maps to DONT_RESOLVE_DLL_REFERENCES).
///
/// @return Handle to the loaded module on success, or NULL on failure.
/// On failure, error details are available via dlerror().
///
/// @note If __file is NULL, sets ERROR_INVALID_PARAMETER and returns NULL.
extern void *
dlopen (const char *__file, int __mode);

/// @brief Retrieves the address of a symbol in a loaded module.
///
/// On first call for a module, this patches the module's Import Address Table
/// (IAT) if necessary, resolving imports manually if the module was loaded with
/// delayed or lazy loading.
///
/// @param __handle Handle to the loaded module.
/// @param __name Name of the symbol to locate.
///
/// @return Pointer to the symbol on success, or NULL on failure.
/// On failure, thread-local error message is set and retrievable via dlerror().
extern void *
dlsym (void *__restrict __handle, const char *__restrict __name);

/// @brief Unloads a dynamically loaded module.
///
/// Calls FreeLibrary on the given handle, releasing the loaded module.
///
/// @param __handle Handle to the module to unload (returned by dlopen).
///
/// @return 0 on success, -1 on failure.
/// On failure, the Windows last error code is set.
///
/// @note On failure, you can call dlerror() to get a textual error description.
extern int
dlclose (void *__handle);

/// @brief Returns a human-readable string describing the last error that occurred.
///
/// Retrieves the Windows last error code via GetLastError(), formats it into a
/// descriptive message, and returns a thread-local buffer with this message.
///
/// If there is no error (GetLastError returns 0), returns NULL.
///
/// @return Pointer to a thread-local static buffer containing the error message,
/// or NULL if no error.
///
/// @note Calling this function clears the Windows last error (sets it to 0).
extern char *
dlerror (void);

/**
 * @def RTLD_DEFAULT
 * POSIX macro not used in Windows, define as NULL.
 */
#define RTLD_DEFAULT NULL

/**
 * @def RTLD_LOCAL
 * The symbols defined in this library are not made available to resolve references in subsequently loaded libraries.
 */
#define RTLD_LOCAL 0

/**
 * @def RTLD_LAZY
 * Perform lazy binding. Only resolve symbols as the code that references them is executed.
 */
#define RTLD_LAZY 1

/**
 * @def RTLD_NOW
 * Perform all necessary relocations when `dlopen` is called.
 */
#define RTLD_NOW 2

/**
 * @def RTLD_GLOBAL
 * Make symbols available for symbol resolution of subsequently loaded libraries.
 */
#define RTLD_GLOBAL 4

/**
 * @def RTLD_NODELETE
 * Do not unload the library during dlclose.
 * Non-standard GLIBC extension, no effect on Windows.
 */
#define RTLD_NODELETE 8

/**
 * @def RTLD_NOLOAD
 * Do not load the library if it is not already loaded.
 * Non-standard GLIBC extension, not supported on Windows.
 */
#define RTLD_NOLOAD 16

/**
 * @def RTLD_DEEPBIND
 * Place the library at the top of the symbol resolution scope.
 * Non-standard GLIBC extension, not supported on Windows.
 */
#define RTLD_DEEPBIND 32

/**
 * @struct Dl_info
 * Structure returned by dladdr with symbolic information about a pointer.
 */
typedef struct Dl_info
{
  const char *dli_fname; /**< Filename of the shared object that contains the address */
  void *dli_fbase;	 /**< Base address at which the object is loaded */
  const char *dli_sname; /**< Name of the nearest symbol with address lower than or equal to the specified address */
  void *dli_saddr;	 /**< Exact address of the symbol named in dli_sname */
} Dl_info;

/// @brief Provides symbolic information about a given address.
///
/// If successful, info will contain the filename of the module, the base address,
/// the nearest symbol name, and the exact address of that symbol.
///
/// @param addr Pointer/address to query.
/// @param info Pointer to Dl_info struct to fill.
/// @return Non-zero on success, zero on failure.
///
/// @note The caller is responsible for freeing dli_fname and dli_sname in Dl_info.
extern int
dladdr (const void *addr, Dl_info *info);

__END_DECLS

#endif /* _DLFCN_H */
