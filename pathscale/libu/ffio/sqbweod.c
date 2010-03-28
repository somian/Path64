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


#pragma ident "@(#) libu/ffio/sqbweod.c	92.1	06/29/99 13:16:47"


#include <stdio.h>
#include <ffio.h>
#include <errno.h>
#include "sqbio.h"

/*
 * Write an EOD to a buffer-layer file.
 *
 */
int
_sqb_weod(struct fdinfo *fio, struct ffsw *stat)
{
	struct fdinfo	*llfio;


	if (fio->rwflag == WRITIN) {
		if (_sqb_flush(fio, stat) < 0) {
			return(ERR);
		}
	}
	else if (fio->rwflag == READIN || fio->rwflag == POSITIN) {
		/* synchronize physical position with logical */
		if (_sqb_sync(fio, stat, 1) < 0) {
			return(ERR);
		}
	}
	fio->rwflag = WRITIN;

/*
 *	Truncate the underlying layer at the same location.
 */
	llfio  = fio->fioptr;

	if (XRCALL(llfio,weodrtn) llfio, stat) == ERR)
		return(ERR);

	SETSTAT(stat, FFEOD, 0);
	return(0);
}
