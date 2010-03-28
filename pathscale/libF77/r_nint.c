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


/* $Header: /home/bos/bk/kpro64-pending/libF77/r_nint.c 1.6 04/12/21 14:58:04-08:00 bos@eng-25.internal.keyresearch.com $ */
#include "cmplrs/host.h"
#include <math.h>
#include "moremath.h"

#define TWO_EXP_23 8388608.0F

float __rnint(float x)
{
  /* Need to worry about case where LSB of floating-point type
     represents 1.0, e.g., ANINT(8388609.0) needs to return 8388609.0
     (will return 8388610.0 without this additional check). */
  if (fabs(x) >= TWO_EXP_23)
    return (x);
  else
    return (x >= 0.0F ? floorf(x + .5F) : -floorf(.5F - x));
}

float r_nint(float *x)
{
  return (__rnint(*x));
}
