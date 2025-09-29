#include <sys/ipc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TEST_PASSED 0
#define TEST_FAILED 1

int test_ftok() {
    key_t key1 = ftok("/tmp/test", 1);
    key_t key2 = ftok("/tmp/test", 2);
    
    if (key1 == (key_t)-1 || key2 == (key_t)-1) {
        printf("ftok test failed - returned -1\n");
        return TEST_FAILED;
    }
    
    if (key1 == key2) {
        printf("ftok test failed - same keys for different proj_id\n");
        return TEST_FAILED;
    }
    
    printf("ftok test passed - key1: %d, key2: %d\n", (int)key1, (int)key2);
    return TEST_PASSED;
}

int test_ftok_same_params() {
    key_t key1 = ftok("/tmp/same", 5);
    key_t key2 = ftok("/tmp/same", 5);
    
    if (key1 == (key_t)-1 || key2 == (key_t)-1) {
        printf("ftok same params test failed - returned -1\n");
        return TEST_FAILED;
    }
    
    if (key1 != key2) {
        printf("ftok same params test failed - different keys for same params\n");
        return TEST_FAILED;
    }
    
    printf("ftok same params test passed - consistent key: %d\n", (int)key1);
    return TEST_PASSED;
}

int test_ftok_null() {
    key_t key = ftok(NULL, 1);
    
    if (key != (key_t)-1) {
        printf("ftok null test failed - should return -1\n");
        return TEST_FAILED;
    }
    
    printf("ftok null test passed\n");
    return TEST_PASSED;
}

int test_ipc_constants() {
    if (IPC_PRIVATE != 0) {
        printf("IPC constants test failed - IPC_PRIVATE should be 0\n");
        return TEST_FAILED;
    }
    
    if (IPC_CREAT == 0 || IPC_EXCL == 0 || IPC_NOWAIT == 0) {
        printf("IPC constants test failed - mode bits should be non-zero\n");
        return TEST_FAILED;
    }
    
    printf("IPC constants test passed\n");
    return TEST_PASSED;
}

int main() {
    int failures = 0;
    
    printf("===== sys/ipc Test Suite =====\n");
    
    printf("\n[TEST] ftok\n");
    failures += test_ftok();
    
    printf("\n[TEST] ftok same params\n");
    failures += test_ftok_same_params();
    
    printf("\n[TEST] ftok null\n");
    failures += test_ftok_null();
    
    printf("\n[TEST] IPC constants\n");
    failures += test_ipc_constants();
    
    printf("\n===== Test Suite Completed =====\n");
    printf("%d tests failed\n", failures);
    
    return failures ? TEST_FAILED : TEST_PASSED;
}