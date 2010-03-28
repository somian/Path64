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


#pragma ident "@(#) libu/util/bto.c	92.1	07/07/99 13:18:33"

#include <fortran.h>

#define	BLANK	(short) (' ')
#define	ZERO	(short) ('0')

#pragma _CRI duplicate BTO as _BTO

_f_int
BTO(_f_int *word)
{
	register long	result, value;
	register short	ch, i;

	value	= *word & 077777777;

	result	= (value & 07) + ZERO;
	value	= value >> 3;

	for (i = 1; i < 8; i++) {

		if (value == 0)
			ch	= BLANK;
		else {
			ch	= (value & 07) + ZERO;
			value	= value >> 3;
		}

		result	= result | (ch << (i << 3));

	}

	return (result);
}

#pragma _CRI duplicate BTOL as _BTOL

_f_int
BTOL(_f_int *word)
{
	register long	result, value;
	register short	ch, i, shift;

	value	= *word & 077777777;

	result	= (value & 07) + ZERO;
	value	= value >> 3;
	shift	= 8;

	while (value != 0) {

		ch	= (value & 07) + ZERO;
		value	= value >> 3;
		result	= result | (ch << shift);
		shift	= shift + 8;

	}

	result	= result << (64 - shift);

	return (result);
}

#pragma _CRI duplicate BTOR as _BTOR

_f_int
BTOR(_f_int *word)
{
	register long	result, value;
	register short	ch, i, shift;

	value	= *word & 077777777;

	result	= (value & 07) + ZERO;
	value	= value >> 3;
	shift	= 0;

	while (value != 0) {

		ch	= (value & 07) + ZERO;
		value	= value >> 3;
		shift	= shift + 8;
		result	= result | (ch << shift);

	}

	return (result);
}
