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


/* USMID @(#) libu/util/i3e/i3eintrin.h	92.0	10/08/98 14:57:41 */


#ifndef _I3EINTRIN_H
#define _I3EINTRIN_H

/* These definitions are not part of */
/* SC22 WG14 N403 and X3J11/95-004 definition. */

/* pieces of a IEEE 32 bit floating point number */

#define IEEE_32_SIGN_BIT        0X80000000
#define IEEE_32_EXPONENT        0X7F800000
#define IEEE_32_MANTISSA        0X007FFFFF
#define IEEE_32_IMPLICIT_BIT    0X00800000
#define IEEE_32_EXPO_BIAS       127
#define IEEE_32_EXPO_BITS       8
#define IEEE_32_EXPO_MAX        0X7F8
#define IEEE_32_MANT_BITS       23
#define IEEE_32_MANT_MAX        0X7FFFFF
#define IEEE_32_EXPO_ALL_ONES(X) (((X)&IEEE_32_EXPONENT)==IEEE_32_EXPONENT)
#define IEEE_32_ABS_VALUE       0X7FFFFFFF

/* Value representing positive infinity */
/* Exponent all 1's, mantissa all zeros */
#define IEEE_32_INFINITY        0x7F800000

/* pieces of a IEEE 64 bit floating point number */

/* Value representing positive infinity */
/* Exponent all 1's, mantissa all zeros */
#define IEEE_64_INFINITY        0X7FF0000000000000

#ifndef _LD64
/* pieces of 128-bit positive infinity */
/* Exponent is all 1's, mantissa is all zeros */
#define INFINITY_128_UP         0X7FFF000000000000
#define INFINITY_128_LOW        0
#endif  /* not _LD64 */

#endif  /* ! _I3EINTRIN_H */
