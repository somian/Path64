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

extern	long double __qdim(double, double, double, double);

	/* intrinsic QDIM */

	/* by address version */

	/* computes the postive difference of two long doubles */

long double
__q_dim( u, v )
long double *u, *v;
{
ldquad	x, y;

	x.ld = *u;
	y.ld = *v;

	return ( __qdim(x.q.hi, x.q.lo, y.q.hi, y.q.lo) );
}

