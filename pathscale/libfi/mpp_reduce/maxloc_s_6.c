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


#pragma ident "@(#) libfi/mpp_reduce/maxloc_s_6.c	92.1	07/09/99 17:05:42"


#include <stdlib.h>
#include <liberrno.h>
#include <fmath.h>
#include <cray/dopevec.h>
#include "f90_macros.h"

#define RANK	6

/*
 *	Compiler generated call: CALL _MAXLOC_JS6(RESULT, SOURCE, MASK)
 *
 *	Purpose: Determine the location of the first element of SOURCE
 *	         having the maximum value of the elements identified
 *	         by MASK. This particular routine handles source arrays
 *	         of rank 6 with a data type of 64-bit integer or
 *	         64-bit floating point.
 *
 *	Arguments:
 *	        RESULT - Dope vector for result array
 *	        SOURCE - Dope vector for user source array
 *	        MASK   - Dope vector for logical mask array
 *
 *	Description:
 *		This is the MPP single PE version of MAXLOC. This
 *	        routine checks the scope of the source and mask
 *	        arrays and if they are private, calls the current
 *	        Y-MP single processor version of the routine. If they
 *	        are shared, then it allocates a result array before
 *	        calling a Fortran routine which declares the source
 *	        and mask arguments to be UNKNOWN SHARED.
 *
 *		Include file maxloc_s.h contains the rank independent
 *		source code for this routine.
 */

#include "maxloc_s.h"
