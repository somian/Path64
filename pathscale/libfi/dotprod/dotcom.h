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


* USMID @(#) libfi/dotprod/dotcom.h	92.0	10/08/98 14:37:14 
*
* Definition of variables for SUM intrinsic functions
*
* Author:
*	Math Software Group
*	Cray Research, Inc.
*	Eagan, Minnesota
*
* The following common block contains the number of elements,
* the base addresses for each vector, and the increment.
*
* NOTE:  If these variables change, you must also change predot.c
*
* Definitions:
*	n 	number of elements
*	locx	address of x array
*	incx	increment for x array
*	locy	address of y array
*	incy	increment for y array
*
	integer	         n, locx, incx, locy, incy
