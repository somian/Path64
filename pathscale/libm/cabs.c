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
 * Module: cabs.c
 * $Revision: 1.5 $
 * $Date: 04/12/21 14:58:20-08:00 $
 * $Author: bos@eng-25.internal.keyresearch.com $
 * $Source: /home/bos/bk/kpro64-pending/libm/SCCS/s.cabs.c $
 *
 * Revision history:
 *  20-Jun-93 - Original Version
 *
 * Description:	source code for cabs function
 *
 * ====================================================================
 * ====================================================================
 */

static char *rcs_id = "$Source: /home/bos/bk/kpro64-pending/libm/SCCS/s.cabs.c $ $Revision: 1.5 $";

#include "libm.h"

extern	double	__hypot(double, double);

struct __cabs_s { double r, i; };

#if defined(mips) && !defined(__GNUC__)
extern double	cabs(struct __cabs_s);

#pragma weak cabs = __cabs
#endif

#if defined(BUILD_OS_DARWIN) /* Mach-O doesn't support aliases */
#if 0 /* Builtin cabs had conflicting declaration */
extern double __cabs(struct __cabs_s z);
#pragma weak cabs
double cabs(struct __cabs_s z) {
  return __cabs(z);
}
#endif
#elif defined(__GNUC__)
extern  double  __cabs(struct __cabs_s);
double    cabs() __attribute__ ((weak, alias ("__cabs")));
#endif

double
__cabs(z)
struct __cabs_s	z;
{
	return __hypot(z.r, z.i);
}

