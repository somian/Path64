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


#pragma ident "@(#) libfi/mathlb/cdsqrt.c	92.1	07/09/99 11:00:36"

#include <fortran.h>
#include <math.h>
#include "mathdefs.h"

extern void _CDSQRT(d_complex_t *ret_val, d_complex_t z);
extern void _CDSQRT_(d_complex_t *ret_val, d_complex_t *z);

/*
 * CDSQRT:  complex(kind=16)  square root - pass by value
 * 128-bit float
 */

void
_CDSQRT(d_complex_t *ret_val,
	d_complex_t z)
{
	_f_real16 __sqrtl(_f_real16 x);
	_f_real16 __fabsl(_f_real16 x);
	_f_real16 _CDABS(d_complex_t z );
	_f_real16 q, x, y;
	q = _CDABS(z) / (_f_real16) 2.0 + __fabsl(z.real) /
			(_f_real16) 2.0;
	x = __sqrtl(q);
	if (x == 0.0) {
		ret_val->real = 0.0;
		ret_val->imag = 0.0;
		return;
	}
	y = __fabsl(z.imag) / ((_f_real16) 2.0*x);
	if (z.real < (_f_real8) 0.0) {
		q = x;
		x = y;
		y = q;
	}
	if (z.imag < (_f_real16) 0.0)
		y = -y;
	ret_val->real = x;
	ret_val->imag = y;
}

/*
 * CDSQRT:  complex(kind=16)  square root - pass by address
 * 128-bit float
 */

void
_CDSQRT_(d_complex_t *ret_val,
	d_complex_t *z)
{
	_CDSQRT(ret_val, *z);
}
