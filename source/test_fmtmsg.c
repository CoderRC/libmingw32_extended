#include <fmtmsg.h>
#include <stdio.h>
#include <stdlib.h>

#define TEST_PASSED 0
#define TEST_FAILED 1

int test_basic_message() {
    int result = fmtmsg(MM_PRINT | MM_SOFT | MM_ERROR, "testapp", MM_ERROR,
                       "Test error message", "Check configuration", "TAG001");
    
    if (result < 0) {
        printf("Basic message test failed with result %d\n", result);
        return TEST_FAILED;
    }
    
    printf("Basic message test passed\n");
    return TEST_PASSED;
}

int test_null_values() {
    int result = fmtmsg(MM_PRINT, MM_NULLLBL, MM_NULLSEV,
                       "Message with nulls", MM_NULLACT, MM_NULLTAG);
    
    if (result < 0) {
        printf("Null values test failed with result %d\n", result);
        return TEST_FAILED;
    }
    
    printf("Null values test passed\n");
    return TEST_PASSED;
}

int test_severity_levels() {
    int results[4];
    results[0] = fmtmsg(MM_PRINT, "test", MM_HALT, "Halt message", NULL, NULL);
    results[1] = fmtmsg(MM_PRINT, "test", MM_ERROR, "Error message", NULL, NULL);
    results[2] = fmtmsg(MM_PRINT, "test", MM_WARNING, "Warning message", NULL, NULL);
    results[3] = fmtmsg(MM_PRINT, "test", MM_INFO, "Info message", NULL, NULL);
    
    for (int i = 0; i < 4; i++) {
        if (results[i] < 0) {
            printf("Severity levels test failed at index %d with result %d\n", i, results[i]);
            return TEST_FAILED;
        }
    }
    
    printf("Severity levels test passed\n");
    return TEST_PASSED;
}

int main() {
    int failures = 0;
    
    printf("===== fmtmsg Test Suite =====\n");
    
    printf("\n[TEST] Basic message\n");
    failures += test_basic_message();
    
    printf("\n[TEST] Null values\n");
    failures += test_null_values();
    
    printf("\n[TEST] Severity levels\n");
    failures += test_severity_levels();
    
    printf("\n===== Test Suite Completed =====\n");
    printf("%d tests failed\n", failures);
    
    return failures ? TEST_FAILED : TEST_PASSED;
}