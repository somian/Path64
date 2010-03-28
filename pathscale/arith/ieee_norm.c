/*

  Copyright (C) 2000, 2001 Silicon Graphics, Inc.  All Rights Reserved.

   Path64 is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

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


/*
 *	Common IEEE 754 normalization routines
 */

#include "arith.internal.h"


/* Normalization, rounding, and final processing common to all IEEE
 * arithmetic
 */

int
ar_i32norm (signed int expo,		/* exponent */
			unsigned long lbits,	/* left-hand bits */
			unsigned long rbits,	/* right-hand rounding bits */
			AR_IEEE_32 *x,			/* result to package; coeff and sign */
			int roundmode) {		/* IEEE rounding mode */

	int res = AR_STAT_OK;
	unsigned long carry;

	if (x->sign)
		res |= AR_STAT_NEGATIVE;

	/* Zero means zero */
	if (!(lbits | x->coeff0 | x->coeff1 | rbits)) {
		x->expo = 0;
		return res | AR_STAT_ZERO;
	}

	/* Normalize if necessary; preserve "sticky" rounding bit */
	while (expo > 1 && !lbits) {
		lbits = x->coeff0 >> (AR_IEEE32_C0_BITS - 1);
		SHLEFTIEEE32 (*x);
		x->coeff1 |= rbits >> (AR_IEEE32_ROUND_BITS - 1);
		rbits = (rbits << 1) & MASKR (AR_IEEE32_ROUND_BITS) | rbits & 1;
		expo--;
	}
	if (expo == 1 && !lbits)
		expo = 0;
	else if (!expo && lbits)
		expo = 1;

	/* Shift right if carry; keep the sticky bit */
	while (lbits > 1 || expo < 0) {
		rbits = (rbits >> 1) |
			rbits & 1 |
			((x->coeff1 & 1) << (AR_IEEE32_ROUND_BITS - 1));
		SHRIGHTIEEE32 (*x);
		x->coeff0 |= lbits << (AR_IEEE32_C0_BITS - 1);
		lbits >>= 1;
		expo++;
	}

	/* Rounding decision */
	switch (roundmode) {
	case AR_ROUND_PLUS_INFINITY:
		if (!rbits == !x->sign)
			goto noround;
		break;
	case AR_ROUND_MINUS_INFINITY:
		if (!rbits != !x->sign)
			goto noround;
		break;
	case AR_ROUND_ZERO:
		goto noround;
	default:
		if (!(rbits >> (AR_IEEE32_ROUND_BITS - 1)))
			goto noround;
		if (!(rbits & MASKR (AR_IEEE32_ROUND_BITS - 1)) &&
			!(x->coeff1 & 1))
			goto noround;
		break;
	}

	/* Round up the coefficient */
	carry = 1;
	INCIEEE32 (*x, carry);
	lbits += carry;

noround:

	/* Shift right if necessary */
	if (lbits > 1) {
		rbits = (rbits >> 1) |
			((x->coeff1 & 1) << (AR_IEEE32_ROUND_BITS - 1));
		SHRIGHTIEEE32 (*x);
		x->coeff0 |= lbits << (AR_IEEE32_C0_BITS - 1);
		lbits >>= 1;
		expo++;
	}
	else if (lbits) {
		if (!expo)
			expo = 1;
	}
	else
		expo = 0;

	if (!(lbits | x->coeff0 | x->coeff1)) {
		expo = 0;
		res |= AR_STAT_ZERO | AR_STAT_UNDERFLOW;
	}

	/* Test for out-of-range result; turn into (signed) infinity */
	if (expo > AR_IEEE32_MAX_EXPO) {
		x->expo = AR_IEEE32_MAX_EXPO + 1;
		x->coeff0 = x->coeff1 = 0;
		return res | AR_STAT_OVERFLOW;
	}

	/* Test for inexact */
	if (rbits) {
		res |= AR_STAT_INEXACT;
	}

	x->expo = expo;

	if (!expo &&
		ar_state_register.ar_underflow_mode != AR_UNDERFLOW_TO_DENORM) {
		/* don't want denorms; convert to zero */
		if (x->coeff0 | x->coeff1) {
			/* got a denorm */
			x->coeff0 = x->coeff1 = 0;
			if (ar_state_register.ar_underflow_mode ==
				AR_UNDERFLOW_TO_PLUS_ZERO)
				x->sign = 0;
			else if (ar_state_register.ar_underflow_mode ==
					 AR_UNDERFLOW_TO_SIGNED_TINY)
				x->expo = AR_IEEE64_MIN_EXPO+1;
			return res | AR_STAT_ZERO | AR_STAT_UNDERFLOW;
		}
	}

	return res;
}


int
ar_i64norm (signed int expo,		/* exponent */
			unsigned long lbits,	/* left-hand bits */
			unsigned long rbits,	/* right-hand rounding bits */
			AR_IEEE_64 *x,			/* result to package; coeff and sign */
			int roundmode) {		/* IEEE rounding mode */

	int res = AR_STAT_OK;
	unsigned long carry;

	if (x->sign)
		res |= AR_STAT_NEGATIVE;

	/* Zero means zero */
	if (!(lbits | x->coeff0 | x->coeff1 | x->coeff2 | x->coeff3 | rbits)) {
		x->expo = 0;
		return res | AR_STAT_ZERO;
	}

	/* Normalize if necessary; preserve "sticky" rounding bit */
	while (expo > 1 && !lbits) {
		lbits = x->coeff0 >> (AR_IEEE64_C0_BITS - 1);
		SHLEFTIEEE64 (*x);
		x->coeff3 |= rbits >> (AR_IEEE64_ROUND_BITS - 1);
		rbits = (rbits << 1) & MASKR (AR_IEEE64_ROUND_BITS) | rbits & 1;
		expo--;
	}
	if (expo == 1 && !lbits)
		expo = 0;
	else if (!expo && lbits)
		expo = 1;

	/* Shift right if carry; keep the sticky bit */
	while (lbits > 1 || expo < 0) {
		rbits = (rbits >> 1) |
			rbits & 1 |
			((x->coeff3 & 1) << (AR_IEEE64_ROUND_BITS - 1));
		SHRIGHTIEEE64 (*x);
		x->coeff0 |= lbits << (AR_IEEE64_C0_BITS - 1);
		lbits >>= 1;
		expo++;
	}

	/* Rounding decision */
	switch (roundmode) {
	case AR_ROUND_PLUS_INFINITY:
		if (!rbits == !x->sign)
			goto noround;
		break;
	case AR_ROUND_MINUS_INFINITY:
		if (!rbits != !x->sign)
			goto noround;
		break;
	case AR_ROUND_ZERO:
		goto noround;
	default:
		if (!(rbits >> (AR_IEEE64_ROUND_BITS - 1)))
			goto noround;
		if (!(rbits & MASKR (AR_IEEE64_ROUND_BITS - 1)) &&
			!(x->coeff3 & 1))
			goto noround;
		break;
	}

	/* Round up the coefficient */
	carry = 1;
	INCIEEE64 (*x, carry);
	lbits += carry;

noround:

	/* Shift right if necessary */
	if (lbits > 1) {
		rbits = (rbits >> 1) |
			((x->coeff3 & 1) << (AR_IEEE64_ROUND_BITS - 1));
		SHRIGHTIEEE64 (*x);
		x->coeff0 |= lbits << (AR_IEEE64_C0_BITS - 1);
		lbits >>= 1;
		expo++;
	}
	else if (lbits) {
		if (!expo)
			expo = 1;
	}
	else
		expo = 0;

	if (!(lbits | x->coeff0 | x->coeff1 | x->coeff2 | x->coeff3)) {
		expo = 0;
		res |= AR_STAT_ZERO | AR_STAT_UNDERFLOW;
	}

	/* Test for out-of-range result; turn into (signed) infinity */
	if (expo > AR_IEEE64_MAX_EXPO) {
		x->expo = AR_IEEE64_MAX_EXPO + 1;
		x->coeff0 = x->coeff1 = x->coeff2 = x->coeff3 = 0;
		return res | AR_STAT_OVERFLOW;
	}

	/* Test for inexact */
	if (rbits) {
		res |= AR_STAT_INEXACT;
	}

	x->expo = expo;

	if (!expo &&
		ar_state_register.ar_underflow_mode != AR_UNDERFLOW_TO_DENORM) {
		/* don't want denorms; convert to zero */
		if (x->coeff0 | x->coeff1 | x->coeff2 | x->coeff3) {
			/* got a denorm */
			x->coeff0 = x->coeff1 = x->coeff2 = x->coeff3 = 0;
			if (ar_state_register.ar_underflow_mode ==
				AR_UNDERFLOW_TO_PLUS_ZERO)
				x->sign = 0;
			else if (ar_state_register.ar_underflow_mode ==
					 AR_UNDERFLOW_TO_SIGNED_TINY)
				x->expo = AR_IEEE64_MIN_EXPO+1;
			return res | AR_STAT_ZERO | AR_STAT_UNDERFLOW;
		}
	}

	return res;
}


int
ar_i128norm(signed int expo,		/* exponent */
			unsigned long lbits,	/* left-hand bits */
			unsigned long rbits,	/* right-hand rounding bits */
			AR_IEEE_128 *x, 		/* result to package; coeff and sign */
			int roundmode) {		/* IEEE rounding mode */

	int res = AR_STAT_OK;
	unsigned long carry;

	if (x->sign)
		res |= AR_STAT_NEGATIVE;

	/* Zero means zero */
	if (!(lbits | x->coeff0 | x->coeff1 | x->coeff2 | x->coeff3 |
		  x->coeff4 | x->coeff5 | x->coeff6 | rbits)) {
		x->expo = 0;
		return res | AR_STAT_ZERO;
	}

	/* Normalize if necessary; preserve "sticky" rounding bit */
	while (expo > 1 && !lbits) {
		lbits = x->coeff0 >> (AR_IEEE128_C0_BITS - 1);
		SHLEFTIEEE128 (*x);
		x->coeff6 |= rbits >> (AR_IEEE128_ROUND_BITS - 1);
		rbits = (rbits << 1) & MASKR (AR_IEEE128_ROUND_BITS) | rbits & 1;
		expo--;
	}
	if (expo == 1 && !lbits)
		expo = 0;
	else if (!expo && lbits)
		expo = 1;

	/* Shift right if carry; keep the sticky bit */
	while (lbits > 1 || expo < 0) {
		rbits = (rbits >> 1) |
			rbits & 1 |
			((x->coeff6 & 1) << (AR_IEEE128_ROUND_BITS - 1));
		SHRIGHTIEEE128 (*x);
		x->coeff0 |= lbits << (AR_IEEE128_C0_BITS - 1);
		lbits >>= 1;
		expo++;
	}

	/* Rounding decision */
	switch (roundmode) {
	case AR_ROUND_PLUS_INFINITY:
		if (!rbits == !x->sign)
			goto noround;
		break;
	case AR_ROUND_MINUS_INFINITY:
		if (!rbits != !x->sign)
			goto noround;
		break;
	case AR_ROUND_ZERO:
		goto noround;
	default:
		if (!(rbits >> (AR_IEEE128_ROUND_BITS - 1)))
			goto noround;
		if (!(rbits & MASKR (AR_IEEE128_ROUND_BITS - 1)) &&
			!(x->coeff6 & 1))
			goto noround;
		break;
	}

	/* Round up the coefficient */
	carry = 1;
	INCIEEE128 (*x, carry);
	lbits += carry;

noround:

	/* Shift right if necessary */
	if (lbits > 1) {
		rbits = (rbits >> 1) |
			((x->coeff6 & 1) << (AR_IEEE128_ROUND_BITS - 1));
		SHRIGHTIEEE128 (*x);
		x->coeff0 |= lbits << (AR_IEEE128_C0_BITS - 1);
		lbits >>= 1;
		expo++;
	}
	else if (lbits) {
		if (!expo)
			expo = 1;
	}
	else
		expo = 0;

	if (!(lbits | x->coeff0 | x->coeff1 | x->coeff2 | x->coeff3 |
		  x->coeff4 | x->coeff5 | x->coeff6)) {
		expo = 0;
		res |= AR_STAT_ZERO | AR_STAT_UNDERFLOW;
	}

	/* Test for out-of-range result; turn into (signed) infinity */
	if (expo > AR_IEEE128_MAX_EXPO) {
		x->expo = AR_IEEE128_MAX_EXPO + 1;
		x->coeff0 = x->coeff1 = x->coeff2 = x->coeff3 =
			x->coeff4 = x->coeff5 = x->coeff6 = 0;
		return res | AR_STAT_OVERFLOW;
	}

	/* Test for inexact */
	if (rbits) {
		res |= AR_STAT_INEXACT;
	}

	x->expo = expo;

	if (!expo &&
		ar_state_register.ar_underflow_mode != AR_UNDERFLOW_TO_DENORM) {
		/* don't want denorms; convert to zero */
		if (x->coeff0 | x->coeff1 | x->coeff2 | x->coeff3 |
			x->coeff4 | x->coeff5 | x->coeff6) {
			/* got a denorm */
			x->coeff0 = x->coeff1 = x->coeff2 = x->coeff3 =
				x->coeff4 = x->coeff5 = x->coeff6 = 0;
			if (ar_state_register.ar_underflow_mode ==
				AR_UNDERFLOW_TO_PLUS_ZERO)
				x->sign = 0;
			else if (ar_state_register.ar_underflow_mode ==
					 AR_UNDERFLOW_TO_SIGNED_TINY)
				x->expo = AR_IEEE128_MIN_EXPO+1;
			return res | AR_STAT_ZERO | AR_STAT_UNDERFLOW;
		}
	}

	return res;
}


static char USMID [] = "\n%Z%%M%	%I%	%G% %U%\n";
static char rcsid [] = "$Id$";

