#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TEST_PASSED 0
#define TEST_FAILED 1

int test_mq_open_close() {
    mqd_t mq = mq_open("/test_queue", O_CREAT | O_RDWR, 0644);
    
    if (mq == (mqd_t)-1) {
        printf("mq_open test failed - returned -1\n");
        return TEST_FAILED;
    }
    
    int result = mq_close(mq);
    if (result != 0) {
        printf("mq_close test failed - returned %d\n", result);
        return TEST_FAILED;
    }
    
    printf("mq_open/close test passed\n");
    return TEST_PASSED;
}

int test_mq_send_receive() {
    mqd_t mq = mq_open("/test_queue2", O_CREAT | O_RDWR, 0644);
    
    if (mq == (mqd_t)-1) {
        printf("mq_send_receive test failed - mq_open returned -1\n");
        return TEST_FAILED;
    }
    
    const char *msg = "test message";
    int send_result = mq_send(mq, msg, strlen(msg), 0);
    
    if (send_result != 0) {
        printf("mq_send test failed - returned %d\n", send_result);
        mq_close(mq);
        return TEST_FAILED;
    }
    
    char buffer[100];
    unsigned prio;
    ssize_t recv_result = mq_receive(mq, buffer, sizeof(buffer), &prio);
    
    if (recv_result < 0) {
        printf("mq_receive test failed - returned %ld\n", (long)recv_result);
        mq_close(mq);
        return TEST_FAILED;
    }
    
    mq_close(mq);
    printf("mq_send/receive test passed\n");
    return TEST_PASSED;
}

int test_mq_getattr() {
    mqd_t mq = mq_open("/test_queue3", O_CREAT | O_RDWR, 0644);
    
    if (mq == (mqd_t)-1) {
        printf("mq_getattr test failed - mq_open returned -1\n");
        return TEST_FAILED;
    }
    
    struct mq_attr attr;
    int result = mq_getattr(mq, &attr);
    
    if (result != 0) {
        printf("mq_getattr test failed - returned %d\n", result);
        mq_close(mq);
        return TEST_FAILED;
    }
    
    if (attr.mq_maxmsg <= 0 || attr.mq_msgsize <= 0) {
        printf("mq_getattr test failed - invalid attributes\n");
        mq_close(mq);
        return TEST_FAILED;
    }
    
    mq_close(mq);
    printf("mq_getattr test passed - maxmsg: %ld, msgsize: %ld\n", attr.mq_maxmsg, attr.mq_msgsize);
    return TEST_PASSED;
}

int test_mq_unlink() {
    int result = mq_unlink("/test_queue_unlink");
    
    if (result != 0) {
        printf("mq_unlink test failed - returned %d\n", result);
        return TEST_FAILED;
    }
    
    printf("mq_unlink test passed\n");
    return TEST_PASSED;
}

int test_mq_timedsend_timedreceive() {
    mqd_t mq = mq_open("/test_timed_queue", O_CREAT | O_RDWR, 0644);
    
    if (mq == (mqd_t)-1) {
        printf("mq_timedsend test failed - mq_open returned -1\n");
        return TEST_FAILED;
    }
    
    struct timespec timeout = {1, 0}; // 1 second timeout
    const char *msg = "timed message";
    
    int send_result = mq_timedsend(mq, msg, strlen(msg), 0, &timeout);
    if (send_result != 0) {
        printf("mq_timedsend test failed - returned %d\n", send_result);
        mq_close(mq);
        return TEST_FAILED;
    }
    
    char buffer[100];
    unsigned prio;
    ssize_t recv_result = mq_timedreceive(mq, buffer, sizeof(buffer), &prio, &timeout);
    
    if (recv_result < 0) {
        printf("mq_timedreceive test failed - returned %ld\n", (long)recv_result);
        mq_close(mq);
        return TEST_FAILED;
    }
    
    mq_close(mq);
    printf("mq_timedsend/timedreceive test passed\n");
    return TEST_PASSED;
}

int test_mq_setattr() {
    mqd_t mq = mq_open("/test_setattr_queue", O_CREAT | O_RDWR, 0644);
    
    if (mq == (mqd_t)-1) {
        printf("mq_setattr test failed - mq_open returned -1\n");
        return TEST_FAILED;
    }
    
    struct mq_attr new_attr = {0};
    new_attr.mq_flags = O_NONBLOCK;
    
    struct mq_attr old_attr;
    int result = mq_setattr(mq, &new_attr, &old_attr);
    
    if (result != 0) {
        printf("mq_setattr test failed - returned %d\n", result);
        mq_close(mq);
        return TEST_FAILED;
    }
    
    mq_close(mq);
    printf("mq_setattr test passed\n");
    return TEST_PASSED;
}

int main() {
    int failures = 0;
    
    printf("===== mqueue Test Suite =====\n");
    
    printf("\n[TEST] mq_open/close\n");
    failures += test_mq_open_close();
    
    printf("\n[TEST] mq_send/receive\n");
    failures += test_mq_send_receive();
    
    printf("\n[TEST] mq_getattr\n");
    failures += test_mq_getattr();
    
    printf("\n[TEST] mq_unlink\n");
    failures += test_mq_unlink();
    
    printf("\n[TEST] mq_timedsend/timedreceive\n");
    failures += test_mq_timedsend_timedreceive();
    
    printf("\n[TEST] mq_setattr\n");
    failures += test_mq_setattr();
    
    printf("\n===== Test Suite Completed =====\n");
    printf("%d tests failed\n", failures);
    
    return failures ? TEST_FAILED : TEST_PASSED;
}