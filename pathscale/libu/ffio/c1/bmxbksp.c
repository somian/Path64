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


static char USMID[] = "@(#) libu/ffio/c1/bmxbksp.c	92.0	10/08/98 14:57:41";


#include <sys/types.h>
#include <sys/iosw.h>
#include <errno.h>
#include <stdio.h>
#include <fcntl.h>
#include <ffio.h>
#include <taperr.h>
#include "bmxio.h"


_bmx_bksp(fio, stat)
struct fdinfo *fio;
struct ffsw *stat;
	{
	int ret;

	fio->last_recbits = fio->recbits;
	fio->recbits = 0;
	ret = _bmx_stpos(fio->lyr_info, FP_TPOS_BACK, 1,0,0,0);
	if ( ret < 0  && errno != ETBOF)
		ERETURN(stat, errno, 0);
	fio->rwflag = POSITIN;
	return(0);
	}
