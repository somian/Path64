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


#pragma ident "@(#) libu/ibm/mpp/usccti.c	92.1	06/25/99 17:25:50"
#include <fortran.h>
#include <stdlib.h>
#include <cray/portdefs.h>

extern uint64	_ASCII_TO_EBCDIC[16];
extern uint64	_ASCII_TO_EBCDIC_UC[16];

/*
 *	USCCTI	Fortran-callable routine to convert ASCII character
 *		data to IBM EBCDIC character data.
 *
 *	CALL USCCTI (SRC, DEST, ISB, NUM, NPW <,VAL>)
 *
 *	SRC	Variable or array of any type (except CHARACTER)
 *		containing the character data to be converted.
 *
 *	DEST	Variable or array of any type (except CHARACTER)
 *		to receive the converted character data.
 *
 *	ISB	Integer variable or constant containing the starting
 *		byte number within DEST.  Bytes are numbered from
 *		left to right with the leftmost byte as byte 1 and
 *		the rightmost byte as byte 8.
 *
 *	NUM	Integer variable or constant containing the number
 *		of characters to be converted.
 *
 *	NPW	Integer variable or constant containing the number of
 *		characters per word in the input array.  If NPW is
 *		positive (1 to 8), each SRC element is assumed to
 *		contain NPW characters per word, left-justified.  If
 *		NPW is negative (-1 to -8), each SRC element is
 *		assumed to contain -NPW characters per word, right-
 *		justified.
 *
 *	VAL	(Optional parameter) Integer variable or constant
 *		which--if specified and nonzero (.TRUE.)--indicates
 *		that all lower case input is to be translated to upper
 *		case output.
 *
 *	Note that the following conditions must be met for any character
 *	data to be converted:
 *		NUM >= 0
 *		ISB > 0
 *		0 < |NPW| < 9
 */

#if	defined(__mips)
_f_int8
usccti_(
#else
_f_int8
USCCTI(
#endif
	_f_int8	*src,
	_f_int8	*dest,
	_f_int8	*isb,
	_f_int8	*num,
	_f_int8	*npw,
	_f_int8	*val)
{
	register short	bytes;
	register short	ishft;
	register short	lrshft;
	register short	oshft;
	register int64	count;
	register uint64	input;
	register uint64	output;
	uint64		*in;
	uint64		*out;
	uint64		*table;

	table	= _ASCII_TO_EBCDIC;

#ifdef	_CRAY
	if (_numargs() < 5)
		return((_f_int8) -1);
	else
		if (_numargs() > 5)
#endif
			if (val != NULL && *val != 0)
				table	= _ASCII_TO_EBCDIC_UC;

	count	= *num;

	if (*npw == 0 || *npw > 8 || *npw < -8 || *isb < 1 || count < 0)
		return((_f_int8) -1);

	if (count == 0)
		return((_f_int8) 0);

	if (*npw < 0) {
		bytes	= (8 + *npw) << 3;
		lrshft	= bytes;
	}
	else {
		bytes	= (8 - *npw) << 3;
		lrshft	= 0;
	}

	oshft	= *isb - 1;
	out	= (uint64 *) dest + (oshft >> 3);
	output	= *out;
	in	= (uint64 *) src;
	input	= *in++ << lrshft;
	ishft	= 64;
	oshft	= (8 - (oshft & 07)) << 3;
	output	= output >> oshft;

	do {
		register short	ch;

		ishft	= ishft - 8;
		ch	= (input >> ishft) & 0177;
		ch	= (table[ch >> 3] >> ((ch & 07) << 3)) & 0377;
		output	= (output << 8) | ch;
		count	= count - 1;

		if (ishft == bytes) {
			input	= *in++ << lrshft;
			ishft	= 64;
		}

		oshft	= oshft - 8;

		if (oshft == 0) {
			*out++	= output;
			oshft	= 64;
		}

	} while (count > 0);

	if (oshft != 64)	/* If partial word available */
		*out	= (output << oshft) | (*out & ~(~0 << oshft));

	return((_f_int8) 0);
}
