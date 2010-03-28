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


/* $Header: /home/bos/bk/kpro64-pending/libF77/r_cnjg.c 1.6 04/12/21 14:58:03-08:00 bos@eng-25.internal.keyresearch.com $ */

#include "cmplx.h"
#include "defalias.h"

complex __rconjg(float zreal, float zimag)
{
  complex r;

  r.real = zreal;
  r.imag = -zimag;
  return r;
}

complex r_cnjg_(complex *z)
{
  return __rconjg(z->real, z->imag);
}

#if defined(BUILD_OS_DARWIN)
/* Mach-O doesn't support aliases */
complex r_cnjg(complex *z) { return r_cnjg_(z); }
#else /* defined(BUILD_OS_DARWIN) */
defalias(r_cnjg_, r_cnjg);
#endif /* defined(BUILD_OS_DARWIN) */
