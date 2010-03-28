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


#pragma ident "@(#) libfi/mathlb/chexp.c	92.1	07/09/99 11:00:36"


#include <fortran.h>
#include <math.h>
#include "mathdefs.h"

#ifdef _CRAYMPP

extern _f_comp4 _CHEXP_(_f_comp4 *z);
extern _f_comp4 _CHEXP(_f_comp4 z);

/* _CHEXP_ - complex(kind=4) - pass by address */
_f_comp4
_CHEXP_( _f_comp4 *z)
{
	return(_CHEXP(*z));
}
#else

extern void _CHEXP_(h_complex_t *ret_val, h_complex_t *z);
extern void _CHEXP(h_complex_t *ret_val, h_complex_t z);

/* _CHEXP_ - complex(kind=4) - pass by address */
void
_CHEXP_(h_complex_t *ret_val,
	h_complex_t *z)
{
	_CHEXP(ret_val, *z);
}
#endif

/*
 * _CHEXP - complex(kind=4) - pass by value
 *
 * Semantics:  exp(z) = exp(a)*cos(b) + exp(a)*sin(b)*i
 *   (where z = a + b*i)
 * For T3X, use COSS to return cos(b) in real and sin(b) in imag
 */
#ifdef _CRAYMPP
_f_comp4
_CHEXP( _f_comp4 z )
{
	union hl_complx4 {
		_f_comp4        cpx4;
		struct {
			_f_real4        real;
			_f_real4        imag;
		} rlim4;
	} ret_val, f;
	union hl_complx8 {
		_f_comp8	cpx8;
		struct {
			_f_real8	real;
			_f_real8	imag;
		} rlim8;
	} ztmp;
	_f_real8 _EXP(_f_real8 x);
	_f_comp8 _COSS(_f_real8 x);
	_f_real8 real, imag, realtmp;
	f.cpx4 = z;
	real = (_f_real8) f.rlim4.real;;
	imag = (_f_real8) f.rlim4.imag;;
	ztmp.cpx8 = _COSS(imag);
	realtmp = _EXP(real);
	ret_val.rlim4.real = (_f_real4) (realtmp * ztmp.rlim8.real);
	ret_val.rlim4.imag = (_f_real4) (realtmp * ztmp.rlim8.imag);
	return(ret_val.cpx4);
}
#else
void
_CHEXP(h_complex_t *ret_val,
	h_complex_t z )
{
	_f_real8 __exp(_f_real8 x);
	_f_real8 __cos(_f_real8 x);
	_f_real8 __sin(_f_real8 x);
	_f_real8 real = (_f_real8) z.real;
	_f_real8 imag = (_f_real8) z.imag;
	_f_real8 realtmp = __exp(real);

	ret_val->real = (_f_real4) (realtmp * __cos(imag));
	ret_val->imag = (_f_real4) (realtmp * __sin(imag));
}
#endif
