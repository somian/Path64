/*
 * Copyright (C) 2007. PathScale, LLC. All Rights Reserved.
 */
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


#pragma ident "@(#) libu/util/i3e/ieee_remainderr.c	92.1	07/07/99 14:37:09"

#include <fp.h>

extern double __remainder_r(double xarg, double yarg);

#define IEEE_64_MANT_BITS_H1	26
#define IEEE_64_MANT_BITS_H2	27

/*
 * Use the following algorithm for x/y rounded quantity:
 *   1.  x/y
 *   2. if(fraction(x/y) = .5 exactly, round to next EVEN number.
 *           0.5 = 0.0, 1.5 + 2.0, 10.5 = 10, etc.
 * Use the following algorithm for the multiply:
 *   ((((x - tdivU *yU) - tdivU * yL) -tdivL * yU) - tdivL * yL)
 */

double
__remainder_r(double argx, double argy)
{
	union _ieee_double {
		double		dword;
		long long	lword;
		unsigned long long llword;
		struct {
#if defined(_LITTLE_ENDIAN)
			unsigned int mantissa_lo : IEEE_64_MANT_BITS_H2;
			unsigned int mantissa_up : IEEE_64_MANT_BITS_H1;
			unsigned int exponent	: IEEE_64_EXPO_BITS;
			unsigned int sign	: 1;
#else
			unsigned int sign	: 1;
			unsigned int exponent	: IEEE_64_EXPO_BITS;
			unsigned int mantissa_up : IEEE_64_MANT_BITS_H1;
			unsigned int mantissa_lo : IEEE_64_MANT_BITS_H2;
#endif
		} parts1;
	};
	union _ieee_double x_val, y_val, tdiv, two_52, evenchk, div_52;
	union _ieee_double nearint, y_up, y_lo, tdiv_up, tdiv_lo, res;
	double		scalet, scaley;
	unsigned long	even_x = 0X0000000000000001;
	two_52.llword	= 0x4330000000000000;		/* 2**52 */
	div_52.llword	= 0x3CB0000000000000;		/* 2**-52 */

	tdiv.dword	= argx / argy;
	x_val.dword	= argx;
	y_val.dword	= argy;

	/* check for 2**52 or greater = already integer */
	if ((long) (tdiv.llword & (~IEEE_64_SIGN_BIT)) < two_52.lword) {

		/* calculate fraction */
                evenchk.dword =
                        tdiv.dword - (double )((long)tdiv.dword);

                if (tdiv.dword < 0.0) {
                        nearint.lword = (long) (tdiv.dword - 0.5);
                        if ((evenchk.dword == -0.5) &&
                            ((nearint.lword & even_x) != 0))
                                nearint.lword += 1;
                } else {
                        nearint.lword = (long) (tdiv.dword + 0.5);
                        if ((evenchk.dword == 0.5) &&
                           ((nearint.lword & even_x) != 0))
                                nearint.lword -= 1;
                }
                tdiv.dword = (double) nearint.lword;
        }

	/* Calculate upper and lower for y and tdiv. */
	y_up.dword = y_val.dword;
	tdiv_up.dword = tdiv.dword;
	y_up.parts1.mantissa_lo = 0x0;
	tdiv_up.parts1.mantissa_lo = 0x0;
	scalet = 1.0;
	scaley = 1.0;

	/* If tdiv_up exponent < 27, scale up to prevent underflow. */
	if ((int) tdiv.parts1.exponent < 27) {
		tdiv_lo.dword = (tdiv.dword * two_52.dword) -
			(tdiv_up.dword * two_52.dword);
		scalet = div_52.dword;
	} else
		tdiv_lo.dword = tdiv.dword - tdiv_up.dword;

	/* If y_up exponent < 27, scale up to prevent underflow. */
	if ((int) y_val.parts1.exponent < 27) {
		y_lo.dword = (y_val.dword * two_52.dword) -
			(y_val.dword * two_52.dword);
		scaley = div_52.dword;
	} else
		y_lo.dword = y_val.dword - y_up.dword;

	/* algorithm for ieee for x - (x/y)*y. */
	res.dword = ((((x_val.dword - (tdiv_up.dword * y_up.dword)) -
			(scaley * (tdiv_up.dword * y_lo.dword))) -
			(scalet * (tdiv_lo.dword * y_up.dword))) -
			(scalet * scaley * (tdiv_lo.dword * y_lo.dword)));
	if (res.dword == 0.0)
		res.parts1.sign = x_val.parts1.sign;
	return(res.dword);
}
