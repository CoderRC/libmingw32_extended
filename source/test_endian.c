#include <endian.h>
#include <stdio.h>
#include <stdint.h>

#define TEST_PASSED 0
#define TEST_FAILED 1

int test_byte_order() {
    printf("BYTE_ORDER: %d\n", BYTE_ORDER);
    printf("LITTLE_ENDIAN: %d\n", LITTLE_ENDIAN);
    printf("BIG_ENDIAN: %d\n", BIG_ENDIAN);
    
    if (BYTE_ORDER != LITTLE_ENDIAN && BYTE_ORDER != BIG_ENDIAN) {
        printf("ERROR: Invalid BYTE_ORDER value\n");
        return TEST_FAILED;
    }
    
    uint32_t test = 0x01020304;
    uint8_t *bytes = (uint8_t*)&test;
    
    if (BYTE_ORDER == LITTLE_ENDIAN && bytes[0] != 0x04) {
        printf("ERROR: BYTE_ORDER says little-endian but system appears big-endian\n");
        return TEST_FAILED;
    }
    if (BYTE_ORDER == BIG_ENDIAN && bytes[0] != 0x01) {
        printf("ERROR: BYTE_ORDER says big-endian but system appears little-endian\n");
        return TEST_FAILED;
    }
    
    printf("Byte order test passed\n");
    return TEST_PASSED;
}

int test_16bit_conversion() {
    uint16_t patterns[] = {0x0000, 0xFFFF, 0x1234, 0x8765, 0xFF00};
    int count = sizeof(patterns) / sizeof(patterns[0]);
    
    for (int i = 0; i < count; i++) {
        uint16_t host_val = patterns[i];
        uint16_t be_val = htobe16(host_val);
        uint16_t le_val = htole16(host_val);
        
        if (be16toh(be_val) != host_val || le16toh(le_val) != host_val) {
            printf("ERROR: 16-bit conversion failed for 0x%04X\n", host_val);
            return TEST_FAILED;
        }
        
        if (BYTE_ORDER == LITTLE_ENDIAN && htole16(host_val) != host_val) {
            printf("ERROR: htole16 should be no-op on little-endian\n");
            return TEST_FAILED;
        }
    }
    
    printf("16-bit conversion test passed\n");
    return TEST_PASSED;
}

int test_32bit_conversion() {
    uint32_t patterns[] = {0x00000000, 0xFFFFFFFF, 0x12345678, 0x87654321, 0xFF00FF00};
    int count = sizeof(patterns) / sizeof(patterns[0]);
    
    for (int i = 0; i < count; i++) {
        uint32_t host_val = patterns[i];
        uint32_t be_val = htobe32(host_val);
        uint32_t le_val = htole32(host_val);
        
        if (be32toh(be_val) != host_val || le32toh(le_val) != host_val) {
            printf("ERROR: 32-bit conversion failed for 0x%08X\n", host_val);
            return TEST_FAILED;
        }
        
        if (BYTE_ORDER == LITTLE_ENDIAN && htole32(host_val) != host_val) {
            printf("ERROR: htole32 should be no-op on little-endian\n");
            return TEST_FAILED;
        }
    }
    
    printf("32-bit conversion test passed\n");
    return TEST_PASSED;
}

int test_64bit_conversion() {
    uint64_t patterns[] = {0x0000000000000000ULL, 0xFFFFFFFFFFFFFFFFULL, 0x123456789ABCDEF0ULL, 0xFEDCBA9876543210ULL};
    int count = sizeof(patterns) / sizeof(patterns[0]);
    
    for (int i = 0; i < count; i++) {
        uint64_t host_val = patterns[i];
        uint64_t be_val = htobe64(host_val);
        uint64_t le_val = htole64(host_val);
        
        if (be64toh(be_val) != host_val || le64toh(le_val) != host_val) {
            printf("ERROR: 64-bit conversion failed for 0x%016llX\n", host_val);
            return TEST_FAILED;
        }
        
        if (BYTE_ORDER == LITTLE_ENDIAN && htole64(host_val) != host_val) {
            printf("ERROR: htole64 should be no-op on little-endian\n");
            return TEST_FAILED;
        }
    }
    
    printf("64-bit conversion test passed\n");
    return TEST_PASSED;
}

int main() {
    int failures = 0;
    
    printf("===== Endian Test Suite =====\n");
    
    printf("\n[TEST] Byte order\n");
    failures += test_byte_order();
    
    printf("\n[TEST] 16-bit conversion\n");
    failures += test_16bit_conversion();
    
    printf("\n[TEST] 32-bit conversion\n");
    failures += test_32bit_conversion();
    
    printf("\n[TEST] 64-bit conversion\n");
    failures += test_64bit_conversion();
    
    printf("\n===== Test Suite Completed =====\n");
    printf("%d tests failed\n", failures);
    
    return failures ? TEST_FAILED : TEST_PASSED;
}