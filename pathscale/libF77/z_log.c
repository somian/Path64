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


/* $Header: /home/bos/bk/kpro64-pending/libF77/z_log.c 1.8 04/12/21 14:58:06-08:00 bos@eng-25.internal.keyresearch.com $ */
#include <math.h>
#include "cmplrs/host.h"
#include "cmplx.h"
#include "defalias.h"
#include "moremath.h"

dcomplex __zlog(double zdreal, double zdimag)
{
  dcomplex r;

  r.dimag = atan2(zdimag, zdreal);
  r.dreal = log(hypot(zdreal, zdimag));
  return r;
}

dcomplex z_log_(dcomplex *z)
{
  return __zlog(z->dreal, z->dimag);
}

#if defined(BUILD_OS_DARWIN)
/* Mach-O doesn't support aliases */
dcomplex z_log(dcomplex *z) { return z_log_(z); }
#pragma weak z_log
#else /* defined(BUILD_OS_DARWIN) */
#ifdef KEY /* Bug 15104 */
weakalias(z_log_, z_log);
#else /* KEY Bug 15104 */
defalias(z_log_, z_log);
#endif /* KEY Bug 15104 */
#endif /* defined(BUILD_OS_DARWIN) */
