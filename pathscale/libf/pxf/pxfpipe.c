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


#pragma ident "@(#) libf/pxf/pxfpipe.c	92.1	06/29/99 11:36:06"
/*
 *  PXFPIPE -- Create an Inter-Process Channel (pipe) and return
 *             fds for the read and write ends of the pipe.
 *             (section 6.1 of Posix 1003.9-1992)
 *
 *  Synopsis:
 *
 *     SUBROUTINE PXFPIPE(ireadfd, iwrtfd, ierror)
 *     INTEGER ireadfd, iwrtfd, ierror
 *
 *  Where:
 *
 *      ireadfd -  default integer output variable containing the
 *                 file descriptor for the read end of the pipe.
 *      iwrtfd  -  default integer output variable containing the
 *                 file descriptor for the write end of the pipe.
 *      ierror  -  default integer output variable that contains zero
 *                 if the operation was successful or nonzero if the
 *                 operation was not successful.
 *
 *   PXFPIPE may return one of the following error values:
 *
 *   EMFILE     If too many file descriptors are currently open for the
 *              process.
 *
 *   ENFILE     If too many file descriptors are currently open for the
 *              system.
 *
 * On PVP systems, the following may also be returned:
 *
 *   ENOSPC     If no free space left on device.
 *
 *   EFLNEQ     If the active security label of the process does not fall
 *              within the security label range of the root file system.
 *
 */

#include <fortran.h>
#include <errno.h>
#include <liberrno.h>
#include <unistd.h>

#ifdef _UNICOS
void
PXFPIPE(
#else	/* _UNICOS */
void
pxfpipe_(
#endif	/* _UNICOS */
	   _f_int *ireadfd,
	   _f_int *iwrtfd,
	   _f_int *ierror)
{
	int	istat;
	int	fds[2];
	if (istat = pipe(fds) == -1) {
		*ierror		= errno;
	} else {
		*ireadfd	= fds[0];
		*iwrtfd		= fds[1];
		*ierror		= 0;
	}
	return;
}
