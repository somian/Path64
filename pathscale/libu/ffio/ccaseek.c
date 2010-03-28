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


#pragma ident "@(#) libu/ffio/ccaseek.c	92.1	06/29/99 13:16:47"


#include <stdio.h>
#include <ffio.h>
#include "ccaio.h"

/*
 * _cca_seek
 *
 * Perform a seek operation on a CACHE file.  
 *
 * Return value:
 *
 *	The new byte postion in the file.
 *	If an error occurs, -1 is returned, and the stat->sw_error field is 
 *	set.
 */
_ffseek_t
_cca_seek(
struct fdinfo	*fio, 		/* ffio file descriptor. */
off_t		pos,		/* requested byte offset */
int		whence,		/* SEEK_SET, SEEK_CUR, or SEEK_END */
struct ffsw	*stat		/* status return word */
)
{
	off_t		oldpos;		/* old bit offset within file */
	off_t		newpos;		/* new bit offset within file */
	int		newstat;	/* status for FFSTAT */
	_ffseek_t	ret;
	struct cca_f	*cca_info;
        struct fdinfo   *llfio;
	int		err;

	cca_info = (struct cca_f *)fio->lyr_info;
        llfio    = fio->fioptr;

        if(CCA_SOFT_BYPASS)
        {
             ret = XRCALL(llfio,seekrtn) llfio, pos, whence, stat);
             return( ret );
        }

/*
 *	Do special CACHE class things
 */
	oldpos = cca_info->cpos;
	switch (whence) {
	case SEEK_SET:
		newpos = BYTES2BITS(pos);
		break;
	case SEEK_CUR:
		newpos = oldpos + BYTES2BITS(pos);
		break;
	case SEEK_END:
		if (cca_info->is_blkspec) {
			err = FDC_ERR_NOSUP;
			goto err_ret;
		}
		newpos = cca_info->fsize + BYTES2BITS(pos);
		break;
	default:
		err = FDC_ERR_BADSK;
		goto err_ret;
	}
/*
 *	Set up FFSTAT return, according to resulting position, but don't set
 *	it yet.
 */
	newstat = FFCNT;
	if (newpos == 0)
		newstat = FFBOD;
	else if (newpos == cca_info->fsize)
		newstat = FFEOD;

	if (newpos < 0) {
		err = FDC_ERR_BADSK;
		goto err_ret;
	}


	cca_info->cpos = newpos;
	fio->rwflag = POSITIN;
	if (newpos < cca_info->fsize) {
		fio->ateof = 0;
		fio->ateod = 0;
	}
	fio->recbits = 0;
	ret = BITS2BYTES(newpos);
	SETSTAT(stat, newstat, 0);
	return (ret);

err_ret:
        ERETURN(stat, err, 0);
}
