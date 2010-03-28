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

#include <math.h>
#include "cmplrs/host.h"
#include "cmplx.h"
#include "defalias.h"

extern dcomplex __powzi(double adreal, double adimag, int32 n);
extern dcomplex __powzl(double adreal, double adimag, int64 n);

void pow_zi__(dcomplex *p, dcomplex *a, int32 *b)   /* p = a**b  */
{
  *p = __powzi(a->dreal, a->dimag,*b);
}

#if defined(BUILD_OS_DARWIN)
/* Mach-O doesn't support aliases */
void pow_zi_(dcomplex *p, dcomplex *a, int32 *b) { pow_zi__(p, a, b); }
void pow_zif_(dcomplex *p, dcomplex *a, int32 *b) { pow_zi__(p, a, b); }
#else /* defined(BUILD_OS_DARWIN) */
defalias(pow_zi__, pow_zi_);
defalias(pow_zi__, pow_zif_);
#endif /* defined(BUILD_OS_DARWIN) */

void pow_zl__(dcomplex *p, dcomplex *a, int64 *b)   /* p = a**b  */
{
  *p = __powzl(a->dreal, a->dimag,*b);
}

#if defined(BUILD_OS_DARWIN)
/* Mach-O doesn't support aliases */
void pow_zl_(dcomplex *p, dcomplex *a, int64 *b) { pow_zl__(p, a, b); }
void pow_zlf_(dcomplex *p, dcomplex *a, int64 *b) { pow_zl__(p, a, b); }
#else /* defined(BUILD_OS_DARWIN) */
defalias(pow_zl__, pow_zl_);
defalias(pow_zl__, pow_zlf_);
#endif /* defined(BUILD_OS_DARWIN) */
