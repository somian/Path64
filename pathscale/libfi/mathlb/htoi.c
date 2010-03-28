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


#pragma ident "@(#) libfi/mathlb/htoi.c	92.2	09/27/99 14:56:21"


#include <fortran.h>

extern _f_real4 _HTOI( _f_real4 r, _f_int8 i );

/*
 * HTOI: real(kind=4) raised to an integer(kind=8) power
 */
_f_real4
_HTOI( _f_real4 r,
	_f_int8 i )
{
	_f_real8 _RTOI( _f_real8 r, _f_int8 i);
	return ((_f_real4) _RTOI( (_f_real8) r, i));
}

#if	defined(__mips) || defined(_LITTLE_ENDIAN)
_f_real4
__powrl( _f_real4 x,
	_f_int8 y )
{
	_f_real8 _RTOI( _f_real8 r, _f_int8 i);
	return ((_f_real4) _RTOI( (_f_real8) x, y));
}

/* if needed for compatibility with f77 for call by address */
#if 0
pow_rl( _f_real4 *x,
	_f_int8 *y )
{
	_f_real8 _RTOI( _f_real8 r, _f_int8 i);
	_f_real8 xtemp = *x;
	return ((_f_real4) _RTOI( xtemp, *y));
}
#endif	/* end of if 0 */

#endif	/* end of mips or little endian */
