#include <nl_types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TEST_PASSED 0
#define TEST_FAILED 1

int test_catopen_close() {
    nl_catd catd = catopen("test.cat", NL_CAT_LOCALE);
    
    if (catd == (nl_catd)-1) {
        printf("catopen test failed - returned -1\n");
        return TEST_FAILED;
    }
    
    int result = catclose(catd);
    if (result != 0) {
        printf("catclose test failed - returned %d\n", result);
        return TEST_FAILED;
    }
    
    printf("catopen/close test passed\n");
    return TEST_PASSED;
}

int test_catgets() {
    nl_catd catd = catopen("test.cat", 0);
    
    if (catd == (nl_catd)-1) {
        printf("catgets test failed - catopen returned -1\n");
        return TEST_FAILED;
    }
    
    const char *default_msg = "default message";
    char *result = catgets(catd, NL_SETD, 1, default_msg);
    
    if (!result) {
        printf("catgets test failed - returned NULL\n");
        catclose(catd);
        return TEST_FAILED;
    }
    
    if (strcmp(result, default_msg) != 0) {
        printf("catgets test failed - expected '%s', got '%s'\n", default_msg, result);
        catclose(catd);
        return TEST_FAILED;
    }
    
    catclose(catd);
    printf("catgets test passed\n");
    return TEST_PASSED;
}

int test_catopen_null() {
    nl_catd catd = catopen(NULL, 0);
    
    if (catd != (nl_catd)-1) {
        printf("catopen null test failed - should return -1\n");
        if (catd != -1) catclose(catd);
        return TEST_FAILED;
    }
    
    printf("catopen null test passed\n");
    return TEST_PASSED;
}

int main() {
    int failures = 0;
    
    printf("===== nl_types Test Suite =====\n");
    
    printf("\n[TEST] catopen/close\n");
    failures += test_catopen_close();
    
    printf("\n[TEST] catgets\n");
    failures += test_catgets();
    
    printf("\n[TEST] catopen null\n");
    failures += test_catopen_null();
    
    printf("\n===== Test Suite Completed =====\n");
    printf("%d tests failed\n", failures);
    
    return failures ? TEST_FAILED : TEST_PASSED;
}