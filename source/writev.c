#include <sys/uio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

ssize_t writev(int filedes, const struct iovec *vector, int count) {
    ssize_t number_of_bytes_written = 0;
    size_t total_length = 0;

    //total length of all iovec buffers
    for (int i = 0; i < count; i++) {
        total_length += vector[i].iov_len;
    }

    //single buffer to hold all data
    char *buffer = (char *)malloc(total_length);
    if (buffer == NULL) {
        return -1; // Allocation fail
    }

    size_t offset = 0;
    for (int i = 0; i < count; i++) {
        memcpy(buffer + offset, vector[i].iov_base, vector[i].iov_len);
        offset += vector[i].iov_len;
    }
  
    number_of_bytes_written = write(filedes, buffer, total_length);
    free(buffer);

    return number_of_bytes_written;
}
