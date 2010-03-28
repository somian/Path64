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


#pragma ident "@(#) libu/ffio/mrgrow.c	92.1	06/29/99 13:16:47"

#include <stdio.h>
#include <stdlib.h>
#include <ffio.h>
#include "fssio.h"

/*
 * _mr_grow()
 *	Grow the MR allocation to accomodate the new bit size.
 *	grow by at least mininc sectors, and no more than maxsize.
 */
int
_mr_grow(fio, reqinc, stat)
struct fdinfo *fio;
long reqinc;
struct ffsw *stat;
	{
	int retval;
	char *newbase;
	long bits, mrsizeb, newsizeb, maxinc;
	struct mr_f *mr_info;

	retval = 0;
	mr_info = (struct mr_f *)fio->lyr_info;
/*
 *	If increment is zero, don't bother.
 */
	if (reqinc <= 0) return(retval);
/*
 *	If we have already overflowed, don't even try
 */
	if (mr_info->overflowed == YES) return(1);

	newsizeb = BITS2BLKS((mr_info->mrsize + reqinc) + (512*64 -1));
	if (newsizeb > mr_info->maxsize)
		{
		retval = 1;
		newsizeb = mr_info->maxsize;
		}
/*
 *	Bump the MR allocation, but don't overflow due to min inc.
 */
	mrsizeb = BITS2BLKS(mr_info->mrsize);	/* always mult of blks, so OK */
	maxinc = mr_info->maxsize - mrsizeb;
	if (maxinc <=0) return(retval);

	if ((newsizeb - mrsizeb) < mr_info->mininc)
		{
		if (mr_info->mininc < maxinc)
			newsizeb = mrsizeb + mr_info->mininc;
		else
			newsizeb = mrsizeb + maxinc;
		}
	if (mrsizeb == 0)
		{
		if (mr_info->minsize > newsizeb) newsizeb = mr_info->minsize;
		newbase = malloc(newsizeb * 4096);
		}
	else
		newbase = realloc(BPTR2CP(fio->_base), newsizeb*4096);
/*
 *	If allocation fails, just overflow.
 */
	if (newbase == NULL)
		return(1);
/*
 *	Remember that even if this is the first allocation, the fio->_ptr
 *	can be set off the end of the file.  This is an offset from a
 *	NULL base pointer, but is still the file position, and must be
 *	preserved.
 */
	bits = SUBT_BPTR(fio->_ptr, fio->_base); /* old ptr offset */
	SET_BPTR(fio->_base, CPTR2BP(newbase));		/* update base */
	SET_BPTR(fio->_ptr, INC_BPTR(fio->_base, bits)); /* ptr = base + bits */
	mr_info->mrsize = BLKS2BITS(newsizeb);
	if (fio->rtype != TR_FSS_SAVE)
		mr_info->ovoff = mr_info->mrsize;
	fio->_ffbufsiz = BLKS2BITS(newsizeb);
	return(retval);
	}
