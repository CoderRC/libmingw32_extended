#include <glob.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TEST_PASSED 0
#define TEST_FAILED 1

int test_basic_glob() {
    glob_t g;
    int result = glob("*.exe", 0, NULL, &g);
    
    if (result != 0) {
        printf("Basic glob failed with result %d\n", result);
        return TEST_FAILED;
    }
    
    printf("Found %zu .exe files\n", g.gl_pathc);
    for (size_t i = 0; i < g.gl_pathc; i++) {
        printf("  %s\n", g.gl_pathv[i]);
    }
    
    globfree(&g);
    printf("Basic glob test passed\n");
    return TEST_PASSED;
}

int test_nocheck_flag() {
    glob_t g;
    int result = glob("nonexistent_pattern_*.xyz", GLOB_NOCHECK, NULL, &g);
    
    if (result != 0) {
        printf("NOCHECK flag test failed with result %d\n", result);
        return TEST_FAILED;
    }
    
    if (g.gl_pathc != 1 || strcmp(g.gl_pathv[0], "nonexistent_pattern_*.xyz") != 0) {
        printf("NOCHECK flag test failed - expected pattern returned\n");
        globfree(&g);
        return TEST_FAILED;
    }
    
    globfree(&g);
    printf("NOCHECK flag test passed\n");
    return TEST_PASSED;
}

int test_mark_flag() {
    glob_t g;
    int result = glob("*", GLOB_MARK, NULL, &g);
    
    if (result != 0) {
        printf("MARK flag test failed with result %d\n", result);
        return TEST_FAILED;
    }
    
    printf("Found %zu entries with GLOB_MARK:\n", g.gl_pathc);
    for (size_t i = 0; i < g.gl_pathc; i++) {
        printf("  %s\n", g.gl_pathv[i]);
    }
    
    globfree(&g);
    printf("MARK flag test passed\n");
    return TEST_PASSED;
}

int test_nomatch() {
    glob_t g;
    int result = glob("*.nonexistent_extension", 0, NULL, &g);
    
    if (result != GLOB_NOMATCH) {
        printf("NOMATCH test failed - expected GLOB_NOMATCH, got %d\n", result);
        if (result == 0) globfree(&g);
        return TEST_FAILED;
    }
    
    printf("NOMATCH test passed\n");
    return TEST_PASSED;
}

int main() {
    int failures = 0;
    
    printf("===== glob Test Suite =====\n");
    
    printf("\n[TEST] Basic glob\n");
    failures += test_basic_glob();
    
    printf("\n[TEST] NOCHECK flag\n");
    failures += test_nocheck_flag();
    
    printf("\n[TEST] MARK flag\n");
    failures += test_mark_flag();
    
    printf("\n[TEST] No match\n");
    failures += test_nomatch();
    
    printf("\n===== Test Suite Completed =====\n");
    printf("%d tests failed\n", failures);
    
    return failures ? TEST_FAILED : TEST_PASSED;
}