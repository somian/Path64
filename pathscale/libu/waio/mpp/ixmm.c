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


#pragma ident "@(#) libu/waio/mpp/ixmm.c	92.1	06/23/99 13:55:46"

/*
 *	IXMM, IXMM@ - search a masked array.
 *
 *	Call from FORTRAN
 *
 *      	IX = IXMM(VEC,LV,MASK,VAL)
 *
 *		where:	VEC  =  array
 *			LV   =  array length
 *			MASK =  mask to apply to array elements
 *			VAL  =  test value
 *
 *	Return value
 *
 *		One-based index into VEC of the first location where
 *
 *			(AND(VEC(IX), MASK)) .EQ. VAL
 *
 *		Returns 0 if no match found.
 */

int
IXMM@(int *vec, int *lv, int *mask, int *val)
{
	int index;
	int length, msk, target;
	
	length = *lv;
	msk    = *mask;
	target = *val;

	for (index=0; index<length; index++) {
		if ((vec[index] & msk) == target)
			return (index + 1);
	}
	return (0);
}
