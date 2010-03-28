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


#pragma ident "@(#) libfi/mathlb/cdton.c	92.1	07/09/99 11:00:36"


#include <fortran.h>
#include <fp.h>
#include <math.h>
#include "mathdefs.h"

extern void _CDTON(d_complex_t *ret_val, d_complex_t x, _f_int4 *y );

/*
 *      complex(kind=16) raised to an Integer*32 = _CDTON
 *
 *      x = a+b*i
 *
 *      if ((x == 0+0*i) && (i == 0)) then return(NAN)
 *      if (x == 0+0*i) then return(0+0*i)
 */
void
_CDTON(d_complex_t *ret_val,
	d_complex_t x,
	_f_int4 *y )
{
	long i =	*y;
	_f_real16 t, a, b;  /* temporary values */
	a =	x.real;
	b =	x.imag;
	if (a == (_f_real16) 0.0 && b == (_f_real16) 0.0) {
		if (i == 0) {
			ret_val->real =	_DBL_NaN;
			ret_val->imag =	_DBL_NaN;
		}
		else {
			ret_val->real =	(_f_real16) 0.0;
			ret_val->imag =	(_f_real16) 0.0;
		}
		return;
	}
	if (i <  0) {
		i =	-i;
		t =	a;
		a =	a / (a*a + b*b);
		b =	(- b) / (t*t + b*b);
	}
	ret_val->real =	(_f_real16) 1.0;
	ret_val->imag =	(_f_real16) 0.0;
	while (i !=  0) {
		if ((i & 1) == 1) {
			t =     ret_val->real;
			ret_val->real = t * a - ret_val->imag * b;
			ret_val->imag = ret_val->imag * a + t * b;
		}
		t =	a;
		a =	a*a - b*b;
		b =	b*t + t*b;
		i =	(unsigned) i >> 1;
	}
}
