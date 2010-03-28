/*
 * Copyright 2004, 2005, 2006 PathScale, Inc.  All Rights Reserved.
 */

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


#pragma ident "@(#) libu/ffio/ffflush.c	92.3	10/11/99 15:30:43"

#include <errno.h>
#include <stdarg.h>
#include <ffio.h>

/*
 * This module contains a driver for the flush function.
 *
 */

int
#if	defined(__mips) || defined(_LITTLE_ENDIAN)
ffflush(int fd, struct ffsw *stat)
#else
ffflush(int fd, ...)
#endif
	{
	register int	ret;
	struct fdinfo	*fio;
	struct ffsw	*pstat;
#ifdef	_CRAY
	register int	na;
	struct ffsw	locstat;
	va_list		ap;

	NUMARG(na);

	if (na < 2)
		pstat	= &locstat;
	else
		{
		va_start(ap, fd);
		pstat	= va_arg(ap, struct ffsw *);
		}
#else
	pstat	= stat;
#endif

	fio	= GETIOB(fd);
	_PRAGMA_INLINE(__ffflush)
	ret	= __ffflush(fio, pstat);

	/* set errno only if stat was not passed */

#ifdef	_CRAY
	if (na < 2)
		errno	= locstat.sw_error;
#endif

	return (ret);
	}

int
__ffflush(struct fdinfo *fio, struct ffsw *pstat)
{
	register int	ret;

	CHECK_FIOPTR(fio, pstat);
	ret	= XRCALL(fio, flushrtn) fio, pstat);
	return (ret);
}

/*
 * _ff_flush()
 * Generic flush routine.  If no special handling is required.
 * It simply writes out the buffer and quits.  If in read mode,
 * it resets the buffer pointers.
 */

int
_ff_flush(struct fdinfo *fio, struct ffsw *stat)
	{
	int	ubc;
	ssize_t	wret;

	if(fio->rwflag == WRITIN)
		{
		ubc	= (8 - (fio->_cnt & 7)) & 7;
		if (fio->_cnt > 0)
			{
			WRITEBLK(wret, fio, (size_t)(fio->_cnt >> 3), stat, FULL, &ubc);
			if (wret < 0)
				return(-1);
			}
		}
	else
		{
/*
 *		If in read mode, simply throw away anything already in the
 *		buffer.
 */
		fio->_ptr	= fio->_base;
		fio->_cnt	= 0;
		fio->segbits	= 0;
		fio->last_recbits = fio->recbits;
		fio->recbits	= 0;
		fio->scc	= FULL;
		fio->ateof	= NO;
		fio->ateod	= NO;
		fio->lastscc	= FULL;
		fio->rwflag	= POSITIN;
		}

	return (0);
	}
