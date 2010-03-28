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


static char USMID[] = "@(#) libu/ffio/c1/bmxseek.c	92.0	10/08/98 14:57:41";


#include <sys/types.h>
#include <sys/iosw.h>
#include <errno.h>
#include <stdio.h>
#include <fcntl.h>
#include <ffio.h>
#include "bmxio.h"

_bmx_seek(fio, pos, whence, stat)
struct fdinfo *fio;
int pos, whence;
struct ffsw *stat;
	{
	int ret;

	if ((pos | whence) != 0)
		ERETURN(stat, FDC_ERR_NOSUP, 0);

	fio->last_recbits = fio->recbits;
	fio->recbits = 0;
	ret = _bmx_rewd(fio->lyr_info);
	if ( ret < 0 )
		ERETURN(stat, errno, 0);
	SETSTAT(stat, FFBOD, 0);
	fio->rwflag = POSITIN;
	return(ret);
	}
