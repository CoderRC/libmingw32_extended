#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <errno.h>

#define TIMEOUT_MS 2000
#define TEST_PASSED 0
#define TEST_FAILED 1

// Helper function to describe poll events
const char* describe_events(short events) {
    static char buffer[256];
    char* ptr = buffer;
    
    if (events & POLLIN)  ptr += sprintf(ptr, "POLLIN|");
    if (events & POLLRDNORM) ptr += sprintf(ptr, "POLLRDNORM|");
    if (events & POLLRDBAND) ptr += sprintf(ptr, "POLLRDBAND|");
    if (events & POLLPRI) ptr += sprintf(ptr, "POLLPRI|");
    if (events & POLLOUT) ptr += sprintf(ptr, "POLLOUT|");
    if (events & POLLWRNORM) ptr += sprintf(ptr, "POLLWRNORM|");
    if (events & POLLWRBAND) ptr += sprintf(ptr, "POLLWRBAND|");
    if (events & POLLERR) ptr += sprintf(ptr, "POLLERR|");
    if (events & POLLHUP) ptr += sprintf(ptr, "POLLHUP|");
    if (events & POLLNVAL) ptr += sprintf(ptr, "POLLNVAL|");
    
    if (ptr == buffer) return "NONE";
    *(ptr - 1) = '\0';  // Remove trailing '|'
    return buffer;
}

// Test case 1: Basic read readiness
int test_read_ready() {
    int pipefd[2];
    struct pollfd fds[1];
    
    if (pipe(pipefd)) {
        perror("pipe creation failed");
        return TEST_FAILED;
    }
    
    // Set up poll structure
    fds[0].fd = pipefd[0];
    fds[0].events = POLLIN;
    
    // Write data to make pipe readable
    write(pipefd[1], "test", 4);
    
    int ret = poll(fds, 1, TIMEOUT_MS);
    if (ret <= 0) {
        perror("poll() failed for readable pipe");
        close(pipefd[0]);
        close(pipefd[1]);
        return TEST_FAILED;
    }
    
    printf("Read readiness: revents = %s\n", describe_events(fds[0].revents));
    
    if (!(fds[0].revents & POLLIN)) {
        fprintf(stderr, "POLLIN not set for readable pipe\n");
        close(pipefd[0]);
        close(pipefd[1]);
        return TEST_FAILED;
    }
    
    close(pipefd[0]);
    close(pipefd[1]);
    return TEST_PASSED;
}

// Test case 2: Basic write readiness
int test_write_ready() {
    struct pollfd fds[1];
    int fd = open("delete", O_CREAT | O_WRONLY);
    
    if (fd < 0) {
        perror("open delete failed");
        return TEST_FAILED;
    }
    
    fds[0].fd = fd;
    fds[0].events = POLLOUT;
    
    int ret = poll(fds, 1, TIMEOUT_MS);
    if (ret <= 0) {
        perror("poll() failed for writable file");
        close(fd);
        return TEST_FAILED;
    }
    
    printf("Write readiness: revents = %s\n", describe_events(fds[0].revents));
    
    if (!(fds[0].revents & POLLOUT)) {
        fprintf(stderr, "POLLOUT not set for writable file\n");
        close(fd);
        return TEST_FAILED;
    }
    
    close(fd);
    return TEST_PASSED;
}

// Test case 3: Timeout handling
int test_timeout() {
    struct pollfd fds[1];
    int pipefd[2];
    
    if (pipe(pipefd)) {
        perror("pipe creation failed");
        return TEST_FAILED;
    }
    
    fds[0].fd = pipefd[0];
    fds[0].events = POLLIN;
    
    time_t start = time(NULL);
    int ret = poll(fds, 1, TIMEOUT_MS);
    time_t elapsed = time(NULL) - start;
    
    if (ret != 0) {
        fprintf(stderr, "poll() returned %d, expected timeout (0)\n", ret);
        close(pipefd[0]);
        close(pipefd[1]);
        return TEST_FAILED;
    }
    
    printf("Timeout: revents = %s\n", describe_events(fds[0].revents));
    
    if (elapsed < TIMEOUT_MS/1000 - 1 || elapsed > TIMEOUT_MS/1000 + 1) {
        fprintf(stderr, "Timeout duration incorrect: %ld seconds\n", elapsed);
        fprintf(stderr, "Fix Timeout\n"); // yes an actual fix must be made to poll.c
        close(pipefd[0]);
        close(pipefd[1]);
        return TEST_FAILED;
    }
    
    close(pipefd[0]);
    close(pipefd[1]);
    return TEST_PASSED;
}

// Test case 4: Invalid file descriptor
int test_invalid_fd() {
    struct pollfd fds[1];
    
    fds[0].fd = 17;//-1;  // Invalid FD
    fds[0].events = POLLIN;
    
    int ret = poll(fds, 1, TIMEOUT_MS);
    if (ret < 0) {
        perror("poll() failed with invalid fd");
        return TEST_FAILED;
    }
    
    printf("Invalid FD: revents = %s\n", describe_events(fds[0].revents));
    
    if (!(fds[0].revents & POLLNVAL)) {
        fprintf(stderr, "POLLNVAL not set for invalid fd\n");
        return TEST_FAILED;
    }
    
    return TEST_PASSED;
}

// Test case 5: ppoll with signal mask probs will not compile in phase 1 of libmingw32_extended only phase 2 since signal.h must be overrided to work
//int test_ppoll_sigmask() {
//    struct pollfd fds[1];
//    sigset_t mask, orig_mask;
//    struct timespec timeout = { .tv_sec = TIMEOUT_MS / 1000, .tv_nsec = 0 };
//    
//    // Block SIGUSR1
//    sigemptyset(&mask);
//    sigaddset(&mask, SIGUSR1);
//    if (sigprocmask(SIG_BLOCK, &mask, &orig_mask) < 0) {
//        perror("sigprocmask failed");
//        return TEST_FAILED;
//    }
//    
//    fds[0].fd = STDIN_FILENO;
//    fds[0].events = POLLIN;
//    
//    // Should return immediately since signal is blocked
//    int ret = ppoll(fds, 1, &timeout, &mask);
//    if (ret < 0) {
//        perror("ppoll() failed");
//        sigprocmask(SIG_SETMASK, &orig_mask, NULL);
//        return TEST_FAILED;
//    }
//    
//    sigprocmask(SIG_SETMASK, &orig_mask, NULL);
//    return TEST_PASSED;
//}

// Test case 6: Multiple file descriptors
int test_multiple_fds() {
    int pipe1[2], pipe2[2];
    struct pollfd fds[2];
    
    if (pipe(pipe1) || pipe(pipe2)) {
        perror("pipe creation failed");
        return TEST_FAILED;
    }
    
    // Set up first pipe (readable)
    fds[0].fd = pipe1[0];
    fds[0].events = POLLIN;
    write(pipe1[1], "test", 4);
    
    // Set up second pipe (writable)
    fds[1].fd = pipe2[1];
    fds[1].events = POLLOUT;
    
    int ret = poll(fds, 2, TIMEOUT_MS);
    if (ret <= 0) {
        perror("poll() failed for multiple fds");
        close(pipe1[0]); close(pipe1[1]);
        close(pipe2[0]); close(pipe2[1]);
        return TEST_FAILED;
    }
    
    printf("Multiple FDs:\n");
    printf("  FD %d: revents = %s %d\n", fds[0].fd, describe_events(fds[0].revents), fds[0].revents);
    printf("  FD %d: revents = %s %d\n", fds[1].fd, describe_events(fds[1].revents), fds[1].revents);
    
    int passed = TEST_PASSED;
    if (!(fds[0].revents & POLLIN)) {
        fprintf(stderr, "POLLIN not set for readable pipe\n");
        passed = TEST_FAILED;
    }
    if (!(fds[1].revents & POLLOUT)) {
        fprintf(stderr, "POLLOUT not set for writable pipe\n");
        passed = TEST_FAILED;
    }
    
    close(pipe1[0]); close(pipe1[1]);
    close(pipe2[0]); close(pipe2[1]);
    return passed;
}

int main() {
    int failures = 0;
    
    printf("===== Starting poll.h test suite =====\n");
    
    // Run test cases
    printf("\n[TEST] Read readiness test\n");
    failures += test_read_ready();
    
    printf("\n[TEST] Write readiness test\n");
    failures += test_write_ready();
    
    printf("\n[TEST] Timeout test\n");
    failures += test_timeout();
    
    printf("\n[TEST] Invalid FD test\n");
    failures += test_invalid_fd();
    
    printf("\n[TEST] Multiple FDs test\n");
    failures += test_multiple_fds();
    
    //1printf("\n[TEST] ppoll with signal mask\n");
    //failures += test_ppoll_sigmask();
    
    printf("\n===== Test suite completed =====\n");
    printf("%d tests failed\n", failures);
    
    return failures ? EXIT_FAILURE : EXIT_SUCCESS;
}