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
/* $Header: /home/bos/bk/kpro64-pending/libF77/d_mod.c 1.7 04/12/21 14:58:02-08:00 bos@eng-25.internal.keyresearch.com $ */

#include <math.h>
#include "moremath.h"

#include <cmplrs/host.h>

#ifdef KEY
double __dmod(double x, double y)
#else
double __dmod(double x, double_t y)
#endif // KEY
{
  double q1, q2;

  if ((q1 = x/y) >= 0.0F)
    q2 = floor(q1);
  else
    q2 = -floor(-q1);
  return (x - y * q2);
}

#ifdef KEY
double d_mod(double *x, double *y)
#else
double d_mod(double *x, double_t *y)
#endif // KEY
{
  return (__dmod(*x, *y));
}
