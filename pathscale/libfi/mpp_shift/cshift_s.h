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


/* USMID @(#) libfi/mpp_shift/cshift_s.h	92.0	05/26/95 17:16:53 */


	int		i;
	int		src_extents[RANK];
	int		shft_extents[RANK];
	int		dim_val;
	int		shflag, shftval, *shptr;
	void		*result_base_ptr;
	void		*source_sdd_ptr;
	void		*shift_sdd_ptr;
	int		*iptr;
	_f_real		*fptr;
	_f_comp		*cptr;
	void		*vptr;

/*
 *	If the source array is not distributed, call the current Y-MP
 *	version of CSHIFT.
 */
	if (!_is_shared_pointer(source->base_addr.a.ptr)) {
	    _CSHIFT(result, source, shift, dim);
	} else {

/*
 *	If any of the extents for the source, shift, or result array
 *	arguments are zero, return to the caller with a NULL result.
 *	See f90_preproc.h for macro definition.
 */

	    EXTENT_CHK (source);
	    if (result->assoc)
		EXTENT_CHK (result);
	    if (shift->n_dim > 0)
		EXTENT_CHK (shift);

/*
 *	Check to insure that DIM contains a legal value from 1 through the
 *	extent of the source array.  If the DIM argument was not specified,
 *	set it to a default value of 1.
 */

	    INIT_DIM_COUNT();

/*
 *	Allocate the result temporary and fill in the dope vector.
 */

	    _malloc_result(result,source,&result_base_ptr);
	    result->base_addr.a.ptr = (void *) result_base_ptr;

/*
 *	Set up sdd pointers to the source, and shift arrays.
 *	Check to see if the SHIFT argument provided by the user is an
 *	array or a scalar.  If the SHIFT argument is a shared array,
 *	set up the shift's sdd pointer.
 */

	    source_sdd_ptr = source->base_addr.a.ptr;
	    shift_sdd_ptr = shift->base_addr.a.ptr;

/*
 *	Determine if the shift count is a scalar or an array.  If scalar,
 *	set up the flag and the scalar value counter.
 */

	    INIT_SHIFT_COUNT();

/*
 *      Set up the extent arrays
 */

#pragma _CRI    shortloop
	    for (i = 0; i < RANK; i++) {
		src_extents[i] = source->dimension[i].extent;
	    }
	    if (!shflag) {
#pragma _CRI    shortloop
		for (i = 0; i < RANK-1; i++) {
		    shft_extents[i] = shift->dimension[i].extent;
		}
	    } else {
#pragma _CRI	shortloop
		for (i = 0; i < RANK-1; i++) {
		    shft_extents[i] = 1;
		}
	    }
	}
