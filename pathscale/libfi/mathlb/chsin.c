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


#pragma ident "@(#) libfi/mathlb/chsin.c	92.1	07/09/99 11:00:36"


#include <fortran.h>
#include <math.h>
#include "mathdefs.h"

#ifdef _CRAYMPP

extern _f_comp4 _CHSIN_(_f_comp4 *z);
extern _f_comp4 _CHSIN(_f_comp4 z);

/* _CHSIN_  - complex(kind=4) - pass by address */
_f_comp4
_CHSIN_( _f_comp4 *z)
{
	return(_CHSIN(*z));
}
#else

extern void _CHSIN_(h_complex_t *ret_val, h_complex_t *z);
extern void _CHSIN(h_complex_t *ret_val, h_complex_t z);

/* _CHSIN_  - complex(kind=4) - pass by address */
void
_CHSIN_(h_complex_t *ret_val,
	h_complex_t *z)
{
	_CHSIN(ret_val, *z);
}
#endif

/*
 * _CHSIN  - complex(kind=4) - pass by value
 *
 * Semantics:  sin(z) = sin(a)*cosh(b) + cos(a)*sinh(b)*i
 *   (where z = a + b*i)
 * For T3X, call coss to return cos(a) in real and sin(a) in imag.
 */
#ifdef _CRAYMPP
_f_comp4
_CHSIN( _f_comp4 z)
{
	union hl_complx4 {
		_f_comp4	cpx4;
		struct {
			_f_real4	real;
			_f_real4	imag;
		} rlim4;
	} ret_val, f;
	union hl_complx8 {
		_f_comp8	cpx8;
		struct {
			_f_real8	real;
			_f_real8	imag;
		} rlim8;
	} ztmp;
	_f_comp8 _COSS(_f_real8 x);
	_f_real8 _COSH(_f_real8 x);
	_f_real8 _SINH(_f_real8 x);
	_f_real8 real, imag;
	f.cpx4 = z;
	real = (_f_real8) f.rlim4.real;
	imag = (_f_real8) f.rlim4.imag;
	ztmp.cpx8	= _COSS(real);
	ret_val.rlim4.real = (_f_real4) (ztmp.rlim8.imag * _COSH(imag));
	ret_val.rlim4.imag = (_f_real4) (ztmp.rlim8.real * _SINH(imag));
	return(ret_val.cpx4);
}
#else
void
_CHSIN(h_complex_t *ret_val,
	   h_complex_t z)
{
	_f_real8 __cos(_f_real8 x);
	_f_real8 __cosh(_f_real8 x);
	_f_real8 __sin(_f_real8 x);
	_f_real8 __sinh(_f_real8 x);
	_f_real8 real = (_f_real8) z.real;
	_f_real8 imag = (_f_real8) z.imag;

	ret_val->real = (_f_real4) (__sin(real) * __cosh(imag));
	ret_val->imag = (_f_real4) (__cos(real) * __sinh(imag));
}
#endif
