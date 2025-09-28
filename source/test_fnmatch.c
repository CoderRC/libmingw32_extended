#include <fnmatch.h>
#include <stdio.h>

#define TEST_PASSED 0
#define TEST_FAILED 1

int test_basic_patterns() {
    if (fnmatch("*.txt", "file.txt", 0) != 0) return TEST_FAILED;
    if (fnmatch("*.txt", "file.doc", 0) == 0) return TEST_FAILED;
    if (fnmatch("test?", "test1", 0) != 0) return TEST_FAILED;
    if (fnmatch("test?", "test", 0) == 0) return TEST_FAILED;
    
    printf("Basic patterns test passed\n");
    return TEST_PASSED;
}

int test_case_insensitive() {
    if (fnmatch("*.TXT", "file.txt", FNM_CASEFOLD) != 0) return TEST_FAILED;
    if (fnmatch("TEST*", "test.file", FNM_IGNORECASE) != 0) return TEST_FAILED;
    if (fnmatch("*.TXT", "file.txt", 0) == 0) return TEST_FAILED;
    
    printf("Case insensitive test passed\n");
    return TEST_PASSED;
}

int test_pathname_flag() {
    /* Test that ? doesn't match / with FNM_PATHNAME */
    if (fnmatch("dir?file", "dir/file", FNM_PATHNAME) == 0) return TEST_FAILED;
    if (fnmatch("dir?file", "dir/file", 0) != 0) return TEST_FAILED;
    
    /* Test basic directory matching */
    if (fnmatch("dir/*", "dir/file", FNM_PATHNAME) != 0) return TEST_FAILED;
    
    printf("Pathname flag test passed\n");
    return TEST_PASSED;
}

int test_period_flag() {
    /* Test that ? doesn't match leading . with FNM_PERIOD */
    if (fnmatch("?hidden", ".hidden", FNM_PERIOD) == 0) return TEST_FAILED;
    if (fnmatch("?hidden", ".hidden", 0) != 0) return TEST_FAILED;
    
    /* Test explicit . matching */
    if (fnmatch(".*", ".hidden", FNM_PERIOD) != 0) return TEST_FAILED;
    
    printf("Period flag test passed\n");
    return TEST_PASSED;
}

int test_character_classes() {
    if (fnmatch("[abc]", "b", 0) != 0) return TEST_FAILED;
    if (fnmatch("[abc]", "d", 0) == 0) return TEST_FAILED;
    if (fnmatch("[a-z]", "m", 0) != 0) return TEST_FAILED;
    if (fnmatch("[!abc]", "d", 0) != 0) return TEST_FAILED;
    if (fnmatch("[!abc]", "a", 0) == 0) return TEST_FAILED;
    
    printf("Character classes test passed\n");
    return TEST_PASSED;
}

int main() {
    int failures = 0;
    
    printf("===== fnmatch Test Suite =====\n");
    
    printf("\n[TEST] Basic patterns\n");
    failures += test_basic_patterns();
    
    printf("\n[TEST] Case insensitive\n");
    failures += test_case_insensitive();
    
    printf("\n[TEST] Pathname flag\n");
    failures += test_pathname_flag();
    
    printf("\n[TEST] Period flag\n");
    failures += test_period_flag();
    
    printf("\n[TEST] Character classes\n");
    failures += test_character_classes();
    
    printf("\n===== Test Suite Completed =====\n");
    printf("%d tests failed\n", failures);
    
    return failures ? TEST_FAILED : TEST_PASSED;
}