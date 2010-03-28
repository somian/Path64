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
/* $Header: /home/bos/bk/kpro64-pending/libF77/r_sign.c 1.9 04/12/21 14:58:04-08:00 bos@eng-25.internal.keyresearch.com $ */
#include <math.h>
#include "cmplrs/host.h"

#ifdef KEY
float r_sign(float *a,float *b)
#else
float r_sign(float *a,float_t *b)
#endif // KEY
{
float x;
x = (*a >= 0.0F ? *a : - *a);
return( *b >= 0.0F ? x : -x);
}

#ifdef KEY
float __rsign(float a,float b)
#else
float __rsign(float a,float_t b)
#endif // KEY
{
float x;
x = (a >= 0.0F ? a : - a);
return( b >= 0.0F ? x : -x);
}

#ifdef KEY /* Bug 3405 */
/* Copy IEEE sign bit from b to a */
float rIsign(float *a, float *b)
{
#define SIGN 0x80000000
  unsigned int aval = *(unsigned int *)a;
  unsigned int bval = *(unsigned int *)b;
  aval = (aval & ~SIGN) | (bval & SIGN);
  return *(float *)&aval;
}
#endif /* KEY Bug 3405 */

