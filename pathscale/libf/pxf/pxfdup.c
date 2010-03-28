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


#pragma ident "@(#) libf/pxf/pxfdup.c	92.1	06/29/99 11:36:06"
/*
 *  PXFDUP --  Duplicate an Open File Descriptor 
 *             (section 6.2.1 of Posix 1003.9-1992)
 *
 *  Synopsis:
 *
 *     SUBROUTINE PXFDUP(ifildes, ifid, ierror)
 *     INTEGER ifildes, ifid, ierror
 *
 *     SUBROUTINE PXFDUP2(ifildes, ifildes2, ierror)
 *     INTEGER ifildes, ifildes2, ierror
 *
 *  Description:
 *
 *  PXFDUP uses the dup system call to duplicate an open file
 *  descriptor.  The argument ifildes is a file descriptor obtained
 *  from a pxfcreat, pxfopen, pxfdup, pxffcntl, or pxfpipe call.
 *  ifid is a new file descriptor with the following characteristics:
 *
 *  1.  Same open file or pipe as ifildes.
 *  2.  Same file pointer as ifildes, i.e., both file descriptors
 *      share one file pointer.
 *  3.  Same access mode as ifildes, i.e., read, write, read/write.
 *
 *  PXFDUP2 uses the dup2 function to duplicate an open file descriptor
 *  ifildes onto a new file descriptor ifildes2.  ifildes2 must be a
 *  nonnegative integer less than the maximum number of open files that
 *  will refer to the same file as ifildes after dup2.  If ifildes2
 *  already refers to an open file, that file is first closed, as if 
 *  the close system call had been performed.  If ifildes2 already
 *  refers to ifildes or if ifildes is not a valid open file descriptor,
 *  ifildes2 will not be closed first.
 *
 *  The arguments are:
 *
 *      ifildes  -  default integer input variable containing a file
 *                  descriptor.
 *      ifildes2 -  default integer input variable containing a file
 *                  descriptor.
 *      ifid     -  default integer output variable containing the
 *                  duplicate file descriptor.
 *      ierror   -  default integer output variable that contains zero
 *                  if the operation was successful or nonzero if the
 *                  operation was not successful.
 *
 *   PXFDUP may return one of the following error values:
 *
 *   EBADF      If ifildes is not a valid open file descriptor.
 *
 *   EMFILE     If the number of file descriptors would exceed OPEN_MAX.
 *
 *   On IRIX systems, PXFDUP may also return:
 *
 *   EINTR      If the dup system call was interrupted by a signal.
 *
 *   ENOLINK    If file descriptor is on a remote machine and the link
 *              to that machine is no longer active.
 *
 *   PXFDUP2 may return one of the following error values:
 *
 *   EBADF      If ifildes is not a valid open file descriptor, or if
 *              ifildes2 is negative or greater than or equal to the
 *              maximum number of open files available.
 *
 *   EINTR      If the dup2 system call was interrupted by a signal.
 *
 *   EMFILE     If the maximum number of files are currently open.
 *
 *
 */

#include <fortran.h>
#include <errno.h>
#include <liberrno.h>
#include <unistd.h>

#ifdef _UNICOS
void
PXFDUP(
#else	/* _UNICOS */
void
pxfdup_(
#endif	/* _UNICOS */
	   _f_int *ifildes,
	   _f_int *ifid,
	   _f_int *ierror)
{
	int	istat;
	*ierror	= 0;
	*ifid	= -1;
	istat	= dup(*ifildes);
	if (istat < 0)
		*ierror	= errno;
	else
		*ifid	= istat;
	return;
}

#ifdef _UNICOS
void
PXFDUP2(
#else	/* _UNICOS */
void
pxfdup2_(
#endif	/* _UNICOS */
	   _f_int *ifildes,
	   _f_int *ifildes2,
	   _f_int *ierror)
{
	int	istat;
	*ierror	= 0;
	istat	= dup2(*ifildes,*ifildes2);
	if (istat < 0)
		*ierror	= errno;
	return;
}
