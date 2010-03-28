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


static char USMID[] = "@(#) libu/ffio/c1/ffsetsp.c	92.0	10/08/98 14:57:41";


#include <errno.h>
#include <stdarg.h>
#include <ffio.h>
extern void _eov_load();

/*
 * Initiate special end of volume processing for tapes.
 * This is necessary so that eov routines can be soft referenced
 * in bmxfcntl.c
 */

ffsetsp(int fd, ...)
	{
	struct fdinfo *fio;
	struct ffsw locstat, *pstat;
	int ret, na;
	va_list ap;

	fio = GETIOB(fd);
	NUMARG(na);
	if (na < 2)
		pstat = &locstat;
	else
		{
		va_start(ap, fd);
		pstat = va_arg(ap, struct ffsw *);
		}
	CHECK_FIOPTR(fio, pstat);
	_eov_load(1);	/* Call a routine that provides hard references */
			/* to eov routines. */
	ret = XRCALL(fio, fcntlrtn) fio, FC_SETSP, 1, pstat);
	if (na < 2)
		errno = locstat.sw_error;
	return (ret);
	}
