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


/* $Header$ */
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	actual or intended publication of such source code.	*/

#include "bit.h"
#include "cmplrs/host.h"

/* Circular shift - The right most ic bits are shifted circularly k-places.
 * k > 0 => left shift.
 * k < 0 => right shift.
 * k = 0 => no shift.
 *	left shift			 right shift
 *	[  b1   |   k2   |   d   ]       [  b1   |   d   |   k2   ]
 */

int64 shftc_ll(int64 *m,int64 *k,int64 *ic)
{
	int64 b1, b2, b3, k2, d;
	int32 left;

	left = (*k > 0);
	k2 = left ? *k : -*k;
	if (k2 == 0 || k2 > NBLL || k2 > *ic || *ic < 1 || *ic > NBLL)
		return(*m);
	d = (*ic - k2);
	b1 = *m & ~F77llmask[*ic];		/* mask off left most bits */

	if (left) {
		b2 = ( (unsigned)(*m & F77llmask[  d]) ) << k2; /* get  d bits */
		b3 = ( (unsigned)(*m & F77llmask[*ic]) ) >> d;  /* get k2 bits */
		}
	else {
		b2 = ( (unsigned)(*m & F77llmask[ k2]) ) <<   d;/* get k2 bits */
		b3 = ( (unsigned)(*m & F77llmask[*ic]) ) >>  k2;/* get d  bits */
		}
	return( b1 | b2 | b3 );
}
