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


#pragma ident "@(#) libfi/mathlb/cdtod.c	92.1	07/09/99 11:00:36"
 
#include <fortran.h>
#include <math.h>
#include <fp.h>
#include "mathdefs.h"

extern void _CDTOD(d_complex_t *ret_val, d_complex_t x, _f_real16 *r);

/*
 * CDTOD: complex(kind=16) raised to a real(kind=16): pass by value
 *
 *	x = a+b*i
 *
 *	if ((x == 0+0*i) && (y == 0)) then return(NAN)
 *	if (x == 0+0*i) then return(0+0*i)
 *	if (y == 0) then return(1+0*i)
 */

void
_CDTOD(d_complex_t *ret_val,
        d_complex_t x,
        _f_real16 *r)
{
	_f_real16 __atan2l(_f_real16 ax, _f_real16 bx);
	_f_real16 __cosl(_f_real16 ax);
	_f_real16 __expl(_f_real16 ax);
	_f_real16 __logl(_f_real16 ax);
	_f_real16 __sinl(_f_real16 ax);
	_f_real16 _CDABS(d_complex_t z);
	_f_real16 y = *r;
	_f_real16 one;
	_f_real16 two;
	if (x.real == (_f_real16) 0.0 && x.imag == (_f_real16) 0.0) {
		if (y == (_f_real16) 0.0) {
			ret_val->real = _DBL_NaN;
			ret_val->imag = _DBL_NaN;
		}
		else {
			ret_val->real = (_f_real16) 0.0;
			ret_val->imag = (_f_real16) 0.0;
		}
		return;
	}
	one = y * __atan2l(x.imag, x.real);
	two = y * __logl(_CDABS(x));
	ret_val->real = __expl(two) * __cosl(one);
	ret_val->imag = __expl(two) * __sinl(one);
}
