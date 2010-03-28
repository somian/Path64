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
#include "cmplrs/host.h"
#include "cmplx.h"
#include "defalias.h"
#include "moremath.h"

#if defined(BUILD_OS_DARWIN)
static void
sincos(double d, double *s, double *c) {
  *s = sin(d);
  *c = cos(d);
  }
#else /* defined(BUILD_OS_DARWIN) */
extern	void	sincos(double, double *, double *);
#endif /* defined(BUILD_OS_DARWIN) */

#ifdef KEY
dcomplex __powzz(double adreal, double adimag, double bdreal, double bdimag)
#else
dcomplex __powzz(double adreal, double adimag, double_t bdreal, double_t bdimag)
#endif // KEY
{
  double logr, logi, x, y;
  double sinx, cosx;
  dcomplex r;

  logr = log(hypot(adreal,adimag));
  logi = atan2(adimag,adreal);

  x = exp(logr*bdreal-logi*bdimag);
  y = logr*bdimag+logi*bdreal;

  sincos(y, &sinx, &cosx);
  r.dreal = x*cosx;
  r.dimag = x*sinx;

  return r;


}

void pow_zz_(dcomplex *r, dcomplex *a, dcomplex *b)
{
  *r = __powzz(a->dreal, a->dimag, b->dreal, b->dimag);
}

#if defined(BUILD_OS_DARWIN)
/* Mach-O doesn't support aliases */
void pow_zz(dcomplex *r, dcomplex *a, dcomplex *b) { pow_zz_(r, a, b); }
void pow_zz__(dcomplex *r, dcomplex *a, dcomplex *b) { pow_zz_(r, a, b); }
#else /* defined(BUILD_OS_DARWIN) */
defalias(pow_zz_, pow_zz);
defalias(pow_zz_, pow_zz__);
#endif /* defined(BUILD_OS_DARWIN) */
