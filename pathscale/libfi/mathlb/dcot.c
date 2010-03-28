/*
 * Copyright 2004, 2005, 2006 PathScale, Inc.  All Rights Reserved.
 */

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


#pragma ident "@(#) libfi/mathlb/dcot.c	92.1	07/09/99 11:00:36"

#include <fortran.h>
#include <math.h>

#ifndef	__mips
extern _f_real16 _DCOT_( _f_real16 *x );
extern _f_real16 _DCOT( _f_real16 x );

/*
 * DCOT - real(kind=16) - pass by address
 * 128-bit float cotangent
 */
_f_real16
_DCOT_( _f_real16 *x )
{
	return (_DCOT(*x));
}

/*
 * DCOT  - real(kind=16) - pass by value
 * 128-bit float cotangent
 */
_f_real16
_DCOT( _f_real16 x )
{
	return ( ((_f_real16) 1.0) / tanl(x) );
}

#else	/* NOT __mips */

/*
 * __q_cot  - real(kind=16) - pass by address
 * 128-bit float cotangent
 */
_f_real16
__q_cot(_f_real16 *arg)
{
        return (1.L / __qtan(*arg) );
}

#endif	/* NOT __mips */
