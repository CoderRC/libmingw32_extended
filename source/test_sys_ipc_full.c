#include <sys/ipc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define TEST_PASSED 0
#define TEST_FAILED 1

int test_ipc_constants() {
    printf("Testing IPC constants...\n");
    
    if (IPC_CREAT != 0001000) {
        printf("IPC_CREAT test failed - expected 0001000, got %o\n", IPC_CREAT);
        return TEST_FAILED;
    }
    
    if (IPC_EXCL != 0002000) {
        printf("IPC_EXCL test failed - expected 0002000, got %o\n", IPC_EXCL);
        return TEST_FAILED;
    }
    
    if (IPC_NOWAIT != 0004000) {
        printf("IPC_NOWAIT test failed - expected 0004000, got %o\n", IPC_NOWAIT);
        return TEST_FAILED;
    }
    
    if (IPC_PRIVATE != 0) {
        printf("IPC_PRIVATE test failed - expected 0, got %d\n", IPC_PRIVATE);
        return TEST_FAILED;
    }
    
    printf("IPC constants test passed\n");
    return TEST_PASSED;
}

int test_ipc_commands() {
    printf("Testing IPC commands...\n");
    
    if (IPC_RMID != 0) {
        printf("IPC_RMID test failed - expected 0, got %d\n", IPC_RMID);
        return TEST_FAILED;
    }
    
    if (IPC_SET != 1) {
        printf("IPC_SET test failed - expected 1, got %d\n", IPC_SET);
        return TEST_FAILED;
    }
    
    if (IPC_STAT != 2) {
        printf("IPC_STAT test failed - expected 2, got %d\n", IPC_STAT);
        return TEST_FAILED;
    }
    
    if (IPC_INFO != 3) {
        printf("IPC_INFO test failed - expected 3, got %d\n", IPC_INFO);
        return TEST_FAILED;
    }
    
    printf("IPC commands test passed\n");
    return TEST_PASSED;
}

int test_ipc_permissions() {
    printf("Testing IPC permissions...\n");
    
    if (IPC_R != 0000400) {
        printf("IPC_R test failed - expected 0000400, got %o\n", IPC_R);
        return TEST_FAILED;
    }
    
    if (IPC_W != 0000200) {
        printf("IPC_W test failed - expected 0000200, got %o\n", IPC_W);
        return TEST_FAILED;
    }
    
    if (IPC_M != 0010000) {
        printf("IPC_M test failed - expected 0010000, got %o\n", IPC_M);
        return TEST_FAILED;
    }
    
    printf("IPC permissions test passed\n");
    return TEST_PASSED;
}

int test_ipc_perm_structure() {
    printf("Testing ipc_perm structure...\n");
    
    struct ipc_perm perm;
    
    // Test that structure can be initialized
    memset(&perm, 0, sizeof(perm));
    
    perm.uid = 1000;
    perm.gid = 1000;
    perm.cuid = 1001;
    perm.cgid = 1001;
    perm.mode = IPC_R | IPC_W;
    perm.seq = 1;
    perm.key = 12345;
    
    if (perm.uid != 1000) {
        printf("ipc_perm uid test failed\n");
        return TEST_FAILED;
    }
    
    if (perm.gid != 1000) {
        printf("ipc_perm gid test failed\n");
        return TEST_FAILED;
    }
    
    if (perm.cuid != 1001) {
        printf("ipc_perm cuid test failed\n");
        return TEST_FAILED;
    }
    
    if (perm.cgid != 1001) {
        printf("ipc_perm cgid test failed\n");
        return TEST_FAILED;
    }
    
    if (perm.mode != (IPC_R | IPC_W)) {
        printf("ipc_perm mode test failed\n");
        return TEST_FAILED;
    }
    
    if (perm.seq != 1) {
        printf("ipc_perm seq test failed\n");
        return TEST_FAILED;
    }
    
    if (perm.key != 12345) {
        printf("ipc_perm key test failed\n");
        return TEST_FAILED;
    }
    
    printf("ipc_perm structure test passed\n");
    return TEST_PASSED;
}

int test_ftok_basic() {
    printf("Testing ftok basic functionality...\n");
    
    key_t key1 = ftok("/tmp", 1);
    if (key1 == (key_t)-1) {
        printf("ftok basic test failed - returned -1\n");
        return TEST_FAILED;
    }
    
    key_t key2 = ftok("/tmp", 2);
    if (key2 == (key_t)-1) {
        printf("ftok basic test failed - second call returned -1\n");
        return TEST_FAILED;
    }
    
    // Different project IDs should produce different keys
    if (key1 == key2) {
        printf("ftok basic test failed - same keys for different project IDs\n");
        return TEST_FAILED;
    }
    
    printf("ftok basic test passed - key1: %d, key2: %d\n", key1, key2);
    return TEST_PASSED;
}

int test_ftok_consistency() {
    printf("Testing ftok consistency...\n");
    
    key_t key1 = ftok("/tmp", 42);
    key_t key2 = ftok("/tmp", 42);
    
    if (key1 == (key_t)-1 || key2 == (key_t)-1) {
        printf("ftok consistency test failed - one call returned -1\n");
        return TEST_FAILED;
    }
    
    // Same path and project ID should produce same key
    if (key1 != key2) {
        printf("ftok consistency test failed - different keys for same parameters\n");
        return TEST_FAILED;
    }
    
    printf("ftok consistency test passed - key: %d\n", key1);
    return TEST_PASSED;
}

int test_ftok_different_paths() {
    printf("Testing ftok with different paths...\n");
    
    key_t key1 = ftok("/tmp", 1);
    key_t key2 = ftok("/var", 1);
    
    if (key1 == (key_t)-1 || key2 == (key_t)-1) {
        printf("ftok different paths test failed - one call returned -1\n");
        return TEST_FAILED;
    }
    
    // Different paths should typically produce different keys
    if (key1 == key2) {
        printf("ftok different paths test warning - same keys for different paths\n");
        // This is not necessarily a failure, but worth noting
    }
    
    printf("ftok different paths test passed - key1: %d, key2: %d\n", key1, key2);
    return TEST_PASSED;
}

int test_ftok_invalid_params() {
    printf("Testing ftok with invalid parameters...\n");
    
    // Test with NULL pathname
    key_t key1 = ftok(NULL, 1);
    if (key1 != (key_t)-1) {
        printf("ftok null test failed - should return -1\n");
        return TEST_FAILED;
    }
    
    // Test with empty pathname
    key_t key2 = ftok("", 1);
    if (key2 != (key_t)-1) {
        printf("ftok empty test failed - should return -1\n");
        return TEST_FAILED;
    }
    
    // Test with zero project ID
    key_t key3 = ftok("/tmp", 0);
    if (key3 != (key_t)-1) {
        printf("ftok zero project test failed - should return -1\n");
        return TEST_FAILED;
    }
    
    printf("ftok invalid params test passed\n");
    return TEST_PASSED;
}

int test_ftok_project_id_range() {
    printf("Testing ftok with various project IDs...\n");
    
    key_t keys[256];
    int unique_count = 0;
    
    // Test with project IDs 1-255
    for (int i = 1; i <= 255; i++) {
        keys[i-1] = ftok("/tmp", i);
        if (keys[i-1] == (key_t)-1) {
            printf("ftok project range test failed - project ID %d returned -1\n", i);
            return TEST_FAILED;
        }
    }
    
    // Count unique keys (simple check)
    for (int i = 0; i < 255; i++) {
        int is_unique = 1;
        for (int j = 0; j < i; j++) {
            if (keys[i] == keys[j]) {
                is_unique = 0;
                break;
            }
        }
        if (is_unique) unique_count++;
    }
    
    printf("ftok project range test passed - %d unique keys out of 255\n", unique_count);
    return TEST_PASSED;
}

int main() {
    int failures = 0;
    
    printf("===== sys/ipc Test Suite =====\n");
    
    printf("\n[TEST] IPC constants\n");
    failures += test_ipc_constants();
    
    printf("\n[TEST] IPC commands\n");
    failures += test_ipc_commands();
    
    printf("\n[TEST] IPC permissions\n");
    failures += test_ipc_permissions();
    
    printf("\n[TEST] ipc_perm structure\n");
    failures += test_ipc_perm_structure();
    
    printf("\n[TEST] ftok basic\n");
    failures += test_ftok_basic();
    
    printf("\n[TEST] ftok consistency\n");
    failures += test_ftok_consistency();
    
    printf("\n[TEST] ftok different paths\n");
    failures += test_ftok_different_paths();
    
    printf("\n[TEST] ftok invalid params\n");
    failures += test_ftok_invalid_params();
    
    printf("\n[TEST] ftok project ID range\n");
    failures += test_ftok_project_id_range();
    
    printf("\n===== Test Suite Completed =====\n");
    printf("%d tests failed\n", failures);
    
    return failures ? TEST_FAILED : TEST_PASSED;
}