/*
 * Copyright (C) 2007. PathScale, LLC. All Rights Reserved.
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


#pragma ident "@(#) libfi/mathlb/cton.c	92.2	11/16/99 18:14:06"


#include <fortran.h>
#include <fp.h>
#include <math.h>
#include "mathdefs.h"

/* KEY compilations define _LITTLE_ENDIAN but not __LITTLE_ENDIAN, so this
 * file creates _CTON but not __powzi even though _LITTLE_ENDIAN is defined.
 * Strange, but __powzi would duplicate the symbol from powzi.c. Elsewhere,
 * KEY Linux compilations seem to rely on "#if __BYTE_ORDER == __LITTLE_ENDIAN"
 * evaluating true even though __LITTLE_ENDIAN is undefined. */
#if	!defined(__LITTLE_ENDIAN) && !defined(__mips)
extern void _CTON(c_complex_t *ret_val, c_complex_t x, _f_int4 *y );

/*
 *	complex(kind=8) raised to an Integer*32 = _CTON
 *
 *	x = a+b*i
 *
 *	if ((x == 0+0*i) && (i == 0)) then return(NAN)
 *	if (x == 0+0*i) then return(0+0*i)
 */
void
_CTON(c_complex_t *ret_val,
	c_complex_t x,
	_f_int4 *y )
{
	long   i =	*y;
	_f_real8 t, a, b;  /* temporary values */
	a =	x.real;
	b =	x.imag;
	if (a == (_f_real8) 0.0 && b == (_f_real8) 0.0) {
		if (i == 0) {
			ret_val->real =	_SGL_NaN;
			ret_val->imag =	_SGL_NaN;
		}
		else {
			ret_val->real =	(_f_real8) 0.0;
			ret_val->imag =	(_f_real8) 0.0;
		}
		return;
	}
	if (i <  0) {
		i =	-i;
		t =	a;
		a =	a / (a*a + b*b);
		b =	(- b) / (t*t + b*b);
	}
        ret_val->real =	(_f_real8) 1.0;
        ret_val->imag =	(_f_real8) 0.0;
	while (i !=  0) {
		if ((i & 1) == 1) {
			t =	ret_val->real;
			ret_val->real =	t * a - ret_val->imag * b;
			ret_val->imag =	ret_val->imag * a + t * b;
		}
		t =	a;
		a =	a*a - b*b;
		b =	b*t + t*b;
		i =	(unsigned) i >> 1;
	}
}

#elif	defined(_LITTLE_ENDIAN)
/* __powzi = a**n  */
dbl_complex_t
__powzi(_f_real8 arel, _f_real8 aimg, _f_int4 n)
{
	dbl_complex_t	x, ret_val;
	_f_int4		i;
	_f_real8	t, a, b;

	i	= n;
	a	= arel;
	b	= aimg;
	ret_val.real	= 1.0;
	ret_val.imag	= 0.0;

	if (i < 0) {
		i	= -i;
		t =	a;
		a =	a / (a*a + b*b);
		b =	(- b) / (t*t + b*b);
	}

	while (i !=  0) {
		if ((i & 1) == 1) {
			t =	ret_val.real;
			ret_val.real =	t * a - ret_val.imag * b;
			ret_val.imag =	ret_val.imag * a + t * b;
		}
		t =	a;
		a =	a*a - b*b;
		b =	b*t + t*b;
		i =	(unsigned) i >> 1;
	}

	return ret_val;
}
#endif
