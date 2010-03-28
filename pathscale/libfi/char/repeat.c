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


#pragma ident "@(#) libfi/char/repeat.c	92.2	07/30/99 10:09:59"

/*	REPEAT - Concatenate several copies of a string		*/

#include <fortran.h>
#include <liberrno.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <cray/dopevec.h>

void
_REPEAT(
	DopeVectorType	*result,
	_fcd		source,
	_f_int		*ncopies)
{
	char	*sptr;			/* source pointer		*/
	char	*rptr;			/* result pointer		*/
	char	*rptr1;			/* result pointer		*/
	int	src_len;		/* source length		*/
	int	i, j, k;		/* index variables		*/
	int	tot_chr;		/* total characters to copy	*/
	int	lp_cnt;			/* loop count			*/
	_f_int	copies;			/* number of copies		*/

/*	Get source and result pointers and lengths	*/

	sptr = _fcdtocp (source);
	src_len = _fcdlen (source);

/*	See if ncopies is valid				*/

	copies = (_f_int) *ncopies;

	if (copies < 0)
	    _lerror (_LELVL_ABORT, FERPTNEG);
	else if (copies == 0 || src_len == 0) {
	    result->base_addr.charptr = _cptofcd ((char *) NULL, 0);
#if !defined(_ADDR64) && !defined(_WORD32) && !defined(__mips) && !defined(_LITTLE_ENDIAN)
	    result->base_addr.a.el_len = 0;
#endif
	    return;
	}

/*	Determine total count of characters to copy and loop count	*/

	tot_chr = src_len * copies;

/*	If necessary, allocate space for result		*/

	if (result->assoc)
	    _lerror (_LELVL_ABORT, FEINTUNK);

	result->assoc = 1;
	result->base_addr.a.ptr = (void *) malloc (tot_chr);
	if (result->base_addr.a.ptr == NULL)
	    _lerror (_LELVL_ABORT, FENOMEMY);
	rptr = (char *) result->base_addr.a.ptr;
	result->base_addr.charptr = _cptofcd (rptr, tot_chr);
	result->orig_base = result->base_addr.a.ptr;
	result->orig_size = tot_chr;
#if !defined(_ADDR64) && !defined(_WORD32) && !defined(__mips) && !defined(_LITTLE_ENDIAN)
	result->base_addr.a.el_len = tot_chr << 3;
#endif

/*	Copy characters	and return	*/

	for (i = 0; i < copies; i++) {
	    rptr1 = (char *) rptr + (i * src_len);
	    (void) memcpy (rptr1, sptr, src_len);
	}

	return;
}
