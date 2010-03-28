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


#pragma ident "@(#) libfi/mathlb/ctoch.c	92.1	07/09/99 11:00:36"


#include <fortran.h>
#include <math.h>
#include "mathdefs.h"

extern void _CTOCH(c_complex_t *ret_val, c_complex_t x, h_complex_t y);

/*
 * complex(kind=8) raised to a complex(kind=4):  _CTOCH
 */
void
_CTOCH(c_complex_t *ret_val,
	c_complex_t x,
	h_complex_t y)
{
	void _CTOC(c_complex_t *ret_val, c_complex_t x, c_complex_t y);
	c_complex_t b, c;

	b.real = (_f_real8) y.real;
	b.imag = (_f_real8) y.imag;
	(void) _CTOC( &c, x, b );
	ret_val->real = c.real;
	ret_val->imag = c.imag;
}
