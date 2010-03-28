/*
 * Copyright (C) 2009. PathScale, LLC. All Rights Reserved.
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


#include <math.h>
#include "cmplrs/host.h"
#include "cmplx.h"
#include "defalias.h"
#include "moremath.h"

dcomplex __zsqrt(double zdreal, double zdimag)
{
  double mag;
  dcomplex result;

  if ((mag = hypot(zdreal, zdimag)) == 0.) {
    result.dreal = result.dimag = 0.;
  } else if (zdreal > 0.) {
    result.dreal = sqrt(0.5 * (mag + zdreal));
    result.dimag = zdimag/result.dreal/2;
  } else {
    result.dimag = sqrt(0.5 * (mag - zdreal));
    if (zdimag < 0.)
      result.dimag = - result.dimag;
    result.dreal = zdimag/result.dimag/2;
  }
  return result;
}

dcomplex z_sqrt_(dcomplex *z)
{
  return __zsqrt(z->dreal, z->dimag);
}

#if defined(BUILD_OS_DARWIN)
/* Mach-O doesn't support aliases */
dcomplex z_sqrt(dcomplex *z) { return z_sqrt_(z); }
#pragma weak z_sqrt
#else /* defined(BUILD_OS_DARWIN) */
#ifdef KEY /* Bug 15104 */
weakalias(z_sqrt_, z_sqrt);
#else /* KEY Bug 15104 */
defalias(z_sqrt_, z_sqrt);
#endif /* KEY Bug 15104 */
#endif /* defined(BUILD_OS_DARWIN) */
