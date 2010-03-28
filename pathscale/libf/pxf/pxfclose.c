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


#pragma ident "@(#) libf/pxf/pxfclose.c	92.1	06/29/99 11:36:06"
/*
 *  PXFCLOSE -- Close a File
 *             (section 6.3.1 of Posix 1003.9-1992)
 *
 *  Synopsis:
 *
 *     SUBROUTINE PXFCLOSE(ifildes, ierror)
 *     INTEGER ifildes, ierror
 *
 *  Description:
 *
 *  PXFCLOSE uses the close system call to close a file descriptor.
 *
 *  The arguments are:
 *
 *      ifildes  -  default integer input variable containing a file
 *                  descriptor.
 *      ierror   -  default integer output variable that contains zero
 *                  if the operation was successful or nonzero if the
 *                  operation was not successful.
 *
 *   PXFCLOSE may return one of the following error values:
 *
 *   EBADF      If ifildes is not a valid open file descriptor.
 *
 *   EINTR      If the close system call was interrupted by a signal.
 *
 *   On IRIX systems, PXFCLOSE may also return:
 *
 *   ETIMEDOUT  If the object of the close system call is located on
 *              a remote system which is not available.
 *
 */

#include <fortran.h>
#include <errno.h>
#include <liberrno.h>
#include <unistd.h>

#ifdef _UNICOS
void
PXFCLOSE(
#else	/* _UNICOS */
void
pxfclose_(
#endif	/* _UNICOS */
	   _f_int *ifildes,
	   _f_int *ierror)
{
	int	istat;
	*ierror	= 0;
	istat	= close(*ifildes);
	if (istat < 0)
		*ierror	= errno;
	return;
}
