#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>

int test_ppoll_basic() {
    int pipefd[2];
    struct pollfd fds[1];
    struct timespec timeout = {2, 0}; // 2 seconds
    
    if (pipe(pipefd)) {
        perror("pipe creation failed");
        return 1;
    }
    
    fds[0].fd = pipefd[0];
    fds[0].events = POLLIN;
    
    write(pipefd[1], "test", 4);
    
    int ret = ppoll(fds, 1, &timeout, NULL);
    if (ret <= 0) {
        perror("ppoll() failed for readable pipe");
        close(pipefd[0]);
        close(pipefd[1]);
        return 1;
    }
    
    printf("ppoll basic test: revents = %d (POLLIN=%d)\n", fds[0].revents, POLLIN);
    
    if (!(fds[0].revents & POLLIN)) {
        fprintf(stderr, "POLLIN not set for readable pipe\n");
        close(pipefd[0]);
        close(pipefd[1]);
        return 1;
    }
    
    close(pipefd[0]);
    close(pipefd[1]);
    return 0;
}

int test_ppoll_timeout() {
    int pipefd[2];
    struct pollfd fds[1];
    struct timespec timeout = {1, 500000000}; // 1.5 seconds
    
    if (pipe(pipefd)) {
        perror("pipe creation failed");
        return 1;
    }
    
    fds[0].fd = pipefd[0];
    fds[0].events = POLLIN;
    
    time_t start = time(NULL);
    int ret = ppoll(fds, 1, &timeout, NULL);
    time_t elapsed = time(NULL) - start;
    
    printf("ppoll timeout test: returned %d in %ld seconds\n", ret, elapsed);
    
    close(pipefd[0]);
    close(pipefd[1]);
    
    if (ret != 0 || elapsed < 1 || elapsed > 3) {
        fprintf(stderr, "ppoll timeout test failed\n");
        return 1;
    }
    
    return 0;
}

int main() {
    int failures = 0;
    
    printf("===== ppoll test suite =====\n");
    
    printf("\n[TEST] ppoll basic functionality\n");
    failures += test_ppoll_basic();
    
    printf("\n[TEST] ppoll timeout\n");
    failures += test_ppoll_timeout();
    
    printf("\n===== ppoll test suite completed =====\n");
    printf("%d tests failed\n", failures);
    
    return failures ? EXIT_FAILURE : EXIT_SUCCESS;
}