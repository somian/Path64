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


/* $Header: /home/bos/bk/kpro64-pending/libF77/d_nint.c 1.7 04/12/21 14:58:02-08:00 bos@eng-25.internal.keyresearch.com $ */

#include <math.h>
#include "moremath.h"
#include <cmplrs/host.h>

#define TWO_EXP_52 4503599627370496.0

double __dnint(double x)
{

  /* Need to worry about case where LSB of floating-point type
     represents 1.0, e.g., ANINT(4503599627370496.0) needs to return
     4503599627370496.0 (will return 4503599627370497.0 without this
     additional check). */

  if (fabs(x) >= TWO_EXP_52)
    return (x);
  else
    return (x >= 0.0 ? floor(x + .5) : -floor(.5 - x));
}

double d_nint(double *x)
{
  return (__dnint(*x));
}
