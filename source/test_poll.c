#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define TIMEOUT_MS 2000
#define TEST_PASSED 0
#define TEST_FAILED 1

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
    *(ptr - 1) = '\0';
    return buffer;
}

int test_read_ready() {
    int pipefd[2];
    struct pollfd fds[1];
    
    if (pipe(pipefd)) {
        perror("pipe creation failed");
        return TEST_FAILED;
    }
    
    fds[0].fd = pipefd[0];
    fds[0].events = POLLIN;
    
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
        fprintf(stderr, "Fix Timeout\n");
        close(pipefd[0]);
        close(pipefd[1]);
        return TEST_FAILED;
    }
    
    close(pipefd[0]);
    close(pipefd[1]);
    return TEST_PASSED;
}

int test_invalid_fd() {
    struct pollfd fds[1];
    
    fds[0].fd = 17;
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

int test_multiple_fds() {
    int pipe1[2], pipe2[2];
    struct pollfd fds[2];
    
    if (pipe(pipe1) || pipe(pipe2)) {
        perror("pipe creation failed");
        return TEST_FAILED;
    }
    
    fds[0].fd = pipe1[0];
    fds[0].events = POLLIN;
    write(pipe1[1], "test", 4);
    
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

int test_pipe_hangup() {
    int pipefd[2];
    struct pollfd fds[1];
    
    if (pipe(pipefd) < 0) {
        perror("pipe creation failed");
        return TEST_FAILED;
    }
    
    fds[0].fd = pipefd[0];
    fds[0].events = POLLIN;
    
    close(pipefd[1]);
    
    int ret = poll(fds, 1, TIMEOUT_MS);
    printf("Pipe hangup: revents = %s\n", describe_events(fds[0].revents));
    
    close(pipefd[0]);
    
    if (ret <= 0 || !(fds[0].revents & POLLHUP)) {
        fprintf(stderr, "POLLHUP not detected\n");
        return TEST_FAILED;
    }
    
    return TEST_PASSED;
}

int test_mixed_events() {
    int pipe1[2], pipe2[2];
    struct pollfd fds[3];
    
    if (pipe(pipe1) < 0 || pipe(pipe2) < 0) {
        perror("pipe creation failed");
        return TEST_FAILED;
    }
    
    fds[0].fd = pipe1[0];
    fds[0].events = POLLIN;
    write(pipe1[1], "test", 4);
    
    fds[1].fd = pipe2[1];
    fds[1].events = POLLOUT;
    
    fds[2].fd = 999;
    fds[2].events = POLLIN;
    
    int ret = poll(fds, 3, TIMEOUT_MS);
    printf("Mixed events: %d ready\n", ret);
    
    for (int i = 0; i < 3; i++) {
        printf("  FD %d: revents = %s\n", fds[i].fd, describe_events(fds[i].revents));
    }
    
    close(pipe1[0]); close(pipe1[1]);
    close(pipe2[0]); close(pipe2[1]);
    
    return ret >= 2 ? TEST_PASSED : TEST_FAILED;
}

int test_zero_timeout() {
    int pipefd[2];
    struct pollfd fds[1];
    
    if (pipe(pipefd) < 0) {
        perror("pipe creation failed");
        return TEST_FAILED;
    }
    
    fds[0].fd = pipefd[0];
    fds[0].events = POLLIN;
    
    time_t start = time(NULL);
    int ret = poll(fds, 1, 0);
    time_t elapsed = time(NULL) - start;
    
    printf("Zero timeout: returned %d in %ld seconds\n", ret, elapsed);
    
    close(pipefd[0]);
    close(pipefd[1]);
    
    if (ret != 0 || elapsed > 1) {
        fprintf(stderr, "Zero timeout failed\n");
        return TEST_FAILED;
    }
    
    return TEST_PASSED;
}

int test_socket_read() {
    int server_fd, client_fd, conn_fd;
    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);
    
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket creation failed");
        return TEST_FAILED;
    }
    
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = 0;
    
    if (bind(server_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind failed");
        close(server_fd);
        return TEST_FAILED;
    }
    
    if (listen(server_fd, 1) < 0) {
        perror("listen failed");
        close(server_fd);
        return TEST_FAILED;
    }
    
    if (getsockname(server_fd, (struct sockaddr*)&addr, &addr_len) < 0) {
        perror("getsockname failed");
        close(server_fd);
        return TEST_FAILED;
    }
    
    client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_fd < 0) {
        perror("client socket creation failed");
        close(server_fd);
        return TEST_FAILED;
    }
    
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    if (connect(client_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("connect failed");
        close(server_fd);
        close(client_fd);
        return TEST_FAILED;
    }
    
    conn_fd = accept(server_fd, NULL, NULL);
    if (conn_fd < 0) {
        perror("accept failed");
        close(server_fd);
        close(client_fd);
        return TEST_FAILED;
    }
    
    if (send(client_fd, "test", 4, 0) < 0) {
        perror("send failed");
        close(server_fd);
        close(client_fd);
        close(conn_fd);
        return TEST_FAILED;
    }
    
    struct pollfd fds[1];
    fds[0].fd = conn_fd;
    fds[0].events = POLLIN;
    
    int ret = poll(fds, 1, TIMEOUT_MS);
    printf("Socket read: revents = %s\n", describe_events(fds[0].revents));
    
    close(server_fd);
    close(client_fd);
    close(conn_fd);
    
    if (ret <= 0 || !(fds[0].revents & POLLIN)) {
        fprintf(stderr, "Socket read test failed\n");
        return TEST_FAILED;
    }
    
    return TEST_PASSED;
}

int test_socket_write() {
    int sock_fd;
    struct sockaddr_in addr;
    
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0) {
        perror("socket creation failed");
        return TEST_FAILED;
    }
    
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    addr.sin_port = htons(80);
    
    struct pollfd fds[1];
    fds[0].fd = sock_fd;
    fds[0].events = POLLOUT;
    
    int ret = poll(fds, 1, 1000);
    printf("Socket write: revents = %s\n", describe_events(fds[0].revents));
    
    close(sock_fd);
    
    if (ret < 0) {
        fprintf(stderr, "Socket write test failed\n");
        return TEST_FAILED;
    }
    
    return TEST_PASSED;
}

int test_ppoll_basic() {
    int pipefd[2];
    struct pollfd fds[1];
    struct timespec timeout = {2, 0};
    
    if (pipe(pipefd)) {
        perror("pipe creation failed");
        return TEST_FAILED;
    }
    
    fds[0].fd = pipefd[0];
    fds[0].events = POLLIN;
    
    write(pipefd[1], "test", 4);
    
    int ret = ppoll(fds, 1, &timeout, NULL);
    if (ret <= 0) {
        perror("ppoll() failed for readable pipe");
        close(pipefd[0]);
        close(pipefd[1]);
        return TEST_FAILED;
    }
    
    printf("ppoll basic: revents = %s\n", describe_events(fds[0].revents));
    
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

int main() {
    int failures = 0;
    
    printf("===== Enhanced poll.h test suite =====\n");
    
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
    
    printf("\n[TEST] Pipe hangup test\n");
    failures += test_pipe_hangup();
    
    printf("\n[TEST] Mixed events test\n");
    failures += test_mixed_events();
    
    printf("\n[TEST] Zero timeout test\n");
    failures += test_zero_timeout();
    
    printf("\n[TEST] Socket read test\n");
    failures += test_socket_read();
    
    printf("\n[TEST] Socket write test\n");
    failures += test_socket_write();
    
    printf("\n[TEST] ppoll basic test\n");
    failures += test_ppoll_basic();
    
    printf("\n===== Test suite completed =====\n");
    printf("%d tests failed\n", failures);
    
    return failures ? EXIT_FAILURE : EXIT_SUCCESS;
}