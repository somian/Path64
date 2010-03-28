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


#pragma ident "@(#) libfi/mathlb/hton.c	92.2	09/27/99 14:56:21"


#include <fortran.h>
#include <math.h>			/* for isnan */

extern _f_real4 _HTON( _f_real4 r, _f_int4 i );

/*
 * HTON real(kind=4)  raised to an integer*32 
 */
_f_real4
_HTON( _f_real4 r,
	   _f_int4 i )
{
#if	defined(__mips) || defined(_LITTLE_ENDIAN)
	_f_real4 base, result;
	if (i == 0) {
		if (r != 0.0) {
			result	= 1.0;
			/* use isnan since there are many forms
			 * of nans.
			 */
			if (isnan(r))
				result	= r;
		}
		else {
			result	= 1.0;
		}
		return(result);
	}
	result	= 1.0;
	base	= r;
	if (i < 0) {
		if (r != 0.0) {
			base	= 1.0/r;
			i	= -i;
		}
	}
	while (i != 0) {
		if ((i & 1) == 1)
			result	= result * base;
		i	= (unsigned) i >> 1;
		if (i != 0)
			base	= base * base;
	}
	return (result);
#else
	_f_real8 _RTON( _f_real8 r, _f_int4 i);
	return ((_f_real4) _RTON( (_f_real8) r, i));
#endif
}


#if	defined(__mips) || defined(_LITTLE_ENDIAN)
_f_real4
__powri( _f_real4 x,
	_f_int4 y )
{
	return ((_f_real4) _HTON(x, y));
}

/* if needed for compatibility with f77 for pass by address */
#if 0
_f_real4
pow_ri( _f_real4 *x,
	_f_int4 *y )
{
	return ((_f_real4) _HTON(*x, *y));
}
#endif	/* end of if 0 */

#endif	/* end of mips or little endian */
