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


#pragma ident "@(#) libu/ffio/ffseek.c	92.2	10/11/99 15:30:43"

#include <errno.h>
#include <stdarg.h>
#include <ffio.h>

/*
 * This module contains a driver that starts the chain of calls
 * through the foreign dataset control layers.
 *
 */

#if	defined(__mips) || defined(_LITTLE_ENDIAN)
_ffseek_t
ffseek(int fd, off_t pos, int whence)
#elif _USE_OLD_PROTOTYPE == 0
_ffseek_t
ffseek(int fd, off_t pos, int whence, ...)	
#else
int
ffseek(int fd, int pos, int whence, ...)	
#endif
	{
	struct fdinfo *fio;
	_ffseek_t ret;
	int na;
	struct ffsw locstat, *pstat;
	va_list ap;

	fio = GETIOB(fd);
#if	defined(__mips) || defined(_LITTLE_ENDIAN)
	na = 3;
	pstat = &locstat;
#else
	NUMARG(na);
	if (na < 3)
		{
		errno = FDC_ERR_NOPARM;
		return(ERR);
		}
	if (na < 4)
		pstat = &locstat;
	else
		{
		va_start(ap, whence);
		pstat = va_arg(ap, struct ffsw *);
		}
#endif
	CHECK_FIOPTR(fio, pstat);
	ret = XRCALL(fio, seekrtn) fio, pos, whence, pstat);
	if (na < 4)
		errno = locstat.sw_error;
	return (ret);
	}

/*
 * This routine is like ffseek, except is expects 4 parameters
 */
off_t
ffseekf(int fd, off_t pos, int whence, struct ffsw *pstat)
	{
	struct fdinfo *fio;
	_ffseek_t ret;

	fio = GETIOB(fd);
	CHECK_FIOPTR(fio, pstat);
	ret = XRCALL(fio, seekrtn) fio, pos, whence, pstat);
	return (ret);
	}

/*
 * This is a generic seek routine.
 *
 */

_ffseek_t
_ff_seek(struct fdinfo *fio, off_t pos, int whence, struct ffsw *stat)
	{
	struct fdinfo *llfio;
	_ffseek_t ret;

	ret = XRCALL(fio, flushrtn) fio, stat);
	if (ret != 0) return(ERR);
	llfio = fio->fioptr;
	ret = XRCALL(llfio, seekrtn) llfio, pos, whence, stat);
	fio->rwflag = POSITIN;
	fio->ateof = 0;
	fio->ateod = 0;
	fio->last_recbits = fio->recbits;
	fio->recbits = 0;
	return (ret);
	}
