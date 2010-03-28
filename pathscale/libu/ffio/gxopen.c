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


#pragma ident "@(#) libu/ffio/gxopen.c	92.1	06/29/99 13:16:47"

#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <ffio.h>
#include "fxlist.h"
#include "gxio.h"

DECLARE(X_XLIST);
struct xtr_s X_XLIST_P = { X_XLIST };

/*
 * Initialize the state of a foreign dataset.  Allocate buffers
 * and initialize pointers.
 */

_ffopen_t
_gen_xopen(
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
	int isvalid;
	struct gen_xf *xf_info;
/*
 *	Allocate private storage
 */
	xf_info = (struct gen_xf *)calloc(sizeof(struct gen_xf),1);
	if (xf_info == NULL) goto nomem;
	fio->lyr_info = (char *)xf_info;
/*
 *	select parameters based on record type
 */
	switch(fio->rtype)
		{
		case TR_NVE_V:
			xf_info->rdwlen = 112;		/* bits */
			break;
		case TR_CRAY_V:
			xf_info->rdwlen = 64;		/* bits */
			break;
#ifdef _OLD_F77
		case TR_UX_VAX:
		case TR_UX_SUN:
			xf_info->rdwlen = 32;		/* bits */
			break;
#endif
		case TR_205_W:
			xf_info->rdwlen = 64;		/* bits */
			break;
		}
	xf_info->last_lrdwaddr = 0;
	xf_info->lrdwaddr = 0;

/*
 *	Record the maximum record size in bits.   
 *	A value of 0 is stored if this is unspecified.
 */
	fio->maxrecsize = _ff_nparm_getv(spec, 1, &isvalid) * 8;

/*
 *	Record the buffer size in bits.
 */
	blksize = _ff_nparm_getv(spec, 2, &isvalid) * 8;
	if (! isvalid || blksize < 256)		/* bits, mighty small! */
		blksize = X_BUFSIZ * BITPBLOCK;
	else
		blksize = (blksize + 077) & (~077);/* round to word size */
/*
 *	Although the _ffbufsiz field is declared as long, 
 *	these routines use GETDATA and PUTDATA. Those macros
 *	assign the amount to be written to integers. So, to
 *	make this all work we need to be sure that the buffer size
 *	does not exceed the size of an integer. 
 */
	if (blksize > (1<<sizeof(int)*8-5)){
		_SETERROR(stat, FDC_ERR_BUFSIZ, 0);
		goto badret;
        }
	fio->_ffbufsiz = blksize;		/* bit size of buffer */

	ptr = malloc((blksize >> 3) + 16);
	if (ptr == NULL) goto nomem;

	SET_BPTR(fio->_base, CPTR2BP(ptr));
	fio->scc = SCCFULL;
	fio->lastscc = SCCFULL;
	fio->rwflag = POSITIN;
	fio->segbits = 0;
	fio->_cnt = 0;
	fio->_ptr = fio->_base;
/*
 *	Now, open the lower layers
 */
	nspec = spec;
	NEXT_SPEC(nspec);
	nextfio = _ffopen(name, flags, mode, nspec, stat, cbits, cblks, NULL,
			  oinf);
	if (nextfio < 0) goto badret;

	DUMP_IOB(fio); /* debugging only */
	return(nextfio);

nomem:
	_SETERROR(stat, FDC_ERR_NOMEM, 0);
badret:
	if (BPTR2CP(fio->_base) != NULL) free(BPTR2CP(fio->_base));
	if (fio->lyr_info != NULL) free(fio->lyr_info);
	return(_FFOPEN_ERR);
	}
