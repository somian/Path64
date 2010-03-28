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


CDIR$ ID "@(#) libfi/dotprod/dotl90.h	92.0	10/08/98 14:37:14"
*********************************************************************
*  Special version for logical                                      *
*                                                                   *
*  Header file for all dot product routines.  This file is included *
*  by each of the specific dot product routines.  The file that     *
*  includes this one just declares the name of the routine, and the *
*  types of the variables x, y, and s.  This routine does the       *
*  actual computation.                                              *
*                                                                   *
*  Author:                                                          *
*  Math Software Group                                              *
*  Cray Research, Inc.                                              *
*  Eagan, Minnesota                                                 *
*                                                                   *
*********************************************************************

* p_x is a pointer to array x (i.e, contains base address of x)
* p_y is a pointer to array y (i.e, contains base address of y)
*
	pointer (p_x, x)
	pointer (p_y, y)
*
* Both arguments are dope vectors.
*
	integer dopevec1(*), dopevec2(*)
*
	include "dotcom.h"	! common block definition
*
* Predot is the subroutine used to parse the dope vectors,
* storing the required values in the common block.
*
	external _PREDOT
	integer i
*
* Parse the dope vectors, and store the vector addresses
* and increments in the common block.
*
	call _PREDOT(dopevec1, dopevec2, n, locx, incx, locy, incy)
*
*	Store base address of vectors in pointer variables.
*
	p_x = locx
	p_y = locy
*
* Compute the dot product
*
	s = .FALSE.

	do, i = 1, n
	    s = s .OR. ( x(1 + (i-1)*incx) .AND. y(1 + (i-1)*incy) )
	end do
