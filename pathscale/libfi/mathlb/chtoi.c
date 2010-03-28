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


#pragma ident "@(#) libfi/mathlb/chtoi.c	92.2	11/16/99 18:14:06"


#include <fortran.h>
#include <math.h>
#include "mathdefs.h"

#if	!defined(_LITTLE_ENDIAN) && !defined(__mips)
extern void _CHTOI(h_complex_t *ret_val, h_complex_t x, _f_int8 *y);

/*
 * Single complex raised to an integer*64:  _CHTOI
 */
void
_CHTOI(h_complex_t *ret_val,
	h_complex_t x,
	_f_int8 *y)
{
	c_complex_t a, c;
	void _CTOI(c_complex_t *ret_val, c_complex_t x, _f_int8 *y );

	a.real =	(_f_real8) x.real;
	a.imag =	(_f_real8) x.imag;
	(void) _CTOI( &c, a, y );
	ret_val->real =	(_f_real4) c.real;
	ret_val->imag =	(_f_real4) c.imag;
}
#elif	defined(_LITTLE_ENDIAN)
/* __powci = a**n  */
h_complex_t
__powcl(_f_real4 arel, _f_real4 aimg, _f_int8 n)
{
	h_complex_t	x, ret_val;
	_f_int8		i;
	_f_real4	t, a, b;

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
