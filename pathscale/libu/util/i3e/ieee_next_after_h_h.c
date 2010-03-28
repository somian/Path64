/*
 * Copyright (C) 2007. PathScale, LLC. All Rights Reserved.
 */
/*
 * Copyright 2004, 2005, 2006 PathScale, Inc.  All Rights Reserved.
 */

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


static const char USMID[] = "@(#) libu/util/i3e/ieee_next_after_h_h.c	92.0	10/08/98 14:57:41";

#include <fenv.h>
#include <float.h>
#include <fp.h>
#include "i3eintrin.h"

extern float _IEEE_NEXT_AFTER_H_H(float x, float y);

/* NEXT_AFTER - return the nearest different machine representable number
 * 	        in a given direction y for 32-bit values.
 *	INPUT:  32-bit float x
 *	        32-bit float y
 * 	RETURN:
 * 	        the argument x if x = y.
 * 	        the argument x if X = NaN.
 * 	        the argument y if y = NaN.
 * 	        Raise inexact for overflow or underflow and return
 *
 */
#pragma _CRI duplicate _IEEE_NEXT_AFTER_H_H as _IEEE_NEXT_AFTER_4_4
#pragma _CRI duplicate _IEEE_NEXT_AFTER_H_H as _IEEE_NEXT_AFTER_4
#pragma _CRI duplicate _IEEE_NEXT_AFTER_H_H as _IEEE_NEXT_AFTER_H
float
_IEEE_NEXT_AFTER_H_H(float x, float y)
{
	/* Union defined to work with IEEE 32-bit floating point. */
	union _ieee_float {
		float dword;
		short	lword;
		struct {
#if defined(_LITTLE_ENDIAN)
			unsigned int mantissa : IEEE_32_MANT_BITS;
			unsigned int exponent : IEEE_32_EXPO_BITS;
			unsigned int sign     : 1;
#else
			unsigned int sign     : 1;
			unsigned int exponent : IEEE_32_EXPO_BITS;
			unsigned int mantissa : IEEE_32_MANT_BITS;
#endif
		} parts;
	};

	int xfpclas	= _fpclassifyf(x);
	int yfpclas	= _fpclassifyf(y);
	if (xfpclas == FP_NAN) {
		return x;
	} else if (yfpclas == FP_NAN) {
		return y;
	} else if (xfpclas == FP_ZERO && yfpclas == FP_ZERO) {
		return x;
	} else if (x==y || xfpclas == FP_INFINITE) {
		return x;
	} else if (xfpclas == FP_ZERO) {
		union _ieee_float x_val;
		x_val.dword = FLT_MIN;
		x_val.parts.sign	 = x > y;

		/* return smallest normal number */
		return(x_val.dword);
	} else {  /* first argument is normal or denormal */
		union _ieee_float x_val;
		int j;
		int resfpclas;

		x_val.dword	 = x;

		/* move one bit in the correct direction.
		 ** Because of the way the implicit bit works,
		 ** the exponent field is handled correctly.
		 */
		if (x > 0) {
			x_val.lword += (x>y) ? -1 : 1;
		} else {
			x_val.lword += (x>y) ? 1 : -1;
		}

		/* test for underflow or overflow */
		if (_isnormalf(x_val.dword))
			return(x_val.dword);

		/*
		 * Raise overflow exception for infinite result and
		 * underflow exception for too small result.  Raise
		 * inexact exception for both cases. Allow subnormal
		 * values to return without exception.
		 */
		resfpclas	= _fpclassifyf(x_val.dword);
		if (resfpclas == FP_INFINITE) {
			j	= FE_OVERFLOW;
			feraiseexcept(j);
		} else if (resfpclas == FP_ZERO) {
			j	= FE_UNDERFLOW;
			feraiseexcept(j);
		} else {
			return(x_val.dword);
		}
		j	= FE_INEXACT;
		feraiseexcept(j);
		return(x_val.dword);
	}
}
