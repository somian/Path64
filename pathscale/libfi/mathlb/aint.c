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


#pragma ident "@(#) libfi/mathlb/aint.c	92.1	07/09/99 11:00:36"


#include <fortran.h>

extern _f_real8 _AINT_(_f_real8 *x);
extern _f_real8 _AINT(_f_real8 x);

/*
 * AINT:  Truncate real(kind=8) to a whole number
 *        - pass by value
 */
_f_real8
_AINT(_f_real8 x)
{
	union dble_float {
		struct {
			unsigned	upper;
			unsigned	lower;
		} parts;
		_f_real8	whole;
	} f, result;

	static union dble_float two_52 = {0x43300000, 0x00000000};
	unsigned	abs_x;
	unsigned	sign_x = 0x80000000;
	_f_real8	tmp;

	f.whole =	x;

	/* Get the absolute value of x by ANDing the upper half
	 * with the NOT of 0x80000000 (the sign bit mask).
	 */
	abs_x =	f.parts.upper & (~sign_x);

	/* See what the fraction entails.  If greater than 2**52,
	 * return x since value .GE. 2**(52) is an integer.  The
	 * size of the mantissa is 52 and the value of x must be
	 * (f_real8) x >= 4503599627370496 to be an integer.
	 */
	if (abs_x >= two_52.parts.upper)
		return (x);

	/* Otherwise, clear the fraction by adding 2**52, then
	 * subtracting 2**52.  The hardware will round the result,
	 * thus clearing it.
	 */
	result.parts.upper = abs_x;
	result.parts.lower = f.parts.lower;
	tmp = result.whole + two_52.whole;
	tmp -= two_52.whole;

	/* If this is greater than the absolute value of x, subtract 1.0;
	 * otherwise it's already truncated.
	 */
	if (tmp > result.whole)
		tmp -= (_f_real8) 1.0;
	result.whole = tmp;
		
	/* Now reset the sign bit and we're done. */
	result.parts.upper = (f.parts.upper & sign_x) |
		result.parts.upper;
	return (result.whole);
}

/*
 * AINT:  Truncate real(kind=8) to a whole number
 *        - pass by address
 */

_f_real8
_AINT_(_f_real8 *x)
{
	return (_AINT(*x));
}
