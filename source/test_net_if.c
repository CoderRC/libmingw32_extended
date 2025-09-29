#include <net/if.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TEST_PASSED 0
#define TEST_FAILED 1

int test_if_nameindex() {
    struct if_nameindex *list = if_nameindex();
    
    if (!list) {
        printf("if_nameindex test failed - returned NULL\n");
        return TEST_FAILED;
    }
    
    if (list[0].if_index != 1 || !list[0].if_name || strcmp(list[0].if_name, "lo") != 0) {
        printf("if_nameindex test failed - invalid first entry\n");
        if_freenameindex(list);
        return TEST_FAILED;
    }
    
    if (list[1].if_index != 2 || !list[1].if_name || strcmp(list[1].if_name, "eth0") != 0) {
        printf("if_nameindex test failed - invalid second entry\n");
        if_freenameindex(list);
        return TEST_FAILED;
    }
    
    if (list[2].if_index != 3 || !list[2].if_name || strcmp(list[2].if_name, "wlan0") != 0) {
        printf("if_nameindex test failed - invalid third entry\n");
        if_freenameindex(list);
        return TEST_FAILED;
    }
    
    // Check null terminator
    if (list[3].if_index != 0 || list[3].if_name != NULL) {
        printf("if_nameindex test failed - missing null terminator\n");
        if_freenameindex(list);
        return TEST_FAILED;
    }
    
    printf("if_nameindex test passed - found 3 interfaces\n");
    
    if_freenameindex(list);
    return TEST_PASSED;
}

int test_if_nametoindex() {
    unsigned index = if_nametoindex("lo");
    
    if (index != 1) {
        printf("if_nametoindex test failed - expected 1 for 'lo', got %u\n", index);
        return TEST_FAILED;
    }
    
    unsigned index2 = if_nametoindex("eth0");
    if (index2 != 2) {
        printf("if_nametoindex test failed - expected 2 for 'eth0', got %u\n", index2);
        return TEST_FAILED;
    }
    
    unsigned index3 = if_nametoindex("wlan0");
    if (index3 != 3) {
        printf("if_nametoindex test failed - expected 3 for 'wlan0', got %u\n", index3);
        return TEST_FAILED;
    }
    
    printf("if_nametoindex test passed - 'lo' has index %u\n", index);
    return TEST_PASSED;
}

int test_if_indextoname() {
    char name[IF_NAMESIZE];
    char *result = if_indextoname(1, name);
    
    if (!result) {
        printf("if_indextoname test failed - returned NULL\n");
        return TEST_FAILED;
    }
    
    if (strcmp(name, "lo") != 0) {
        printf("if_indextoname test failed - expected 'lo', got '%s'\n", name);
        return TEST_FAILED;
    }
    
    // Test additional interfaces
    char *result2 = if_indextoname(2, name);
    if (!result2 || strcmp(name, "eth0") != 0) {
        printf("if_indextoname eth0 test failed\n");
        return TEST_FAILED;
    }
    
    char *result3 = if_indextoname(3, name);
    if (!result3 || strcmp(name, "wlan0") != 0) {
        printf("if_indextoname wlan0 test failed\n");
        return TEST_FAILED;
    }
    
    printf("if_indextoname test passed - index 1 is '%s'\n", "lo");
    return TEST_PASSED;
}

int test_if_invalid_params() {
    unsigned index = if_nametoindex(NULL);
    if (index != 0) {
        printf("if_nametoindex null test failed - should return 0\n");
        return TEST_FAILED;
    }
    
    char *result = if_indextoname(0, NULL);
    if (result != NULL) {
        printf("if_indextoname invalid test failed - should return NULL\n");
        return TEST_FAILED;
    }
    
    printf("Invalid params test passed\n");
    return TEST_PASSED;
}

int main() {
    int failures = 0;
    
    printf("===== net/if Test Suite =====\n");
    
    printf("\n[TEST] if_nameindex\n");
    failures += test_if_nameindex();
    
    printf("\n[TEST] if_nametoindex\n");
    failures += test_if_nametoindex();
    
    printf("\n[TEST] if_indextoname\n");
    failures += test_if_indextoname();
    
    printf("\n[TEST] Invalid params\n");
    failures += test_if_invalid_params();
    
    printf("\n===== Test Suite Completed =====\n");
    printf("%d tests failed\n", failures);
    
    return failures ? TEST_FAILED : TEST_PASSED;
}