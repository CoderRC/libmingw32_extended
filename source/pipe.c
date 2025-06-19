#include <fcntl.h>
#ifndef _O_BINARY
#define _O_BINARY 0x8000
#endif
int
pipe (int __fildes[2])
{
  _pipe (__fildes, 65536, _O_BINARY);
}
