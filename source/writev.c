#include <sys/uio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

ssize_t writev(int filedes, const struct iovec *vector, int count) {
    ssize_t number_of_bytes_written = 0;
    size_t total_length = 0;

    // Calculate the total length of all iovec buffers
    for (int i = 0; i < count; i++) {
        total_length += vector[i].iov_len;
    }

    // Allocate a single buffer to hold all data
    char *buffer = (char *)malloc(total_length);
    if (buffer == NULL) {
        // Fallback to writing each iovec separately if allocation fails
        for (int i = 0; i < count; i++) {
            ssize_t bytes_written = write(filedes, vector[i].iov_base, vector[i].iov_len);
            if (bytes_written == -1) {
                return -1; // Write failed
            }
            number_of_bytes_written += bytes_written;
        }
        return number_of_bytes_written;
    }

    // Copy data from all iovec buffers into the single buffer
    size_t offset = 0;
    for (int i = 0; i < count; i++) {
        memcpy(buffer + offset, vector[i].iov_base, vector[i].iov_len);
        offset += vector[i].iov_len;
    }

    // Write the single buffer to the file descriptor
    number_of_bytes_written = write(filedes, buffer, total_length);

    // Free the allocated buffer
    free(buffer);

    return number_of_bytes_written;
}
