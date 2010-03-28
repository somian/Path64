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


#pragma ident "@(#) libfi/mathlb/cabs.c	92.1	07/09/99 11:00:36"


#include <fortran.h>
#include <math.h>
#include "mathdefs.h"

extern _f_real8 _CABS(c_complex_t z);
extern _f_real8 _CABS_(c_complex_t *z);

/*
 * CABS:  complex(kind=8) absolute value, returns real(kind=8)
 *        - pass by value
 */

_f_real8
_CABS(c_complex_t z)
{
	_f_real8 __fabs(_f_real8 x);
	_f_real8 __sqrt(_f_real8 x);
	_f_real8 real = __fabs(z.real);
	_f_real8 imag = __fabs(z.imag);
	_f_real8 ret_val;

	if (real == 0.0 && imag == 0.0)
		return((_f_real8) 0.0);
	if (real > imag)
		ret_val = real * __sqrt((_f_real8) 1.0 +
				(imag/real) * (imag/real));
	else
		ret_val = imag * __sqrt((_f_real8) 1.0 +
				(real/imag) * (real/imag));

	return (ret_val);
}

/*
 * CABS:  complex(kind=8) absolute value, returns real(kind=8)
 *        - pass by address
 */

_f_real8
_CABS_(c_complex_t *z)
{
        return (_CABS(*z));
}
