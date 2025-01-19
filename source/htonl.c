#include <arpa/inet.h>
#include <stdint.h>

uint32_t htonl(uint32_t hostlong) {
    #if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
        return ((hostlong & 0xFF000000) >> 24) |
               ((hostlong & 0x00FF0000) >> 8) |
               ((hostlong & 0x0000FF00) << 8) |
               ((hostlong & 0x000000FF) << 24);
    #else
        return hostlong;
    #endif
}