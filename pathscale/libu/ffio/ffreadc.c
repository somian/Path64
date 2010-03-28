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


#pragma ident "@(#) libu/ffio/ffreadc.c	92.2	10/11/99 15:30:43"

#include <errno.h>
#include <stdarg.h>
#include <ffio.h>
#include <stdlib.h>
#include <cray/portdefs.h>

/*
 * ffreadc()
 * This is the driver module that starts the chain of calls through the
 * layers for readc.  The difference between this and read is that the
 * user data is packed one character per word (long), right-justified.
 */

#if	!defined(__mips) && !defined(_LITTLE_ENDIAN)
ffreadc(int fd, char *buf, int nbytes, ...)
	{
	struct fdinfo *fio;
	int ret, locfulp;
	bitptr bufptr;
	int na;	/* need a place to put result */
	struct ffsw locstat, *pstat;	/* need a place to put result */
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
	ret = XRCALL(fio, readcrtn) fio, bufptr, nbytes,
						pstat, locfulp);
	if (na < 4)
		errno = locstat.sw_error;
	return (ret);
	}
#endif
/*
 * _ff_readc()
 *	Generic readc routine.
 */

#define LBSZ 512	/* local buffer size */

ssize_t
_ff_readc(struct fdinfo *fio, bitptr bufptr, size_t nbytes, struct ffsw *stat, int fulp)
	{
	int ubc;
	ssize_t ret;
	size_t nb;
	ssize_t movd;
	int i, lfulp;
	bitptr bufp;
	char locbuf[LBSZ];

	nb = nbytes;
	bufp = bufptr;
	lfulp = PARTIAL;		/* all slices partial 'till last */
	movd = 0;
	do
		{
		i = LBSZ;
		if (i > nb)		/* if not enough room in loc buffer */
			{
			i = nb;
			lfulp = fulp;	/* last slice, use actual mode */
			}
		ubc = 0;
		ret = XRCALL(fio, readrtn)
			fio, CPTR2BP(locbuf),(size_t) i, stat, lfulp, &ubc);
		if (ret <= 0) return(ret); /* EOF, EOD, and error */
/*
 *		Data was returned.  Unpack it.
 */
		_unpack(locbuf, BPTR2WP(bufp), (long)ret, -1);
		nb -= ret;
		movd += ret;
		if (FFSTAT(*stat) != FFCNT) break;
/*
 *		Increment bit pointer.
 */
		SET_BPTR(bufp, INC_BPTR(bufp, ret * _BITS_PER_LONG));

		} while(nb > 0);

	return (movd);
	}
