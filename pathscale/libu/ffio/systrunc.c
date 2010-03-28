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


#pragma ident "@(#) libu/ffio/systrunc.c	92.1	06/29/99 13:16:47"

#include <errno.h>
#include <ffio.h>
#ifndef	_CRAY
#include <stdio.h>
#endif
#include <sys/types.h>
#include <sys/stat.h>
#include "sysio.h"

/*
 * _sys_trunc() calls the system call trunc(2) and returns status
 */

int
_sys_trunc(fio, stat)
struct fdinfo *fio;
struct ffsw *stat;
{
	int ret;
	struct stat statbuf;

#ifdef  _CRAY
	LOOP_SYSCALL(ret, trunc(fio->realfd));
#else
	off_t offset;

#ifdef __mips
	if (((struct sys_f *)fio->lyr_info)->needpos) {
		if (lseek( fio->realfd, ((struct sys_f *)fio->lyr_info)->curpos,
				0)  < 0)
		ERETURN(stat, errno, 0);
		((struct sys_f *)fio->lyr_info)->needpos = 0;
		offset = ((struct sys_f *)fio->lyr_info)->curpos;
	}
	else
#endif
	{
		offset = lseek(fio->realfd, 0, SEEK_CUR);
	}
	LOOP_SYSCALL(ret, ftruncate(fio->realfd, offset));
#endif
	if (ret < 0) {
		if (fstat(fio->realfd, &statbuf) < 0)
			ERETURN(stat, errno, 0);
		if (S_ISREG(statbuf.st_mode))
			ERETURN(stat, errno, 0);
#ifdef __mips
		ret = 0;
#else
		ret = statbuf.st_size;
#endif
	}
#ifdef __mips
	((struct sys_f *)fio->lyr_info)->endpos = offset;
	((struct sys_f *)fio->lyr_info)->curpos = offset;
#endif
	SETSTAT(stat, FFEOD, 0);
	fio->ateod = YES;
	fio->ateof = NO;
	return (ret);
}
