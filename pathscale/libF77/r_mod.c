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
/* $Header: /home/bos/bk/kpro64-pending/libF77/r_mod.c 1.7 04/12/21 14:58:04-08:00 bos@eng-25.internal.keyresearch.com $ */
#include "cmplrs/host.h"
#include <math.h>
#include "moremath.h"

#ifdef KEY
float __rmod(float x, float y)
#else
float __rmod(float x, float_t y)
#endif // KEY
{
  float q1, q2;

  if ((q1 = x/y) >= 0.0F)
    q2 = floorf(q1);
  else
    q2 = -floorf(-q1);
  return (x - y * q2);
}

#ifdef KEY
float r_mod(float *x, float *y)
#else
float r_mod(float *x, float_t *y)
#endif // KEY
{
  return (__rmod(*x, *y));
}
