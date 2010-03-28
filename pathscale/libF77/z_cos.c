/*
 * Copyright (C) 2009. PathScale, LLC. All Rights Reserved.
 */
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


/* $Header: /home/bos/bk/kpro64-pending/libF77/z_cos.c 1.8 04/12/21 14:58:05-08:00 bos@eng-25.internal.keyresearch.com $ */

#include <math.h>
#include "cmplx.h"
#include "cmplrs/host.h"
#include "defalias.h"
#include "moremath.h"

dcomplex __zcos(double zdreal, double zdimag)
{
  dcomplex r;

  r.dreal = cos(zdreal) * cosh(zdimag);
  r.dimag = -sin(zdreal) * sinh(zdimag);
  return r;
}

dcomplex z_cos_(dcomplex *z)
{
  return __zcos(z->dreal, z->dimag);
}

#if defined(BUILD_OS_DARWIN)
/* Mach-O doesn't support aliases */
dcomplex z_cos(dcomplex *z) { return z_cos_(z); }
#pragma weak z_cos
#else /* defined(BUILD_OS_DARWIN) */
#ifdef KEY /* Bug 15104 */
weakalias(z_cos_, z_cos);
#else /* KEY Bug 15104 */
defalias(z_cos_, z_cos);
#endif /* KEY Bug 15104 */
#endif /* defined(BUILD_OS_DARWIN) */
