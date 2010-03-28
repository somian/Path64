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


#include <errno.h>
#include <inttypes.h>
#include "quad.h"

	/* intrinsic QSIND */

	/* by address version only */

typedef union
{
	struct
	{
		unsigned int hi;
		unsigned int lo;
	} word;

	double	d;
} du;

typedef union
{
	struct
	{
		uint32_t hi[2];
		uint32_t lo[2];
	} word;

	long double	ld;
} ldu;

#define	ROUND(d)	(int)(((d) >= 0.0) ? ((d) + 0.5) : ((d) - 0.5))

static const	ldu	radspdeg =
{0x3f91df46,	0xa2529d39,
 0x3c15c1d8,	0xbecdd290};

static const	du	rninety =
{0x3f86c16c,	0x16c16c17};

long double __q_sind(long double *x)
{
ldquad	u;
int	n;
double	dn;
long double	result;

	u.ld = *x;

	if ( u.q.hi != u.q.hi )
	{
		/* x is a NaN; return a quiet NaN */

#ifdef _IP_NAN_SETS_ERRNO

		*__errnoaddr = EDOM;
#endif
		return ( __libm_qnan_ld );
	}

	if ( __qabs(*x) == __libm_inf_ld )
	{
		/* x is +/-Inf; return a quiet NaN */

		*__errnoaddr = EDOM;

		return ( __libm_qnan_ld );
	}

	if ( *x == 0.0L )
		return ( 0.0L );

	/* reduce arg to +/- 360 degrees */

	u.ld = __qmod(u.ld, 360.0L);

	/* next, reduce to +/-45.0 */

	if ( __qabs(u.ld) <= 45.0L )
	{
		n = 0;
	}
	else
	{
		dn = u.q.hi*rninety.d;
		n = ROUND(dn);
		dn = n;

		u.ld = u.ld - dn*90.0L;
	}

	/* convert x to radians */

	u.ld = u.ld*radspdeg.ld;

	if ( n&1)
	{
		if ( n&2 )
		{
			/*
			 *  n%4 = 3
			 *  result is -cos(u)
			 */

			result = -__qcos(u.ld);
		}
		else
		{
			/*
			 *  n%4 = 1
			 *  result is cos(u)
			 */

			result = __qcos(u.ld);
		}

		return ( result );
	}

	if ( n&2 )
	{
		/*
		 *  n%4 = 2
		 *  result is -sin(u)
		 */

		result = -__qsin(u.ld);
	}
	else
	{
		/*
		 *  n%4 = 0
		 *  result is sin(u)
		 */

		result = __qsin(u.ld);
	}

	return( result );
}

