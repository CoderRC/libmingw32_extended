#include <nl_types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define TEST_PASSED 0
#define TEST_FAILED 1

int test_nl_types_constants() {
    printf("Testing nl_types constants...\n");
    
    if (NL_SETD != 1) {
        printf("NL_SETD test failed - expected 1, got %d\n", NL_SETD);
        return TEST_FAILED;
    }
    
    if (NL_CAT_LOCALE != 1) {
        printf("NL_CAT_LOCALE test failed - expected 1, got %d\n", NL_CAT_LOCALE);
        return TEST_FAILED;
    }
    
    if (NL_SETMAX != 255) {
        printf("NL_SETMAX test failed - expected 255, got %d\n", NL_SETMAX);
        return TEST_FAILED;
    }
    
    if (NL_MSGMAX != 32767) {
        printf("NL_MSGMAX test failed - expected 32767, got %d\n", NL_MSGMAX);
        return TEST_FAILED;
    }
    
    printf("nl_types constants test passed\n");
    return TEST_PASSED;
}

int test_catopen_catclose() {
    printf("Testing catopen/catclose...\n");
    
    nl_catd catd = catopen("test_catalog", NL_CAT_LOCALE);
    
    if (catd == CATD_ERR) {
        printf("catopen test failed - returned CATD_ERR\n");
        return TEST_FAILED;
    }
    
    int result = catclose(catd);
    if (result != 0) {
        printf("catclose test failed - returned %d\n", result);
        return TEST_FAILED;
    }
    
    printf("catopen/catclose test passed\n");
    return TEST_PASSED;
}

int test_catgets_basic() {
    printf("Testing catgets basic functionality...\n");
    
    nl_catd catd = catopen("test_catalog2", NL_CAT_LOCALE);
    
    if (catd == CATD_ERR) {
        printf("catgets test failed - catopen returned CATD_ERR\n");
        return TEST_FAILED;
    }
    
    const char *default_msg = "default message";
    char *result = catgets(catd, 1, 1, default_msg);
    
    if (!result) {
        printf("catgets test failed - returned NULL\n");
        catclose(catd);
        return TEST_FAILED;
    }
    
    // Should return default message since catalog doesn't exist
    if (strcmp(result, default_msg) != 0) {
        printf("catgets test failed - expected '%s', got '%s'\n", default_msg, result);
        catclose(catd);
        return TEST_FAILED;
    }
    
    catclose(catd);
    printf("catgets basic test passed\n");
    return TEST_PASSED;
}

int test_catgets_boundary_conditions() {
    printf("Testing catgets boundary conditions...\n");
    
    nl_catd catd = catopen("boundary_test", NL_CAT_LOCALE);
    if (catd == CATD_ERR) {
        printf("catgets boundary test failed - catopen returned CATD_ERR\n");
        return TEST_FAILED;
    }
    
    const char *default_msg = "boundary default";
    
    // Test maximum set number
    char *result1 = catgets(catd, NL_SETMAX, 1, default_msg);
    if (strcmp(result1, default_msg) != 0) {
        printf("catgets max set test failed\n");
        catclose(catd);
        return TEST_FAILED;
    }
    
    // Test maximum message number
    char *result2 = catgets(catd, 1, NL_MSGMAX, default_msg);
    if (strcmp(result2, default_msg) != 0) {
        printf("catgets max msg test failed\n");
        catclose(catd);
        return TEST_FAILED;
    }
    
    catclose(catd);
    printf("catgets boundary conditions test passed\n");
    return TEST_PASSED;
}

int test_invalid_params() {
    printf("Testing invalid parameters...\n");
    
    // Test catopen with NULL name
    nl_catd catd = catopen(NULL, NL_CAT_LOCALE);
    if (catd != CATD_ERR) {
        printf("catopen null test failed - should return CATD_ERR\n");
        catclose(catd);
        return TEST_FAILED;
    }
    
    // Test catopen with empty name
    catd = catopen("", NL_CAT_LOCALE);
    if (catd != CATD_ERR) {
        printf("catopen empty test failed - should return CATD_ERR\n");
        catclose(catd);
        return TEST_FAILED;
    }
    
    // Test catclose with invalid descriptor
    int result = catclose(CATD_ERR);
    if (result == 0) {
        printf("catclose invalid test failed - should return -1\n");
        return TEST_FAILED;
    }
    
    // Test catgets with invalid parameters
    catd = catopen("valid_catalog", NL_CAT_LOCALE);
    if (catd != CATD_ERR) {
        const char *default_msg = "default";
        
        // Test with negative set_id
        char *result_str = catgets(catd, -1, 1, default_msg);
        if (strcmp(result_str, default_msg) != 0) {
            printf("catgets negative set test failed\n");
            catclose(catd);
            return TEST_FAILED;
        }
        
        // Test with set_id too large
        result_str = catgets(catd, NL_SETMAX + 1, 1, default_msg);
        if (strcmp(result_str, default_msg) != 0) {
            printf("catgets large set test failed\n");
            catclose(catd);
            return TEST_FAILED;
        }
        
        catclose(catd);
    }
    
    printf("Invalid params test passed\n");
    return TEST_PASSED;
}

int test_double_close() {
    printf("Testing double close protection...\n");
    
    nl_catd catd = catopen("double_close_test", NL_CAT_LOCALE);
    if (catd == CATD_ERR) {
        printf("double close test failed - catopen returned CATD_ERR\n");
        return TEST_FAILED;
    }
    
    // First close should succeed
    int result1 = catclose(catd);
    if (result1 != 0) {
        printf("double close test failed - first close returned %d\n", result1);
        return TEST_FAILED;
    }
    
    // Second close should fail
    int result2 = catclose(catd);
    if (result2 == 0) {
        printf("double close test failed - second close should fail\n");
        return TEST_FAILED;
    }
    
    printf("Double close protection test passed\n");
    return TEST_PASSED;
}

int test_catgets_multiple_sets() {
    printf("Testing catgets with multiple sets...\n");
    
    nl_catd catd = catopen("multi_set_test", NL_CAT_LOCALE);
    if (catd == CATD_ERR) {
        printf("multi set test failed - catopen returned CATD_ERR\n");
        return TEST_FAILED;
    }
    
    const char *default1 = "default set 1";
    const char *default2 = "default set 2";
    
    char *result1 = catgets(catd, 1, 1, default1);
    char *result2 = catgets(catd, 2, 1, default2);
    
    if (strcmp(result1, default1) != 0) {
        printf("multi set test failed - set 1 result wrong\n");
        catclose(catd);
        return TEST_FAILED;
    }
    
    if (strcmp(result2, default2) != 0) {
        printf("multi set test failed - set 2 result wrong\n");
        catclose(catd);
        return TEST_FAILED;
    }
    
    catclose(catd);
    printf("Multiple sets test passed\n");
    return TEST_PASSED;
}

int test_catgets_large_message_numbers() {
    printf("Testing catgets with large message numbers...\n");
    
    nl_catd catd = catopen("large_msg_test", NL_CAT_LOCALE);
    if (catd == CATD_ERR) {
        printf("large msg test failed - catopen returned CATD_ERR\n");
        return TEST_FAILED;
    }
    
    const char *default_msg = "large message default";
    
    // Test with large message number
    char *result = catgets(catd, 1, 1000, default_msg);
    if (strcmp(result, default_msg) != 0) {
        printf("large msg test failed - should return default\n");
        catclose(catd);
        return TEST_FAILED;
    }
    
    catclose(catd);
    printf("Large message numbers test passed\n");
    return TEST_PASSED;
}

int main() {
    int failures = 0;
    
    printf("===== nl_types Test Suite =====\n");
    
    printf("\n[TEST] nl_types constants\n");
    failures += test_nl_types_constants();
    
    printf("\n[TEST] catopen/catclose\n");
    failures += test_catopen_catclose();
    
    printf("\n[TEST] catgets basic\n");
    failures += test_catgets_basic();
    
    printf("\n[TEST] catgets boundary conditions\n");
    failures += test_catgets_boundary_conditions();
    
    printf("\n[TEST] Invalid params\n");
    failures += test_invalid_params();
    
    printf("\n[TEST] Double close protection\n");
    failures += test_double_close();
    
    printf("\n[TEST] Multiple sets\n");
    failures += test_catgets_multiple_sets();
    
    printf("\n[TEST] Large message numbers\n");
    failures += test_catgets_large_message_numbers();
    
    printf("\n===== Test Suite Completed =====\n");
    printf("%d tests failed\n", failures);
    
    return failures ? TEST_FAILED : TEST_PASSED;
}