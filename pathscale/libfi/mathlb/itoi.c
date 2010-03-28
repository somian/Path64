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


#pragma ident "@(#) libfi/mathlb/itoi.c	92.3	09/27/99 14:56:21"


#include <fortran.h>
#include <liberrno.h>
#include <math.h>

extern _f_int8 _ITOI( _f_int8 x, _f_int8 y );
/*
 * ITOI - integer*64 raised to an integer*64
 */
_f_int8
_ITOI( _f_int8 x,
	   _f_int8 y )
{
	_f_int8 base, result, i;
	if (x == 0) {
		if (y == 0) {
#if	defined(__mips) || defined(_LITTLE_ENDIAN)
			return(1);
#else
			_lerror(_LELVL_ABORT, FEIPOWZR);
#endif
		}
		return(0);
	}
	if (y < 0) {
		result =	0;
		if ((x == 1) || (x == -1)) {
			result =        1;
			if (((y & 1) == 1) && (x == -1))
				result =        -1;
		}
	} else {
		if (y == 0)
			return(1);
		base =	x;
		if (x < 0)
			base =	-x;
		result =	1;
		i =	y;
		while (i != 0) {
			if ((i & 1) == 1)
				result *=	base;
			base *=	base;
			/* Right shift is arithmetic shift on WORD32.
			 * Unsigned is required to prevent sign extension.
			 */
			i =	(unsigned) i >> 1;
		};
		if ((x < 0) && ((y & 1) == 1))
			result =	-result;
	};
	return(result);
}

#if	defined(__mips) || defined(_LITTLE_ENDIAN)
_f_int8
__powll( _f_int8 x,
	_f_int8 y )
{
	return(_ITOI(x, y));
}

/* do not use these unless needed for compatibility with f77
 * and call by address.
 */
#if 0
_f_int8
pow_ll( _f_int8 *x,
	_f_int8 *y )
{
	return(_ITOI(*x, *y));
}

_f_int8
pow_il( _f_int4 *x,
	_f_int8 *y )
{
	_f_int8	xtemp	= *x;
	return(_ITOI(xtemp, *y));
}

_f_int8
pow_li( _f_int8 *x,
	_f_int4 *y )
{
	_f_int8	ytemp	= *y;
	return(_ITOI(*x, ytemp));
}
#endif	/* end of if 0 */

#endif	/* end of mips or little endian */
