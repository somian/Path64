/*
 * Copyright (C) 2008. PathScale, LLC. All Rights Reserved.
 */
/*
 *  Copyright (C) 2007. QLogic Corporation. All Rights Reserved.
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


#pragma ident "@(#) libfi/element/associated.c	92.1	06/16/99 15:47:23"
#include <fortran.h>
#include <stdlib.h>
#include <liberrno.h>
#include <cray/dopevec.h>

#define TRUE 1
#define FALSE 0

/*
 *  ASSOCIATED  Returns the association status of its pointer argument
 *                or indicates the pointer is associated with the target.
 *              If TARGET argument is NULL, return TRUE if POINTER is
 *                currently associated.  Otherwise return FALSE.
 *              If TARGET argument is present and is a TARGET, return TRUE
 *                if POINTER is currently associated with TARGET.
 *                Otherwise, return FALSE.
 *              If TARGET argument is present and is a pointer, return TRUE
 *                if POINTER and TARGET are currently associated with the
 *                same target.  Otherwise, return FALSE. If either TARGET
 *                or POINTER are disassociated, return FALSE.
 */

_f_log
_ASSOCIATED (DopeVectorType * pointer,
	     DopeVectorType * target)
{
	int iresult;
	int loopj;
	int rank;
	long ptrlen;
	long tarlen;
        iresult = FALSE;

	/* if pointer is not associated, return FALSE */
	if (!pointer->assoc)
		return(_btol(iresult));

	/* if target is not present and pointer is associated,
	 * return TRUE
	 */
	if (target == NULL) {
		iresult = TRUE;
		return(_btol(iresult));
	}

	/* if target is a disassociated pointer,
	 * return FALSE
	 */
	if ((target->p_or_a == POINTTR) && (!target->assoc))
		return(_btol(iresult));

	rank = pointer->n_dim;
#if defined(_CRAY1) && !defined(_ADDR64) && !defined(_WORD32)
	ptrlen = pointer->base_addr.a.el_len;
	tarlen = target->base_addr.a.el_len;
#else
	ptrlen = _fcdlen(pointer->base_addr.charptr);
	tarlen = _fcdlen(target->base_addr.charptr);
#endif

	/* compare pointer and target fields */
	if ((pointer->base_addr.a.ptr == target->base_addr.a.ptr) &&
	    (ptrlen == tarlen) &&
	    (pointer->n_dim == target->n_dim) &&
	    (pointer->type_lens.type == target->type_lens.type)) {

		/* interp 000027, different low bounds are okay */
		for (loopj = 0; loopj < rank; loopj++) {
			if((pointer->dimension[loopj].extent !=
			    target->dimension[loopj].extent) ||
#ifdef KEY /* Bug 11834 */
                            (target->dimension[loopj].extent <= 0) ||
#endif /* KEY Bug 11834 */
			   (pointer->dimension[loopj].stride_mult !=
			    target->dimension[loopj].stride_mult))
				return(_btol(iresult));
		}
		iresult = TRUE;
	}
	return(_btol(iresult));
}

#ifdef	_F_LOG4
/*
 *  ASSOCIATED_4  Returns the association status of its pointer argument
 *                or indicates the pointer is associated with the target.
 *              If TARGET argument is NULL, return TRUE if POINTER is
 *                currently associated.  Otherwise return FALSE.
 *              If TARGET argument is present and is a TARGET, return TRUE
 *                if POINTER is currently associated with TARGET.
 *                Otherwise, return FALSE.
 *              If TARGET argument is present and is a pointer, return TRUE
 *                if POINTER and TARGET are currently associated with the
 *                same target.  Otherwise, return FALSE. If either TARGET
 *                or POINTER are disassociated, return FALSE.
 */

_f_log4
_ASSOCIATED_4 (DopeVectorType * pointer,
	     DopeVectorType * target)
{
	_f_int4 iresult;
	int loopj;
	int rank;
	long ptrlen;
	long tarlen;
        iresult = FALSE;

	/* if pointer is not associated, return FALSE */
	if (!pointer->assoc)
		return(_btol(iresult));

	/* if target is not present and pointer is associated,
	 * return TRUE
	 */
	if (target == NULL) {
		iresult = TRUE;
		return(_btol(iresult));
	}

	/* if target is a disassociated pointer,
	 * return FALSE
	 */
	if ((target->p_or_a == POINTTR) && (!target->assoc))
		return(_btol(iresult));

	rank = pointer->n_dim;
#if defined(_CRAY1) && !defined(_ADDR64) && !defined(_WORD32)
	ptrlen = pointer->base_addr.a.el_len;
	tarlen = target->base_addr.a.el_len;
#else
	ptrlen = _fcdlen(pointer->base_addr.charptr);
	tarlen = _fcdlen(target->base_addr.charptr);
#endif

	/* compare pointer and target fields */
	if ((pointer->base_addr.a.ptr == target->base_addr.a.ptr) &&
	    (ptrlen == tarlen) &&
#ifdef KEY /* Bug 7735 */
	    /* Element size must not be zero */
	    (tarlen != 0) &&
#endif /* Bug 7735 */
	    (pointer->n_dim == target->n_dim) &&
	    (pointer->type_lens.type == target->type_lens.type)) {

		/* interp 000027, different low bounds are okay */
		for (loopj = 0; loopj < rank; loopj++) {
			if((pointer->dimension[loopj].extent !=
			    target->dimension[loopj].extent) ||
#ifdef KEY /* Bug 11834 */
                            (target->dimension[loopj].extent <= 0) ||
#endif /* KEY Bug 11834 */
#ifdef KEY /* Bug 7735 */
			    /* Stride mult doesn't matter if only one element */
			    (pointer->dimension[loopj].extent > 1 &&
			      (pointer->dimension[loopj].stride_mult !=
			       target->dimension[loopj].stride_mult)
			    )
#else /* Bug 7735 */
			   (pointer->dimension[loopj].stride_mult !=
			    target->dimension[loopj].stride_mult)
#endif /* Bug 7735 */
			   )
				return(_btol(iresult));
		}
		iresult = TRUE;
	}
	return(_btol(iresult));
}

#endif


#ifdef	_F_LOG8
/*
 *  ASSOCIATED_8  Returns the association status of its pointer argument
 *                or indicates the pointer is associated with the target.
 *              If TARGET argument is NULL, return TRUE if POINTER is
 *                currently associated.  Otherwise return FALSE.
 *              If TARGET argument is present and is a TARGET, return TRUE
 *                if POINTER is currently associated with TARGET.
 *                Otherwise, return FALSE.
 *              If TARGET argument is present and is a pointer, return TRUE
 *                if POINTER and TARGET are currently associated with the
 *                same target.  Otherwise, return FALSE. If either TARGET
 *                or POINTER are disassociated, return FALSE.
 */

_f_log8
_ASSOCIATED_8 (DopeVectorType * pointer,
	     DopeVectorType * target)
{
	_f_int8 iresult;
	int loopj;
	int rank;
	long ptrlen;
	long tarlen;
        iresult = FALSE;

	/* if pointer is not associated, return FALSE */
	if (!pointer->assoc)
		return(_btol(iresult));

	/* if target is not present and pointer is associated,
	 * return TRUE
	 */
	if (target == NULL) {
		iresult = TRUE;
		return(_btol(iresult));
	}

	/* if target is a disassociated pointer,
	 * return FALSE
	 */
	if ((target->p_or_a == POINTTR) && (!target->assoc))
		return(_btol(iresult));

	rank = pointer->n_dim;
#if defined(_CRAY1) && !defined(_ADDR64) && !defined(_WORD32)
	ptrlen = pointer->base_addr.a.el_len;
	tarlen = target->base_addr.a.el_len;
#else
	ptrlen = _fcdlen(pointer->base_addr.charptr);
	tarlen = _fcdlen(target->base_addr.charptr);
#endif

	/* compare pointer and target fields */
	if ((pointer->base_addr.a.ptr == target->base_addr.a.ptr) &&
	    (ptrlen == tarlen) &&
	    (pointer->n_dim == target->n_dim) &&
	    (pointer->type_lens.type == target->type_lens.type)) {

		/* interp 000027, different low bounds are okay */
		for (loopj = 0; loopj < rank; loopj++) {
			if((pointer->dimension[loopj].extent !=
			    target->dimension[loopj].extent) ||
#ifdef KEY /* Bug 11834 */
                            (target->dimension[loopj].extent <= 0) ||
#endif /* KEY Bug 11834 */
			   (pointer->dimension[loopj].stride_mult !=
			    target->dimension[loopj].stride_mult))
				return(_btol(iresult));
		}
		iresult = TRUE;
	}
	return(_btol(iresult));
}

#endif
