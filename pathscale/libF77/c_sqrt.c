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
#include "moremath.h"
#include "cmplx.h"
#include "defalias.h"

complex __csqrt(float zreal, float zimag)
{
  register float mag;
  complex result;

  if ((mag = hypotf(zreal, zimag)) == 0.0F) {
    result.real = result.imag = 0.0F;
  } else if (zreal > 0.0F) {
    result.real = sqrtf(0.5F * (mag + zreal));
    result.imag = zimag/result.real/2.0F;
  } else {
    result.imag = sqrtf(0.5F * (mag - zreal));
    if(zimag < 0.0F)
      result.imag = - result.imag;
    result.real = zimag/result.imag/2.0F;
  }
  return result;
}

complex c_sqrt_(complex *z)
{
  return __csqrt(z->real, z->imag);
}

#if defined(BUILD_OS_DARWIN)
/* Mach-O doesn't support aliases */
complex c_sqrt(complex *z) { return c_sqrt_(z); }
#else /* defined(BUILD_OS_DARWIN) */
defalias(c_sqrt_, c_sqrt);
#endif /* defined(BUILD_OS_DARWIN) */
