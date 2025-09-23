#include <poll.h>
#include <windows.h>
#include <io.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

typedef int (WINAPI *SelectPtr)(int, fd_set*, fd_set*, fd_set*, const struct timeval*);
typedef int (WINAPI *GetSockOptPtr)(SOCKET, int, int, char*, int*);
typedef int (WINAPI *IOCtlSocketPtr)(SOCKET, long, u_long*);
typedef int (WINAPI *WSAGetLastErrorPtr)(void);
typedef int (WINAPI *WSAStartupPtr)(unsigned short, WSADATA*);

static SelectPtr pSelect = NULL;
static GetSockOptPtr pGetSockOpt = NULL;
static IOCtlSocketPtr pIOCtlSocket = NULL;
static WSAGetLastErrorPtr pWSAGetLastError = NULL;
static WSAStartupPtr pWSAStartup = NULL;

static int load_winsock_functions() {
    static int initialized = 0;
    if (initialized) return 1;
    
    HMODULE ws2_32 = LoadLibraryA("ws2_32.dll");
    if (!ws2_32) {
        errno = ENOENT;
        return 0;
    }
    
    pSelect = (SelectPtr)GetProcAddress(ws2_32, "select");
    pGetSockOpt = (GetSockOptPtr)GetProcAddress(ws2_32, "getsockopt");
    pIOCtlSocket = (IOCtlSocketPtr)GetProcAddress(ws2_32, "ioctlsocket");
    pWSAGetLastError = (WSAGetLastErrorPtr)GetProcAddress(ws2_32, "WSAGetLastError");
    pWSAStartup = (WSAStartupPtr)GetProcAddress(ws2_32, "WSAStartup");
    
    if (!pSelect || !pGetSockOpt || !pIOCtlSocket || !pWSAGetLastError || !pWSAStartup) {
        errno = ENOSYS;
        return 0;
    }
    
    WSADATA wsaData;
    if (pWSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        errno = ENOSYS;
        return 0;
    }
    
    initialized = 1;
    return 1;
}

static int convert_winsock_error(int wsa_error) {
    switch (wsa_error) {
        case WSAEINTR:    return EINTR;
        case WSAEBADF:    return EBADF;
        case WSAEINVAL:   return EINVAL;
        case WSAEFAULT:   return EFAULT;
        case WSAENOTSOCK: return ENOTSOCK;
        default:          return EIO;
    }
}

static int is_socket(int fd) {
    if (!load_winsock_functions()) return 0;
    
    SOCKET s = (SOCKET)_get_osfhandle(fd);
    if (s == INVALID_SOCKET) return 0;
    
    int type;
    int type_len = sizeof(type);
    if (pGetSockOpt(s, SOL_SOCKET, SO_TYPE, (char*)&type, &type_len) == 0) {
        return 1;
    }
    
    return 0;
}

static int fd_isSet(SOCKET s, fd_set* set) {
    for (unsigned int i = 0; i < set->fd_count; i++) {
        if (set->fd_array[i] == s) return 1;
    }
    return 0;
}

// Adapted from waitid.c WaitForProcesses function
typedef struct WaitForHandlesStruct {
    HANDLE *handles;
    nfds_t *handle_map;
    DWORD handle_count;
    DWORD timeout;
    DWORD result;
    int done;
} WaitForHandlesStruct;

static DWORD WINAPI WaitForHandles(LPVOID lpParam) {
    WaitForHandlesStruct *wfhs = (WaitForHandlesStruct *)lpParam;
    DWORD i = 0;
    HANDLE hHandles[MAXIMUM_WAIT_OBJECTS];
    
    for (i = 0; i < wfhs->handle_count && i < MAXIMUM_WAIT_OBJECTS; i++) {
        hHandles[i] = wfhs->handles[i];
    }
    
    if (i > 0) {
        DWORD state = WaitForMultipleObjects(i, hHandles, FALSE, wfhs->timeout);
        if (state >= WAIT_OBJECT_0 && state < WAIT_OBJECT_0 + i) {
            wfhs->result = state;
            wfhs->done = 1;
        } else if (state == WAIT_TIMEOUT) {
            wfhs->result = WAIT_TIMEOUT;
            wfhs->done = 1;
        }
    }
    
    return 0;
}

int poll(struct pollfd fds[], nfds_t nfds, int timeout) {
    int ready_count = 0;
    int socket_count = 0;
    HANDLE handles[MAXIMUM_WAIT_OBJECTS];
    nfds_t handle_map[MAXIMUM_WAIT_OBJECTS];
    DWORD handle_count = 0;
    HANDLE h;
    DWORD file_type;
    DWORD bytes_avail = 0;
    DWORD wait_timeout = (timeout < 0) ? INFINITE : (DWORD)timeout;
    
    char *is_socket_cache = malloc(nfds * sizeof(char));
    if (!is_socket_cache) {
        errno = ENOMEM;
        return -1;
    }
    
    // First pass: check immediate readiness and collect handles
    for (nfds_t i = 0; i < nfds; i++) {
        fds[i].revents = 0;
        if (fds[i].fd < 0) {
            is_socket_cache[i] = 0;
            continue;
        }

        is_socket_cache[i] = is_socket(fds[i].fd);
        if (!is_socket_cache[i]) {
            h = (HANDLE)_get_osfhandle(fds[i].fd);
            if (h == INVALID_HANDLE_VALUE) {
                fds[i].revents = POLLNVAL;
                ready_count++;
                continue;
            }
            
            file_type = GetFileType(h);
            if (file_type == FILE_TYPE_UNKNOWN && GetLastError() != NO_ERROR) {
                fds[i].revents = POLLNVAL;
                ready_count++;
                continue;
            }
            
            if (file_type == FILE_TYPE_PIPE) {
                if (fds[i].events & (POLLIN | POLLRDNORM)) {
                    bytes_avail = 0;
                    if (PeekNamedPipe(h, NULL, 0, NULL, &bytes_avail, NULL)) {
                        if (bytes_avail > 0) {
                            fds[i].revents |= fds[i].events & (POLLIN | POLLRDNORM);
                        }
                    } else {
                        if (GetLastError() == ERROR_BROKEN_PIPE) {
                            fds[i].revents |= POLLHUP;
                        } else {
                            fds[i].revents |= POLLERR;
                        }
                    }
                }
                if (fds[i].events & (POLLOUT | POLLWRNORM)) {
                    fds[i].revents |= fds[i].events & (POLLOUT | POLLWRNORM);
                }
                
                // Add to wait list if not immediately ready
                if (!fds[i].revents && handle_count < MAXIMUM_WAIT_OBJECTS) {
                    handles[handle_count] = h;
                    handle_map[handle_count] = i;
                    handle_count++;
                }
            } else {
                if (fds[i].events & POLLIN) fds[i].revents |= POLLIN;
                if (fds[i].events & POLLOUT) fds[i].revents |= POLLOUT;
                if (fds[i].events & POLLPRI) fds[i].revents |= POLLPRI;
            }
            
            if (fds[i].revents) ready_count++;
        } else {
            socket_count++;
        }
    }
    
    // Wait for handles using WaitForHandles pattern from waitid.c
    if (handle_count > 0 && ready_count == 0) {
        WaitForHandlesStruct wfhs;
        wfhs.handles = handles;
        wfhs.handle_map = handle_map;
        wfhs.handle_count = handle_count;
        wfhs.timeout = wait_timeout;
        wfhs.result = WAIT_TIMEOUT;
        wfhs.done = 0;
        
        HANDLE hThread = CreateThread(NULL, 0, WaitForHandles, &wfhs, 0, NULL);
        if (hThread) {
            WaitForSingleObject(hThread, INFINITE);
            CloseHandle(hThread);
            
            if (wfhs.result >= WAIT_OBJECT_0 && wfhs.result < WAIT_OBJECT_0 + handle_count) {
                nfds_t ready_index = handle_map[wfhs.result - WAIT_OBJECT_0];
                h = (HANDLE)_get_osfhandle(fds[ready_index].fd);
                
                if (fds[ready_index].events & (POLLIN | POLLRDNORM)) {
                    bytes_avail = 0;
                    if (PeekNamedPipe(h, NULL, 0, NULL, &bytes_avail, NULL)) {
                        if (bytes_avail > 0) {
                            fds[ready_index].revents |= fds[ready_index].events & (POLLIN | POLLRDNORM);
                        }
                    } else {
                        if (GetLastError() == ERROR_BROKEN_PIPE) {
                            fds[ready_index].revents |= POLLHUP;
                        } else {
                            fds[ready_index].revents |= POLLERR;
                        }
                    }
                }
                if (fds[ready_index].events & (POLLOUT | POLLWRNORM)) {
                    fds[ready_index].revents |= fds[ready_index].events & (POLLOUT | POLLWRNORM);
                }
                
                if (fds[ready_index].revents) ready_count++;
            }
        } else {
            // Fallback to direct WaitForMultipleObjects
            DWORD wait_result = WaitForMultipleObjects(handle_count, handles, FALSE, wait_timeout);
            if (wait_result >= WAIT_OBJECT_0 && wait_result < WAIT_OBJECT_0 + handle_count) {
                nfds_t ready_index = handle_map[wait_result - WAIT_OBJECT_0];
                if (fds[ready_index].events & (POLLIN | POLLRDNORM)) {
                    fds[ready_index].revents |= fds[ready_index].events & (POLLIN | POLLRDNORM);
                }
                if (fds[ready_index].events & (POLLOUT | POLLWRNORM)) {
                    fds[ready_index].revents |= fds[ready_index].events & (POLLOUT | POLLWRNORM);
                }
                if (fds[ready_index].revents) ready_count++;
            }
        }
    }

    if (socket_count > 0) {
        if (!load_winsock_functions()) {
            errno = ENOSYS;
            for (nfds_t i = 0; i < nfds; i++) {
                if (fds[i].fd >= 0 && is_socket_cache[i] && !fds[i].revents) {
                    fds[i].revents = POLLERR;
                    ready_count++;
                }
            }
            free(is_socket_cache);
            return ready_count;
        }

        fd_set readfds, writefds, exceptfds;
        FD_ZERO(&readfds);
        FD_ZERO(&writefds);
        FD_ZERO(&exceptfds);

        for (nfds_t i = 0; i < nfds; i++) {
            if (fds[i].fd < 0 || !is_socket_cache[i] || fds[i].revents) continue;
            
            SOCKET s = (SOCKET)_get_osfhandle(fds[i].fd);
            if (s == INVALID_SOCKET) {
                fds[i].revents = POLLNVAL;
                ready_count++;
                continue;
            }

            if (fds[i].events & (POLLIN | POLLRDNORM)) FD_SET(s, &readfds);
            if (fds[i].events & (POLLOUT | POLLWRNORM)) FD_SET(s, &writefds);
            if (fds[i].events & POLLPRI) FD_SET(s, &exceptfds);
        }

        struct timeval tv = {0, 0}, *ptv = (ready_count > 0) ? &tv : NULL;
        if (ready_count == 0 && timeout >= 0) {
            tv.tv_sec = timeout / 1000;
            tv.tv_usec = (timeout % 1000) * 1000;
            ptv = &tv;
        }

        int socket_ready = pSelect(0, &readfds, &writefds, &exceptfds, ptv);
        if (socket_ready < 0) {
            errno = convert_winsock_error(pWSAGetLastError());
        } else {
            for (nfds_t i = 0; i < nfds; i++) {
                if (fds[i].fd < 0 || !is_socket_cache[i] || fds[i].revents) continue;
                
                SOCKET s = (SOCKET)_get_osfhandle(fds[i].fd);
                if (s == INVALID_SOCKET) continue;

                if (fd_isSet(s, &readfds)) {
                    u_long nbytes = 0;
                    if (pIOCtlSocket(s, FIONREAD, &nbytes) == 0 && nbytes > 0) {
                        fds[i].revents |= POLLIN;
                    } else {
                        fds[i].revents |= POLLHUP;
                    }
                }
                if (fd_isSet(s, &writefds)) fds[i].revents |= POLLOUT;
                if (fd_isSet(s, &exceptfds)) fds[i].revents |= POLLPRI;
                
                if (fds[i].revents) ready_count++;
            }
        }
    }
    
    // If no ready FDs and timeout specified, wait for timeout
    if (ready_count == 0 && timeout > 0) {
        Sleep(timeout);
    }
    
    free(is_socket_cache);
    return ready_count;
}