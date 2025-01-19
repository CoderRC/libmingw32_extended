#include <arpa/inet.h>
#include <stdint.h>

uint16_t ntohs(uint16_t netshort) {
    #if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
        return ((netshort & 0xFF00) >> 8) |
               ((netshort & 0x00FF) << 8);
    #else
        return netshort;
    #endif
}