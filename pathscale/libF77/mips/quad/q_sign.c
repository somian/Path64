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

extern	long double	__qsign(double, double, double, double);

	/* intrinsic QSIGN */

	/* by address version */

	/*	computes (sign of v)*|u|	*/

long double
__q_sign(long double *u, long double *v)
{
ldquad	x, y;

#include "msg.h"

	x.ld = *u;
	y.ld = *v;

	return (__qsign(x.q.hi, x.q.lo, y.q.hi, y.q.lo));
}
