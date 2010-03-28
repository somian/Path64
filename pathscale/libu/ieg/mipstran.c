/*
 * Copyright 2004, 2005, 2006 PathScale, Inc.  All Rights Reserved.
 */

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


#pragma ident "@(#) libu/ieg/mipstran.c	92.1	06/25/99 14:37:13"

#include <fortran.h>

#define	QIEEEMX	32767	/* IEEE quad maximum exponent  (077777)	*/
#define	QIEEEBS	16383	/* IEEE quad exponent bias     (037777)	*/
#define	DIEEEMX	2047	/* IEEE double maximum exponent (03777)	*/
#define	DIEEEBS	1023	/* IEEE double exponent bias    (01777)	*/

#define	NANMAN	0x3C4E614E3E	/* Nan Mantissa */

#if _MIPS_SZINT == 32
#define	int64	long long
#define	uint64	unsigned long long
#define	int32	int
#define	uint32	unsigned int
#define	ONE	1LL
#ifdef	_LD64
#define	FLOAT128	_f_real16
#else
typedef struct { double _Dreal, _Dimag; } FLOAT128;
#endif
#else
#define	int64	long
#define	uint64	unsigned long
#define	int32	short
#define	uint32	unsigned short
#define	ONE	1L
#define	FLOAT128	_f_real16
#endif

/* Configure IEEE exception detection and handling */

#define INEXACT_ENABLE		0	/* Enable detection of INEXACT */
#define INVALID_ENABLE		0	/* Enable detection of INVALID */
#define OVERFLOW_ENABLE		0	/* Enable detection of OVERFLOW */
#define UNDERFLOW_ENABLE	0	/* Enable detection of UNDERFLOW */

/* IEEE exception setting */

#define	set_ieee_inexact()
#define	set_ieee_invalid()
#define	set_ieee_overflow()
#define	set_ieee_underflow()

/*
 *	MIPS <-> IEEE floating-point conversion routines
 *
 *	IEM128I$	128-bit IEEE to 128-bit MIPS
 *
 *	IEM128O$	128-bit MIPS to 128-bit IEEE
 */

union Q_box {		/* Container for 128-bit data */
	uint64		qint[2];
	FLOAT128	qflt;
};

/*
 *	IEM128I$ Translate 128-bit IEEE floating-point to 128-bit MIPS
 *		floating-point
 */

FLOAT128
#ifdef	_CRAY
IEM128I$(union Q_box *datum)
#else
iem128i$_(union Q_box *datum)
#endif
{
	register int64	exp1;	/* Upper exponent */
	register int64	exp2;	/* Lower exponent */
	register uint64 man1;	/* First part of mantissa */
	register uint64 man2;	/* Second part of mantissa */
	register uint64 sign;	/* Sign bit */
	register uint64 temp;	/* Temporary */
	union Q_box	r;	/* Result */

	temp	= (*datum).qint[0];
	man2	= (*datum).qint[1];
	sign	= (ONE << 63) & temp;
	man1	= ((temp << 16) >> 12) | (man2 >> 60);
	exp1	= (temp >> 48) & QIEEEMX;

	if (exp1 == QIEEEMX) {
		exp1	= DIEEEMX;

		if ((man1 | man2) != 0 && man1 == 0)
			man1	= 1;	/* Ensure NaN */

		man2	= 0;
		exp2	= 0;
	}
	else
		if (exp1 == 0) {
			man1	= 0;
			man2	= 0;
			exp2	= 0;
		}
		else {
			exp1	= exp1 + (DIEEEBS - QIEEEBS);
			exp2	= exp1 - 53;
			man2	= man2 << 4;

			if (man2 == 0 || exp2 <= 0) {
				exp2	= 0;
				man2	= 0;
			}
			else {
				while ((man2 >> 63) == 0) {
					man2	= man2 << 1;
					exp2	= exp2 - 1;
				}

				man2	= (man2 << 1) >> 12;

				if (exp2 <= 0) {
					exp2	= 0;
					man2	= 0;
				}
			}
		}

	exp1	= exp1 << 52;
	exp2	= exp2 << 52;

	r.qint[0]	= sign | (exp1 + man1);
	r.qint[1]	= sign | (exp2 + man2);

	return (r.qflt);
}

/*
 *	IEM128O$ Translate 128-bit MIPS floating-point to 128-bit IEEE
 *		floating-point
 */

FLOAT128
#ifdef	_CRAY
IEM128O$(union Q_box *datum)
#else
iem128o$_(union Q_box *datum)
#endif
{
	register int64	exp1;	/* Upper exponent */
	register int64	exp2;	/* Lower exponent */
	register uint64 man1;	/* First part of mantissa */
	register uint64 man2;	/* Second part of mantissa */
	register uint64 sign;	/* Sign bit */
	register uint64 temp;	/* Temporary */
	union Q_box	r;	/* Result */

	temp	= (*datum).qint[0];
	man2	= (*datum).qint[1];
	sign	= (ONE << 63) & temp;
	man1	= temp << 12;
	exp1	= (temp >> 52) & DIEEEMX;

	if (exp1 == DIEEEMX) {
		exp1	= QIEEEMX;
		man2	= man2 << 12;
	}
	else {
		register short	j;

		if (exp1 != 0) {
			exp2	= (man2 >> 52) & DIEEEMX;

			if (exp2 == 0)
				man2	= 0;
			else {
				man2	= ((man2 << 12) >> 1) | (ONE << 63);
				j	= (exp1 - exp2) - 53;

				if (j > 0)
					man2	= man2 >> j;
			}

			exp1	= exp1 + (QIEEEBS - DIEEEBS);
		}
		else {
			if (man1 == 0)
				man2	= 0;
			else {	/* If denorm input */
				exp1	= QIEEEBS - DIEEEBS;

				while (man1 > 0) {
					man1	= man1 << 1;
					exp1	= exp1 - 1;
				}
			}
		}
	}

	man2	= (man2 >> 4) | (((man1 >> 12) & 0xF) << 60);
	man1	= man1 >> 16;
	exp1	= exp1 << 48;

	r.qint[0]	= sign | (exp1 + man1);
	r.qint[1]	= man2;

	return (r.qflt);
}
