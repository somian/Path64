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


/* Circular shift - The rightmost ic bits are shifted circularly k-places.
 * k > 0 => left shift.
 * k < 0 => right shift.
 * k = 0 => no shift.
 *	left shift			 right shift
 *	[  b1   |   k2   |   d   ]       [  b1   |   d   |   k2   ]
 */

int16 shftc_h(int16 *m,int16 *k,int16 *ic)
{
	int16 b1, b2, b3, k2, d, left;

	left = (*k > 0);
	k2 = left ? *k : -*k;
	if (k2 == 0 || k2 > NBSI || k2 > *ic || *ic < 1 || *ic > NBSI)
		return(*m);
	d = (*ic - k2);
	b1 = *m & ~F77mask[*ic];		/* mask off left most bits */

	if (left) {
		b2 = (*m & F77mask[d]) << k2;	/* get k2 bits */
		b3 = (*m & F77mask[*ic]) >> d;	/* get d bits  */
		}
	else {
		b2 = (*m & F77mask[k2]) << d;	/* get k2 bits */
		b3 = (*m & F77mask[*ic]) >> k2;	/* get d bits  */
		}
	return( b1 | b2 | b3 );
}
