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


/* --------------------------------------------------- */
/* | All Rights Reserved.                            | */
/* --------------------------------------------------- */
/* $Header: /home/bos/bk/kpro64-pending/libF77/d_sign.c 1.5 04/12/21 14:58:02-08:00 bos@eng-25.internal.keyresearch.com $ */

double d_sign(double *a, double *b)
{
    register double x = *a;

#ifdef KEY /* Bug 3405 */
    /* The code generator interprets IEEE_minus_zero=OFF to mean that
     * we ignore the sign of b when b is zero, not that we refrain from
     * generating negative zero. So this function should behave likewise.
     */
    x = (*a >= 0.0 ? *a : - *a);
    return( *b >= 0.0 ? x : -x);
#else
    if (x <= 0.0)
	x = -x;
    if (*b < 0.0)
	x = -x;
    return (x);
#endif
}

double __dsign(double a, double b)
{
    register double x = a;

    if (x <= 0.0)
	x = -x;
    if (b < 0.0)
	x = -x;
    return (x);
#if 0
    x = (a >= 0.0 ? a : - a);
    return( b >= 0.0 ? x : -x);
#endif
}

#ifdef KEY /* Bug 3405 */
/* Copy IEEE sign bit from b to a */
double dIsign(double *a, double *b)
{
#define SIGN 0x8000000000000000LL
  unsigned long long aval = *(unsigned long long *)a;
  unsigned long long bval = *(unsigned long long *)b;
  aval = (aval & ~SIGN) | (bval & SIGN);
  return *(double *)&aval;
}
#endif /* KEY Bug 3405 */
