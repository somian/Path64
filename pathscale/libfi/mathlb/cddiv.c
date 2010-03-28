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


#pragma ident "@(#) libfi/mathlb/cddiv.c	92.1	07/09/99 11:00:36"

#include <math.h>
#include "mathdefs.h"

extern void _CDDIV(d_complex_t *retval, d_complex_t x, d_complex_t y);

/*
 *      complex(kind=16) / complex(kind=16)
 *          - pass by value
 */
void _CDDIV(d_complex_t *retval, d_complex_t x, d_complex_t y)
{
	_f_real16 __fabsl(_f_real16 x);
	_f_real16 a,b,c,d,e,f,g,q;

	a = x.real;
	b = x.imag;
	c = y.real;
	d = y.imag;

/*
 *	special case d=0 so we have (a+b*i)/c
 */
	if (d == 0.0) {
		f = a/c;
		g = b/c;
		retval->real = f;
		retval->imag = g;
		return;
	}

/*
 *	perform complex/complex with scaling
 *
 *	if (|c| > |d|) then {|d/c| <= 1}
 *		(a+b*i)/(c+d*i) = (a+b*(d/c))/(c+d*(d/c)) +
 *				  (b-a*(d/c))/(c+d*(d/c))*i
 *	else {|c/d| < 1}
 *		(a+b*i)/(c+d*i) = (a*(c/d)+b)/(c*(c/d)+d) +
 *				  (b*(c/d)-a)/(c*(c/d)+d)*i
 */
	if (__fabsl(c) > __fabsl(d)) {
		q = d/c;
		e = c + d*q;
		f = (a + b*q)/e;
		g = (b - a*q)/e;
	}
	else {
		q = c/d;
		e = d + c*q;
		f = (b + a*q)/e;
		g = (-a + b*q)/e;
	}
	retval->real = f;
	retval->imag = g;
	return;
}
