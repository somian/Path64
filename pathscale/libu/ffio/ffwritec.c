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


#pragma ident "@(#) libu/ffio/ffwritec.c	92.2	10/11/99 15:30:43"

#include <errno.h>
#include <stdarg.h>
#include <ffio.h>
#include <stdlib.h>
#include <cray/portdefs.h>

/*
 * ffwritec()
 * Write data to a file/dataset.  The difference between this and ffwrite
 * is that the user data is packed one character per word (long), right-
 * justified.  This interface exists solely to enhance the performance of
 * formatted I/O.
 */

#if	!defined(__mips) && !defined(_LITTLE_ENDIAN)
ffwritec(int fd, char *buf, int nbytes, ...)
        { 
        struct fdinfo *fio;
        int ret; 
        bitptr bufptr;   
        int na;        /* need a place to put result */ 
	int locfulp;
	struct ffsw locstat, *pstat;
	va_list ap;

        fio = GETIOB(fd);
        SET_BPTR(bufptr, CPTR2BP(buf));
#ifdef	_CRAY
        NUMARG(na);
#else
        na = 5;
#endif
        locfulp = FULL;
        pstat = &locstat;
	if (na < 3 || na > 5)
		{
		errno = FDC_ERR_NOPARM;
		return(ERR);
                }
	va_start(ap, nbytes);
	if (na > 3)
		pstat = va_arg(ap, struct ffsw *);
	if (na > 4)
		locfulp = va_arg(ap, int);
	CHECK_FIOPTR(fio, pstat);
        ret = XRCALL(fio, writecrtn) fio, bufptr, nbytes,
						pstat, locfulp);
/*
 *	only set errno if stat was not passed
 */
	if(na < 4)
		errno = locstat.sw_error;
        return (ret);
        }
#endif

/*
 * _ff_writec()
 *	Generic writec routine.
 */

#define LBSZ 512	/* local buffer size */

ssize_t
_ff_writec(struct fdinfo *fio, bitptr bufptr, size_t nbytes, struct ffsw *stat, int fulp)
	{
	int ubc;
	ssize_t ret;
	size_t nb;
	int  i, lfulp;
	bitptr bufp;
	char locbuf[LBSZ];

	nb = nbytes;
	bufp = bufptr;
	lfulp = PARTIAL;		/* all slices partial 'till last */
	do
		{
		i = LBSZ;
		if (i > nb)		/* if slice will fit in loc buffer */
			{
			i = nb;
			lfulp = fulp;	/* last slice, use actual mode */
			}
/*
 *		Pack data (if there is any).
 */
		if (i > 0) _pack((long *)BPTR2CP(bufp), locbuf, (long) i, -1);

		ubc = 0;
		ret = XRCALL(fio, writertn) fio, CPTR2BP(locbuf), (size_t)i,
						stat, lfulp, &ubc);
		if (ret < 0) return(ERR);
		nb -= ret;
/*
 *		Increment bit pointer.
 */
		SET_BPTR(bufp, INC_BPTR(bufp, ret * _BITS_PER_LONG));

		} while(nb > 0);

	return (nbytes);
	}
