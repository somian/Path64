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


/* $Header: /home/bos/bk/kpro64-pending/libF77/c_div.c 1.6 04/12/21 14:58:01-08:00 bos@eng-25.internal.keyresearch.com $ */
#include <stdlib.h>
#include "cmplx.h"
#include "defalias.h"

complex __cdiv(float areal, float aimag, float breal, float bimag)
{
  float ratio, den;
  float abr, abi;
  complex c;

  if( (abr = breal) < 0.0F)
    abr = - abr;
  if( (abi = bimag) < 0.0F)
    abi = - abi;
  if( abr <= abi ) {
    ratio = breal / bimag ;
    den = bimag * (1 + ratio*ratio);
    c.real = (areal*ratio + aimag) / den;
    c.imag = (aimag*ratio - areal) / den;
  } else {
    ratio = bimag / breal ;
    den = breal * (1 + ratio*ratio);
    c.real = (areal + aimag*ratio) / den;
    c.imag = (aimag - areal*ratio) / den;
  }
  return c;
}

complex c_div_(complex *a, complex *b)
{
  return __cdiv(a->real, a->imag, b->real, b->imag);
}

#if defined(BUILD_OS_DARWIN)
/* Mach-O doesn't support aliases */
complex c_div(complex *a, complex *b) { return c_div_(a, b); }
#else /* defined(BUILD_OS_DARWIN) */
defalias(c_div_, c_div);
#endif /* defined(BUILD_OS_DARWIN) */
