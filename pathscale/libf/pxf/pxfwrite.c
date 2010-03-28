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


#pragma ident "@(#) libf/pxf/pxfwrite.c	92.3	10/29/99 21:39:27"
/*
 *  PXFWRITE -- Write a File
 *             (section 6.5.1 of Posix 1003.9-1992)
 *
 *  Synopsis:
 *
 *     SUBROUTINE PXFWRITE(ifildes, buf, nbyte, nwritten, ierror)
 *     INTEGER ifildes, nbyte, nwritten, ierror
 *     CHARACTER BUF(*)
 *
 *  Description:
 *
 *  PXFWRITE uses the write() function to write nbytes to a file
 *           associated with ifildes from a buffer, return the actual
 *           number of bytes written, and return the error status.
 *
 *  The arguments are:
 *
 *      ifildes  -  default integer input variable containing a file
 *                  descriptor.
 *      buf      -  output character variable containing the data to be
 *                  written.
 *      nbyte    -  default input integer variable specifying the
 *                  number of bytes to be written.
 *      nwritten -  default output integer variable containing the
 *                  number of bytes actually written.
 *      ierror   -  default integer output variable that contains zero
 *                  if the operation was successful or nonzero if the
 *                  operation was not successful.
 *
 *   PXFWRITE may return one of the following error values:
 *
 *   EAGAIN     Mandatory file and record locking was set, O_NDELAY was
 *              set, and there was a blocking record lock.
 *
 *   EBADF      If ifildes is not a valid file descriptor open for
 *              writing.
 *
 *   EDEADLK    If the write was going to go to sleep and would cause
 *              a deadlock situation to occur.
 *
 *   EFAULT     If buf points outside the allocated process address
 *              space.
 *
 *   EFBIG      If an attempt was made to write a file that exceeds the
 *              file size limit or the maximum file size of the process.
 *
 *   EINTR      If write was interrupted by a signal.
 *
 *   EINVAL     If the call contains an argument that is not valid.
 *
 *   ENOLCK     If the sytem record lock table was full, the write
 *              could not go to sleep until the block record lock was
 *              removed.
 *
 *   ENOMEM     If PXFWRITE is unable to obtain memory to create an
 *              internal buffer.
 *
 *   ENOSPC     If there is no free space on the device containing the
 *              file.
 *
 *   EPIPE      If attempting to write to a pipe that is not open for
 *              reading by any process.
 *
 *   On PVP systems, PXFWRITE may also return:
 *
 *   EQACT      If a file or inode quota limit was reached for the
 *              current account ID.
 *
 *   EQGRP      If a file or inode quota limit was reached for the
 *              current group ID.
 *
 *   EQUSR      If a file or inode quota limit was reached for the
 *              current user ID.
 *
 *   On IRIX systems, PXFWRITE may also return:
 *
 *   EIO        If a physical I/O error has occurred, or the read is
 *              cannot access the device. or If ifildes has O_DIRECT
 *              or FDIRECT set and nbytes is greater than the number
 *              of bytes between the current file pointer position
 *              and the end of file.
 *              
 *   ENOSR      If there is no free space for the STREAMS memory
 *              resources.
 *
 *   ENOXIO     If the file pointer is out of range for a special
 *              file.
 *
 *   ERANGE     If an attempt is made to write toa stream with nbyte
 *              outside the mininum and maximun write range when the
 *              mininum value is nonzero.
 *
 *   ETIMEDOUT  If the object of the write is located on a remote
 *              system which is not available.
 *
 */

#include <fortran.h>
#include <errno.h>
#include <liberrno.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
/* for malloc */
#ifdef _LITTLE_ENDIAN
#include <stdlib.h>
#endif

#ifdef _UNICOS
void
PXFWRITE(
#else	/* _UNICOS */
void
_PXFWRITE(
#endif	/* _UNICOS */
	_f_int	*ifildes,
	_fcd	buf,
	_f_int	*nbyte,
	_f_int	*nwritten,
	_f_int	*ierror)
{
	char	*buffr;
	size_t	errsts	= 0;
	size_t	tobewritten;
	size_t	waswritten;
	tobewritten	= (size_t)*nbyte;
	*ierror	= 0;
	*nwritten	= 0;
	buffr	= NULL;
	if ((int)tobewritten <= 0)
		return;
	buffr	= (char *) malloc(tobewritten);

	/* return an error if no memory allocated. */
	if (buffr == NULL)
		errsts	= ENOMEM;
	else {
		/* In Fortran 77, a variable is not be an array.
		 * Therefore, buf cannot be more than a character
		 * scalar in the 1003.9 standard.  Fortran 90
		 * extended the definition of variable to include
		 * an array.
		 *
		 * Extend PXFWRITE to allow nbyte to be greater
		 * than the size of buf to allow buf to be an
		 * array.  A subroutine interface does not pass
		 * the number of elements in the array and
		 * overindexing is legal in Fortran.  Sections
		 * will be contiguous when passed.
		 */
		(void) memcpy(buffr, _fcdtocp(buf), tobewritten);
		waswritten	= write(*ifildes, buffr, tobewritten);
		if ((int)waswritten < 0)
			errsts	= errno;
		*nwritten	= waswritten;
		free(buffr);
	}
	*ierror	= (_f_int)errsts;
	return;
}

#ifndef _UNICOS
void
pxfwrite_(
	_f_int	*ifildes,
	char	*buf,
	_f_int	*nbyte,
	_f_int	*nwritten,
	_f_int	*ierror,
	int	lenbuf)
{
	_PXFWRITE(ifildes, _cptofcd(buf, lenbuf), nbyte, nwritten, ierror);
}

void
pxfwrite64_(
	_f_int8	*ifildes,
	char	*buf,
	_f_int8	*nbyte,
	_f_int8	*nwritten,
	_f_int8	*ierror,
	int	lenbuf)
{
	_f_int4	ifildes4;
	_f_int4	nbyte4;
	_f_int4	nwritten4;
	_f_int4	ierror4;
        ifildes4       = *ifildes;
        nbyte4 = *nbyte;
	_PXFWRITE(&ifildes4, _cptofcd(buf, lenbuf), &nbyte4,
		&nwritten4, &ierror4);
}
#elif defined(_UNICOS) && defined(_CRAYMPP)
/* assume integer(kind=4) arguments and default 64-bit integer */
void
PXFWRITE32(
	_f_int4	*ifildes,
	_fcd	buf,
	_f_int4	*nbyte,
	_f_int4	*nwritten,
	_f_int4	*ierror)
{
	_f_int	ifildes8;
	_f_int	nbyte8;
	_f_int	nwritten8;
	_f_int	ierror8;
	ifildes8	= *ifildes;
	nbyte8	= *nbyte;
	PXFWRITE(&ifildes8, buf, &nbyte8, &nwritten8, &ierror8);
	*nwritten	= nwritten8;
	*ierror	= ierror8;
}
#endif	/* end _UNICOS and _CRAYMPP */
