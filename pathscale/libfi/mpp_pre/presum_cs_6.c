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


#pragma ident "@(#) libfi/mpp_pre/presum_cs_6.c	92.1	07/09/99 15:38:17"

#include <stdlib.h>
#include <liberrno.h>
#include <fmath.h>
#include <cray/dopevec.h>
#include "f90_macros.h"

#define RANK		6

/*
 *	Compiler generated call: CALL _PRESUM_CS6(RESULT, SOURCE, DIM, MASK)
 *
 *	Purpose: Determine the maximum value of the elements of SOURCE
 *	         along dimension DIM corresponding to the true elements
 *               of MASK. This particular routine handles source arrays
 *	         of rank 6 with a data type of 128-bit complex.
 *
 *	Arguments:
 *	        RESULT - Dope vector for temporary result array
 *	        SOURCE - Dope vector for user source array
 *              DIM    - Dimension to operate along
 *	        MASK   - Dope vector for logical mask array
 *
 *	Description:
 *		This is the MPP single PE version of PRESUM. This
 *	        routine checks the scope of the source and mask
 *	        arrays and if they are private, calls the current
 *	        Y-MP single processor version of the routine. If they
 *	        are shared, then it allocates a result array before
 *	        calling a Fortran routine which declares the source
 *	        and mask arguments to be UNKNOWN SHARED.
 *
 *		Include file parallel_prefix_s.h contains the rank independent
 *		source code for this routine.
 */

_PRESUM_CS6 (	DopeVectorType	*result,
		DopeVectorType	*source,
		long		*dim,
		DopeVectorType	*mask)
{

#include "parallel_prefix_s.h"

	if (mask_flag == 1)
	    PRESUM_MASK_CS6@ (result_base_ptr, source_sdd_ptr, &dim_val,
		mask_sdd_ptr, src_extents);
	else
	    PRESUM_NOMASK_CS6@ (result_base_ptr, source_sdd_ptr, &dim_val,
		src_extents);
}
