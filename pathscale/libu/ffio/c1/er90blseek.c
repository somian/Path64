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


static char USMID[] = "@(#) libu/ffio/c1/er90blseek.c	92.0	10/08/98 14:57:41";


#include <errno.h>
#include <ffio.h>
#include "er90by.h"

/*
 * _er90b_lseek() ER90 byte-stream "seeks". We can only rewind,
 * because we cannot know our current position relative to the
 * beginning of the file - this is because files can span volumes
 */

_er90b_lseek(struct fdinfo *fio, int pos, int whence, struct ffsw *stat)
{
int ret;

	if (whence == 0 && pos == 0) {
		ret = _er90b_rewd((ER90BYT *)fio->lyr_info);
		if (ret < 0)
			ERETURN(stat, errno, 0);
		fio->ateof = 0;
		fio->ateod = 0;
		SETSTAT(stat, FFBOD, 0);
	}
	else {
		ERETURN(stat, FDC_ERR_NOSUP, 0);
	}
	fio->rwflag = POSITIN;
	return (0);
}
