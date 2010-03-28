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


#pragma ident "@(#) libf/pxf/pxfwait.c	92.1	06/29/99 11:36:06"

#include <errno.h>
#include <liberrno.h>
#include <fortran.h>
#include <sys/types.h>
#include <sys/wait.h>

/*  PXFWAIT  -- wait for process termination
 *  (section 3.2.1 of Posix 1003.9-1992)
 *
 *  Synopis:
 *     SUBROUTINE PXFWAIT(ISTAT,IRETPID,IERROR)
 *     INTEGER ISTAT,IRETPID,IERROR
 *
 *  Function description:
 *  The routine PXFWAIT uses the wait(2) system call to obtain information
 *  about one of the calling process's child processes. 
 *
 *  Description of arguments:
 *
 *  ISTAT   is an output integer variable for the status information.
 *
 *  IRETPID is an output integer variable for the child process ID.
 *
 *  IERROR  is an output integer variable that will contain
 *          the status:
 *
 *           zero    - PXFWAIT was successful.
 *
 *           nonzero - PXFWAIT was not successful.
 *
 *          PXFWAIT may return any of the following error
 *          values:
 *
 *          ECHILD  If the calling process has no existing
 *                  unwaited-for child processes.
 *
 *          EINTR   If receipt of a signal other than the
 *                  death-of-a-child-process signal.
 *
 */

#ifdef _UNICOS
void
PXFWAIT(
#else
void
_PXFWAIT(
#endif
	 _f_int *ISTAT,
	 _f_int *IRETPID,
	 _f_int *IERROR
)
{
  pid_t child_pid;
  int stat;

  if ((child_pid = wait(&stat)) == -1) {
    *IERROR = errno;
  } else {
    *ISTAT = stat;
    *IRETPID = child_pid;
    *IERROR = 0;
  }
}

#ifndef _UNICOS
void
pxfwait_(
	 _f_int *ISTAT,
	 _f_int *IRETPID,
	 _f_int *IERROR
)
{
  _PXFWAIT(ISTAT,IRETPID,IERROR);
}
#endif
