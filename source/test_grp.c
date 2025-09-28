#include <grp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TEST_PASSED 0
#define TEST_FAILED 1

int test_getgrgid() {
    struct group *grp = getgrgid(1000);
    
    if (!grp) {
        printf("getgrgid test failed - returned NULL\n");
        return TEST_FAILED;
    }
    
    if (!grp->gr_name || grp->gr_gid != 1000) {
        printf("getgrgid test failed - invalid data\n");
        return TEST_FAILED;
    }
    
    printf("getgrgid test passed - name: %s, gid: %u\n", grp->gr_name, grp->gr_gid);
    return TEST_PASSED;
}

int test_getgrnam() {
    struct group *grp = getgrnam("users");
    
    if (!grp) {
        printf("getgrnam test failed - returned NULL\n");
        return TEST_FAILED;
    }
    
    if (!grp->gr_name || strcmp(grp->gr_name, "users") != 0) {
        printf("getgrnam test failed - invalid name\n");
        return TEST_FAILED;
    }
    
    printf("getgrnam test passed - name: %s, gid: %u\n", grp->gr_name, grp->gr_gid);
    return TEST_PASSED;
}

int test_getgrnam_null() {
    struct group *grp = getgrnam(NULL);
    
    if (grp != NULL) {
        printf("getgrnam null test failed - should return NULL\n");
        return TEST_FAILED;
    }
    
    printf("getgrnam null test passed\n");
    return TEST_PASSED;
}

int test_getgrent() {
    setgrent();
    
    struct group *grp = getgrent();
    if (!grp) {
        printf("getgrent test failed - first call returned NULL\n");
        return TEST_FAILED;
    }
    
    printf("getgrent first call - name: %s, gid: %u\n", grp->gr_name, grp->gr_gid);
    
    // Second call should return NULL based on our implementation
    grp = getgrent();
    if (grp != NULL) {
        printf("getgrent second call - name: %s, gid: %u\n", grp->gr_name, grp->gr_gid);
    }
    
    endgrent();
    printf("getgrent test passed\n");
    return TEST_PASSED;
}

int test_setgrent_endgrent() {
    setgrent();
    struct group *grp1 = getgrent();
    
    if (!grp1) {
        printf("setgrent test failed - first getgrent returned NULL\n");
        return TEST_FAILED;
    }
    
    // Reset and try again
    setgrent();
    struct group *grp2 = getgrent();
    
    if (!grp2) {
        printf("setgrent test failed - second getgrent returned NULL\n");
        return TEST_FAILED;
    }
    
    // Should get same result after reset
    if (strcmp(grp1->gr_name, grp2->gr_name) != 0 || grp1->gr_gid != grp2->gr_gid) {
        printf("setgrent test failed - different results after reset\n");
        return TEST_FAILED;
    }
    
    endgrent();
    printf("setgrent/endgrent test passed\n");
    return TEST_PASSED;
}

int main() {
    int failures = 0;
    
    printf("===== grp Test Suite =====\n");
    
    printf("\n[TEST] getgrgid\n");
    failures += test_getgrgid();
    
    printf("\n[TEST] getgrnam\n");
    failures += test_getgrnam();
    
    printf("\n[TEST] getgrnam with NULL\n");
    failures += test_getgrnam_null();
    
    printf("\n[TEST] getgrent\n");
    failures += test_getgrent();
    
    printf("\n[TEST] setgrent/endgrent\n");
    failures += test_setgrent_endgrent();
    
    printf("\n===== Test Suite Completed =====\n");
    printf("%d tests failed\n", failures);
    
    return failures ? TEST_FAILED : TEST_PASSED;
}