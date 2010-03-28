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


#pragma ident "@(#) libu/ffio/ffreada.c	92.1	06/29/99 13:16:47"

#include <errno.h>
#include <stdarg.h>
#include <ffio.h>
/*
 * This module contains a driver that starts the chain of calls
 * through the foreign dataset control layers. to do asynchronous reads
 *
 */

ffreada(int fd, char *buf, int nbytes, struct ffsw *stat, ...)
	{
	struct fdinfo *fio;
	int ret, locfulp;
	bitptr bufptr;
	int locubc, *pubc, na;	/* need a place to put result */
	va_list ap;

	fio = GETIOB(fd);
	SET_BPTR(bufptr, CPTR2BP(buf));
	/* adjust number of bits requested if ubc passed in */
	NUMARG(na);
	locubc = 0;
	pubc = &locubc;
	locfulp = FULL;
	if (na < 4 || na > 6)
		{
		errno = FDC_ERR_NOPARM;
		return(ERR);
		}
	va_start(ap, stat);
	if (na > 4)
		locfulp = va_arg(ap, int);
	if (na > 5)
		pubc = va_arg(ap, int *);
	CHECK_FIOPTR(fio, stat);
	ret = XRCALL(fio, readartn) fio, bufptr, nbytes,
						stat, locfulp, pubc);
	return (ret);
	}
