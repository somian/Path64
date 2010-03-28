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


#pragma ident "@(#) libfi/mathlb/idnint.c	92.1	07/09/99 11:00:36"


#include <fortran.h>
#include <liberrno.h>

extern _f_int8 _IDNINT_(_f_real16 *x);

/*
 * IDNINT:  Nearest integer(kind=8) in real(kind=16)
 *        - pass by address
 */

_f_int8
_IDNINT_(_f_real16 *x)
{
	_f_real16 _DINT(_f_real16 arg);
        _f_real16 y = *x;
        _f_real16 result;
        if (y < 0.0)
                y -= (_f_real16) 0.5;
        else
                y += (_f_real16) 0.5;
        result	= _DINT(y);

	/* hardware will raise invalid operation exception and
	 * return HUGE if float value too large for integer.
	 */
        return ((_f_int8) result);
}
