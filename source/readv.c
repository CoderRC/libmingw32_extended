#include <sys/uio.h>
#include <unistd.h>

ssize_t readv
__P ((int filedes, const struct iovec * vector, int count))
{
  ssize_t number_of_bytes_read = 0;
  for (int i = 0; i != count; i++)
    {
      read (filedes, vector[i].iov_base, vector[i].iov_len);
      number_of_bytes_read += vector[i].iov_len;
    }
  return number_of_bytes_read;
}
