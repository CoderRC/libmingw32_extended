#ifndef _ENDIAN_H
#define _ENDIAN_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Endianness constants */
#define LITTLE_ENDIAN 1234
#define BIG_ENDIAN    4321

/* Determine byte order for this platform */
#if defined(__BYTE_ORDER__)
  #if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    #define BYTE_ORDER LITTLE_ENDIAN
  #elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    #define BYTE_ORDER BIG_ENDIAN
  #endif
#elif defined(_WIN32) || defined(__i386__) || defined(__x86_64__) || defined(__amd64__)
  #define BYTE_ORDER LITTLE_ENDIAN
#else
  #define BYTE_ORDER LITTLE_ENDIAN  /* Default assumption */
#endif

/* Byte swap functions */
static inline uint16_t bswap16(uint16_t x) {
    return (x << 8) | (x >> 8);
}

static inline uint32_t bswap32(uint32_t x) {
    return ((x << 24) & 0xff000000) |
           ((x <<  8) & 0x00ff0000) |
           ((x >>  8) & 0x0000ff00) |
           ((x >> 24) & 0x000000ff);
}

static inline uint64_t bswap64(uint64_t x) {
    return ((x << 56) & 0xff00000000000000ULL) |
           ((x << 40) & 0x00ff000000000000ULL) |
           ((x << 24) & 0x0000ff0000000000ULL) |
           ((x <<  8) & 0x000000ff00000000ULL) |
           ((x >>  8) & 0x00000000ff000000ULL) |
           ((x >> 24) & 0x0000000000ff0000ULL) |
           ((x >> 40) & 0x000000000000ff00ULL) |
           ((x >> 56) & 0x00000000000000ffULL);
}

/* Big endian conversion functions */
#if BYTE_ORDER == LITTLE_ENDIAN
#define htobe16(x) bswap16(x)
#define htobe32(x) bswap32(x)
#define htobe64(x) bswap64(x)
#define be16toh(x) bswap16(x)
#define be32toh(x) bswap32(x)
#define be64toh(x) bswap64(x)
#else
#define htobe16(x) (x)
#define htobe32(x) (x)
#define htobe64(x) (x)
#define be16toh(x) (x)
#define be32toh(x) (x)
#define be64toh(x) (x)
#endif

/* Little endian conversion functions */
#if BYTE_ORDER == LITTLE_ENDIAN
#define htole16(x) (x)
#define htole32(x) (x)
#define htole64(x) (x)
#define le16toh(x) (x)
#define le32toh(x) (x)
#define le64toh(x) (x)
#else
#define htole16(x) bswap16(x)
#define htole32(x) bswap32(x)
#define htole64(x) bswap64(x)
#define le16toh(x) bswap16(x)
#define le32toh(x) bswap32(x)
#define le64toh(x) bswap64(x)
#endif

#ifdef __cplusplus
}
#endif

#endif /* _ENDIAN_H */