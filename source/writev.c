#include <sys/uio.h>
#include <unistd.h>
#include <errno.h>

ssize_t writev
__P ((int filedes, const struct iovec * vector, int count))
{
  ssize_t number_of_bytes_written = 0;
  ssize_t write_result;

  if (vector == NULL) {
    errno = EINVAL;
    return -1;
  }

  if (count <= 0) {
    errno = EINVAL;
    return -1;
  }

  for (int i = 0; i != count; i++) {
    if (vector[i].iov_base == NULL || vector[i].iov_len == 0) {
      continue;
    }

    write_result = write(filedes, vector[i].iov_base, vector[i].iov_len);
    
    if (write_result == -1) {
      return -1;
    }
    
    number_of_bytes_written += write_result;
  }

  return number_of_bytes_written;
}
