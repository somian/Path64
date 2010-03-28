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

	/* intrinsic QSIGN */

	/* by value version */

	/*	computes (sign of v)*|u|	*/

long double
__qsign(double  uhi,double  ulo,double  vhi,double  vlo )
{
ldquad	result;

#include "msg.h"

	if ( uhi != uhi )
	{
		result.q.hi = uhi;
		result.q.lo = ulo;

		return ( result.ld );
	}

	if ( vhi != vhi )
	{
		result.q.hi = vhi;
		result.q.lo = vlo;

		return ( result.ld );
	}

	if ( uhi >= 0.0 )
	{
		if ( vhi >= 0.0 )
		{
			result.q.hi = uhi;
			result.q.lo = ulo;
		}
		else
		{
			result.q.hi = -uhi;
			result.q.lo = -ulo;
		}
	}
	else
	{
		if ( vhi >= 0.0 )
		{
			result.q.hi = -uhi;
			result.q.lo = -ulo;
		}
		else
		{
			result.q.hi = uhi;
			result.q.lo = ulo;
		}
	}

	return ( result.ld );
}

