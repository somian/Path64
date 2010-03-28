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

	/* intrinsic QASIND */

	/* by address version only */

typedef union
{
	struct
	{
		uint32_t hi[2];
		uint32_t lo[2];
	} word;

	long double	ld;
} ldu;

static const	ldu	degprad =
{0x404ca5dc,	0x1a63c1f8,
 0xbce1e7ab,	0x456405f9};

long double __q_asind(long double *x)
{
ldquad	u;
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

	if ( *x == 0.0L )
		return ( 0.0L );

	if ( __qabs(*x) < 1.0L )
	{
		result = __qasin(*x);

		return ( result*degprad.ld );
	}

	if ( *x == 1.0L )
		return ( 90.0L );

	if ( *x == -1.0L )
		return ( -90.0L );

	*__errnoaddr = EDOM;

	return ( __libm_qnan_ld );
}

