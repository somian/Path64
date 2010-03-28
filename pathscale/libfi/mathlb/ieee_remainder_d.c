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


#pragma ident "@(#) libfi/mathlb/ieee_remainder_d.c	92.1	07/09/99 11:00:36"

#include <fortran.h>
#include <fp.h>
#include "inline.h"

/* use 55 bits for the first half and 57 bits for second half */
#define IE128_64_MANT_BTS_P1	16
#define IE128_64_MANT_BTS_P2	32
#define IE128_64_MANT_BTS_P3	 7
#define IE128_64_MANT_BTS_P4	25
#define IE128_64_MANT_BTS_P5	32


extern _f_real16 _IEEE_REMAINDER_H_D(_f_real4 x, _f_real16 y);
extern _f_real16 _IEEE_REMAINDER_R_D(_f_real8 x, _f_real16 y);
extern _f_real16 _IEEE_REMAINDER_D_D(_f_real16 x, _f_real16 y);
extern _f_real16 _IEEE_REMAINDER_D_H(_f_real16 x, _f_real4 y);
extern _f_real16 _IEEE_REMAINDER_D_R(_f_real16 x, _f_real8 y);

static _f_real8 _raisinvld(_f_real8 x, _f_real8 y);

static _f_real8 _raisinvld(_f_real8 x, _f_real8 y)
{
        return (x/y);
}

#define CALC_DINT()							\
	tmpb = tmptdiva.ldword + two_112.ldword;			\
	tmpb -= two_112.ldword;						\
	if (tmpb > tmptdiva.ldword)					\
		tmpb -= (_f_real16) 1.0;				\
	tmptdiva.ldword = tmpb;

/*
 * _IEEE_REMAINDER_D_D(X,Y) - calculate remainder of two real(16)
 *                            arguments.
 *
 *              The standard definition for real is:
 *                 If y = 0, remainder(x,y) = NaN and raise the INVALID
 *                           exception.
 *                 else if x = INFINITY, remainder(x,y) = NaN and raise
 *                           the INVALID exception.
 *                 else remainder(x,y) = x - (REAL(NINT(x/y))) * y
 *
 *              The algorithm for real is:
 *                x - (REAL(NINT(x/y))) * y.
 *
 * Use the following algorithm for x/y rounded quantity:
 *   1.  x/y
 *   2. if(fraction(x/y) = .5 exactly, round to next EVEN number.
 *           0.5 = 0.0, 1.5 + 2.0, 10.5 = 10, etc.
 * Use the following algorithm for the multiply:
 *   ((((x - tdivU *yU) - tdivU * yL) -tdivL * yU) - tdivL * yL)
 */

_f_real16
_IEEE_REMAINDER_D_D(_f_real16 argx, _f_real16 argy)
{
	/* Union defined to work with IEEE 64-bit floating point. */
	union _ieee_ldouble {
		struct { unsigned int sign	: 1;
			 unsigned int exponent	: IEEE_128_EXPO_BITS;
			 unsigned int mantissa_up1 : IEEE_128_MANT_BTS_UP1;
			 unsigned int mantissa_up2 : IEEE_128_MANT_BTS_UP2;
			 unsigned int mantissa_lo1 : IEEE_128_MANT_BTS_LO1;
			 unsigned int mantissa_lo2 : IEEE_128_MANT_BTS_LO2;
		} parts1;
		_f_real16		ldword;
		unsigned long long	llword[2];
	};

	union _ieee_ldouble x_val, y_val;
	_f_real16 __remainderd();

	x_val.ldword	= argx;
	y_val.ldword	= argy;

	/* check input values: x for infinity and y for zero. */
	if ((((x_val.llword[0] & ~IEEE_128_64_SIGN_BIT) ==
		IEEE_128_64_EXPO) && x_val.llword[1] == 0) ||
	   ((!(isnan128(y_val.ldword)) && (y_val.ldword == 0.0)))) {
		union _ieee_ldouble x_val;
		_f_real8	result8;
		_f_real8	arg8 = 0.0;

		x_val.ldword	= _DBL_NaN;

		/* need to emit invalid exception */
		result8		= _raisinvld(arg8,arg8);
		return(x_val.ldword);
	}
	return(__remainderd(x_val, y_val));
}

_f_real16
_IEEE_REMAINDER_D_H(_f_real16 argx, _f_real4 argy)
{
	/* Union defined to work with IEEE 64-bit floating point. */
	union _ieee_ldouble {
		struct { unsigned int sign	: 1;
			 unsigned int exponent	: IEEE_64_EXPO_BITS;
			 unsigned int mantissa_up1 : IEEE_128_MANT_BTS_UP1;
			 unsigned int mantissa_up2 : IEEE_128_MANT_BTS_UP2;
			 unsigned int mantissa_lo1 : IEEE_128_MANT_BTS_LO1;
			 unsigned int mantissa_lo2 : IEEE_128_MANT_BTS_LO2;
		} parts1;
		_f_real16		ldword;
		unsigned long long	llword[2];
	};

	/* Union defined to work with IEEE 32-bit floating point. */
	union _ieee_fdouble {
		_f_real4	fpword;
		unsigned int	l4word;
	};

	union _ieee_ldouble x_val, y_val;
	union _ieee_fdouble y4_val;
	_f_real16 __remainderd();

	x_val.ldword	= argx;
	y4_val.fpword	= argy;

	/* check input values: x for infinity and y for zero. */
	if ((((x_val.llword[0] & ~IEEE_128_64_SIGN_BIT) ==
	    IEEE_128_64_EXPO) && x_val.llword[1] == 0) ||
	   ((!(isnan32(y4_val.fpword)) && (y4_val.fpword == 0.0)))) {
		union _ieee_ldouble x_val;
		_f_real8	result8;
		_f_real8	arg8 = 0.0;

		x_val.ldword	= _DBL_NaN;

		/* need to emit invalid exception */
		result8		= _raisinvld(arg8,arg8);
		return(x_val.ldword);
	}
	y_val.ldword	= (_f_real16) y4_val.fpword;
	return(__remainderd(x_val, y_val));
}

_f_real16
_IEEE_REMAINDER_D_R(_f_real16 argx, _f_real8 argy)
{
	/* Union defined to work with IEEE 64-bit floating point. */
	union _ieee_ldouble {
		struct { unsigned int sign	: 1;
			 unsigned int exponent	: IEEE_64_EXPO_BITS;
			 unsigned int mantissa_up1 : IEEE_128_MANT_BTS_UP1;
			 unsigned int mantissa_up2 : IEEE_128_MANT_BTS_UP2;
			 unsigned int mantissa_lo1 : IEEE_128_MANT_BTS_LO1;
			 unsigned int mantissa_lo2 : IEEE_128_MANT_BTS_LO2;
		} parts1;
		_f_real16		ldword;
		unsigned long long	llword[2];
	};

	/* Union defined to work with IEEE 64-bit floating point. */
	union _ieee_double {
		_f_real8		dword;
		unsigned long long	int8word;
		unsigned int		l8word[2];
	};

	union _ieee_ldouble x_val, y_val;
	union _ieee_double y8_val;
	_f_real16 __remainderd();

	x_val.ldword	= argx;
	y8_val.dword	= argy;

	/* check input values */
	if ((((x_val.llword[0] & ~IEEE_128_64_SIGN_BIT) ==
	    IEEE_128_64_EXPO) && x_val.llword[1] == 0) ||
	   ((!(isnan64(y8_val.dword)) && (y8_val.dword == 0.0)))) {
		union _ieee_ldouble x_val;
		_f_real8	result8;
		_f_real8	arg8 = 0.0;

		x_val.ldword	= _DBL_NaN;

		/* need to emit invalid exception */
		result8		= _raisinvld(arg8,arg8);
		return(x_val.ldword);
	}
	y_val.ldword	= (_f_real16) y8_val.dword;
	return(__remainderd(x_val, y_val));
}

_f_real16
_IEEE_REMAINDER_H_D(_f_real4 argx, _f_real16 argy)
{
	/* Union defined to work with IEEE 64-bit floating point. */
	union _ieee_ldouble {
		struct { unsigned int sign	: 1;
			 unsigned int exponent	: IEEE_128_EXPO_BITS;
			 unsigned int mantissa_up1 : IEEE_128_MANT_BTS_UP1;
			 unsigned int mantissa_up2 : IEEE_128_MANT_BTS_UP2;
			 unsigned int mantissa_lo1 : IEEE_128_MANT_BTS_LO1;
			 unsigned int mantissa_lo2 : IEEE_128_MANT_BTS_LO2;
		} parts1;
		_f_real16		ldword;
		unsigned long long	llword[2];
	};

	/* Union defined to work with IEEE 32-bit floating point. */
	union _ieee_fdouble {
		_f_real4	fpword;
		unsigned int	l4word;
	};

	union _ieee_ldouble x_val, y_val;
	union _ieee_fdouble x4_val;
	_f_real16 __remainderd();

	x4_val.fpword	= argx;
	y_val.ldword	= argy;

	/* check input values: x for infinity and y for zero. */
	if (((x4_val.l4word & ~IEEE_32_SIGN_BIT) == IEEE_32_INFINITY) ||
	   ((!(isnan128(y_val.ldword)) && (y_val.ldword == 0.0)))) {
		union _ieee_ldouble x_val;
		_f_real8	result8;
		_f_real8	arg8 = 0.0;

		x_val.ldword	= _DBL_NaN;

		/* need to emit invalid exception */
		result8		= _raisinvld(arg8,arg8);
		return(x_val.ldword);
	}
	x_val.ldword	= (_f_real16) x4_val.fpword;
	return(__remainderd(x_val, y_val));
}

_f_real16
_IEEE_REMAINDER_R_D(_f_real8 argx, _f_real16 argy)
{
	/* Union defined to work with IEEE 64-bit floating point. */
	union _ieee_ldouble {
		struct { unsigned int sign	: 1;
			 unsigned int exponent	: IEEE_128_EXPO_BITS;
			 unsigned int mantissa_up1 : IEEE_128_MANT_BTS_UP1;
			 unsigned int mantissa_up2 : IEEE_128_MANT_BTS_UP2;
			 unsigned int mantissa_lo1 : IEEE_128_MANT_BTS_LO1;
			 unsigned int mantissa_lo2 : IEEE_128_MANT_BTS_LO2;
		} parts1;
		_f_real16		ldword;
		unsigned long long	llword[2];
	};

	/* Union defined to work with IEEE 64-bit floating point. */
	union _ieee_double {
		_f_real8		dword;
		unsigned long long	int8word;
	};
	union _ieee_ldouble x_val, y_val;
	union _ieee_double x8_val;
	_f_real16 __remainderd();

	x8_val.dword	= argx;
	y_val.ldword	= argy;

	/* check input values: x for infinity and y for zero. */
	if (((x8_val.int8word & ~IEEE_64_SIGN_BIT) == IEEE_64_INFINITY) ||
	   ((!(isnan128(y_val.ldword)) && (y_val.ldword == 0.0)))) {
		union _ieee_ldouble x_val;
		_f_real8	result8;
		_f_real8	arg8 = 0.0;

		x_val.ldword	= _DBL_NaN;

		/* need to emit invalid exception */
		result8		= _raisinvld(arg8,arg8);
		return(x_val.ldword);
	}
	x_val.ldword	= (_f_real16) x8_val.dword;
	return(__remainderd(x_val, y_val));
}

_f_real16
__remainderd(_f_real16 xarg, _f_real16 yarg)
{
	/* Union defined to work with IEEE 64-bit floating point. */
	union _ieee_ldouble {
		struct { unsigned int sign	: 1;
			 unsigned int exponent	: IEEE_128_EXPO_BITS;
			 unsigned int mantissa_1 : IE128_64_MANT_BTS_P1;
			 unsigned int mantissa_2 : IE128_64_MANT_BTS_P2;
			 unsigned int mantissa_3 : IE128_64_MANT_BTS_P3;
			 unsigned int mantissa_4 : IE128_64_MANT_BTS_P4;
			 unsigned int mantissa_5 : IE128_64_MANT_BTS_P5;
		} parts1;
		_f_real16		ldword;
		unsigned int		lword[4];
		long long		int64[2];
	};

	static union _ieee_ldouble two_112 =
	    {0, 0x406F, 0x0000, 0x00000000, 0x0, 0x0000000, 0x00000000};
	static union _ieee_ldouble div_112 =
	    {0, 0x3F8F, 0x0000, 0x00000000, 0x0, 0x0000000, 0x00000000};
	_f_real16	scalet, scaley;
	unsigned int	sign_x = 0X80000000;
	unsigned int	even_x = 0X00000001;
	union _ieee_ldouble x_val, y_val, tdiv;
	union _ieee_ldouble y_up, y_lo, tdiv_up, tdiv_lo, res;

	tdiv.ldword = xarg/yarg;
	x_val.ldword = xarg;
	y_val.ldword = yarg;

	/* check for 2**112 or greater = already integer */
	if ((int) (tdiv.lword[0] & (~sign_x)) < (int) two_112.lword[0]) {
		union _ieee_ldouble tmptdiva, tmptdivb;
		union _ieee_ldouble nearint, evenchk;
		unsigned int	two_64 = 0X403F0000;
		_f_real16	tmpb, tmpc;

		if (((int) tdiv.lword[0] & (~sign_x)) < (int) two_64) {
			/* calculate fraction */
			evenchk.ldword =
				tdiv.ldword - (_f_real16)((_f_int8)tdiv.ldword);

			if (tdiv.ldword < 0.0) {
				nearint.int64[1] = (_f_int8) (tdiv.ldword - 0.5);
				if ((evenchk.ldword == -0.5) &&
				    ((nearint.lword[3] & even_x) != 0))
					nearint.int64[1] += 1;
			} else {
				nearint.int64[1] = (_f_int8) (tdiv.ldword + 0.5);
				if ((evenchk.ldword == 0.5) &&
				   ((nearint.lword[3] & even_x) != 0))
					nearint.int64[1] -= 1;
			}
			tdiv.ldword = (_f_real16) nearint.int64[1];
		} else {
			/* These are values between 2**64 and 2**112.
			 * Calculate DINT(x/y) and use result in
			 * (x/y) - DINT(x/y) to find fraction.
			 */
			tmptdiva.ldword = tdiv.ldword;
			tmptdiva.lword[0] &= (~sign_x);
			CALC_DINT();
			tmptdiva.parts1.sign = tdiv.parts1.sign;

			/* calculate fraction */
			evenchk.ldword = tdiv.ldword - tmptdiva.ldword;

			if (tdiv.ldword < 0.0) {
				/* round to nearest integer */
				tmptdivb.ldword = tdiv.ldword - 0.5;

				/* truncate to a nearest whole number */
				tmptdiva.ldword = tmptdivb.ldword;
				tmptdiva.lword[0] &= (~sign_x);
				CALC_DINT();
				tmptdiva.parts1.sign =
						tmptdivb.parts1.sign;
				tmpc = tmptdiva.ldword;

				/* create DINT(0.5*x) for even number
				 * check using 2*DINT(0.5*x)
				 */
				tmptdiva.ldword = tmpc * 0.5;
				CALC_DINT();
				tmptdiva.parts1.sign =
						tmptdivb.parts1.sign;

				/* check for nearest even whole number */
				if ((evenchk.ldword == -0.5) &&
				    (tmpc != 2.0 * tmptdiva.ldword))
					tmpc += 1.0;
			} else {
				/* round to nearest integer */
				tmptdivb.ldword = tdiv.ldword + 0.5;

				/* truncate to a nearest whole number */
				tmptdiva.ldword = tmptdivb.ldword;
				tmptdiva.lword[0] &= (~sign_x);
				CALC_DINT();
				tmptdiva.parts1.sign =
						tmptdivb.parts1.sign;
				tmpc = tmptdiva.ldword;

				/* create DINT(0.5*x) for even number
				 * check using 2*DINT(0.5*x)
				 */
				tmptdiva.ldword = tmpc * 0.5;
				CALC_DINT();
				tmptdiva.parts1.sign =
						tmptdivb.parts1.sign;

				/* check for nearest even whole number */
				if ((evenchk.ldword == 0.5) &&
				    (tmpc != 2.0 * tmptdiva.ldword))
					tmpc -= 1.0;
			}
			tdiv.ldword = tmpc;
		}
	}

	/* Calculate upper and lower for y and tdiv. */
	y_up.ldword = y_val.ldword;
	tdiv_up.ldword = tdiv.ldword;
	y_up.parts1.mantissa_4 = 0x0;
	y_up.parts1.mantissa_5 = 0x0;
	tdiv_up.parts1.mantissa_4 = 0x0;
	tdiv_up.parts1.mantissa_5 = 0x0;
	scalet = 1.0;
	scaley = 1.0;

	/* If tdiv_up exponent < 57, scale up to prevent underflow. */
	if ((int) tdiv.parts1.exponent < 57) {
		tdiv_lo.ldword = (tdiv.ldword * two_112.ldword) -
			(tdiv_up.ldword * two_112.ldword);
		scalet = div_112.ldword;
	} else
		tdiv_lo.ldword = tdiv.ldword - tdiv_up.ldword;

	/* If y_up exponent < 57, scale up to prevent underflow. */
	if ((int) y_val.parts1.exponent < 57) {
		y_lo.ldword = (y_val.ldword * two_112.ldword) -
			(y_val.ldword * two_112.ldword);
		scaley = div_112.ldword;
	} else
		y_lo.ldword = y_val.ldword - y_up.ldword;

	/* algorithm for ieee for x - (x/y)*y. */
	res.ldword = ((((x_val.ldword - (tdiv_up.ldword * y_up.ldword)) -
			(scaley * (tdiv_up.ldword * y_lo.ldword))) -
			(scalet * (tdiv_lo.ldword * y_up.ldword))) -
		(scalet * scaley * (tdiv_lo.ldword * y_lo.ldword)));
	if (res.ldword == 0.0)
		res.parts1.sign = x_val.parts1.sign;
	return(res.ldword);
}
