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


#pragma ident "@(#) libf/pxf/pxfsetsid.c	92.1	06/29/99 11:36:06"

#include <errno.h>
#include <liberrno.h>
#include <fortran.h>
#include <sys/types.h>
#include <unistd.h>

/*  PXFSETSID  -- create session and set process group ID
 *  (section 4.3.2 of Posix 1003.9-1992)
 *
 *  Synopsis:
 *
 *     SUBROUTINE PXFSETSID(ISID, IERROR)
 *     INTEGER ISID, IERROR
 *
 *  Function description:
 *  The routine PXFSETSID uses the setsid(2) system call to create
 *  new session for the calling process. The calling process must
 *  not be the process leader for PXFSETSID to be successful.
 *
 *  After the successful completion of PXFSETSID, the calling
 *  process will be the session leader for the new session and the
 *  process group leader for the new process group. The calling
 *  process also will have no controlling terminal.
 *
 *  Function arguments:
 *
 *  ISID   is an output integer variable for the new process
 *         group ID of the calling process.
 *
 *  IERROR is an output integer variable that will contain
 *         the status:
 *
 *          zero    - PXFSETSID was successful.
 *
 *          nonzero - PXFSETSID was not successful.
 *
 *         PXFSETSID may return any of the following error values:
 *
 *         EPERM If the calling process is already a process group
 *               leader, or the calling process group ID of a
 *               process other than the calling process
 *               matches the process ID of the calling process.
 *
 *
 *
 */

#ifdef _UNICOS
void
PXFSETSID(
#else
void
_PXFSETSID(
#endif
	   _f_int *ISID,
	   _f_int *IERROR
)
{
  pid_t cisid;

  if ((cisid = setsid()) == -1) {
    *IERROR = errno;
    return;
  }

  *ISID = cisid;
  *IERROR = 0;
}


#ifndef _UNICOS
void
pxfsetsid_(
	   _f_int *ISID,
	   _f_int *IERROR
)
{
  _PXFSETSID(ISID, IERROR);
}
#endif
