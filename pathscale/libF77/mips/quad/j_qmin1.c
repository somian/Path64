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

extern	long double	__q_min1(double, double, double, double);
extern	int32_t	__ji_qint(double, double);

	/* intrinsic MIN1 */

	/* by value only */

	/* computes the integral part of the minimum of two long doubles */

int32_t
__j_qmin1( uhi, ulo, vhi, vlo )
double	uhi, ulo, vhi, vlo;
{
ldquad	x;
int32_t	result;

#include "msg.h"

	x.ld = __q_min1(uhi, ulo, vhi, vlo);	/* fetch minimum value */

	result = __ji_qint(x.q.hi, x.q.lo);	/* convert it to an integer */

	return ( result );
}

