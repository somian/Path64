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


#pragma ident "@(#) libfi/mathlb/iton.c	92.1	07/09/99 11:00:36"


#include <fortran.h>
#include <liberrno.h>
#include <math.h>

extern _f_int8 _ITON( _f_int8 x, _f_int4 y );
/*
 * ITON - integer*64 raised to an integer*32
 */
_f_int8
_ITON( _f_int8 x,
	   _f_int4 y )
{
	_f_int8 base, result, i;
	if (x == 0) {
		if (y == 0) {
			_lerror(_LELVL_ABORT, FEIPOWZR);
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
			i >>=	1;
		};
		if ((x < 0) && ((y & 1) == 1))
			result =	-result;
	};
	return(result);
}
