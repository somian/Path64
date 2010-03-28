/*

  Copyright (C) 2000, 2001 Silicon Graphics, Inc.  All Rights Reserved.

   Path64 is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

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


#include <inttypes.h>
#include "quad.h"

extern	long double	__qnint(double, double);
extern	int16_t	__ii_qint(double, double);

	/* intrinsic IIQNNT */

	/* by value version */

	/* computes the nearest int16_t to a long double */

int16_t
__iiqnnt( double uhi, double ulo )
{
ldquad	x;
int16_t	result;

#include "msg.h"

	x.ld = __qnint(uhi, ulo);	/* find nearest whole number */

	result = __ii_qint(x.q.hi, x.q.lo);	/* convert to a int16_t */

	return  ( result );
}

