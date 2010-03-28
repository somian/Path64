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


#pragma ident "@(#) libfi/mpp_scan/scanmin_jp_1.c	92.1	07/13/99 10:21:33"

#include <stdlib.h>
#include <liberrno.h>
#include <fmath.h>
#include <cray/dopevec.h>
#include "f90_macros.h"

#define RANK		1

/*
 *	Compiler generated call: CALL _SCANMIN_JP1(RES, SRC, STOP, DIM, MASK)
 *
 *	Purpose: Determine the maximum value of the elements of SRC
 *	         along dimension DIM corresponding to the true elements
 *	         of MASK. This particular routine handles source arrays
 *	         of rank 1 with a data type of 64-bit integer.
 *
 *	Arguments:
 *	        RES    - Dope vector for temporary result array
 *		SRC    - Dope vector for user source array
 *		STOP   - Dope vector for stop array
 *	        DIM    - Dimension to operate along
 *	        MASK   - Dope vector for logical mask array
 *
 *      Description:
 *		This is the MPP version of SCANMIN. This particular
 *		file contains the the intermediate type-specific
 *		routines. These routines parse and update the dope
 *		vectors, allocate either shared or private space for
 *		the result temporary, and possibly update the shared
 *		data desriptor (sdd) for the result temporary. Once
 *		this set-up work is complete, a Fortran subroutine
 *		is called which uses features from the Fortran
 *		Programming Model to distribute the word across all
 *		processors.
 *
 *		Include file segmented_scan_p.h contains the rank independent
 *		source code for this routine.
 */

void
_SCANMIN_JP1 (	DopeVectorType	*result,
		DopeVectorType	*source,
		DopeVectorType	*stop,
		long		*dim,
		DopeVectorType	*mask)
{
	
#include "segmented_scan_p.h"

	if (stop_flag > 0) {
	    if (mask_flag == 1) {
		SCANMIN_MASK_JP1@ (result_sdd_ptr, source_sdd_ptr,
			stop_sdd_ptr, &dim_val, mask_sdd_ptr, src_extents,
			blkcnts);
	    } else {
		SCANMIN_NOMASK_JP1@ (result_sdd_ptr, source_sdd_ptr,
			stop_sdd_ptr, &dim_val, src_extents, blkcnts);
	    }
	}
}
