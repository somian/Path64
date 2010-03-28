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


#pragma ident "@(#) libu/ffio/ffbksp.c	92.2	10/11/99 15:30:43"

#include <errno.h>
#include <stdarg.h>
#include <ffio.h>

int
#if	defined(__mips) || defined(_LITTLE_ENDIAN)
ffbksp(int fd)
#else
ffbksp(int fd, ...)
#endif
	{
	struct fdinfo *fio;
	int ret, na;
	struct ffsw locstat, *pstat;
#ifdef _CRAY
	va_list ap;
#endif

	fio = GETIOB(fd);
#ifdef _CRAY
	NUMARG(na);
	if (na < 2)
		pstat = &locstat;
	else
		{
		va_start(ap, fd);
		pstat = va_arg(ap, struct ffsw *);
		}
#else
	pstat = &locstat;
	na = 1;
#endif
	CHECK_FIOPTR(fio, pstat);
	ret = XRCALL(fio, backrtn) fio, pstat);
	/* set errno only if stat was not passed */
	if (na < 2)
		errno = locstat.sw_error;
	return (ret);
	}

int
ffbkspf(int fd, struct ffsw *pstat)
{
	struct fdinfo *fio;
	fio = GETIOB(fd);
	CHECK_FIOPTR(fio, pstat);
	return(XRCALL(fio, backrtn) fio, pstat));
}
