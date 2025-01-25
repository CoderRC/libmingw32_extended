/* sys/wait.h
*/

#ifndef _SYS_WAIT_H_
#define _SYS_WAIT_H_

#ifdef __cplusplus
extern "C"
{
#endif				/* __cplusplus */

#include <sys/types.h>
#include <signal.h>

/* Flags for wait. */
#define WNOHANG 1
#define WUNTRACED 2
#define WIFEXITED(s) (((s) & 0xff) == 0)
#define WIFSIGNALED(s) (((s) & 0x7f) > 0 \
 && (((s) & 0x7f) < 0x7f))
#define WIFSTOPPED(s) (((s) & 0xff) == 0x7f)
#define WIFCONTINUED(s) (((s) & 0xffff) == 0xffff)
#define WEXITSTATUS(s) (((s) >> 8) & 0xff)
#define WTERMSIG(s) ((s) & 0x7f)
#define WSTOPSIG WEXITSTATUS
#define WSTOPPED 2
#define WEXITED 4
#define WCONTINUED 8
#define WNOWAIT 0x01000000

  typedef enum
  {
    P_ALL,
    P_PID,
    P_PGID
  } idtype_t;

#ifndef _ID_T_
#define _ID_T_
  typedef pid_t id_t;
#endif

#ifndef _SIGINFO_T_
#define _SIGINFO_T_
#ifndef SIGSTKFLT
#define SIGSTKFLT 16
#endif
#ifndef SIGPWR
#define SIGPWR 30
#endif
#ifndef SIGURG
#define SIGURG 23
#endif
#ifndef SIGSTOP
#define SIGSTOP 19
#endif
#ifndef SIGTSTP
#define SIGTSTP 20
#endif
#ifndef SIGCONT
#define SIGCONT 18
#endif
#ifndef SIGCHLD
#define SIGCHLD 17
#endif
#ifndef SIGPOLL
#define SIGPOLL 29
#endif
#ifndef SIGXFSZ
#define SIGXFSZ 25
#endif
#ifndef SIGXCPU
#define SIGXCPU 24
#endif
#ifndef SIGVTALRM
#define SIGVTALRM 26
#endif
#ifndef SIGPROF
#define SIGPROF 27
#endif
#ifndef SIGWINCH
#define SIGWINCH 28
#endif
#ifndef SIGIO
#define SIGIO SIGPOLL
#endif
#ifndef SIGCLD
#define SIGCLD SIGCHLD
#endif
  enum
  {
    SI_ASYNCNL = -60,
    SI_DETHREAD = -7,
    SI_TKILL,
    SI_SIGIO,
    SI_ASYNCIO,
    SI_MESGQ,
    SI_TIMER,
    SI_QUEUE,
    SI_USER,
    SI_KERNEL = 0x80
#ifndef SI_ASYNCNL
#define SI_ASYNCNL SI_ASYNCNL
#endif
#ifndef SI_DETHREAD
#define SI_DETHREAD SI_DETHREAD
#endif
#ifndef SI_TKILL
#define SI_TKILL SI_TKILL
#endif
#ifndef SI_SIGIO
#define SI_SIGIO SI_SIGIO
#endif
#ifndef SI_ASYNCIO
#define SI_ASYNCIO SI_ASYNCIO
#endif
#ifndef SI_MESGQ
#define SI_MESGQ SI_MESGQ
#endif
#ifndef SI_TIMER
#define SI_TIMER SI_TIMER
#endif
#ifndef SI_ASYNCIO
#define SI_ASYNCIO SI_ASYNCIO
#endif
#ifndef SI_QUEUE
#define SI_QUEUE SI_QUEUE
#endif
#ifndef SI_USER
#define SI_USER SI_USER
#endif
#ifndef SI_KERNEL
#define SI_KERNEL SI_KERNEL
#endif
  };
  enum
  {
    ILL_ILLOPC = 1,
    ILL_ILLOPN,
    ILL_ILLADR,
    ILL_ILLTRP,
    ILL_PRVOPC,
    ILL_PRVREG,
    ILL_COPROC,
    ILL_BADSTK,
    ILL_BADIADDR
#ifndef ILL_ILLOPC
#define ILL_ILLOPC ILL_ILLOPC
#endif
#ifndef ILL_ILLOPN
#define ILL_ILLOPN ILL_ILLOPN
#endif
#ifndef ILL_ILLADR
#define ILL_ILLADR ILL_ILLADR
#endif
#ifndef ILL_ILLTRP
#define ILL_ILLTRP ILL_ILLTRP
#endif
#ifndef ILL_PRVOPC
#define ILL_PRVOPC ILL_PRVOPC
#endif
#ifndef ILL_PRVREG
#define ILL_PRVREG ILL_PRVREG
#endif
#ifndef ILL_COPROC
#define ILL_COPROC ILL_COPROC
#endif
#ifndef ILL_BADSTK
#define ILL_BADSTK ILL_BADSTK
#endif
#ifndef ILL_BADIADDR
#define ILL_BADIADDR ILL_BADIADDR
#endif
  };
  enum
  {
    FPE_INTDIV = 1,
    FPE_INTOVF,
    FPE_FLTDIV,
    FPE_FLTOVF,
    FPE_FLTUND,
    FPE_FLTRES,
    FPE_FLTINV,
    FPE_FLTSUB,
    FPE_FLTUNK = 14,
    FPE_CONDTRAP
#ifndef FPE_INTDIV
#define FPE_INTDIV FPE_INTDIV
#endif
#ifndef FPE_INTOVF
#define FPE_INTOVF FPE_INTOVF
#endif
#ifndef FPE_FLTDIV
#define FPE_FLTDIV FPE_FLTDIV
#endif
#ifndef FPE_FLTOVF
#define FPE_FLTOVF FPE_FLTOVF
#endif
#ifndef FPE_FLTUND
#define FPE_FLTUND FPE_FLTUND
#endif
#ifndef FPE_FLTRES
#define FPE_FLTRES FPE_FLTRES
#endif
#ifndef FPE_FLTINV
#define FPE_FLTINV FPE_FLTINV
#endif
#ifndef FPE_FLTSUB
#define FPE_FLTSUB FPE_FLTSUB
#endif
#ifndef FPE_FLTUNK
#define FPE_FLTUNK FPE_FLTUNK
#endif
#ifndef FPE_CONDTRAP
#define FPE_CONDTRAP FPE_CONDTRAP
#endif
  };
  enum
  {
    SEGV_MAPERR = 1,
    SEGV_ACCERR,
    SEGV_BNDERR,
    SEGV_PKUERR,
    SEGV_ACCADI,
    SEGV_ADIDERR,
    SEGV_ADIPERR,
    SEGV_MTEAERR,
    SEGV_MTESERR
#ifndef SEGV_MAPERR
#define SEGV_MAPERR SEGV_MAPERR
#endif
#ifndef SEGV_ACCERR
#define SEGV_ACCERR SEGV_ACCERR
#endif
#ifndef SEGV_BNDERR
#define SEGV_BNDERR SEGV_BNDERR
#endif
#ifndef SEGV_PKUERR
#define SEGV_PKUERR SEGV_PKUERR
#endif
#ifndef SEGV_ACCADI
#define SEGV_ACCADI SEGV_ACCADI
#endif
#ifndef SEGV_ADIDERR
#define SEGV_ADIDERR SEGV_ADIDERR
#endif
#ifndef SEGV_ADIPERR
#define SEGV_ADIPERR SEGV_ADIPERR
#endif
#ifndef SEGV_MTEAERR
#define SEGV_MTEAERR SEGV_MTEAERR
#endif
#ifndef SEGV_MTESERR
#define SEGV_MTESERR SEGV_MTESERR
#endif
  };
  enum
  {
    BUS_ADRALN = 1,
    BUS_ADRERR,
    BUS_OBJERR,
    BUS_MCEERR_AR,
    BUS_MCEERR_AO
#ifndef BUS_ADRALN
#define BUS_ADRALN BUS_ADRALN
#endif
#ifndef BUS_ADRERR
#define BUS_ADRERR BUS_ADRERR
#endif
#ifndef BUS_OBJERR
#define BUS_OBJERR BUS_OBJERR
#endif
#ifndef BUS_MCEERR_AR
#define BUS_MCEERR_AR BUS_MCEERR_AR
#endif
#ifndef BUS_MCEERR_AO
#define BUS_MCEERR_AO BUS_MCEERR_AO
#endif
  };
  enum
  {
    TRAP_BRKPT = 1,
    TRAP_TRACE,
    TRAP_BRANCH,
    TRAP_HWBKPT,
    TRAP_UNK
#ifndef TRAP_BRKPT
#define TRAP_BRKPT TRAP_BRKPT
#endif
#ifndef TRAP_TRACE
#define TRAP_TRACE TRAP_TRACE
#endif
#ifndef TRAP_BRANCH
#define TRAP_BRANCH TRAP_BRANCH
#endif
#ifndef TRAP_HWBKPT
#define TRAP_HWBKPT TRAP_HWBKPT
#endif
#ifndef TRAP_UNK
#define TRAP_UNK TRAP_UNK
#endif
  };
  enum
  {
    CLD_EXITED = 1,
    CLD_KILLED,
    CLD_DUMPED,
    CLD_TRAPPED,
    CLD_STOPPED,
    CLD_CONTINUED
#ifndef CLD_EXITED
#define CLD_EXITED CLD_EXITED
#endif
#ifndef CLD_KILLED
#define CLD_KILLED CLD_KILLED
#endif
#ifndef CLD_DUMPED
#define CLD_DUMPED CLD_DUMPED
#endif
#ifndef CLD_TRAPPED
#define CLD_TRAPPED CLD_TRAPPED
#endif
#ifndef CLD_STOPPED
#define CLD_STOPPED CLD_STOPPED
#endif
#ifndef CLD_CONTINUED
#define CLD_CONTINUED CLD_CONTINUED
#endif
  };
  enum
  {
    POLL_IN = 1,
    POLL_OUT,
    POLL_MSG,
    POLL_ERR,
    POLL_PRI,
    POLL_HUP
#ifndef POLL_IN
#define POLL_IN POLL_IN
#endif
#ifndef POLL_OUT
#define POLL_OUT POLL_OUT
#endif
#ifndef POLL_MSG
#define POLL_MSG POLL_MSG
#endif
#ifndef POLL_ERR
#define POLL_ERR POLL_ERR
#endif
#ifndef POLL_PRI
#define POLL_PRI POLL_PRI
#endif
#ifndef POLL_HUP
#define POLL_HUP POLL_HUP
#endif
  };
  typedef pid_t uid_t;
  union sigval
  {
    int sival_int;
    void *sival_ptr;
  };
  typedef struct
  {
    int si_signo;
    int si_code;
    int si_errno;
    pid_t si_pid;
    uid_t si_uid;
    void *si_addr;
    int si_status;
    long si_band;
    union sigval si_value;
  } siginfo_t;
#endif

/* wait process */
  extern pid_t wait (int *__status);
  extern pid_t waitpid (pid_t __pid, int *__status, int __options);
  extern int waitid (idtype_t __idtype, id_t __id, siginfo_t * __infop,
		     int __options);

#ifdef __cplusplus
}
#endif				/* __cplusplus */

#endif				/* _SYS_WAIT_H_ */
