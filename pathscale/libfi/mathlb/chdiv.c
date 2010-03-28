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


#pragma ident "@(#) libfi/mathlb/chdiv.c	92.1	07/09/99 11:00:36"

#include <math.h>
#include "mathdefs.h"

extern void _CHDIV(h_complex_t *ret_val, h_complex_t x, h_complex_t y);

/*
 *	complex(kind=4) / complex(kind=4) - pass by value
 */
void _CHDIV(h_complex_t *ret_val, h_complex_t x, h_complex_t y)
{
	_f_real8	a,b,c,d,recip;
	
	a = (_f_real8) x.real;
	b = (_f_real8) x.imag;
	c = (_f_real8) y.real;
	d = (_f_real8) y.imag;

/*
 *	No need to scale when casting to double because
 *	the exponent range is 8x larger for double and
 *	all that is needed is 2x to protect against under/overflow.
 */

	recip = c*c+d*d;
	ret_val->real = (_f_real4)((a*c+b*d)/recip);
	ret_val->imag = (_f_real4)((b*c-a*d)/recip);
}
