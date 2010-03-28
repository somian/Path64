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


#pragma ident"@(#) libu/ffio/ffpos.c	92.2	10/11/99 15:30:43"

#include <errno.h>
#include <ffio.h>

/*
 * ffpos()
 *	position a dataset/file.  returns zero on success.  ERR on failure
 *	with the stat->sw_error containing error code.
 *
 *	All parameters are mandatory
 *
 *	fd - file descriptor (dummy)
 *	cmd - command code
 *	arg - pointer to input or output data
 *	len - length of the space available at arg.  This is used
 *		primarily on output to avoid overwriting the available
 *		memory.
 *	stat - status return parameter
 */

#if	defined(__mips) || defined(_LITTLE_ENDIAN)
off_t
#else
int
#endif
ffpos(
int fd,
int cmd,
#if	defined(__mips) || defined(_LITTLE_ENDIAN)
void *arg,
#else
long *arg,
#endif
int len,
struct ffsw *stat
)
	{
	struct fdinfo *fio;
	_ffseek_t ret;

	fio = GETIOB(fd);
	CHECK_FIOPTR(fio, stat);
#if	defined(__mips) || defined(_LITTLE_ENDIAN)
	if (cmd == FP_BSEEK)
		ERETURN(stat, FDC_ERR_NOSUP, 0);
#endif
	ret = XRCALL(fio, posrtn) fio, cmd, arg, len, stat);
	return (ret);
	}

_ffseek_t
_ff_pos(
struct fdinfo	*fio,
int		cmd,
void		*argp,
int		len,
struct ffsw	*stat)
{
	int ret;
	long bytes;
	long *arg;
	arg = (long *)argp;

	switch (cmd) {
#if	!defined(__mips) && !defined(_LITTLE_ENDIAN)
	/* Before supporting this on MIPS, we need to look at what the */
	/* arguments should be. A long (for *arg) is not big enough */
	/* to hold the total file offset on 32-bit machines */
	case FP_BSEEK:
/*
 *		This should be changed someday to properly
 *		handle a bit position...
 */
		if ((*arg & 7) != 0)
			ERETURN(stat, FDC_ERR_BADSK, 0);
		if (*arg < 0) {
			bytes = (-(*arg))>>3;
			bytes = -bytes;
		}
		else
			bytes = *arg>>3;

		ret = XRCALL(fio, seekrtn) fio, bytes, *(arg+1), stat);

		return(_dshiftl(ret, ret, 3)); 

#endif
	default:
		ERETURN(stat, FDC_ERR_NOSUP, 0);
	}
}
