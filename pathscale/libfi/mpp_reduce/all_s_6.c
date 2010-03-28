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


#pragma ident "@(#) libfi/mpp_reduce/all_s_6.c	92.1	07/09/99 17:05:42"


#include <stdlib.h>
#include <liberrno.h>
#include <fmath.h>
#include <cray/dopevec.h>
#include "f90_macros.h"

#define RANK	6

/*
 *	Compiler generated call: CALL _ALL_LS6(RESULT, SOURCE, DIM)
 *
 *	Purpose: Determine whether all values are true in SOURCE along
 *	         dimension DIM.  This particular routine handles
 *		 source arrays of rank 6.
 *
 *	Arguments:
 *	        SOURCE - Dope vector for logical source array
 *	        DIM    - Dimension to operate along
 *
 *      Description:
 *		This is the MPP version of ALL. This particular
 *		file contains the the intermediate type-specific
 *		routines. These routines parse and update the dope
 *		vectors, allocate either shared or private space for
 *		the result temporary, and possibly update the shared
 *		data desriptor (sdd) for the result temporary. Once
 *		this set-up work is complete, a Fortran subroutine
 *		is called which uses features from the Fortran
 *		Programming Model to distribute the work across all
 *		processors.
 *
 *		Include file all_s.h contains the rank independent
 *		source code for this routine.
 */

#include "all_s.h"
