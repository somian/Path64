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


/* USMID @(#) libfi/mathlb/leadz.h	92.0	10/08/98 14:37:14 */


#include <cray/portdefs.h>
/* functions in this file are for inline use only.  They are NOT user
 * callable as declared in this file.
 */
#ifndef LEADZ_H
#define LEADZ_H

#define BIC(a, b) (a & (~ b))

/******************************   _my_leadz4 and _my_leadz8
 * Emulate CRI 64-bit leadz for 32-bit values and 64-bit values
 * # compute leading zeros of value in R01
 * # result is in R03
 * # uses registers R01-R04
 ************************************/
static int32
_leadz4(uint32 r01)
{
	uint32 r02 = 0, r03 = 0, r04 = 0;
	r02 = r01 >> 16;
	if (r02 > 0)
		r01 = r02;
	else /* if (r02 == 0) */
		r04 = 16;
	r02 = r01 >> 8;
	r03 = r04;
	if (r02 > 0)
		r01 = r02;
	else /* if (r02 == 0) */
	r04 = 8;
	r02 = r01 >> 4;
	r03 |= r04;
	if (r02 > 0)
		r01 = r02;
	else /*  if (r02 == 0) */
		r04 = 4;
	r02 = r01 >> 2;
	r03 |= r04;
	if (r02 > 0)
		r01 = r02;
	else /* if (r02 == 0) */
		r04 = 2;
	r02 = r01 < 2 ? 1 : 0;
	r01 = r01 < 1 ? 1 : 0;
	r03 |= r04;
	return r03 + r02 + r01;
}

static int64
_leadz8(uint64 r01)
{
	uint64  r02 = 0, r03 = 0, r04 = 0;
	r02 = r01 >> 32;
	if (r02 > 0)
		r01 = r02;
	else if (r02 == 0)
		r04 = 32;
	r02 = r01 >> 16;
	r03 = r04;
	if (r02 > 0)
		r01 = r02;
	else if (r02 == 0)
		r04 = 16;
	r02 = r01 >> 8;
	r03 |= r04;
	if (r02 > 0)
		r01 = r02;
	else if (r02 == 0)
		r04 = 8;
	r02 = r01 >> 4;
	r03 |= r04;
	if (r02 > 0)
		r01 = r02;
	else if (r02 == 0)
		r04 = 4;
	r02 = r01 >> 2;
	r03 |= r04;
	if (r02 > 0)
		r01 = r02;
	else if (r02 == 0)
		r04 = 2;
	r02 = r01 < 2 ? 1 : 0;
	r01 = r01 < 1 ? 1 : 0;
	r03 |= r04;
	return r03 + r02 + r01;
}
#endif                                 /* !LEADZ_H */
