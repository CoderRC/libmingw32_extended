#include <windows.h>
#include <sys/types.h>
#include <tlhelp32.h>
#include <errno.h>
#include <unistd.h>
#ifndef _WAIT_CHILD
  #define _WAIT_CHILD 0
#endif
pid_t wait (int *__status){
  pid_t mepid = getpid();
  HANDLE hProcessSnap;
  HANDLE hProcess;
  PROCESSENTRY32 pe32;
  DWORD dwPriorityClass;

  hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
  if(hProcessSnap == INVALID_HANDLE_VALUE){
    return -1;
  }

  pe32.dwSize = sizeof(PROCESSENTRY32);

  if(!Process32First(hProcessSnap, &pe32)){
    CloseHandle(hProcessSnap);
    return -1;
  }

  do
  {
    if(pe32.th32ParentProcessID == mepid){
      dwPriorityClass = 0;
      hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);
      if(hProcess){
        dwPriorityClass = GetPriorityClass(hProcess);
        if(!dwPriorityClass){
        }
        if(__status) {*__status = 0;}
        _cwait(NULL, (intptr_t)hProcess, _WAIT_CHILD);
        CloseHandle(hProcess);
        return pe32.th32ProcessID;
      }
    }
  } while(Process32Next(hProcessSnap, &pe32));

  CloseHandle(hProcessSnap);
  _set_errno(ECHILD);
  return -1;
}