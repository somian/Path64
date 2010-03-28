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


#pragma ident "@(#) libfi/mathlb/ctor.c	92.1	07/09/99 11:00:36"


#include <fortran.h>
#include <fp.h>
#include <math.h>
#include "mathdefs.h"

extern void _CTOR(c_complex_t *ret_val, c_complex_t x, _f_real8 *r);

/*
 *      complex(kind=8) raised to a real(kind=8) = _CTOR
 *
 *	x = a+b*i
 *
 *	if ((x == 0+0*i) && (y == 0)) then return(NAN)
 *	if (x == 0+0*i) then return(0+0*i)
 *	if (y == 0) then return(1+0*i)
 */
void
_CTOR(c_complex_t *ret_val,
	c_complex_t x,
	_f_real8 *r)
{
	_f_real8 __atan2(_f_real8 x, _f_real8 y);
	_f_real8 __cos(_f_real8 x);
	_f_real8 __exp(_f_real8 x);
	_f_real8 __log(_f_real8 x);
	_f_real8 __sin(_f_real8 x);
	_f_real8 _CABS(c_complex_t z);
	_f_real8 y = *r;
	_f_real8 one;
	_f_real8 two;
	if (x.real == (_f_real8) 0.0 && x.imag == (_f_real8) 0.0) {
		if (y == (_f_real8) 0.0) {
			ret_val->real = _SGL_NaN;
			ret_val->imag = _SGL_NaN;
		}
		else {
			ret_val->real = (_f_real8) 0.0;
			ret_val->imag = (_f_real8) 0.0;
		}
		return;
	}
	one = y * __atan2(x.imag, x.real);
	two = y * __log(_CABS(x));
	ret_val->real = __exp(two) * __cos(one);
	ret_val->imag = __exp(two) * __sin(one);
}
