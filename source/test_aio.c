#include <aio.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <time.h>

#define TEST_PASSED 0
#define TEST_FAILED 1
#define TEST_FILE "aio_test.dat"
#define TEST_DATA "AIO Test Data"
#define LARGE_SIZE 8192

int test_aio_read() {
    int fd = open(TEST_FILE, O_CREAT | O_WRONLY | O_TRUNC, 0644);
    write(fd, TEST_DATA, strlen(TEST_DATA));
    close(fd);
    
    fd = open(TEST_FILE, O_RDONLY);
    struct aiocb cb = {0};
    char buffer[64] = {0};
    
    cb.aio_fildes = fd;
    cb.aio_buf = buffer;
    cb.aio_nbytes = strlen(TEST_DATA);
    
    if (aio_read(&cb) != 0) {
        close(fd);
        return TEST_FAILED;
    }
    
    while (aio_error(&cb) == EINPROGRESS) usleep(1000);
    
    if (aio_error(&cb) != 0 || aio_return(&cb) != strlen(TEST_DATA) || strcmp(buffer, TEST_DATA) != 0) {
        close(fd);
        return TEST_FAILED;
    }
    
    close(fd);
    printf("aio_read test passed\n");
    return TEST_PASSED;
}

int test_aio_write() {
    int fd = open(TEST_FILE, O_CREAT | O_WRONLY | O_TRUNC, 0644);
    struct aiocb cb = {0};
    
    cb.aio_fildes = fd;
    cb.aio_buf = TEST_DATA;
    cb.aio_nbytes = strlen(TEST_DATA);
    
    if (aio_write(&cb) != 0) {
        close(fd);
        return TEST_FAILED;
    }
    
    while (aio_error(&cb) == EINPROGRESS) usleep(1000);
    
    if (aio_error(&cb) != 0 || aio_return(&cb) != strlen(TEST_DATA)) {
        close(fd);
        return TEST_FAILED;
    }
    
    close(fd);
    printf("aio_write test passed\n");
    return TEST_PASSED;
}

int test_aio_fsync() {
    int fd = open(TEST_FILE, O_CREAT | O_WRONLY | O_TRUNC, 0644);
    write(fd, TEST_DATA, strlen(TEST_DATA));
    
    struct aiocb cb = {0};
    cb.aio_fildes = fd;
    
    if (aio_fsync(0, &cb) != 0) {
        close(fd);
        return TEST_FAILED;
    }
    
    while (aio_error(&cb) == EINPROGRESS) usleep(1000);
    
    if (aio_error(&cb) != 0) {
        close(fd);
        return TEST_FAILED;
    }
    
    close(fd);
    printf("aio_fsync test passed\n");
    return TEST_PASSED;
}

int test_aio_suspend() {
    int fd = open(TEST_FILE, O_CREAT | O_WRONLY | O_TRUNC, 0644);
    struct aiocb cb = {0};
    
    cb.aio_fildes = fd;
    cb.aio_buf = TEST_DATA;
    cb.aio_nbytes = strlen(TEST_DATA);
    
    if (aio_write(&cb) != 0) {
        close(fd);
        return TEST_FAILED;
    }
    
    const struct aiocb *list[] = {&cb};
    struct timespec timeout = {2, 0};
    
    if (aio_suspend(list, 1, &timeout) != 0 || aio_error(&cb) != 0) {
        close(fd);
        return TEST_FAILED;
    }
    
    close(fd);
    printf("aio_suspend test passed\n");
    return TEST_PASSED;
}

int test_lio_listio() {
    int fd1 = open("test1.dat", O_CREAT | O_RDWR | O_TRUNC, 0644);
    int fd2 = open("test2.dat", O_CREAT | O_RDWR | O_TRUNC, 0644);
    write(fd1, TEST_DATA, strlen(TEST_DATA));
    lseek(fd1, 0, SEEK_SET);
    
    struct aiocb cb1 = {0}, cb2 = {0};
    char buffer[64] = {0};
    
    cb1.aio_fildes = fd1;
    cb1.aio_buf = buffer;
    cb1.aio_nbytes = strlen(TEST_DATA);
    cb1.aio_lio_opcode = LIO_READ;
    
    cb2.aio_fildes = fd2;
    cb2.aio_buf = "Write Test";
    cb2.aio_nbytes = 10;
    cb2.aio_lio_opcode = LIO_WRITE;
    
    struct aiocb *list[] = {&cb1, &cb2};
    
    if (lio_listio(LIO_WAIT, list, 2, NULL) != 0 || aio_error(&cb1) != 0 || aio_error(&cb2) != 0) {
        close(fd1); close(fd2);
        return TEST_FAILED;
    }
    
    close(fd1); close(fd2);
    unlink("test1.dat"); unlink("test2.dat");
    printf("lio_listio test passed\n");
    return TEST_PASSED;
}

int test_aio_cancel() {
    int fd = open(TEST_FILE, O_CREAT | O_WRONLY | O_TRUNC, 0644);
    struct aiocb cb = {0};
    
    cb.aio_fildes = fd;
    cb.aio_buf = TEST_DATA;
    cb.aio_nbytes = strlen(TEST_DATA);
    
    if (aio_write(&cb) != 0) {
        close(fd);
        return TEST_FAILED;
    }
    
    int result = aio_cancel(fd, &cb);
    if (result != AIO_CANCELED && result != AIO_ALLDONE && result != AIO_NOTCANCELED) {
        close(fd);
        return TEST_FAILED;
    }
    
    close(fd);
    printf("aio_cancel test passed\n");
    return TEST_PASSED;
}

int test_invalid_params() {
    struct aiocb cb = {0};
    cb.aio_fildes = -1;
    
    if (aio_read(&cb) == 0 || aio_write(&cb) == 0 || aio_fsync(0, &cb) == 0) {
        return TEST_FAILED;
    }
    
    if (aio_read(NULL) == 0 || aio_write(NULL) == 0 || aio_fsync(0, NULL) == 0) {
        return TEST_FAILED;
    }
    
    if (aio_suspend(NULL, 1, NULL) == 0 || lio_listio(LIO_WAIT, NULL, 1, NULL) == 0) {
        return TEST_FAILED;
    }
    
    printf("Invalid parameters test passed\n");
    return TEST_PASSED;
}

int test_large_io() {
    int fd = open(TEST_FILE, O_CREAT | O_WRONLY | O_TRUNC, 0644);
    char *large_data = malloc(LARGE_SIZE);
    memset(large_data, 'A', LARGE_SIZE);
    
    struct aiocb cb = {0};
    cb.aio_fildes = fd;
    cb.aio_buf = large_data;
    cb.aio_nbytes = LARGE_SIZE;
    
    if (aio_write(&cb) != 0) {
        free(large_data);
        close(fd);
        return TEST_FAILED;
    }
    
    while (aio_error(&cb) == EINPROGRESS) usleep(1000);
    
    if (aio_error(&cb) != 0 || aio_return(&cb) != LARGE_SIZE) {
        free(large_data);
        close(fd);
        return TEST_FAILED;
    }
    
    free(large_data);
    close(fd);
    printf("Large I/O test passed\n");
    return TEST_PASSED;
}

int test_offset_operations() {
    int fd = open(TEST_FILE, O_CREAT | O_WRONLY | O_TRUNC, 0644);
    write(fd, "0123456789", 10);
    close(fd);
    
    fd = open(TEST_FILE, O_RDONLY);
    struct aiocb cb = {0};
    char buffer[4] = {0};
    
    cb.aio_fildes = fd;
    cb.aio_offset = 5;
    cb.aio_buf = buffer;
    cb.aio_nbytes = 3;
    
    if (aio_read(&cb) != 0) {
        close(fd);
        return TEST_FAILED;
    }
    
    while (aio_error(&cb) == EINPROGRESS) usleep(1000);
    
    if (aio_error(&cb) != 0 || strcmp(buffer, "567") != 0) {
        close(fd);
        return TEST_FAILED;
    }
    
    close(fd);
    printf("Offset operations test passed\n");
    return TEST_PASSED;
}

int test_timeout_suspend() {
    int fd = open(TEST_FILE, O_CREAT | O_WRONLY | O_TRUNC, 0644);
    struct aiocb cb = {0};
    
    cb.aio_fildes = fd;
    cb.aio_buf = TEST_DATA;
    cb.aio_nbytes = strlen(TEST_DATA);
    
    const struct aiocb *list[] = {&cb};
    struct timespec timeout = {0, 1000000}; // 1ms
    
    int result = aio_suspend(list, 1, &timeout);
    
    close(fd);
    printf("Timeout suspend test passed\n");
    return TEST_PASSED;
}

int test_write_then_read() {
    int fd = open(TEST_FILE, O_CREAT | O_RDWR | O_TRUNC, 0644);
    struct aiocb write_cb = {0}, read_cb = {0};
    char write_data[] = "Hello AIO World!";
    char read_buffer[32] = {0};
    
    write_cb.aio_fildes = fd;
    write_cb.aio_buf = write_data;
    write_cb.aio_nbytes = strlen(write_data);
    
    if (aio_write(&write_cb) != 0) {
        close(fd);
        return TEST_FAILED;
    }
    
    while (aio_error(&write_cb) == EINPROGRESS) usleep(1000);
    
    if (aio_error(&write_cb) != 0) {
        close(fd);
        return TEST_FAILED;
    }
    
    read_cb.aio_fildes = fd;
    read_cb.aio_offset = 0;
    read_cb.aio_buf = read_buffer;
    read_cb.aio_nbytes = strlen(write_data);
    
    if (aio_read(&read_cb) != 0) {
        close(fd);
        return TEST_FAILED;
    }
    
    while (aio_error(&read_cb) == EINPROGRESS) usleep(1000);
    
    if (aio_error(&read_cb) != 0 || strcmp(read_buffer, write_data) != 0) {
        close(fd);
        return TEST_FAILED;
    }
    
    close(fd);
    printf("Write-then-read test passed\n");
    return TEST_PASSED;
}

int test_concurrent_read_write() {
    int fd1 = open("file1.dat", O_CREAT | O_RDWR | O_TRUNC, 0644);
    int fd2 = open("file2.dat", O_CREAT | O_RDWR | O_TRUNC, 0644);
    write(fd2, "PreWritten", 10);
    lseek(fd2, 0, SEEK_SET);
    
    struct aiocb write_cb = {0}, read_cb = {0};
    char write_data[] = "Concurrent Write";
    char read_buffer[16] = {0};
    
    write_cb.aio_fildes = fd1;
    write_cb.aio_buf = write_data;
    write_cb.aio_nbytes = strlen(write_data);
    
    read_cb.aio_fildes = fd2;
    read_cb.aio_buf = read_buffer;
    read_cb.aio_nbytes = 10;
    
    if (aio_write(&write_cb) != 0 || aio_read(&read_cb) != 0) {
        close(fd1); close(fd2);
        return TEST_FAILED;
    }
    
    while (aio_error(&write_cb) == EINPROGRESS || aio_error(&read_cb) == EINPROGRESS) {
        usleep(1000);
    }
    
    if (aio_error(&write_cb) != 0 || aio_error(&read_cb) != 0 || strcmp(read_buffer, "PreWritten") != 0) {
        close(fd1); close(fd2);
        return TEST_FAILED;
    }
    
    close(fd1); close(fd2);
    unlink("file1.dat"); unlink("file2.dat");
    printf("Concurrent read-write test passed\n");
    return TEST_PASSED;
}

int test_multiple_operations() {
    int fd = open(TEST_FILE, O_CREAT | O_RDWR | O_TRUNC, 0644);
    struct aiocb ops[4] = {0};
    char data1[] = "Part1", data2[] = "Part2";
    char buf1[8] = {0}, buf2[8] = {0};
    
    ops[0].aio_fildes = fd;
    ops[0].aio_offset = 0;
    ops[0].aio_buf = data1;
    ops[0].aio_nbytes = 5;
    
    ops[1].aio_fildes = fd;
    ops[1].aio_offset = 5;
    ops[1].aio_buf = data2;
    ops[1].aio_nbytes = 5;
    
    if (aio_write(&ops[0]) != 0 || aio_write(&ops[1]) != 0) {
        close(fd);
        return TEST_FAILED;
    }
    
    while (aio_error(&ops[0]) == EINPROGRESS || aio_error(&ops[1]) == EINPROGRESS) {
        usleep(1000);
    }
    
    ops[2].aio_fildes = fd;
    ops[2].aio_offset = 0;
    ops[2].aio_buf = buf1;
    ops[2].aio_nbytes = 5;
    
    ops[3].aio_fildes = fd;
    ops[3].aio_offset = 5;
    ops[3].aio_buf = buf2;
    ops[3].aio_nbytes = 5;
    
    if (aio_read(&ops[2]) != 0 || aio_read(&ops[3]) != 0) {
        close(fd);
        return TEST_FAILED;
    }
    
    while (aio_error(&ops[2]) == EINPROGRESS || aio_error(&ops[3]) == EINPROGRESS) {
        usleep(1000);
    }
    
    if (strcmp(buf1, "Part1") != 0 || strcmp(buf2, "Part2") != 0) {
        close(fd);
        return TEST_FAILED;
    }
    
    close(fd);
    printf("Multiple operations test passed\n");
    return TEST_PASSED;
}

int main() {
    int failures = 0;
    
    printf("===== Comprehensive AIO Test Suite =====\n");
    
    printf("\n[TEST] aio_read\n");
    failures += test_aio_read();
    
    printf("\n[TEST] aio_write\n");
    failures += test_aio_write();
    
    printf("\n[TEST] aio_fsync\n");
    failures += test_aio_fsync();
    
    printf("\n[TEST] aio_suspend\n");
    failures += test_aio_suspend();
    
    printf("\n[TEST] lio_listio\n");
    failures += test_lio_listio();
    
    printf("\n[TEST] aio_cancel\n");
    failures += test_aio_cancel();
    
    printf("\n[TEST] Invalid parameters\n");
    failures += test_invalid_params();
    
    printf("\n[TEST] Large I/O\n");
    failures += test_large_io();
    
    printf("\n[TEST] Offset operations\n");
    failures += test_offset_operations();
    
    printf("\n[TEST] Timeout suspend\n");
    failures += test_timeout_suspend();
    
    printf("\n[TEST] Write then read\n");
    failures += test_write_then_read();
    
    printf("\n[TEST] Concurrent read-write\n");
    failures += test_concurrent_read_write();
    
    printf("\n[TEST] Multiple operations\n");
    failures += test_multiple_operations();
    
    unlink(TEST_FILE);
    
    printf("\n===== Test Suite Completed =====\n");
    printf("%d tests failed\n", failures);
    
    return failures ? EXIT_FAILURE : EXIT_SUCCESS;
}