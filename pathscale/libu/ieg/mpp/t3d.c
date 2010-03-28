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


#pragma ident "@(#) libu/ieg/mpp/t3d.c	92.1	06/25/99 14:59:36"

#define	CRAYMX	24576	/* CRAY maximum exponent (060000)	*/
#define	CRAYBS	16384	/* CRAY exponent bias (040000)		*/

#define	DIEEEMX	2047	/* IEEE 64-bit maximum exponent (0377)	*/
#define	DIEEEBS	1023	/* IEEE 32-bit exponent bias (0177)	*/
#define	SIEEEMX	255	/* IEEE 32-bit maximum exponent (0377)	*/
#define	SIEEEBS	127	/* IEEE 32-bit exponent bias (0177)	*/

#include <cray/portdefs.h>

#ifdef __mips
#define OT3D __ot3d
#else
#define OT3D O@T3D
#endif
unsigned long	OT3D;


/*
 *	_ITH64O	Translate CRAY REAL (64-bit) to T3D 32-bit
 */

void
_ITH64O(uint64 datum, int32 *sp)
{
	uint64  	man, sign;
	int64		exp;
	int32 		noflow;

	sign	= (datum >> 63) << 31;
	man	= (datum << 16) >> 39;
	exp	= (datum <<  1) >> 49;
	noflow	= (exp < CRAYMX);
	exp	= exp + (SIEEEBS - CRAYBS - 2);

	if (exp >= SIEEEMX - 1) {
		man	= 0;
		exp	= SIEEEMX << 23;

		if (noflow)
			OT3D	= OT3D + 1;
	}
	else
		if (exp < 0) {
			man	= 0;
			exp	= 0;
		}
		else {		/* Round mantissa and position exponent */
			man	= (man + 1) >> 1;
			exp	= exp << 23;
		}

	*sp	= (int32) (sign | (exp + man));

	return;
}

/*
 *	_ITF64O	Translate CRAY REAL (64-bit) to T3D 64-bit
 */

void
_ITF64O(uint64 datum, int64 *lp)
{
	uint64 		man, sign;
	int64 		exp;
	int32 		noflow;

	sign	= (datum >> 63) << 63;
	man	= (datum << 16) >> 11;
	exp	= (datum <<  1) >> 49;
	noflow	= (exp < CRAYMX);
	exp	= exp + (DIEEEBS - CRAYBS - 2);

	if (exp >= DIEEEMX - 1) {
		man	= 0;
		exp	= (int64)DIEEEMX << 52;

		if (noflow)
			OT3D	= OT3D + 1;
	}
	else
		if (exp < 0) {
			man	= 0;
			exp	= 0;
		}
		else
			exp	= exp << 52;

	*lp	= sign | (exp + man);

	return;
}

/*
 *	_ITF128O	Translate CRAY DOUBLE PRECISION (128-bit) to T3D 64-bit
 */

void
_ITF128O(uint64 datum1, uint64 datum2, int64 *lp)
{
	uint64 		man1, man2, sign;
	int64 		exp;
	int32 		noflow;

	man2	= (datum2 << 16) >> 58;
	sign	= (datum1 >> 63) << 63;
	man1	= (datum1 << 16) >> 10;
	exp	= (datum1 <<  1) >> 49;
	noflow	= (exp < CRAYMX);
	exp	= exp + (DIEEEBS - CRAYBS - 2);

	if (exp >= DIEEEMX - 1) {
		man1	= 0;
		exp	= (int64)DIEEEMX << 52;

		if (noflow)
			OT3D	= OT3D + 1;
	}
	else
		if (exp < 0) {
			man1	= 0;
			exp	= 0;
		}
		else {		/* Round mantissa and position exponent */
			man1	= ((man1 | man2) + 1) >> 1;
			exp	= exp << 52;
		}

	*lp	= sign | (exp + man1);

	return;
}

/*
 *	_ITSC128O	Translate CRAY COMPLEX (2x64-bit) to IEEE 2x32-bit
 */
void
_ITSC128O(uint64 datum1, uint64 datum2, int32 *lp)
{
	uint64 		man1, man2, sign1, sign2;
	int64 		exp1, exp2;
	int32 		noflow1, noflow2;

	sign2 	= (datum2 >> 63) << 31;
	sign1 	= (datum1 >> 63) << 31;
	man2	= (datum2 << 16) >> 39;
	man1	= (datum1 << 16) >> 39;
	exp2	= (datum2 <<  1) >> 49;
	exp1	= (datum1 <<  1) >> 49;
	noflow1	= (exp1 < CRAYMX);
	noflow2	= (exp2 < CRAYMX);
	exp1	= exp1 + (SIEEEBS - CRAYBS - 2);
	exp2	= exp2 + (SIEEEBS - CRAYBS - 2);

	if (exp1 >= SIEEEMX - 1) {
		man1	= 0;
		exp1	= SIEEEMX << 23;

		if (noflow1)
			OT3D	= OT3D + 1;
	}
	else
		if (exp1 <= 0) {
			man1	= 0;
			exp1	= 0;
		}
		else {		/* Round mantissa and position exponent */
			man1	= (man1 + 1) >> 1 ;
			exp1	= exp1 << 23;
		}

	if (exp2 >= SIEEEMX - 1) {
		man2	= 0;
		exp2	= SIEEEMX << 23;

		if (noflow2)
			OT3D	= OT3D + 1;
	}
	else
		if (exp2 <= 0) {
			man2	= 0;
			exp2	= 0;
		}
		else {		/* Round mantissa and position exponent */
			man2	= (man2 + 1) >> 1;
			exp2	= exp2 << 23;
		}


	*lp	= sign1 | (exp1 + man1);
	*(lp+1) = sign2 | (exp2 + man2);

	return;
}
