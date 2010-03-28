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
#include "z_div.h"

dcomplex __powzi(double adreal, double adimag, int32 n)
/* __powzi = a**n  */
{
  double t;
  dcomplex x, p;

  p.dreal = 1;
  p.dimag = 0;

  if(n == 0)
    return p;

  if(n < 0) {
    n = -n;
    x = __zdiv(p.dreal, p.dimag, adreal, adimag);
  } else {
    x.dreal = adreal;
    x.dimag = adimag;
  }

  for( ; ; ) {
    if(n & 01) {
      t = p.dreal * x.dreal - p.dimag * x.dimag;
      p.dimag = p.dreal * x.dimag + p.dimag * x.dreal;
      p.dreal = t;
    }
    if(n >>= 1) {
      t = x.dreal * x.dreal - x.dimag * x.dimag;
      x.dimag = 2 * x.dreal * x.dimag;
      x.dreal = t;
    } else {
      break;
    }
  }

  return p;
}
