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


* USMID @(#) libfi/prodsum/prod0_90.h	92.0	10/08/98 14:37:14
*********************************************************************
*                                                                   *
*  Name:   PROD0_90.h                                               *
*                                                                   *
*  Description:                                                     *
*          Header file for the Fortran 90 array intrinsic function  *
*          PRODUCT.  This file is included in all of the specific   *
*          PRODUCT intrinsics in which the result is a scalar.      *
*                                                                   *
*  Arguments:                                                       *
*     dv_array     Dope Vector describing the array to prod         *
*                                                                   *
*     dim          integer (optional),                              *
*                  Dimension over which to prod                     *
*                                                                   *
*     dv_mask      Dope vector (optional) describing MASK array.    *
*                                                                   *
*          Author: Math Software Group, Cray Research, Inc.         *
*                                                                   *
*********************************************************************
*	.. arguments ..
	integer dv_array(*)	! dope vector for array
	integer dim		! dimension on which to prod
	logical dv_mask(*)	! dope vector for mask

*	.. subscripts ..	
	integer i, j, k, ia, jb, km
	integer i1, i2, i3, i4, i5, i6, i7

*	.. dummy argument ..
	integer dummy

*	.. functions ..
	external _PRESUM
	include "sumcom.h"	! include variables declaration
	
*	.. begin execution ..
*
*       Call routine PRESUM@ to parse the dope vectors and do preliminary
*       processing.  Always pass the same number of arguments to presum,
*       ie., dim and mask are present or are zero.  Pass 15 local
*       arguments rather than use task common. The first argument is zero
*       to indicate result is a scalar, not an array.
*
        call _PRESUM(0, dummy, dv_array, dim, dv_mask,
     +        ndim, dimarg, dimx, maskarg, scalar, loca, lima, inca,
     +        locb, limb, incb, locm, limm, incm, sizem, loc4m)
*
*	Assign addresses to pointers for arrays a, b, mask.
*
	p_a = loca
	if (maskarg) then
	   if (sizem .EQ. 4) then
	      p_mask4= loc4m
           else
	      p_mask = locm
           endif
        endif
*
*	Initialize result
*
	result = 1

	if (.not. maskarg) then
*
*	    Case: mask argument IS NOT given
*
	    if (ndim .eq. 1) then
*
*		Array is a vector (1 dimension)
*
		do, i1 = 0, lima(1) - 1
		    result = result * a(i1*inca(1))
		end do

	    else if (ndim .eq. 2) then
*
*		Array is a matrix (2 dimensions).
*
		do, i2 = 0, lima(2) - 1
*-----------------------------------------------------------------------
		    ia = i2*inca(2)
		    do, i1 = 0, lima(1) - 1
			result = 
     &			result * a(i1*inca(1) + ia)
		    end do
*-----------------------------------------------------------------------
		end do

	    else 
*
*		Array has more than 2 dimensions --
*	        treat it as though all 7 dimensions were
*		given.  Sum over first dimension.
*
*		result = Sum a(i1,i2,i3,i4,i5,i6,i7)
*                         i1
*		Vectorize on i1 (inner loop)
*
*		Fill in default values for limit and increment
*
		do, i = ndim + 1, maxdim
		    lima(i) = 1
		    inca(i) = 0
		    limm(i) = 1
		    incm(i) = 0
		end do
*
		do, i7 = 0, lima(7) - 1
		do, i6 = 0, lima(6) - 1
		do, i5 = 0, lima(5) - 1
		do, i4 = 0, lima(4) - 1
		do, i3 = 0, lima(3) - 1
		do, i2 = 0, lima(2) - 1
*-----------------------------------------------------------------------
		    ia = i2*inca(2) + i3*inca(3) + i4*inca(4) + 
     &			 i5*inca(5) + i6*inca(6) + i7*inca(7)
		    do, i1 = 0, lima(1) - 1
			result = 
     &			result * a(i1*inca(1) + ia)
		    end do
*-----------------------------------------------------------------------
		end do
		end do
		end do
		end do
		end do
		end do

	    end if

	else 
*
*	    Case: mask argument IS given
*
	    if (ndim .eq. 1) then
*
*		Array is a vector (1 dimension)
*
                if (sizem .eq. 4) then

*               Size of mask is 4 bytes, use 4-byte pointer
		    do, i1 = 0, lima(1) - 1
		        if (mask4(i1*incm(1))) then
			    result = result * a(i1*inca(1))
		        end if
		    end do

                else

*               Size of mask is not 4 bytes
		    do, i1 = 0, lima(1) - 1
		        if (mask(i1*incm(1))) then
		   	    result = result * a(i1*inca(1))
		        end if
		    end do
		end if

	    else if (ndim .eq. 2) then
*
*		Array is a matrix (2 dimensions).
*		Sum over first dimension.
*
                if (sizem .eq. 4) then

*               Size of mask is 4 bytes, use 4-byte pointer
		    do, i1 = 0, lima(1) - 1
		        ia = i1*inca(1)
		        km = i1*incm(1)
		        do, i2 = 0, lima(2) - 1
			    if ( mask4(i2*incm(2) + km) ) then
			        result = 
     &			        result * a(i2*inca(2) + ia)
			    end if
		        end do
		    end do

                else

*               Size of mask is not 4 bytes
		    do, i1 = 0, lima(1) - 1
*-----------------------------------------------------------------------
		        ia = i1*inca(1)
		        km = i1*incm(1)
		        do, i2 = 0, lima(2) - 1
			    if ( mask(i2*incm(2) + km) ) then
			        result = 
     &			        result * a(i2*inca(2) + ia)
			    end if
		        end do
*-----------------------------------------------------------------------
		    end do
		end if

	    else 
*
*		Array has more than 2 dimensions --
*	        treat it as though all 7 dimensions were
*		given.  Sum over first dimension.
*
*		result = Sum a(i1,i2,i3,i4,i5,i6,i7)
*                                       i1
*		Vectorize on i2 (inner loop)
*
*		Fill in default values for limit and increment
*
		do, i = ndim + 1, maxdim
		   lima(i) = 1
		   inca(i) = 0
		   limm(i) = 1
		   incm(i) = 0
		end do

                if (sizem .eq. 4) then

*               Size of mask is 4 bytes, use 4-byte pointer
*
		    do, i7 = 0, lima(7) - 1
		    do, i6 = 0, lima(6) - 1
		    do, i5 = 0, lima(5) - 1
		    do, i4 = 0, lima(4) - 1
		    do, i3 = 0, lima(3) - 1
		    do, i2 = 0, lima(2) - 1
*-----------------------------------------------------------------------
		        ia = i2*inca(2) + i3*inca(3) + i4*inca(4) + 
     &			     i5*inca(5) + i6*inca(6) + i7*inca(7)
		        km = i2*incm(2) + i3*incm(3) + i4*incm(4) + 
     &			     i5*incm(5) + i6*incm(6) + i7*incm(7)

		    do, i1 = 0, lima(1) - 1
			if ( mask4(i1*incm(1) + km) ) then
			    result = 
     &			    result * a(i1*inca(1) + ia)
			end if
		    end do
*-----------------------------------------------------------------------
		    end do
		    end do
		    end do
		    end do
		    end do
		    end do

                else

*               Size of mask is not 4 bytes
*
		    do, i7 = 0, lima(7) - 1
		    do, i6 = 0, lima(6) - 1
		    do, i5 = 0, lima(5) - 1
		    do, i4 = 0, lima(4) - 1
		    do, i3 = 0, lima(3) - 1
		    do, i2 = 0, lima(2) - 1
*-----------------------------------------------------------------------
		        ia = i2*inca(2) + i3*inca(3) + i4*inca(4) + 
     &			     i5*inca(5) + i6*inca(6) + i7*inca(7)
		        km = i2*incm(2) + i3*incm(3) + i4*incm(4) + 
     &			     i5*incm(5) + i6*incm(6) + i7*incm(7)

		    do, i1 = 0, lima(1) - 1
			if ( mask(i1*incm(1) + km) ) then
			    result = 
     &			    result * a(i1*inca(1) + ia)
			end if
		    end do
*-----------------------------------------------------------------------
		    end do
		    end do
		    end do
		    end do
		    end do
		    end do
	        end if

	    end if
	end if
*	.. end of prod0_x@ ..
