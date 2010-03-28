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


/* ====================================================================
 * ====================================================================
 *
 * Module: vsqrt.c
 * $Revision: 1.5 $
 * $Date: 04/12/21 14:58:21-08:00 $
 * $Author: bos@eng-25.internal.keyresearch.com $
 * $Source: /home/bos/bk/kpro64-pending/libm/SCCS/s.vsqrt.c $
 *
 * Revision history:
 *  10-Feb-95 - Original Version
 *
 * Description:	source code for vector sqrt function
 *
 * ====================================================================
 * ====================================================================
 */

static char *rcs_id = "$Source: /home/bos/bk/kpro64-pending/libm/SCCS/s.vsqrt.c $ $Revision: 1.5 $";

#include "libm.h"

#if defined(mips) && !defined(__GNUC__)
extern	void	vsqrt(double *, double *, long, long, long);

#pragma weak vsqrt = __vsqrt
#endif

#if defined(BUILD_OS_DARWIN) /* Mach-O doesn't support aliases */
extern void __vsqrt( double *x, double *y, long count, long stridex,
  long stridey );
#pragma weak vsqrt
void vsqrt( double *x, double *y, long count, long stridex, long stridey ) {
  __vsqrt(x, y, count, stridex, stridey);
}
#elif defined(__GNUC__)
extern void __vsqrt(double *, double *, long, long, long);
void    vsqrt() __attribute__ ((weak, alias ("__vsqrt")));
#endif

static const	du	Inf =
{D(0x7ff00000, 0x00000000)};


/* ====================================================================
 *
 * FunctionName		vsqrt
 *
 * Description		computes vector sqrt of arg
 *
 * ====================================================================
 */

void
__vsqrt( double *x, double *y, long count, long stridex, long stridey )
{
long	i;
double	u;
double	z;
double	result;

	/* i = 0, 1, ..., count-1; y[i*stridey] = sqrt(x[i*stridex])	*/


	for ( i=0; i<count; i++ )
	{
#ifdef _PREFETCH
#pragma prefetch_ref=*(x+8)
#pragma prefetch_ref=*(y+8)
#endif

		u = z = *x;

#ifdef _USES_RECIP_SQRT

/* If _USES_RECIP_SQRT is defined, the compiler uses a reciprocal sqrt to
 * compute sqrt, so we have to special case zero and infinity.
 */

		if ( z == 0.0 )
			u = 1.0;

		if ( z == Inf.d )
			u = 1.0;
#endif
		result = sqrt(u);

#ifdef _USES_RECIP_SQRT

		if ( z == 0.0 )
			result = z;

		if ( z == Inf.d )
			result = z;
#endif
		*y = result;

		x += stridex;
		y += stridey;
	}
}

