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


#pragma ident "@(#) libu/ffio/blxopen.c	92.2	08/18/99 14:45:43"

#include <stdio.h>
#include <stdlib.h>
#include <ffio.h>
#include "blxio.h"
#include "fxlist.h"

DECLARE(BLX_XLIST);
struct xtr_s BLX_XLIST_P = { BLX_XLIST };

/*
 * Initialize the state of a foreign dataset.  Allocate buffers
 * and initialize pointers.
 */

_ffopen_t
_blx_open(
char		*name,
int		flags,
mode_t		mode,
struct fdinfo	*fio,
union spec_u	*spec,
struct ffsw	*stat,
long		cbits,
int		cblks,
struct gl_o_inf *oinf)
	{
	char *ptr;
	union spec_u *nspec;
	int blksize;
	_ffopen_t nextfio;
	struct blx_info *blx_dat = NULL;
	struct ffsw *dumstat;
	struct ffc_info_s ffci;
/*
 *	Set up buffer.  No provision exists to change the size.
 */
	blksize =  512 * 8 * 8; /* bits */
  	fio->maxblksize = blksize;
	fio->_ffbufsiz = blksize; /* bit size of buffer */

	ptr = malloc((blksize >> 3) + 16);
	if (ptr == NULL) goto nomem;

	SET_BPTR(fio->_base, CPTR2BP(ptr));
	fio->rwflag = POSITIN;
	fio->segbits = 0;
	fio->_cnt = 0;
	fio->_ptr = fio->_base;
/*
 *	Get memory for blxio structure and let the layer know where to
 * 	find it.
 */
	blx_dat = (struct blx_info *)calloc(sizeof(struct blx_info), 1);	
	if (blx_dat == NULL) goto nomem;
	fio->lyr_info = (char *)blx_dat;

/* 
 * 	Set up the blx_char, cmp_char(character to be compressed), and 
 *	blx_off (offset to be used); defaults are COS defaults.
 */
	blx_dat->blx_char = 033 << 56; /* ASCII ESC */
	blx_dat->cmp_char = ' ' << 56; /* ASCII blank */

	if (fio->rtype == 0) fio->rtype = BLX_COS;
	switch(fio->rtype)
		{
		case BLX_COS:
			blx_dat->blx_off = 0x1e;
			break;
		case BLX_CTSS:
			blx_dat->blx_off = 0x30;
			break;
		}			

/*
 *	If caller wants to change the blx_char or the character being
 *	compressed...
 */
	if (spec->fld.recsize != 0)
		{
		blx_dat->blx_char = spec->fld.recsize << 56; /* normally ESC */
		}

	if (spec->fld.mbs != 0)
		{
		blx_dat->cmp_char = spec->fld.mbs << 56; /* normally 0x20 */
		}

/*
 *	Now, open the lower layers
 */
	nspec = spec;
	NEXT_SPEC(nspec);
	nextfio = _ffopen(name, flags, mode, nspec, stat, cbits, cblks, NULL,
			oinf);
	if (nextfio < 0) goto badret;

	fio->fioptr = (struct fdinfo *)nextfio;
	XRCALL(fio->fioptr, fcntlrtn) fio->fioptr, FC_GETINFO,
		&ffci, &dumstat);
	if (ffci.ffc_flags & FFC_REC)
		blx_dat->llblocked = 1;
	DUMP_IOB(fio); /* debugging only */
	return(nextfio);

nomem:
	_SETERROR(stat, FDC_ERR_NOMEM, 0);
badret:
	if (BPTR2CP(fio->_base) != NULL) free(BPTR2CP(fio->_base));
	if ((char *)blx_dat != NULL) free((char *)blx_dat);
	return(ERR);
	}
