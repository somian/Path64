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


#pragma ident "@(#) libfi/mathlb/rrspacing.c	92.1	07/09/99 11:00:36"


#include <fortran.h>
#ifdef  __mips
#include <math.h>
#endif
#include "inline.h"


/* RRSPACING - return reciprocal of relative spacing of model numbers near
 * the argument value.
 */
_f_real4
_RRSPACING_4(_f_real4 x)
{
#if defined (_CRAY1) && defined(_CRAYIEEE)
	_f_real8 _SCALE(_f_real8 zr, _f_int zi);
	_f_real8 _FRACTION(_f_real8 x);
	REGISTER_8      y;
	y.f = _FRACTION(x);
	y.ui &= ~(IEEE_64_SIGN_BIT);	/* clear sign bit to take absolute
					 * value. */
	/* use mantissa bits +1 since the implicit bit is not set after
	 * fraction().
	 */
	return(_SCALE(y.f, (IEEE_32_MANT_BITS+1)));
#else
	_f_real4 _SCALE_4(_f_real4 zr, _f_int zi);
	_f_real4 _FRACTION_4(_f_real4 x);
	REGISTER_4      y;
	y.f = _FRACTION_4(x);
	y.ui &= ~(IEEE_32_SIGN_BIT);	/* clear sign bit to take absolute
					 * value. */
	/* use mantissa bits +1 since the implicit bit is not set after
	 * fraction().
	 */
	return(_SCALE_4(y.f, (IEEE_32_MANT_BITS+1)));
#endif
}

_f_real8
_RRSPACING_8(_f_real8 x)
{
	_f_real8 _RRSPACING(_f_real8 x);
	return (_RRSPACING(x));
}

_f_real8
_RRSPACING(_f_real8 x)
{
	_f_real8 _SCALE(_f_real8 zr, _f_int zi);
	_f_real8 _FRACTION(_f_real8 x);
	REGISTER_8      y;
	y.f = _FRACTION(x);
	y.ui &= ~(IEEE_64_SIGN_BIT);	/* clear sign bit to take absolute
					 * value. */
	/* use mantissa bits +1 since the implicit bit is not preset after
	 * fraction().  Fraction uses 3FE, not 3FF.
	 */
	return(_SCALE(y.f, IEEE_64_MANT_BITS+1));
}

#ifndef	__mips
#if _F_REAL16 == 1
/* RRSPACING - return reciprocal of relative spacing of model numbers
 * near the argument value for 128-bit float.
 */
_f_real16
_RRSPACING_16(_f_real16 x)
{
	_f_real16 _SCALE_16(_f_real16 zr, _f_int zi);
	_f_real16 _FRACTION_16(_f_real16 x);
#if defined(_WORD32)
	union ldble_float {
		_f_real16		whole;
		unsigned long long	ui[1];
	} f;
#else
	union ldble_float {
		_f_real16		whole;
		unsigned long		ui[1];
	} f;
#endif
	_f_real16	y;

	f.whole =	_FRACTION_16(x);

	/* clear sign bit */
	f.ui[0] &= ~IEEE_128_64_SIGN_BIT;

	/* use mantissa bits +1 since the implicit bit is not preset
	 * after fraction().  Fraction uses 3FFE, not 3FFF.
	 */
	return(_SCALE_16(f.whole, IEEE_128_MANT_BITS+1));
}

#endif	/* _F_REAL16 */
#else	/* NOT mips */
/* use ldexpl routine in libc prototyped in math.h */
_f_real16
_RRSPACING_16(_f_real16 aa)
{
	_f_real16	result;
	_f_int4		i;
	if (aa < 0)
		aa	= -aa;
	if (aa == 0) {
		result	= 0.0L;
	} else {
		result	= ldexpl(_get_frac_and_exp(aa, &i),DBL_DBL_MANT_BITS);
	}
	return result;
}
#endif	/* NOT mips */
