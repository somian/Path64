/*
 * Copyright 2005, 2006 PathScale, Inc.  All Rights Reserved.
 */

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


#pragma ident "@(#) libu/ffio/f77open.c	92.1	06/29/99 13:16:47"


#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <ffio.h>
#include "fxlist.h"
#include "f77io.h"

DECLARE(F77_XLIST);
struct xtr_s F77_XLIST_P = { F77_XLIST };

/*
 * Open an f77 file. These routines handle f77 and f77.vax
 * The only difference is that bytes in the control word are
 * swapped in f77.vax
 */
_ffopen_t
_f77_xopen(
const char	*name,
int		flags,
mode_t		mode,
struct fdinfo	*fio,
union spec_u	*spec,
struct ffsw	*stat,
long		cbits,
int		cblks,
struct gl_o_inf *oinf
)
{
	char *ptr;
	union spec_u *nspec;
	int blksize;
	_ffopen_t  nextfio;
        struct fdinfo *nfioptr;
	int isvalid;
	struct f77_xf *xf_info;
	struct ffc_info_s layer_info;

/*
 *	Allocate private storage
 */
	xf_info = (struct f77_xf *)calloc(sizeof(struct f77_xf),1);
	if (xf_info == NULL) goto nomem;
	fio->lyr_info = (char *)xf_info;

/*
 *	Record the maximum record size in bytes.   
 */
	xf_info->maxrecsize = _ff_nparm_getv(spec, 1, &isvalid) ;
	if (!isvalid)
#ifdef KEY /* Bug 4663 */
		xf_info->maxrecsize = 0xffffffff;
#else /* KEY Bug 4663 */
		xf_info->maxrecsize = 0x7fffffff;
#endif /* KEY Bug 4663 */

/*
 *	Record the buffer size in bytes
 *      The user enters this in bytes!
 */
	blksize = _ff_nparm_getv(spec, 2, &isvalid) ;
	if (! isvalid  || blksize < 64)		
		blksize = F77_BUFSIZ *BYTPBLOCK; /* default */
        else
                blksize = (blksize + 07) & (~07);/* round to word size */

	xf_info->_ffbufsiz = blksize;	/* byte size of buffer */


	ptr = malloc(xf_info->_ffbufsiz  + 16);
	if (ptr == NULL) goto nomem;

	xf_info->_base = ptr;
	fio->rwflag = POSITIN;
	xf_info->_ptr = xf_info->_base;
/*
 *	Now, open the lower layers
 */
	nspec = spec;
	NEXT_SPEC(nspec);
	nextfio = _ffopen(name, flags, mode, nspec, stat, cbits, cblks, NULL,
			  oinf);
	if (nextfio == _FFOPEN_ERR) goto badret;

        nfioptr = (struct fdinfo *)nextfio;

	DUMP_IOB(fio); /* debugging only */
        if (XRCALL(nfioptr,fcntlrtn) nfioptr, FC_GETINFO, &layer_info, stat) == ERR)
        	goto badret;

        if ( layer_info.ffc_flags & FFC_CANSYLISTIO )
                xf_info->do_sylistio = 1;

	return(nextfio);

nomem:
	_SETERROR(stat, FDC_ERR_NOMEM, 0);
badret:
	if (fio->lyr_info != NULL) { 
		if (xf_info->_base != NULL) free(xf_info->_base);
		free(fio->lyr_info);
	}
	return(_FFOPEN_ERR);
}
