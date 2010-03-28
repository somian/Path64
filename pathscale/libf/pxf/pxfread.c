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


#pragma ident "@(#) libf/pxf/pxfread.c	92.2	06/29/99 11:36:06"

#ifndef	BLANK
#define BLANK ((int) ' ')
#endif

/*
 *  PXFREAD -- Read a File
 *             (section 6.4.1 of Posix 1003.9-1992)
 *
 *  Synopsis:
 *
 *     SUBROUTINE PXFREAD(ifildes, buf, nbyte, nread, ierror)
 *     INTEGER ifildes, nbyte, nread, ierror
 *     CHARACTER BUF(*)
 *
 *  Description:
 *
 *  PXFREAD uses the read() function to read from a file associated
 *          with ifildes into a buffer.  Return the actual number
 *          of bytes read and return the error status.
 *
 *  The arguments are:
 *
 *      ifildes  -  default integer input variable containing a file
 *                  descriptor.
 *      buf      -  output character variable containing the data read.
 *      nbyte    -  default input integer variable specifying the
 *                  number of bytes to be read.
 *      nread    -  default output integer variable containing the
 *                  number of bytes actually read.
 *      ierror   -  default integer output variable that contains zero
 *                  if the operation was successful or nonzero if the
 *                  operation was not successful.
 *
 *   PXFREAD may return one of the following error values:
 *
 *   EAGAIN     Mandatory file and record locking was set, O_NDELAY was
 *              set, and there was a blocking record lock.
 *
 *   EBADF      If ifildes is not a valid file descriptor open for
 *              reading.
 *
 *   EDEADLK    If the read was going to go to sleep and would cause
 *              a deadlock situation to occur.
 *
 *   EFAULT     If buf points outside the allocated process address
 *              space.
 *
 *   EINTR      If read was interrupted by a signal.
 *
 *   EINVAL     If the call contains an argument that is not valid.
 *
 *   ENOLCK     If the sytem record lock table was full, the read
 *              could not go to sleep until the block record lock was
 *              removed.
 *
 *   ENOMEM     If PXFREAD is unable to obtain memory to create an
 *              internal buffer.
 *
 *   ENXIO      If the device associated with ifildes is a character
 *              special file that does not exist or the file pointer
 *              is out of range.
 *
 *   On IRIX systems, PXFREAD may also return:
 *
 *   EACCES     If ifildes is open to a dynamic device and read
 *              permission is denied.
 *
 *   EBADMSG    If the message waiting to be read on a stream is not a
 *              data message.
 *
 *   EIO        If a physical I/O error has occurred, or the read is
 *              cannot access the device. or If ifildes has O_DIRECT
 *              or FDIRECT set and nbytes is greater than the number
 *              of bytes between the current file pointer position
 *              and the end of file.
 *              
 */

#include <errno.h>
#include <fortran.h>
#include <liberrno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/errno.h>
#include <sys/types.h>
#include <unistd.h>

#ifdef _UNICOS
void
PXFREAD(
#else	/* _UNICOS */
void
_PXFREAD(
#endif	/* _UNICOS */
	_f_int	*ifildes,
	_fcd	buf,
	_f_int	*nbyte,
	_f_int	*nread,
	_f_int	*ierror)
{
	char	*buffr;
	char	*strread;
	int	skipconform = 0;
	long	string_len;
	long	copy_len;
	size_t	errsts	= 0;
	size_t	toberead;
	size_t	wasread;
	*ierror	= 0;
	*nread	= 0;
	toberead	= (size_t)*nbyte;
	string_len	= _fcdlen(buf);
	if ((int)toberead <= 0)
		return;
	buffr	= (char *) malloc(toberead);

/*
 *      The PXFREAD_CONFORM environment variable can be set to NO to
 *      indicate that the check on the size of nbyte compared to the
 *      size of buf is ignored.  The default YES assumes that the
 *      check is done.
 */
        strread	= getenv("PXFREAD_CONFORM");
	if (strread != NULL) {
		if (strcmp(strread, "NO") == 0)
			skipconform	= 1;
	}
	/* return an error if no memory allocated. */
	if (buffr == NULL)
		errsts	= ENOMEM;
	else {
		wasread	= read(*ifildes, buffr, toberead);
		if ((int)wasread < 0)
			errsts	= errno;
		else {
			/* In Fortran 77, a variable was not an array.
			 * Therefore, buf cannot be more than a 
			 * character scalar in the 1003.9 standard.
			 * Fortran 90 extended the definition of 
			 * variable to include an array.
			 *
			 * Extend PXFREAD to allow nbyte to be greater
			 * than the size of buf to allow buf to be an
			 * array.  A subroutine interface does not pass
			 * the number of elements in the array and
			 * overindexing is legal in Fortran.  Sections
			 * will be contiguous when passed.
			 *
			 */

			/* Prevent copy from going beyond string. */
			if (skipconform == 0 && (int)wasread > string_len) {
				errsts		= ETRUNC;
				copy_len	= (size_t)string_len;
			} else
				copy_len	= wasread;
			/* copy input buffer to string and blank fill */
			(void) memcpy(_fcdtocp(buf), buffr, copy_len);
			if (string_len > (int)copy_len) {
				(void) memset(_fcdtocp(buf) +
				   sizeof(char) * (int)copy_len, BLANK,
				   (string_len - (int)copy_len) * sizeof(char));
			}
		}
		/* return untruncated read size */
		*nread	= (_f_int)wasread;
		free(buffr);
	}
	*ierror	= (_f_int)errsts;
	return;
}

#ifndef _UNICOS
/* assume default integer */
void
pxfread_(
	_f_int	*ifildes,
	char	*buf,
	_f_int	*nbyte,
	_f_int	*nread,
	_f_int	*ierror,
	int	lenbuf)
{
	_PXFREAD(ifildes, _cptofcd(buf, lenbuf), nbyte, nread, ierror);
}

/* assume integer(kind=8) */
void
pxfread64_(
	_f_int8	*ifildes,
	char	*buf,
	_f_int8	*nbyte,
	_f_int8	*nread,
	_f_int8	*ierror,
	int	lenbuf)
{
	_f_int	ifildes4;
	_f_int	nbyte4;
	_f_int	nread4;
	_f_int	ierror4;
	ifildes4	= *ifildes;
	nbyte4	= *nbyte;
	_PXFREAD(&ifildes4, _cptofcd(buf, lenbuf), &nbyte4,
		&nread4, &ierror4);
	*nread	= nread4;
	*ierror	= ierror4;
}
#elif	defined(_UNICOS) && defined(_CRAYMPP)
/* assume integer(kind=4) arguments and default 64-bit integer */
void
PXFREAD32(
	_f_int4	*ifildes,
	_fcd	buf,
	_f_int4	*nbyte,
	_f_int4	*nread,
	_f_int4	*ierror)
{
	_f_int	ifildes8;
	_f_int	nbyte8;
	_f_int	nread8;
	_f_int	ierror8;
	ifildes8	= *ifildes;
	nbyte8	= *nbyte;
	PXFREAD(&ifildes8, buf, &nbyte8, &nread8, &ierror8);
	*nread	= nread8;
	*ierror	= ierror8;
}
#endif	/* end _UNICOS  and _CRAYMPP */
