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


#pragma ident "@(#) libfi/mathlb/spacing.c	92.1	07/09/99 11:00:36"

#include <fortran.h>
#include <fmath.h>
#ifdef  __mips
#include <math.h>
#endif
#include "inline.h"

/*** SPACING - return the absolute spacing for 32-bit and 64-bit model
 *		numbers near the argument value.
 ***/
#define IEEE_FLT2_23 0x34000000
#define IEEE_DBL2_52 (LL_CONST(0x3cb0000000000000))
#define IEEE_64FLT2_23 (LL_CONST(0x3E80000000000000))

_f_real4
_SPACING_4(_f_real4 x)
{
	REGISTER_4      s1, s2, s3;
	s1.f = x;
#if defined (_CRAY1) && defined(_CRAYIEEE)
	s2.ui = IEEE_64FLT2_23;	/* 2**(-23) */
#else
	s2.ui = IEEE_FLT2_23;	/* 2**(-23) */
#endif
	if (x == 0.0)
		return (_f_real4) TINY_REAL4_F90;
#if defined (_CRAY1) && defined(_CRAYIEEE)
	s3.ui = IEEE_64_EXPONENT;	/* mask for exponent. */
#else
	s3.ui = IEEE_32_EXPONENT;	/* mask for exponent. */
#endif
	s1.f = s1.f * s2.f;
	s1.ui &= s3.ui;
#if defined (_CRAY1) && defined(_CRAYIEEE)
	if ((_f_real4) TINY_REAL4_F90 > s1.f)
		return ((_f_real4) TINY_REAL4_F90);
#endif
	return s1.f == 0.0 ? (_f_real4) TINY_REAL4_F90 : s1.f;
}

_f_real8
_SPACING_8(_f_real8 x)
{
	_f_real8 _SPACING(_f_real8 x);
	return(_SPACING(x));
}

_f_real8
_SPACING(_f_real8 x)
{
	REGISTER_8      s1, s2, s3;
	s1.f = x;
	s2.ui = IEEE_DBL2_52;     /* 2**(-52) */
	if (x == 0.0)
		return TINY_REAL8_F90;
	s3.ui = IEEE_64_EXPONENT;         /* mask for exponent. */
	s1.f = s1.f * s2.f;
	s1.ui &= s3.ui;
	return s1.f == 0.0 ? TINY_REAL8_F90 : s1.f;
}

#ifndef	__mips
#if _F_REAL16 == 1
/*** SPACING - return the absolute spacing for 128-bit model
 *		numbers near the argument value.
 ***/
_f_real16
_SPACING_16(_f_real16 x)
{
#if defined(_WORD32)
	union ldble_float {
		struct {
			unsigned long long	upper;
			unsigned long long	lower;
		} parts;
		_f_real16		whole;
	} f, result;
	unsigned long long	exp_mask;
#else
	union ldble_float {
		struct {
			unsigned long		upper;
			unsigned long		lower;
		} parts;
		_f_real16		whole;
	} f, result;
	unsigned long		exp_mask;
#endif

	static union ldble_float two_112 = {(LL_CONST(0x3F8F000000000000)),
			(LL_CONST(0x0000000000000000))};
	f.whole =	x;
	if (x == 0.0)
		return TINY_REAL16_F90;
	exp_mask = IEEE_128_64_EXPO;         /* mask for exponent. */

	/* multiply by 2**-112) */
	result.whole =	f.whole * two_112.whole;
	result.parts.upper &= exp_mask;
	result.parts.lower = (LL_CONST(0x0000000000000000)); /* zero. */
	return result.whole == 0.0 ? TINY_REAL16_F90 : result.whole;
}

#endif	/* _F_REAL16 */
#else	/* NOT mips */
/*
 *  Return a real with the same type parameter as X whose value is the
 *  absolute spacing of the model number near X, that is, b**(e-p).
 */
_f_real16
_SPACING_16(_f_real16 a)
{
	_f_int4		e;
	_f_real16	f;
	if (a == 0) {
		e = -916;
	} else {
		(void) _get_frac_and_exp(a,&e);
	}
	e	= e - DBL_DBL_MANT_BITS;

	/* Unfortunately, (in Fortran) -1021 is too small.  The
	 * constant -916 is hard-wired to maintain full precision.
	 */
	if (e < -916)
		e	= -916;
	/* use ldexpl routine in libc prototyped in math.h */
	f	= ldexpl(1.0L,e);
	return(f);
}
#endif	/* NOT mips */
