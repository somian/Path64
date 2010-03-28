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


#include <math.h>
#include "cmplx.h"
#include "defalias.h"
#include "moremath.h"

#if defined(BUILD_OS_DARWIN)
static void
sincosf(float f, float *s, float *c) {
  *s = sinf(f);
  *c = cosf(f);
  }
#else /* defined(BUILD_OS_DARWIN) */
extern  void    sincosf(float, float *, float *);
#endif /* defined(BUILD_OS_DARWIN) */

complex __powcc(float areal, float aimag, float breal, float bimag)
{
  float logr, logi, x, y;
  float sinx, cosx;
  complex r;

  logr = logf(hypotf(areal,aimag));
  logi = atan2f(aimag,areal);

  x = expf(logr*breal-logi*bimag);
  y = logr*bimag+logi*breal;

  sincosf(y, &sinx, &cosx);
  r.real = x*cosx;
  r.imag = x*sinx;

  return r;
}

void pow_cc__(complex *r, complex *a, complex *b)
{
  *r = __powcc(a->real, a->imag, b->real, b->imag);
}

#if defined(BUILD_OS_DARWIN)
/* Mach-O doesn't support aliases */
void pow_cc_(complex *r, complex *a, complex *b) { pow_cc__(r, a, b); }
void pow_cc(complex *r, complex *a, complex *b) { pow_cc__(r, a, b); }
#else /* defined(BUILD_OS_DARWIN) */
defalias(pow_cc__, pow_cc_);
defalias(pow_cc__, pow_cc);
#endif /* defined(BUILD_OS_DARWIN) */
