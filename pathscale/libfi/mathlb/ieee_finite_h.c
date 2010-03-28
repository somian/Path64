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


#pragma ident "@(#) libfi/mathlb/ieee_finite_h.c	92.1	07/09/99 11:00:36"

#include <fortran.h>
#include "inline.h"

extern _f_log4 _IEEE_FINITE_L4_H( _f_real4 x);

/* Function which returns TRUE if 32-bit argument a is a finite value.
 * This includes zero, normal, and subnormal numbers.
 * Else, it returns FALSE for NaN or infinity.
 */
_f_log4 _IEEE_FINITE_L4_H( _f_real4 x)
{
	/* Union defined to work with IEEE 32-bit floating point. */
	union _ieee_single {
		_f_real4	hword;
		unsigned int	ui[1];	
	};
	union	_ieee_single x_val;

	x_val.hword	= x;

	/* if x is finite, return TRUE */
	return ((_f_log4) isfinite32(x_val.ui[0]));
}


extern _f_log8 _IEEE_FINITE_L8_H( _f_real4 x);

_f_log8 _IEEE_FINITE_L8_H( _f_real4 x)
{
	/* Union defined to work with IEEE 32-bit floating point. */
	union _ieee_single {
		_f_real4	hword;
		unsigned int	ui[1];	
	};
	union	_ieee_single x_val;

	x_val.hword	= x;

	/* if x is finite, return TRUE */
	return ((_f_log8) isfinite32(x_val.ui[0]));
}
