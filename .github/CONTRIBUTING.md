# Contributing to the source
This describes the contributing guidelines for this project.

## How to properly add new sources to the repository

To properly add new sources to the repository, the sources must be added to the source directory in the repository and in the configure file add paths to the SOURCES.

#### Design to create source:
<-- include headers here

<-- add one line of space between headers and function

<-- write the function for the source here

#### In what branch should sources be added
In the branch that contains the header name.

## How to properly add new include directories to the repository

To properly add new include directories to the repository, the include directories must be added to the include directory in the repository and in the configure file add include paths to the INCLUDE_DIRECTORIES.

## How to properly add new headers to the repository

To properly add new headers to the repository, the headers must be added to the include directory in the repository and in the configure file add paths to the INCLUDE_FILES.

#### Design to create header:
<-- write /* and the location of the header file relative to the include directory in the repository

<-- write */

<-- add one line of space between */ and #ifndef

<-- write #ifndef and _ and the subdirectories names relative to the include directory of the header file separated by _ and the file name of the header without file name extension and _ and file name extension without the starting dot

<-- write #define and _ and the subdirectories names relative to the include directory of the header file separated by _ and the file name of the header without file name extension and _ and file name extension without the starting dot

<-- add one line of space between #define and #ifdef __cplusplus

<-- write #ifdef __cplusplus

<-- write extern "C"

<-- write {

<-- write #endif				/* __cplusplus */

<-- add one line of space between #endif and headers

<-- include headers here

<-- add one line of space between headers and define

<-- define structs and preprocessor macros here

<-- add one line of space between define and #ifdef __cplusplus

<-- write #ifdef __cplusplus

<-- write };

<-- write #endif /* __cplusplus */

<-- add one line of space between #endif and #ifdef

<-- write #ifdef __cplusplus

<-- write extern "C"

<-- write {

<-- write #endif

<-- add one line of space between #endif and function definitions

<-- write the function definitions of all sources that the header defines

<-- add one line of space between function definitions and #ifdef

<-- write #ifdef __cplusplus

<-- write };

<-- write #endif

<-- add one line of space between #endif and #endif

<-- write #endif /* and _ and the subdirectories names relative to the include directory of the header file separated by _ and the file name of the header without file name extension and _ and file name extension without the starting dot and */

#### What headers that are still not created
<aio.h>

<arpa/inet.h>

<cpio.h>

<fmtmsg.h>

<fnmatch.h>

<glob.h>

<grp.h>

<langinfo.h>

<monetary.h>

<mqueue.h>

<ndbm.h>

<net/if.h>

<netdb.h>

<netinet/in.h>

<netinet/tcp.h>

<nl_types.h>

<poll.h>

<pwd.h>

<spawn.h>

<stropts.h>

<sys/ipc.h>

<sys/msg.h>

<sys/resource.h>

<sys/select.h>

<sys/sem.h>

<sys/shm.h>

<sys/statvfs.h>

<sys/times.h>

<sys/un.h>

<sys/utsname.h>

<sys/wait.h>

<syslog.h>

<tar.h>

<termios.h>

<trace.h>

<ulimit.h>

<utmpx.h>

<wordexp.h>

## What issues can be created
Disclaimer: those below are just suggestions

Support for any header that is already finished in any repo for libmingw32_extended

Problems with compiling libmingw32_extended

Any problems with code logic

Improvements to code, headers, contributing guidelines, issue templates, pull request template

## What pull requests can be created
Disclaimer: those below are just suggestions

Support for a header

Solved problems with compiling libmingw32_extended

Solved any problems with code logic

Implemented improvement to code, headers, contributing guidelines, issue templates, pull request template
