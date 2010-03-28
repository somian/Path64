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

extern complex __powci(float areal, float aimag, int32 n);
extern complex __powcl(float areal, float aimag, int64 n);

void pow_ci__(complex *p, complex *a, int32 *b)   /* p = a**b  */
{
	*p = __powci(a->real, a->imag, *b);
}

#if defined(BUILD_OS_DARWIN)
/* Mach-O doesn't support aliases */
void pow_ci_(complex *p, complex *a, int32 *b) { pow_ci__(p, a, b); }
void pow_ci(complex *p, complex *a, int32 *b) { pow_ci__(p, a, b); }
void pow_cif_(complex *p, complex *a, int32 *b) { pow_ci__(p, a, b); }
#else /* defined(BUILD_OS_DARWIN) */
defalias(pow_ci__, pow_ci_);
defalias(pow_ci__, pow_ci);
defalias(pow_ci__, pow_cif_);
#endif /* defined(BUILD_OS_DARWIN) */

void
pow_cl__(complex *p, complex *a, int64 *b)
{
        *p = __powcl(a->real, a->imag, *b);
}

#if defined(BUILD_OS_DARWIN)
/* Mach-O doesn't support aliases */
void pow_cl_(complex *p, complex *a, int64 *b) { pow_cl__(p, a, b); }
void pow_cl(complex *p, complex *a, int64 *b) { pow_cl__(p, a, b); }
void pow_clf_(complex *p, complex *a, int64 *b) { pow_cl__(p, a, b); }
#else /* defined(BUILD_OS_DARWIN) */
defalias(pow_cl__, pow_cl_);
defalias(pow_cl__, pow_cl);
defalias(pow_cl__, pow_clf_);
#endif /* defined(BUILD_OS_DARWIN) */
