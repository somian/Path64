/*

  Copyright (C) 2000, 2001 Silicon Graphics, Inc.  All Rights Reserved.

   Path64 is free software; you can redistribute it and/or modify it
   under the terms of the GNU Lesser General Public License as published by
   the Free Software Foundation version 2.1

   Path64 is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
   or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
   License for more details.

   You should have received a copy of the GNU General Public License
   along with Path64; see the file COPYING.  If not, write to the Free
   Software Foundation, 51 Franklin Street, Fifth Floor, Boston, MA
   02110-1301, USA.

   Special thanks goes to SGI for their continued support to open source

*/


#pragma ident "@(#) libf/pxf/pxfwaitpid.c	92.1	06/29/99 11:36:06"

#include <errno.h>
#include <liberrno.h>
#include <fortran.h>
#include <sys/types.h>
#include <sys/wait.h>

/*  PXFWAITPID  -- wait for process termination
 *  (section 3.2.1 of Posix 1003.9-1992)
 *
 *  Synopis:
 *     SUBROUTINE PXFWAITPID(IPID,ISTAT,IOPTIONS,IRETPID,IERROR)
 *     INTEGER IPID,ISTAT,IOPTIONS,IRETPID,IERROR
 *
 *  Function description:
 *  The routine PXFWAITPID uses the waitpid(2) system call to obtain information
 *  about one of the calling process's child processes.
 *
 *
 *
 *  Description of arguments:
 *
 *  IPID     is an input integer variable containing the child process ID for
 *           which information is requested.
 *
 *  ISTAT    is an output integer variable for the status information.
 *
 *  IOPTIONS is an input integer variable constructed from an inclusive OR of
 *           zero or more of the following:
 *
 *           WNOHANG             The waitpid system call does not suspend
 *                               execution of the calling process if status is
 *                               not immediately available for one of the calling
 *                               processes specified by pid.
 *
 *           WUNTRACED           If job control is supported, the status of any
 *                               child processes specified by pid that are
 *                               stopped, and with a status that has not yet been
 *                               reported since they stopped, are also reported
 *                               to the requesting process.
 *
 *           UNICOS only:
 *
 *           WMTWAIT             Waits for the children of any member of the
 *                               multitasking group.  In UNICOS 9.0 this is the
 *                               default behavior for both wait and waitpid.  The
 *                               flag is still provided for source compatibility.
 *                               To get the previous behavior, see the
 *                               description of the WLWPWAIT flag.
 *
 *           WLWPWAIT            Waits only for the immediate children of the
 *                               calling light-weight process (LWP).  This flag
 *                               is not recommended for general use.
 *
 *  IRETPID  is an output integer variable for the child process ID.
 *
 *  IERROR   is an output integer variable that will contain
 *           the status:
 *
 *            zero    - PXFWAITPID was successful.
 *
 *            nonzero - PXFWAITPID was not successful.
 *
 *           PXFWAITPID may return any of the following error
 *           values:
 *
 *           ECHILD  The process or process group specified by pid does
 *                   not exist or is not a child of the calling
 *                   process.
 *
 *           EINTR   The call was interrupted by a signal.
 *
 *           EINVAL  The value of the options argument is not valid.
 *
 */

#ifdef _UNICOS
void
PXFWAITPID(
#else
void
_PXFWAITPID(
#endif
	 _f_int *IPID,
	 _f_int *ISTAT,
	 _f_int *IOPTIONS,
	 _f_int *IRETPID,
	 _f_int *IERROR
)
{
  pid_t child_pid;
  int stat;

  if ((child_pid = waitpid(*IPID,&stat,*IOPTIONS)) == -1) {
    *IERROR = errno;
  } else {
    *ISTAT = stat;
    *IRETPID = child_pid;
    *IERROR = 0;
  }
}

#ifndef _UNICOS
void
pxfwaitpid_(
	 _f_int *IPID,
	 _f_int *ISTAT,
	 _f_int *IOPTIONS,
	 _f_int *IRETPID,
	 _f_int *IERROR
)
{
  _PXFWAITPID(IPID,ISTAT,IOPTIONS,IRETPID,IERROR);
}
#endif
