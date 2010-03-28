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
#include "c_div.h"

complex __powcl(float areal, float aimag, int64 n)   /* __powcl = a**n  */
{
  float t;
  complex x, p;

  p.real = 1;
  p.imag = 0;

  if(n == 0)
    return p;

  if(n < 0) {
    n = -n;
    x = __cdiv(p.real, p.imag, areal, aimag);
  } else {
    x.real = areal;
    x.imag = aimag;
  }

  for( ; ; ) {
    if(n & 01) {
      t = p.real * x.real - p.imag * x.imag;
      p.imag = p.real * x.imag + p.imag * x.real;
      p.real = t;
    }
    if(n >>= 1) {
      t = x.real * x.real - x.imag * x.imag;
      x.imag = 2 * x.real * x.imag;
      x.real = t;
    } else {
      break;
    }
  }

  return p;
}
