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


#pragma ident "@(#) libfi/mpp_scan/scanprod_ss_3.c	92.1	07/13/99 10:42:37"

#include <stdlib.h>
#include <liberrno.h>
#include <fmath.h>
#include <cray/dopevec.h>
#include "f90_macros.h"

#define RANK		3

/*
 *	Compiler generated call: CALL _SCANPROD_SS3(RES, SRC, STOP, DIM, MASK)
 *
 *	Purpose: Determine the maximum value of the elements of SRC
 *	         along dimension DIM corresponding to the true elements
 *               of MASK. This particular routine handles source arrays
 *	         of rank 3 with a data type of 64-bit floating point.
 *
 *	Arguments:
 *	        RES    - Dope vector for temporary result array
 *		SRC    - Dope vector for user source array
 *		STOP   - Dope vector for stop array
 *              DIM    - Dimension to operate along
 *	        MASK   - Dope vector for logical mask array
 *
 *	Description:
 *		This is the MPP single PE version of SCANPROD. This
 *	        routine checks the scope of the source and mask
 *	        arrays and if they are private, calls the current
 *	        Y-MP single processor version of the routine. If they
 *	        are shared, then it allocates a result array before
 *	        calling a Fortran routine which declares the source
 *	        and mask arguments to be UNKNOWN SHARED.
 *
 *		Include file segmented_scan_s.h contains the rank independent
 *		source code for this routine.
 */

_SCANPROD_SS3 (	DopeVectorType	*result,
		DopeVectorType	*source,  
		DopeVectorType	*stop,
		long		*dim,
		DopeVectorType	*mask)
{

#include "segmented_scan_s.h"

	if (stop_flag == 1) {
	    if (mask_flag == 1) {
		SCANPROD_MASK_SS3@ (result_base_ptr, source_sdd_ptr,
			stop_sdd_ptr, &dim_val, mask_sdd_ptr, src_extents);
	    } else {
		SCANPROD_NOMASK_SS3@ (result_base_ptr, source_sdd_ptr,
			stop_sdd_ptr, &dim_val, src_extents);
	    }
	}
}
