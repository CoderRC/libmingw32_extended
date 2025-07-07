#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>
#include <stdio.h>

#ifndef WSAAPI
#define WSAAPI __stdcall
#endif

/* Windows error codes mapping */
#define WSA_NOT_ENOUGH_MEMORY  8

#define WSAEAFNOSUPPORT        10047
#define WSAEINVAL              10022
#define WSAESOCKTNOSUPPORT     10044
#define WSAHOST_NOT_FOUND      11001
#define WSANO_DATA             11004
#define WSANO_RECOVERY         11003
#define WSANOTINITIALISED      10093
#define WSATRY_AGAIN           11002
#define WSATYPE_NOT_FOUND      10009

/* Windows address info structure */
struct windows_addrinfo {
    int ai_flags;
    int ai_family;
    int ai_socktype;
    int ai_protocol;
    size_t ai_addrlen;
    char *ai_canonname;
    void *ai_addr;  /* sockaddr structure */
    struct windows_addrinfo *ai_next;
};

/* Function pointer types */
typedef int (WSAAPI *GETADDRINFO)(const char *node, const char *service,
                                  const struct windows_addrinfo *hints,
                                  struct windows_addrinfo **res);
typedef void (WSAAPI *FREEADDRINFO)(struct windows_addrinfo *ai);

int getaddrinfo(const char *node, const char *service,
                const struct addrinfo *hints,
                struct addrinfo **res) {
    static void *ws2_32 = NULL;
    static GETADDRINFO p_getaddrinfo = NULL;
    static FREEADDRINFO p_freeaddrinfo = NULL;
    
    /* Load library and resolve functions on first call */
    if (!ws2_32) {
        ws2_32 = dlopen("ws2_32.dll", RTLD_NOW);
        if (!ws2_32) return EAI_FAIL;
        
        p_getaddrinfo = (GETADDRINFO)dlsym(ws2_32, "getaddrinfo");
        p_freeaddrinfo = (FREEADDRINFO)dlsym(ws2_32, "freeaddrinfo");
        
        if (!p_getaddrinfo || !p_freeaddrinfo) {
            dlclose(ws2_32);
            ws2_32 = NULL;
            return EAI_FAIL;
        }
    }

    /* Convert POSIX hints to Windows format */
    struct windows_addrinfo win_hints, *p_win_hints = NULL;
    if (hints) {
        memset(&win_hints, 0, sizeof(win_hints));
        win_hints.ai_flags = hints->ai_flags;
        win_hints.ai_family = hints->ai_family;
        win_hints.ai_socktype = hints->ai_socktype;
        win_hints.ai_protocol = hints->ai_protocol;
        p_win_hints = &win_hints;
    }

    /* Call Windows getaddrinfo */
    struct windows_addrinfo *win_res = NULL;
    int ret = p_getaddrinfo(node, service, p_win_hints, &win_res);
    if (ret != 0) {
        /* Map Windows errors to POSIX codes */
        switch (ret) {
            case WSATRY_AGAIN:       return EAI_AGAIN;
            case WSAEINVAL:          return EAI_BADFLAGS;
            case WSANO_RECOVERY:      return EAI_FAIL;
            case WSAEAFNOSUPPORT:    return EAI_FAMILY;
            case WSA_NOT_ENOUGH_MEMORY: return EAI_MEMORY;
            case WSAHOST_NOT_FOUND:   return EAI_NONAME;
            case WSATYPE_NOT_FOUND:   return EAI_SERVICE;
            case WSAESOCKTNOSUPPORT: return EAI_SOCKTYPE;
            default:                  return EAI_FAIL;
        }
    }

    /* Convert Windows results to POSIX format */
    struct addrinfo *head = NULL, *tail = NULL;
    for (struct windows_addrinfo *wai = win_res; wai != NULL; wai = wai->ai_next) {
        struct addrinfo *ai = malloc(sizeof(struct addrinfo));
        if (!ai) goto mem_error;
        
        memset(ai, 0, sizeof(struct addrinfo));
        ai->ai_flags = wai->ai_flags;
        ai->ai_family = wai->ai_family;
        ai->ai_socktype = wai->ai_socktype;
        ai->ai_protocol = wai->ai_protocol;
        ai->ai_addrlen = (socklen_t)wai->ai_addrlen;
        ai->ai_next = NULL;

        /* Copy canonical name */
        if (wai->ai_canonname) {
            ai->ai_canonname = strdup(wai->ai_canonname);
            if (!ai->ai_canonname) goto mem_error;
        }

        /* Copy socket address */
        if (wai->ai_addr && ai->ai_addrlen > 0) {
            ai->ai_addr = malloc(ai->ai_addrlen);
            if (!ai->ai_addr) goto mem_error;
            memcpy(ai->ai_addr, wai->ai_addr, ai->ai_addrlen);
        }

        /* Add to linked list */
        if (!head) head = ai;
        if (tail) tail->ai_next = ai;
        tail = ai;
    }

    /* Cleanup Windows structures and return */
    p_freeaddrinfo(win_res);
    *res = head;
    return 0;

mem_error:
    /* Cleanup on memory allocation failure */
    if (win_res) p_freeaddrinfo(win_res);
    freeaddrinfo(head);
    return EAI_MEMORY;
}

void freeaddrinfo(struct addrinfo *ai) {
    while (ai) {
        struct addrinfo *next = ai->ai_next;
        free(ai->ai_addr);
        free(ai->ai_canonname);
        free(ai);
        ai = next;
    }
}