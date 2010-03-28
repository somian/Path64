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


#pragma ident "@(#) libu/util/dtb.c	92.1	07/07/99 13:18:33"

#include <fortran.h>

#define	ZERO	(short) ('0')
#define	NINE	(short) ('9')

#pragma _CRI duplicate DTB as _DTB

_f_int
DTB(_f_int *word, _f_int *flag)
{
	register long	error,  result;
	register unsigned long mask, value;
	register short	ch, shift;

	value	= *word;
	mask	= 0xFF00000000000000;
	error	= 0;
	result	= 0;
	shift	= 64 - 8;

	do {
		ch	= (short) ((mask & value) >> shift);

		if (ch < ZERO || ch > NINE) {
			error	= -1;
			goto done;
		}

		result	= (result * 10) + (long) (ch - ZERO);
		value	= value & ~mask;
		shift	= shift - 8;
		mask	= mask >> 8;

	} while (value != 0);

done:

	if (error != 0)
		if (_numargs() > 1)
			*flag	= error;
		else
			abort();	/* TEMPORARY */

	return (result);
}
