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


#pragma ident "@(#) libu/util/i3e/ieee_remainder_d_d.c	92.1	07/07/99 14:37:09"

#ifndef _LD64
#include <fenv.h>
#include <fp.h>
#include <stdio.h>

extern long double _IEEE_REMAINDER_D_D(long double x, long double y);

/*
 * _IEEE_REMAINDER_D_D(X,Y) - calculate remainder of two real(16)
 *                            arguments.
 *
 *              The standard definition for real is:
 *                 If y = 0, remainder(x,y) = NaN and raise the INVALID
 *                           exception.
 *                 else if x = INFINITY, remainder(x,y) = NaN and raise
 *                           the INVALID exception.
 *                 else remainder(x,y) = x - (REAL(NINT(x/y))) * y
 *
 *              The algorithm for real is:
 *                x - (REAL(NINT(x/y))) * y.
 */

#pragma _CRI duplicate _IEEE_REMAINDER_D_D as _IEEE_REMAINDER_16_16
#pragma _CRI duplicate _IEEE_REMAINDER_D_D as _IEEE_REMAINDER_16
#pragma _CRI duplicate _IEEE_REMAINDER_D_D as _IEEE_REMAINDER_D
#pragma _CRI duplicate _IEEE_REMAINDER_D_D as _REMAINDER_D
#pragma _CRI duplicate _IEEE_REMAINDER_D_D as _REMAINDER_16
long double
_IEEE_REMAINDER_D_D(long double argx, long double argy)
{
	union _ieee_ldouble {
		long double	ldword;
		long		lword[2];
	};
	long double	__remainder_d(long double x, long double y);
	int xfpclas	= _fpclassifyl(argx);
	int yfpclas	= _fpclassifyl(argy);

	if ((xfpclas == FP_INFINITE) || yfpclas == FP_ZERO) {
		union _ieee_ldouble xval;
		int	j;

		xval.ldword	= _DBL_NaN;

		/* need to emit invalid exception */
		j	= FE_INVALID;
		feraiseexcept(j);
		return(xval.ldword);
	}
	return(__remainder_d(argx, argy));
}
#endif          /* end NOT _LD64 */
