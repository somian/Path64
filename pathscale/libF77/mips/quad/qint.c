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

	/* intrinsic QINT */

	/* by value version */

	/* truncates a long double, i.e. computes the integral part of
	   a long double
	*/

extern	double	__trunc(double);

extern	double	fabs(double);
#pragma intrinsic (fabs)

typedef union
{
	struct
	{
		uint32_t hi;
		uint32_t lo;
	} word;

	double	d;
} du;

static const du		twop52 =
{0x43300000,	0x00000000};

long double
__qint(double  uhi,double  ulo )
{
ldquad	result;

#include "msg.h"

	if ( uhi != uhi )
	{
		result.q.hi = uhi;
		result.q.lo = ulo;

		return ( result.ld );
	}

	if ( uhi >= 0.0 )
	{
		if ( uhi < twop52.d )
		{
			/* binary point occurs in uhi; truncate uhi to an integer
			*/

			result.q.hi = __trunc(uhi);

			result.q.lo = 0.0;

			if ( result.q.hi < uhi )
				return ( result.ld );

			/* must adjust result by one if ulo < 0.0 */

			if ( ulo < 0.0 )
			{
				result.q.hi -= 1.0;

				return ( result.ld );
			}

			return ( result.ld );
		}
		else if ( fabs(ulo) < twop52.d )
		{
			/* binary point occurs in ulo; truncate ulo to an integer
			*/

			result.q.hi = uhi;

			result.q.lo = __trunc(ulo);

			if ( result.q.lo > ulo )
			{
				result.q.lo -= 1.0;
			}

			return ( result.ld );
		}

		/* arg is an integer */

		result.q.hi = uhi;
		result.q.lo = ulo;

		return ( result.ld );
	}
	else
	{
		if ( fabs(uhi) < twop52.d )
		{
			/* binary point occurs in uhi; truncate uhi to an integer
			*/

			result.q.hi = __trunc(uhi);

			result.q.lo = 0.0;

			if ( result.q.hi > uhi )
				return ( result.ld );

			/* must adjust result by one if ulo > 0.0 */

			if ( ulo > 0.0 )
			{
				result.q.hi += 1.0;

				return ( result.ld );
			}

			return ( result.ld );
		}
		else if ( fabs(ulo) < twop52.d )
		{
			/* binary point occurs in ulo; truncate ulo to an integer
			*/

			result.q.hi = uhi;

			result.q.lo = __trunc(ulo);

			if ( result.q.lo < ulo )
			{
				result.q.lo += 1.0;
			}

			return ( result.ld );
		}

		/* arg is an integer */

		result.q.hi = uhi;
		result.q.lo = ulo;

		return ( result.ld );
	}

}

