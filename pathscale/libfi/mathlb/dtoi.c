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


#pragma ident "@(#) libfi/mathlb/dtoi.c	92.1	07/09/99 11:00:36"
 
#include <fortran.h>
#include <fp.h>
#include <math.h>
#include "inline.h"

extern _f_real16 _DTOI( _f_real16 r, _f_int8 *i);

/*
 * DTOI - Real(kind=16) raised to an integer(kind=8) power
 */
_f_real16
_DTOI( _f_real16 r,
	 _f_int8 *i)
{
	_f_real16 base, result;
	_f_int8 ipow;
	ipow =	*i;
	if (ipow == 0) {
		if (r != 0.0) {
			result =	1.0;
			/* retain isnan since _DBL_NaN
			 * is just one form of many
			 * possible nans.
			 */
			if (isnan128(r))
				result =        r;
		}
		else {
			result = _DBL_NaN;
		}
		return(result);
	}
	result =	1.0;
	base =  r;
	if (ipow < 0) {
		if (r != 0.0) {
			base =  1.0/r;
			ipow =     -ipow;
		}
	}
	while (ipow != 0) {
		if ((ipow & 1) == 1)
			result *=       base;
		ipow =	(unsigned) ipow >> 1;
		if (ipow != 0)
			base *= base;
	}
	return (result);
}

