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


#pragma ident "@(#) libfi/mathlb/cdabs.c	92.1	07/09/99 11:00:36"

#include <fortran.h>
#include <math.h>
#include "mathdefs.h"

extern _f_real16 _CDABS(d_complex_t z);
extern _f_real16 _CDABS_(d_complex_t *z);

/*
 * CDABS:  complex(kind=16) absolute value, returns real(kind=16)
 * 128-bit float
 *        - pass by value
 */

_f_real16
_CDABS(d_complex_t z)
{
	_f_real16 __sqrtl(_f_real16 x);
	_f_real16 __fabsl(_f_real16 x);
	_f_real16 real = __fabsl(z.real);
	_f_real16 imag = __fabsl(z.imag);
	_f_real16 ret_val;

	if (real == 0.0 && imag == 0.0)
		return((_f_real16) 0.0);
	if (real > imag)
		ret_val = real * __sqrtl((_f_real16) 1.0 +
			(imag/real) * (imag/real));
	else
		ret_val = imag * __sqrtl((_f_real16) 1.0 +
			(real/imag) * (real/imag));

	return (ret_val);
}

/*
 * CDABS:  complex(kind=16) absolute value, returns real(kind=16)
 * 128-bit float
 *        - pass by address
 */

_f_real16
_CDABS_(d_complex_t *z)
{
	return (_CDABS(*z));
}
