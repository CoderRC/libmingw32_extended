#include <arpa/inet.h>
#include <stdint.h>

uint16_t htons(uint16_t hostshort) {
    #if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
        return ((hostshort & 0xFF00) >> 8) |
               ((hostshort & 0x00FF) << 8);
    #else
        return hostshort;
    #endif
}