#include <glob.h>
#include <fnmatch.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#ifdef _WIN32
#include <windows.h>
#include <io.h>
#else
#include <dirent.h>
#include <sys/stat.h>
#endif

static int add_path(glob_t *pglob, const char *path) {
    char **new_pathv;
    size_t new_size = pglob->gl_pathc + pglob->gl_offs + 2; /* +2 for new entry and NULL */
    
    new_pathv = realloc(pglob->gl_pathv, new_size * sizeof(char*));
    if (!new_pathv) return GLOB_NOSPACE;
    
    pglob->gl_pathv = new_pathv;
    
    char *path_copy = malloc(strlen(path) + 1);
    if (!path_copy) return GLOB_NOSPACE;
    strcpy(path_copy, path);
    
    pglob->gl_pathv[pglob->gl_offs + pglob->gl_pathc] = path_copy;
    pglob->gl_pathc++;
    pglob->gl_pathv[pglob->gl_offs + pglob->gl_pathc] = NULL;
    
    return 0;
}

static int glob_dir(const char *pattern, const char *dir, int flags, 
                   int (*errfunc)(const char *, int), glob_t *pglob) {
#ifdef _WIN32
    WIN32_FIND_DATA findData;
    HANDLE hFind;
    char searchPath[MAX_PATH];
    char path[MAX_PATH];
    
    snprintf(searchPath, sizeof(searchPath), "%s/*", dir ? dir : ".");
    hFind = FindFirstFile(searchPath, &findData);
    
    if (hFind == INVALID_HANDLE_VALUE) {
        if (errfunc && errfunc(dir ? dir : ".", GetLastError())) return GLOB_ABORTED;
        if (flags & GLOB_ERR) return GLOB_ABORTED;
        return 0;
    }
    
    do {
        if (findData.cFileName[0] == '.' && pattern[0] != '.') continue;
        
        if (fnmatch(pattern, findData.cFileName, (flags & GLOB_NOESCAPE) ? FNM_NOESCAPE : 0) == 0) {
            if (dir) {
                snprintf(path, sizeof(path), "%s/%s", dir, findData.cFileName);
            } else {
                strncpy(path, findData.cFileName, sizeof(path) - 1);
                path[sizeof(path) - 1] = '\0';
            }
            
            if (flags & GLOB_MARK && (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
                strncat(path, "/", sizeof(path) - strlen(path) - 1);
            }
            
            int result = add_path(pglob, path);
            if (result != 0) {
                FindClose(hFind);
                return result;
            }
        }
    } while (FindNextFile(hFind, &findData));
    
    FindClose(hFind);
    return 0;
#else
    /* Unix implementation would go here */
    return GLOB_ABORTED;
#endif
}

int glob(const char *restrict pattern, int flags, 
         int (*errfunc)(const char *, int), glob_t *restrict pglob) {
    
    if (!pattern || !pglob) return GLOB_NOSPACE;
    
    if (!(flags & GLOB_APPEND)) {
        pglob->gl_pathc = 0;
        pglob->gl_pathv = NULL;
        pglob->gl_offs = 0;
        if (flags & GLOB_DOOFFS) {
            pglob->gl_pathv = calloc(pglob->gl_offs + 1, sizeof(char*));
            if (!pglob->gl_pathv) return GLOB_NOSPACE;
        }
    }
    
    /* Simple implementation - just match in current directory */
    int result = glob_dir(pattern, NULL, flags, errfunc, pglob);
    if (result != 0) return result;
    
    if (pglob->gl_pathc == 0) {
        if (flags & GLOB_NOCHECK) {
            result = add_path(pglob, pattern);
            if (result != 0) return result;
        } else {
            return GLOB_NOMATCH;
        }
    }
    
    return 0;
}

void globfree(glob_t *pglob) {
    if (!pglob || !pglob->gl_pathv) return;
    
    for (size_t i = pglob->gl_offs; i < pglob->gl_offs + pglob->gl_pathc; i++) {
        free(pglob->gl_pathv[i]);
    }
    
    free(pglob->gl_pathv);
    pglob->gl_pathv = NULL;
    pglob->gl_pathc = 0;
}