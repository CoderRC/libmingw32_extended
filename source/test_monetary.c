#include <monetary.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TEST_PASSED 0
#define TEST_FAILED 1

typedef struct {
    char *name;
    int category;
} locale_data_t;

int test_strfmon_basic() {
    char buffer[100];
    ssize_t result = strfmon(buffer, sizeof(buffer), "%n", 123.45);
    
    if (result < 0) {
        printf("strfmon basic test failed - returned %ld\n", (long)result);
        return TEST_FAILED;
    }
    
    if (strstr(buffer, "123.45") == NULL) {
        printf("strfmon basic test failed - expected 123.45 in '%s'\n", buffer);
        return TEST_FAILED;
    }
    
    printf("strfmon basic test passed - result: '%s'\n", buffer);
    return TEST_PASSED;
}

int test_strfmon_international() {
    char buffer[100];
    ssize_t result = strfmon(buffer, sizeof(buffer), "%i", 456.78);
    
    if (result < 0) {
        printf("strfmon international test failed - returned %ld\n", (long)result);
        return TEST_FAILED;
    }
    
    if (strstr(buffer, "USD") == NULL || strstr(buffer, "456.78") == NULL) {
        printf("strfmon international test failed - expected USD 456.78 in '%s'\n", buffer);
        return TEST_FAILED;
    }
    
    printf("strfmon international test passed - result: '%s'\n", buffer);
    return TEST_PASSED;
}

int test_strfmon_l_locale() {
    char buffer[100];
    locale_data_t es_locale = {"es_ES", 0};
    
    ssize_t result = strfmon_l(buffer, sizeof(buffer), (locale_t)&es_locale, "%n", 789.12);
    
    if (result < 0) {
        printf("strfmon_l locale test failed - returned %ld\n", (long)result);
        return TEST_FAILED;
    }
    
    if (strstr(buffer, "789.12") == NULL) {
        printf("strfmon_l locale test failed - expected 789.12 in '%s'\n", buffer);
        return TEST_FAILED;
    }
    
    printf("strfmon_l locale test passed - result: '%s'\n", buffer);
    return TEST_PASSED;
}

int test_strfmon_precision() {
    char buffer[100];
    ssize_t result = strfmon(buffer, sizeof(buffer), "%.2n", 123.456);
    
    if (result < 0) {
        printf("strfmon precision test failed - returned %ld\n", (long)result);
        return TEST_FAILED;
    }
    
    printf("strfmon precision test passed - result: '%s'\n", buffer);
    return TEST_PASSED;
}

int test_strfmon_width() {
    char buffer[100];
    ssize_t result = strfmon(buffer, sizeof(buffer), "%10n", 42.0);
    
    if (result < 0) {
        printf("strfmon width test failed - returned %ld\n", (long)result);
        return TEST_FAILED;
    }
    
    printf("strfmon width test passed - result: '%s'\n", buffer);
    return TEST_PASSED;
}

int test_strfmon_null_params() {
    char buffer[100];
    ssize_t result = strfmon(NULL, sizeof(buffer), "%n", 123.45);
    
    if (result != -1) {
        printf("strfmon null test failed - should return -1, got %ld\n", (long)result);
        return TEST_FAILED;
    }
    
    printf("strfmon null test passed\n");
    return TEST_PASSED;
}

int main() {
    int failures = 0;
    
    printf("===== monetary Test Suite =====\n");
    
    printf("\n[TEST] strfmon basic\n");
    failures += test_strfmon_basic();
    
    printf("\n[TEST] strfmon international\n");
    failures += test_strfmon_international();
    
    printf("\n[TEST] strfmon_l locale\n");
    failures += test_strfmon_l_locale();
    
    printf("\n[TEST] strfmon null params\n");
    failures += test_strfmon_null_params();
    
    printf("\n[TEST] strfmon precision\n");
    failures += test_strfmon_precision();
    
    printf("\n[TEST] strfmon width\n");
    failures += test_strfmon_width();
    
    printf("\n===== Test Suite Completed =====\n");
    printf("%d tests failed\n", failures);
    
    return failures ? TEST_FAILED : TEST_PASSED;
}