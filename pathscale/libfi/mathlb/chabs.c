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


#pragma ident "@(#) libfi/mathlb/chabs.c	92.1	07/09/99 11:00:36"
#include <fortran.h>
#include <math.h>
#include "mathdefs.h"

#ifdef _CRAYMPP
extern _f_real4 _CHABS_(_f_comp4 *z);
extern _f_real4 _CHABS(_f_comp4 z);

_f_real4
_CHABS_(_f_comp4 *z)
{
	return (_CHABS(*z));
}
#else
extern _f_real4 _CHABS_(h_complex_t *z);
extern _f_real4 _CHABS(h_complex_t z);

/*
 * _CHABS_:  complex(kind=4) absolute value, returns real(kind=4) value
 *        - pass by address
 */
_f_real4
_CHABS_(h_complex_t *z)
{
	return (_CHABS(*z));
}
#endif

/*
 * _CHABS:  complex(kind=4) absolute value, returns real(kind=4) value
 *        - pass by value
 */
_f_real4

#ifdef _CRAYMPP
_CHABS(_f_comp4 z)
{
	union hl_complx4 {
		_f_comp4	cpx4;
		struct {
			_f_real4	real;
			_f_real4	imag;
		} rlim4;
	} f;
	_f_real4 ret_val;
	_f_real8 _SQRT(_f_real8 x);
	_f_real8 real, imag;
	f.cpx4	= z;
	real = fabs((_f_real8) f.rlim4.real);
	imag = fabs((_f_real8) f.rlim4.imag);
	if (real == 0.0 && imag == 0.0) return((_f_real4) 0.0);
	if (real > imag)
		ret_val =
		 (_f_real4) (real * _SQRT((_f_real8) 1.0 +
				 (imag/real) * (imag/real)));
	else
		ret_val =
		 (_f_real4) (imag * _SQRT((_f_real8) 1.0 +
				 (real/imag) * (real/imag)));
#else
_CHABS(h_complex_t z)
{
	_f_real8 __fabs(_f_real8 x);
	_f_real8 __sqrt(_f_real8 x);
	_f_real8 real = __fabs((_f_real8) z.real);
	_f_real8 imag = __fabs((_f_real8) z.imag);
	_f_real4 ret_val;
	if (real == 0.0 && imag == 0.0) return((_f_real4) 0.0);
	if (real > imag)
		ret_val =
		 (_f_real4) (real * __sqrt((_f_real8) 1.0 +
				 (imag/real) * (imag/real)));
	else
		ret_val =
		 (_f_real4) (imag * __sqrt((_f_real8) 1.0 +
				 (real/imag) * (real/imag)));
#endif
	return (ret_val);
}
