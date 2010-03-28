/*
 * Copyright 2005, 2006 PathScale, Inc.  All Rights Reserved.
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


/* $Header: /home/bos/bk/kpro64-pending/libF77/l_nint.c 1.4 05/03/10 16:47:56-08:00 bos@eng-25.internal.keyresearch.com $ */

#include <math.h>
#include "moremath.h"
#include <cmplrs/host.h>

int64 l_nint(float *x)
{
  return ((*x) >= 0.0F ? (int64)(*x + .5F) : -(int64)(.5F - *x));
}

#ifdef KEY /* Bug 3869 */
int64 l_nint_d(double *x)
{
  return ((*x) >= 0.0 ? (int64)(*x + .5) : -(int64)(.5 - *x));
}
#endif /* KEY Bug 3869 */
