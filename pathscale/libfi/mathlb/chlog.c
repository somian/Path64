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


#pragma ident "@(#) libfi/mathlb/chlog.c	92.1	07/09/99 11:00:36"


#include <fortran.h>
#include <math.h>
#include "mathdefs.h"

#ifdef _CRAYMPP

extern _f_comp4 _CHLOG_(_f_comp4 *z);
extern _f_comp4 _CHLOG(_f_comp4 z);

/* _CHLOG_ complex(kind=4) - pass by address */
_f_comp4
_CHLOG_( _f_comp4 *z)
{
	return(_CHLOG(*z));
}
#else

extern void _CHLOG_(h_complex_t *ret_val, h_complex_t *z);
extern void _CHLOG(h_complex_t *ret_val, h_complex_t z);

/* _CHLOG_ complex(kind=4) - pass by address */
void
_CHLOG_(h_complex_t *ret_val,
	h_complex_t *z)
{
	_CHLOG(ret_val, *z);
}
#endif

/*
 * _CHLOG complex(kind=4) - pass by value
 *
 * Semantics:  log(z) = log(cabs(z)) + atan2(b,a)*i
 *   (where z = a + b*i)
 */
#ifdef _CRAYMPP
_f_comp4
_CHLOG( _f_comp4 z)
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
	_f_real8 _ALOG(_f_real8 x);
	_f_real8 _ATAN2(_f_real8 x, _f_real8 y);
	_f_real4 _CHABS(_f_comp4 z);
	_f_real8 real, imag;
	f.cpx4 = z;
	real = (_f_real8) f.rlim4.real;
	imag = (_f_real8) f.rlim4.imag;
	ret_val.rlim4.real = (_f_real4) _ALOG((_f_real8) _CHABS(z));
	ret_val.rlim4.imag = (_f_real4) _ATAN2(imag, real);
	return(ret_val.cpx4);
}
#else
void
_CHLOG(h_complex_t *ret_val,
	h_complex_t z)
{
	_f_real8 __log(_f_real8 x);
	_f_real8 __atan2(_f_real8 x, _f_real8 y);
	_f_real4 _CHABS(h_complex_t z);
	_f_real8 real = (_f_real8) z.real;
	_f_real8 imag = (_f_real8) z.imag;

	ret_val->real = (_f_real4) __log((_f_real8) _CHABS(z));
	ret_val->imag = (_f_real4) __atan2(imag, real);
}
#endif
