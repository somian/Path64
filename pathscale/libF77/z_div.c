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


/* $Header: /home/bos/bk/kpro64-pending/libF77/z_div.c 1.9 04/12/21 14:58:05-08:00 bos@eng-25.internal.keyresearch.com $ */
#include <math.h>
#include <stdlib.h>
#include "cmplrs/host.h"
#include "cmplx.h"
#include "defalias.h"
#include "moremath.h"

#ifdef KEY
dcomplex __zdiv(double adreal, double adimag, double bdreal, double bdimag)
#else
dcomplex __zdiv(double adreal, double adimag, double_t bdreal, double_t bdimag)
#endif // KEY
{
  double ratio, den;
  double abr, abi;
  dcomplex c;
  
  if( (abr = bdreal) < 0.)
    abr = - abr;
  if( (abi = bdimag) < 0.)
    abi = - abi;
  if( abr <= abi ) {
    ratio = bdreal / bdimag ;
    den = bdimag * (1 + ratio*ratio);
    c.dreal = (adreal*ratio + adimag) / den;
    c.dimag = (adimag*ratio - adreal) / den;
  } else {
    ratio = bdimag / bdreal ;
    den = bdreal * (1 + ratio*ratio);
    c.dreal = (adreal + adimag*ratio) / den;
    c.dimag = (adimag - adreal*ratio) / den;
  }
  return c;
}

dcomplex z_div_(dcomplex *a, dcomplex *b)
{
  return __zdiv(a->dreal, a->dimag, b->dreal, b->dimag);
}

#if defined(BUILD_OS_DARWIN)
/* Mach-O doesn't support aliases */
dcomplex z_div(dcomplex *a, dcomplex *b) { return z_div_(a, b); }
#pragma weak z_div
#else /* defined(BUILD_OS_DARWIN) */
#ifdef KEY /* Bug 15104 */
weakalias(z_div_, z_div);
#else /* KEY Bug 15104 */
defalias(z_div_, z_div);
#endif /* KEY Bug 15104 */
#endif /* defined(BUILD_OS_DARWIN) */
