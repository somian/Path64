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


#pragma ident "@(#) libfi/mathlb/chsqrt.c	92.1	07/09/99 11:00:36"


#include <fortran.h>
#include <math.h>
#include "mathdefs.h"

#ifdef _CRAYMPP

extern _f_comp4 _CHSQRT_(_f_comp4 *z);
extern _f_comp4 _CHSQRT(_f_comp4 z);

/* _CHSQRT_:  complex(kind=4) square root - pass by address */
_f_comp4
_CHSQRT_( _f_comp4 *z)
{
	return(_CHSQRT(*z));
}
#else

extern void _CHSQRT_(h_complex_t *ret_val, h_complex_t *z);
extern void _CHSQRT(h_complex_t *ret_val, h_complex_t z);

/* _CHSQRT_:  complex(kind=4) square root - pass by address */
void
_CHSQRT_(h_complex_t *ret_val,
	h_complex_t *z)
{
	_CHSQRT(ret_val, *z);
}
#endif

/*
 * _CHSQRT:  complex(kind=4) square root - pass by value
 */
#ifdef _CRAYMPP
_f_comp4
_CHSQRT( _f_comp4 z )
{
        union hl_complx4 {
                _f_comp4        cpx4;
                struct {
                        _f_real4        real;
                        _f_real4        imag;
                } rlim4;
        } ret_val, f;
	_f_real8 _SQRT(_f_real8 x);
	_f_real4 _CHABS(_f_comp4 z);
	_f_real8 real, imag;
	_f_real8 q, x, y;
	f.cpx4 = z;
	real = (_f_real8) f.rlim4.real;
	imag = (_f_real8) f.rlim4.imag;

	q = (_f_real8) _CHABS(z) / (_f_real8) 2.0 +
			fabs(real) / (_f_real8) 2.0;
	x = _SQRT(q);
	if (x == 0.0) {
		ret_val.rlim4.real = 0.0;
		ret_val.rlim4.imag = 0.0;
		return(ret_val.cpx4);
	}
	y = fabs(imag) / ((_f_real8) 2.0*x);
	if (real < (_f_real8) 0.0) {
		q = x;
		x = y;
		y = q;
	}
	if (imag < (_f_real8) 0.0)
		y = -y;
	ret_val.rlim4.real = (_f_real4) x;
	ret_val.rlim4.imag = (_f_real4) y;
	return(ret_val.cpx4);
}
#else
void
_CHSQRT(h_complex_t *ret_val,
	h_complex_t z )
{
	_f_real8 __sqrt(_f_real8 x);
	_f_real8 __fabs(_f_real8 x);
	_f_real4 _CHABS(h_complex_t z);
	_f_real8 real = (_f_real8) z.real;
	_f_real8 imag = (_f_real8) z.imag;
	_f_real8 q, x, y;

	q = (_f_real8) _CHABS(z) / (_f_real8) 2.0 +
			__fabs(real) / (_f_real8) 2.0;
	x = __sqrt(q);
	if (x == 0.0) {
		ret_val->real = 0.0;
		ret_val->imag = 0.0;
		return;
	}
	y = __fabs(imag) / ((_f_real8) 2.0*x);
	if (real < (_f_real8) 0.0) {
		q = x;
		x = y;
		y = q;
	}
	if (imag < (_f_real8) 0.0)
		y = -y;
	ret_val->real = x;
	ret_val->imag = y;
}
#endif
