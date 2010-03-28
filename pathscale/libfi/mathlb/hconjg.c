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


#pragma ident "@(#) libfi/mathlb/hconjg.c	92.1	07/09/99 11:00:36"


#include <fortran.h>
#include <math.h>
#include "mathdefs.h"

#ifdef _CRAYMPP
extern _f_comp4 _HCONJG_(_f_comp4 *z);

_f_comp4
_HCONJG_( _f_comp4 *z)
{
	union hl_complx4 {
		_f_comp4	cpx4;
		struct {
			_f_real4	real;
			_f_real4	imag;
		} rlim4;
	} ret_val, y;
	y.cpx4 = *z;
	ret_val.rlim4.real = y.rlim4.real;
	ret_val.rlim4.imag = -y.rlim4.imag;
	return (ret_val.cpx4);
}
#else

extern void _HCONJG_(h_complex_t *ret_val, h_complex_t *z);
/*
 * HCONJG  - complex(kind=4) - pass by value
 *
 * Semantics:    z = a+b*i
 *               conjg(z) = a-b*i
 */
void
_HCONJG_(h_complex_t *ret_val,
	h_complex_t *z)
{
	h_complex_t y = *z;
	ret_val->real = y.real;
	ret_val->imag = -y.imag;
}
#endif
