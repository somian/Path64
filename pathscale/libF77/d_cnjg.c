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


/* $Header: /home/bos/bk/kpro64-pending/libF77/d_cnjg.c 1.6 04/12/21 14:58:02-08:00 bos@eng-25.internal.keyresearch.com $ */

#include "cmplx.h"
#include "defalias.h"

dcomplex __dconjg(double zdreal, double zdimag)
{
  dcomplex r;

  r.dreal = zdreal;
  r.dimag = -zdimag;
  return r;
}

dcomplex d_cnjg_(dcomplex *z)
{
  return __dconjg(z->dreal, z->dimag);
}

#if defined(BUILD_OS_DARWIN)
/* Mach-O doesn't support aliases */
dcomplex d_cnjg(dcomplex *z) { return d_cnjg_(z); }
#else /* defined(BUILD_OS_DARWIN) */
defalias(d_cnjg_, d_cnjg);
#endif /* defined(BUILD_OS_DARWIN) */
