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


#pragma ident "@(#) libfi/mathlb/iabs.c	92.2	10/04/99 16:45:21"

#include <fortran.h>

extern _f_int8 _IABS_(_f_int8 *x);
extern _f_int8 _IABS(_f_int8 x);

/*
 * IABS: integer(kind=8) - pass by address
 */
_f_int8
_IABS_(_f_int8 *x)
{
        return (_IABS(*x) );
}

/*
 * IABS: integer(kind=8)  - pass by value
 */
_f_int8
_IABS( _f_int8 x )
{
        return ( (x > 0) ? x : -x);
}

#if	defined(__mips) || defined(_LITTLE_ENDIAN)

_f_int8
__l_abs_(_f_int8 *x)
{
        return (_IABS(*x));
}

_f_int8
l_abs(_f_int8 *x)
{
        return (_IABS(*x));
}

#endif	/* mips or little endian */
