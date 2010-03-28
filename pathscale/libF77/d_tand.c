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


/* $Header: /home/bos/bk/kpro64-pending/libF77/d_tand.c 1.6 04/12/21 14:58:02-08:00 bos@eng-25.internal.keyresearch.com $ */
/*   VMS Compatibility Version  @(#)d_tand.c	1.1    9/28/87 */

#include <math.h>
#include "moremath.h"
#include <cmplrs/host.h>
#include "d_mod.h"

double __dtand(double x)
{
  double circle = 360.0;
  return tan(.017453292519943295 * __dmod(x, circle));
}

double d_tand(double *x)
{
  return __dtand(*x);
}
