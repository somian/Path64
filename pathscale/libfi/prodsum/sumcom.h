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


* USMID @(#) libfi/prodsum/sumcom.h	92.0	10/08/98 14:37:14
*	Declaration of variables used by the SUM and PRODUCT array
*	intrinsic routines.
*
*	If these variables change, you must also change presum.c
*
*	Author:
*	Math Software Group
*
*       These are F77-style declarations
*	.. variables ..
	integer maxdim
	parameter (maxdim = 7)
	integer ndim		! number of dimensions
	logical	dimarg		! true iff DIM arg was given
	integer dimx		! dimension to sum (if DIM arg given) 
	logical maskarg		! true iff MASK arg was given
	logical scalar		! true iff sum is a scalar
	integer loca		! address of array A
	integer lima(maxdim)	! extent(i) of array A
	integer inca(maxdim)	! increment(i) of array A
	integer locb		! address of array B
	integer limb(maxdim)	! extent(i) of array B
	integer incb(maxdim)	! increment(i) of array B
	integer locm		! address of array MASK (if present)
	integer limm(maxdim)	! extent(i) of array MASK (if present)
	integer incm(maxdim)	! increment(i) of array MASK (if present)
	integer sizem		! size of mask in bytes
	integer loc4m		! address of array MASK (4-byte)
