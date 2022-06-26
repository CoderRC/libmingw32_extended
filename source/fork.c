#include <unistd.h>
pid_t   fork (void){
  pid_t mepid = getpid();
  return mepid;
}
