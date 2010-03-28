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


#pragma ident "@(#) libfi/char/trim.c	92.2	07/30/99 10:09:59"

/*
 *	TRIM - Return the character argument with trailing blank characters
 *		removed
 */

#include <fortran.h>
#include <liberrno.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <cray/dopevec.h>

extern long	_zero_entity;	/* nonzero addr for PRESENT func */

void
_TRIM(
	DopeVectorType	*result,
	_fcd		source)
{
	int		src_len;
	int		leng;
	char		*sptr;
	char		*rptr;
	int		i;

/*	Convert Fortran Character Descriptors to C pointers	*/

	src_len	= _fcdlen (source);
	sptr	= _fcdtocp (source);

/*	Determine length of string to return			*/

	leng	= 0;
	for (i = src_len - 1; i >= 0; i--) {
	    if (sptr[i] != ' ') {
		leng	= i+1;
		break;
	    }
	}

/*	Allocate result space, if necessary	*/

	if (!result->assoc) {

/*	Set the base address of the result dope vector to special
 *	address when source string is null or all blanks so the
 *	compiler can do a check of nonzero to distinguish between
 *	an argument that is not present and one that is zero-sized.
 */

	    if (leng > 0) {
		result->base_addr.a.ptr = (void *) malloc(leng);
		if (result->base_addr.a.ptr == NULL)
		    _lerror(_LELVL_ABORT, FENOMEMY);
		rptr = (char *) result->base_addr.a.ptr;
	    } else 
		rptr	= (void *) &_zero_entity;

	    /* set assoc flag even for zero-sized allocation */
	    result->assoc	= 1;
	    result->orig_base	= result->base_addr.a.ptr;
	    result->base_addr.charptr	= _cptofcd (rptr, leng);
	    result->orig_size	= leng << 3;
#if !defined(_ADDR64) && !defined(_WORD32) && !defined(__mips) && !defined(_LITTLE_ENDIAN)
	    result->base_addr.a.el_len	= leng << 3;
#endif
	} else
	    _lerror(_LELVL_ABORT, FEINTUNK);

/*	If source string is all null, all blanks, or no trailing blanks,
 *	return string without any assignment.  Otherwise, return the
 *	entire string without trailing blanks.
 */
	if (leng > 0)
	    (void) memcpy (rptr, sptr, leng);

	return;
}
