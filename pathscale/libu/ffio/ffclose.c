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


#pragma ident "@(#) libu/ffio/ffclose.c	92.2	10/11/99 15:30:43"

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <stdarg.h>
#include <ffio.h>
#include <cray/portdefs.h>

/*
 * ffclose()
 * Close an FFIO file.  The basic algorithm is as follows:
 *	Handle optional parameter;
 *	Close layer list;
 *	free head of list;
 *	set errno if no stat param passed;
 *	return;
 */

void _zerocnvrttbl(int fd);

#if	defined(__mips) || defined(_LITTLE_ENDIAN)
ffclose(int fd)
#else
ffclose(int fd, ...)
#endif
{
	struct fdinfo *fio;
	int ret, na;
	struct ffsw locstat, *pstat;
	va_list ap;

	fio = GETIOB(fd);
#ifdef	_UNICOS
	NUMARG(na);
#elif   defined(__mips) || defined(_LITTLE_ENDIAN)
	na = 1;
#else
	na = 2;
#endif
	if (na < 2)
		pstat = &locstat;
#if	!defined(__mips) && !defined(_LITTLE_ENDIAN)
	else {
		va_start(ap, fd);
		pstat = va_arg(ap, struct ffsw *);
	}
#endif
	_PRAGMA_INLINE(__ffclose)
	ret = __ffclose(fio, pstat);
	/* set errno only if stat was not passed */
	if (na < 2)
		errno = locstat.sw_error;
#if	defined(_mips) || defined(_LITTLE_ENDIAN)
	_zerocnvrttbl(fd);
#endif
	return (ret);
}

/* This routine is like ffclose, except it expects 2 parameters */
ffclosef(int fd, struct ffsw *pstat)
{
	struct fdinfo *fio;
	int ret;

	fio = GETIOB(fd);
	_PRAGMA_INLINE(__ffclose)
	ret = __ffclose(fio, pstat);
#if	defined(__mip) || defined(_LITTLE_ENDIAN)
	_zerocnvrttbl(fd);
#endif
	return (ret);
}

__ffclose(struct fdinfo *fio, struct ffsw *pstat)
{
	int ret;
	CHECK_FIOPTR(fio, pstat);
	ret = XRCALL(fio, closertn) fio, pstat);
	fio->magic = 0;			/* invalidate this fdinfo struct */
	free(fio);
	return (ret);
}

/*
 * _ff_close()
 * Close an FFIO file.  The basic algorithm is as follows:
 *
 *	flush buffer
 *	if appropriate, truncate the file
 *	do cleanup on your layer
 *	if lower level exists
 *		call lower level close routine
 *		free lower level info block
 *	return
 *
 * Note that it is the callers responsibility to free the memory
 * occupied by the info block of the layer(s) it closes.
 */
int
_ff_close(struct fdinfo *fio, struct ffsw *stat)
{
        struct ffc_info_s info;
	struct fdinfo *llfio;
	int ret;

	ret = 0;

/*
 *	If the file has been writing, flush and truncate after the last
 *	record written.
 */
	if (fio->rwflag == WRITIN) {

		ret = XRCALL(fio, flushrtn) fio, stat);
		if (ret != 0) return(ERR);

		ret = XRCALL(fio, fcntlrtn) fio, FC_GETINFO, &info, stat);
		if (ret != 0) return(ERR);

		if (info.ffc_flags & FFC_WRTRUNC) {
			ret = XRCALL(fio, weodrtn) fio, stat);
			if (ret != 0) return(ERR);
		}
	}

	if (BPTR2CP(fio->_base) != NULL)
		free(BPTR2CP(fio->_base));	/* free buffer */
	if (fio->lyr_info != NULL)
		free((char *)fio->lyr_info);	/* free private storage */
	llfio = fio->fioptr;
	if (llfio != NULL) {
		ret = XRCALL(llfio, closertn) llfio, stat);
		free(llfio);		/* free info block */
	}
	return(ret);
}

