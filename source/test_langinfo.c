#include <langinfo.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TEST_PASSED 0
#define TEST_FAILED 1

typedef struct {
    char *name;
    int category;
} locale_data_t;

int test_nl_langinfo_basic() {
    char *result = nl_langinfo(DAY_1);
    if (!result || strcmp(result, "Sunday") != 0) {
        printf("nl_langinfo basic test failed - expected Sunday, got %s\n", result ? result : "NULL");
        return TEST_FAILED;
    }
    
    result = nl_langinfo(MON_1);
    if (!result || strcmp(result, "January") != 0) {
        printf("nl_langinfo basic test failed - expected January, got %s\n", result ? result : "NULL");
        return TEST_FAILED;
    }
    
    printf("nl_langinfo basic test passed\n");
    return TEST_PASSED;
}

int test_nl_langinfo_l_null() {
    char *result = nl_langinfo_l(DAY_1, NULL);
    if (!result || strcmp(result, "Sunday") != 0) {
        printf("nl_langinfo_l null test failed - expected Sunday, got %s\n", result ? result : "NULL");
        return TEST_FAILED;
    }
    
    printf("nl_langinfo_l null test passed\n");
    return TEST_PASSED;
}

int test_chinese_locale() {
    locale_data_t zh_locale = {"zh_CN", 0};
    
    char *result = nl_langinfo_l(DAY_1, (locale_t)&zh_locale);
    if (!result || strcmp(result, "星期日") != 0) {
        printf("Chinese locale test failed - expected 星期日, got %s\n", result ? result : "NULL");
        return TEST_FAILED;
    }
    
    result = nl_langinfo_l(MON_1, (locale_t)&zh_locale);
    if (!result || strcmp(result, "一月") != 0) {
        printf("Chinese locale test failed - expected 一月, got %s\n", result ? result : "NULL");
        return TEST_FAILED;
    }
    
    printf("Chinese locale test passed\n");
    return TEST_PASSED;
}

int test_spanish_locale() {
    locale_data_t es_locale = {"es_ES", 0};
    
    char *result = nl_langinfo_l(DAY_1, (locale_t)&es_locale);
    if (!result || strcmp(result, "domingo") != 0) {
        printf("Spanish locale test failed - expected domingo, got %s\n", result ? result : "NULL");
        return TEST_FAILED;
    }
    
    result = nl_langinfo_l(MON_1, (locale_t)&es_locale);
    if (!result || strcmp(result, "enero") != 0) {
        printf("Spanish locale test failed - expected enero, got %s\n", result ? result : "NULL");
        return TEST_FAILED;
    }
    
    printf("Spanish locale test passed\n");
    return TEST_PASSED;
}

int test_french_locale() {
    locale_data_t fr_locale = {"fr_FR", 0};
    
    char *result = nl_langinfo_l(DAY_1, (locale_t)&fr_locale);
    if (!result || strcmp(result, "dimanche") != 0) {
        printf("French locale test failed - expected dimanche, got %s\n", result ? result : "NULL");
        return TEST_FAILED;
    }
    
    result = nl_langinfo_l(CRNCYSTR, (locale_t)&fr_locale);
    if (!result || strcmp(result, "€") != 0) {
        printf("French currency test failed - expected €, got %s\n", result ? result : "NULL");
        return TEST_FAILED;
    }
    
    printf("French locale test passed\n");
    return TEST_PASSED;
}

int test_russian_locale() {
    locale_data_t ru_locale = {"ru_RU", 0};
    
    char *result = nl_langinfo_l(DAY_1, (locale_t)&ru_locale);
    if (!result || strcmp(result, "воскресенье") != 0) {
        printf("Russian locale test failed - expected воскресенье, got %s\n", result ? result : "NULL");
        return TEST_FAILED;
    }
    
    printf("Russian locale test passed\n");
    return TEST_PASSED;
}

int test_japanese_locale() {
    locale_data_t ja_locale = {"ja_JP", 0};
    
    char *result = nl_langinfo_l(DAY_1, (locale_t)&ja_locale);
    if (!result || strcmp(result, "日曜日") != 0) {
        printf("Japanese locale test failed - expected 日曜日, got %s\n", result ? result : "NULL");
        return TEST_FAILED;
    }
    
    result = nl_langinfo_l(MON_1, (locale_t)&ja_locale);
    if (!result || strcmp(result, "1月") != 0) {
        printf("Japanese locale test failed - expected 1月, got %s\n", result ? result : "NULL");
        return TEST_FAILED;
    }
    
    printf("Japanese locale test passed\n");
    return TEST_PASSED;
}

int test_numeric_formatting() {
    locale_data_t fr_locale = {"fr_FR", 0};
    
    char *result = nl_langinfo_l(RADIXCHAR, (locale_t)&fr_locale);
    if (!result || strcmp(result, ",") != 0) {
        printf("French radix test failed - expected ',', got %s\n", result ? result : "NULL");
        return TEST_FAILED;
    }
    
    result = nl_langinfo_l(THOUSEP, (locale_t)&fr_locale);
    if (!result || strcmp(result, " ") != 0) {
        printf("French thousands test failed - expected ' ', got %s\n", result ? result : "NULL");
        return TEST_FAILED;
    }
    
    printf("Numeric formatting test passed\n");
    return TEST_PASSED;
}

int main() {
    int failures = 0;
    
    printf("===== langinfo Test Suite =====\n");
    
    printf("\n[TEST] nl_langinfo basic\n");
    failures += test_nl_langinfo_basic();
    
    printf("\n[TEST] nl_langinfo_l with NULL\n");
    failures += test_nl_langinfo_l_null();
    
    printf("\n[TEST] Chinese locale\n");
    failures += test_chinese_locale();
    
    printf("\n[TEST] Spanish locale\n");
    failures += test_spanish_locale();
    
    printf("\n[TEST] French locale\n");
    failures += test_french_locale();
    
    printf("\n[TEST] Russian locale\n");
    failures += test_russian_locale();
    
    printf("\n[TEST] Japanese locale\n");
    failures += test_japanese_locale();
    
    printf("\n[TEST] Numeric formatting\n");
    failures += test_numeric_formatting();
    
    printf("\n===== Test Suite Completed =====\n");
    printf("%d tests failed\n", failures);
    
    return failures ? TEST_FAILED : TEST_PASSED;
}