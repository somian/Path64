/*
 * Copyright 2003, 2004, 2005, 2006 PathScale, Inc.  All Rights Reserved.
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


/*  $Header: /home/bos/bk/kpro64-pending/libF77/c_abs.c 1.6 04/12/21 14:58:01-08:00 bos@eng-25.internal.keyresearch.com $ */
#include <math.h>
#include "moremath.h"
#include "cmplx.h"
#include "defalias.h"

float __c8abs(float zreal, float zimag)
{
  return(hypotf(zreal, zimag));
}

float c_abs__(float z[2])
{
  return(hypotf(z[0], z[1]));
}

#if defined(BUILD_OS_DARWIN)
/* Mach-O doesn't support aliases */
float c_abs_(float z[2]) { return c_abs__(z); }
float c_abs(float z[2]) { return c_abs__(z); }
#else /* defined(BUILD_OS_DARWIN) */
defalias(c_abs__, c_abs_);
defalias(c_abs__, c_abs);
#endif /* defined(BUILD_OS_DARWIN) */
