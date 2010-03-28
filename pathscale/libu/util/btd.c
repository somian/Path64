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


#pragma ident "@(#) libu/util/btd.c	92.1	07/07/99 13:18:33"

#include <fortran.h>

#define	BLANK	(short) (' ')
#define	ZERO	(short) ('0')

#pragma _CRI duplicate BTD as _BTD

_f_int
BTD(_f_int *word)
{
	register long	result, value;
	register short	ch, i;

	value	= *word;

	if (value > 99999999 || value < 0)
		result	= 0x2A2A2A2A2A2A2A2A;	/* 8H******** */
	else {

		result	= (value % 10) + ZERO;
		value	= value / 10;

		for (i = 1; i < 8; i++) {

			if (value == 0)
				ch	= BLANK;
			else {
				ch	= (value % 10) + ZERO;
				value	= value / 10;
			}

			result	= result | (ch << (i << 3));

		}
	}

	return (result);
}

#pragma _CRI duplicate BTDL as _BTDL

_f_int
BTDL(_f_int *word)
{
	register long	result, value;
	register short	ch, i, shift;

	value	= *word;

	if (value > 99999999 || value < 0)
		result	= 0x2A2A2A2A2A2A2A2A;	/* 8H******** */
	else {

		result	= (value % 10) + ZERO;
		value	= value / 10;
		shift	= 8;

		while (value != 0) {

			ch	= (value % 10) + ZERO;
			value	= value / 10;
			result	= result | (ch << shift);
			shift	= shift + 8;

		}

		result	= result << (64 - shift);
	}

	return (result);
}

#pragma _CRI duplicate BTDR as _BTDR

_f_int
BTDR(_f_int *word)
{
	register long	result, value;
	register short	ch, i, shift;

	value	= *word;

	if (value > 99999999 || value < 0)
		result	= 0x2A2A2A2A2A2A2A2A;	/* 8H******** */
	else {

		result	= (value % 10) + ZERO;
		value	= value / 10;
		shift	= 0;

		while (value != 0) {

			ch	= (value % 10) + ZERO;
			value	= value / 10;
			shift	= shift + 8;
			result	= result | (ch << shift);

		}
	}

	return (result);
}
