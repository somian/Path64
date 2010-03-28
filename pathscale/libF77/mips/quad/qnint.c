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


#include "quad.h"

extern	long double	__q_add(double, double, double, double);
extern	long double	__q_sub(double, double, double, double);
extern	long double	__qint(double, double);

	/* intrinsic QNINT */

	/* by value version */

	/* computes the nearest whole number to a long double */

long double
__qnint(double  uhi,double  ulo )
{
ldquad	result, y;


	if ( uhi != uhi )
	{
		result.q.hi = uhi;
		result.q.lo = ulo;

		return ( result.ld );
	}

	if ( uhi >= 0.0 )
	{
		y.ld = __q_add(uhi, ulo, 0.5, 0.0);	/* y = arg + 0.5 */
	}
	else
	{
		y.ld = __q_sub(uhi, ulo, 0.5, 0.0); /* y = arg - 0.5 */
	}

	result.ld = __qint(y.q.hi, y.q.lo);

	return (result.ld);
}

