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


#pragma ident "@(#) libfi/mathlb/cdlog.c	92.1	07/09/99 11:00:36"

#include <fortran.h>
#include <math.h>
#include "mathdefs.h"

extern void _CDLOG(d_complex_t *ret_val, d_complex_t z);
extern void _CDLOG_(d_complex_t *ret_val, d_complex_t *z);

/*
 * CDLOG  - complex(kind=16) - pass by value
 * complex 128-bit float logarithm
 *
 * Semantics:  log(z) = log(cabs(z)) + atan2(b,a)*i
 *   (where z = a + b*i)
 */

void
_CDLOG(d_complex_t *ret_val,
	d_complex_t z)
{
	_f_real16 __logl(_f_real16 x);
	_f_real16 __atan2l(_f_real16 x, _f_real16 y);
	_f_real16 _CDABS(d_complex_t z );
	ret_val->real = __logl(_CDABS(z));
	ret_val->imag = __atan2l(z.imag, z.real);
}

/*
 * CDLOG  - complex(kind=16) - pass by address
 * complex 128-bit float logarithm
 */

void
_CDLOG_(d_complex_t *ret_val,
       	d_complex_t *z)
{
	_CDLOG(ret_val, *z);
}
