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


static char USMID[] = "@(#) libu/ffio/c1/er90bweod.c	92.0	10/08/98 14:57:41";

#include <sys/types.h>
#include <sys/iosw.h>
#include <errno.h>
#include <stdio.h>
#include <fcntl.h>
#include <ffio.h>
#include "er90by.h"


/*
 * _er90b_weod(): ER90 byte stream write EOD
 *	Currently we can't do this if we were reading or if we
 *	just positioned.
 */
_er90b_weod(struct fdinfo *fio, struct ffsw *stat)
{
	fio->ateof = NO;
	fio->ateod = YES;
	if ((fio->rwflag == POSITIN ) ||(fio->rwflag == READIN))
		ERETURN(stat,FENOENDF,0);
	fio->rwflag = WRITIN;
	SETSTAT(stat, FFEOD, 0);
	return(0);
}
