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


#pragma ident "@(#) libf/pxf/pxfucompare.c	92.1	06/29/99 11:36:06"


#include <errno.h>
#include <liberrno.h>
#include <fortran.h>

/*
 *  PXFUCOMPARE  -- unsigned comparison
 *  (section 8.11.1 of Posix 1003.9-1992)
 *
 *  Synopsis:
 *
 *     SUBROUTINE PXFUCOMPARE(I1, I2, ICMPR, IDIFF)
 *     INTEGER I1, I2, ICMPR, IDIFF
 *
 *  Variable arguements:
 *
 *  I1     is an input integer variable for a C unsigned integer.
 *
 *  I2     is an input integer variable for a C unsigned integer.
 *
 *  ICMPR  is an output integer variable that on return from the
 *         routine contains one of these values:
 *
 *           -1    if I1 < I2.
 *            0    if I1 equals I2.
 *            1    if I1 > I2.
 *
 *         All of the comparisons are made using C unsigned integer
 *         comparisons.
 *
 *  IDIFF  is an output integer variable that on return from the
 *         routine contains the absolute value of the difference
 *         of I1 and I2. Since the values are C unsigned integers
 *         and Fortran 77 does not directly support unsigned integers
 *         the value may be negative, which indicates the value
 *         is beyond the maximum positive value of a Fortran 77
 *         integer.
 *
 */


#ifdef _UNICOS
void
PXFUCOMPARE(
#else
void
_PXFUCOMPARE(
#endif
	    _f_int *I1,
	    _f_int *I2,
	    _f_int *ICMPR,
	    _f_int *IDIFF
)
{
  if (((unsigned)*I1) < ((unsigned)*I2)) {
    *ICMPR = -1;
    *IDIFF = ((unsigned)*I2) - ((unsigned)*I1);
  } else if (((unsigned)*I1) == ((unsigned)*I2)) {
    *ICMPR = 0;
    *IDIFF = 0;
  } else if (((unsigned)*I1) > ((unsigned)*I2)) {
    *ICMPR = 1;
    *IDIFF = ((unsigned)*I1) - ((unsigned)*I2);
  }

}

#ifndef _UNICOS
void
pxfucompare_(
	     _f_int *I1,
	    _f_int *I2,
	    _f_int *ICMPR,
	    _f_int *IDIFF
)
{
  _PXFUCOMPARE(I1, I2, ICMPR, IDIFF);
}
#endif
