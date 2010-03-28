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


#pragma ident "@(#) libfi/mathlb/chtoch.c	92.1	07/09/99 11:00:36"


#include <fortran.h>
#include <fp.h>
#include <math.h>
#include "inline.h"
#include "mathdefs.h"

extern void _CHTOCH(h_complex_t *ret_val, h_complex_t x, h_complex_t y);

/*
 * CHTOCH: complex(kind=4) raised to a complex(kind=4): pass by value
 */
void
_CHTOCH(h_complex_t *ret_val,
	h_complex_t x,
	h_complex_t y)
{
	_f_real8 __atan2(_f_real8 x, _f_real8 y);
	_f_real8 __cos(_f_real8 x);
	_f_real8 __exp(_f_real8 x);
	_f_real8 __log(_f_real8 x);
	_f_real8 __sin(_f_real8 x);
	_f_real8 __sqrt(_f_real8 x);
	_f_real8 a;
	_f_real8 b;
	_f_real8 c;
	_f_real8 d;
	_f_real8 one;
	_f_real8 two;
	REGISTER_4 realx;
	REGISTER_4 imagx;
	_f_real8 logabsx, xreal, ximag, atn2, exptwo;
	if (x.real == 0.0 && x.imag == 0.0) {
		if (y.real == 0.0 && y.imag == 0.0) {
			ret_val->real = _HALF_NaN;
			ret_val->imag = _HALF_NaN;
		}
		else {
			ret_val->real = (_f_real4) 0.0;
			ret_val->imag = (_f_real4) 0.0;
		}
		return;
	}
	realx.f = x.real;
	imagx.f = x.imag;
	a = (_f_real8) x.real;
	b = (_f_real8) x.imag;
	c = (_f_real8) y.real;
	d = (_f_real8) y.imag;
	realx.ui &= ~IEEE_32_SIGN_BIT;
	imagx.ui &= ~IEEE_32_SIGN_BIT;
	xreal = (_f_real8) realx.f;
	ximag = (_f_real8) imagx.f;

	atn2 = __atan2(b,a);

	if (realx.f > imagx.f)
		logabsx = __log(xreal * 
			__sqrt(1.0 + (ximag/xreal) * (ximag/xreal)));
	else
		logabsx = __log(ximag *
			__sqrt(1.0 + (xreal/ximag) * (xreal/ximag)));

	one = d * logabsx + c * atn2;
	two = c * logabsx - d * atn2;

	exptwo = __exp(two);

	ret_val->real = (_f_real4) (exptwo * __cos(one));
	ret_val->imag = (_f_real4) (exptwo * __sin(one));
}
