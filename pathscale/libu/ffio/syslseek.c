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


#pragma ident "@(#) libu/ffio/syslseek.c	92.1	06/29/99 13:16:47"

#include <errno.h>
#include <ffio.h>
#include <unistd.h>
#include "sysio.h"

/*
 * _sys_lseek() calls the system call lseek(2) and returns status.
 */

_ffseek_t
_sys_lseek(struct fdinfo *fio, off_t pos, int whence, struct ffsw *stat)
	{
	off_t ret;
	
#ifdef __mips
	if ((((struct sys_f *)(fio->lyr_info))->needpos) &&
		whence == SEEK_CUR)
		ret = lseek(fio->realfd,
		    pos + ((struct sys_f *)(fio->lyr_info))->curpos,
		    SEEK_SET);
	else
		ret = lseek(fio->realfd, pos, whence);
#else
	ret = lseek(fio->realfd, pos, whence);
#endif
	if (ret < 0)
		{
		ERETURN(stat, errno, 0);
		}
#ifdef __mips
	((struct sys_f *)(fio->lyr_info))->curpos = ret;
	((struct sys_f *)(fio->lyr_info))->needpos = 0;
#endif
	fio->rwflag = POSITIN;
	if (whence == SEEK_SET && pos == 0)
		{
		fio->ateof = 0;
		fio->ateod = 0;
		SETSTAT(stat, FFBOD, 0);
		}
	else if (whence == SEEK_END && pos == 0)
		{
		fio->ateof = 1;
		fio->ateod = 1; /* seek to end, set stat */
		SETSTAT(stat, FFEOD, 0);
		}
	else
		SETSTAT(stat, FFCNT, 0);

	fio->last_recbits = fio->recbits;
	fio->recbits = 0;
	return (ret);
	}
