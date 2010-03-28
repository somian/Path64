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
 * Module: cabsf.c
 * $Revision: 1.5 $
 * $Date: 04/12/21 14:58:20-08:00 $
 * $Author: bos@eng-25.internal.keyresearch.com $
 * $Source: /home/bos/bk/kpro64-pending/libm/SCCS/s.cabsf.c $
 *
 * Revision history:
 *  20-Jun-93 - Original Version
 *
 * Description:	source code for cabsf function
 *
 * ====================================================================
 * ====================================================================
 */

static char *rcs_id = "$Source: /home/bos/bk/kpro64-pending/libm/SCCS/s.cabsf.c $ $Revision: 1.5 $";

#include "libm.h"

struct __fcabs_s { float r, i; };

extern	float	__hypotf(float, float);

#if defined(mips) && !defined(__GNUC__)
extern	float	fcabs(struct __fcabs_s);
extern	float	cabsf(struct __fcabs_s);

#pragma weak fcabs = __cabsf
#pragma weak cabsf = __cabsf
#endif

#if defined(BUILD_OS_DARWIN) /* Mach-O doesn't support aliases */
#if 0 /* Builtin cabsf has conflicting declaration */
extern float __cabsf(struct __fcabs_s z);
#pragma weak cabsf
float cabsf(struct __fcabs_s z) {
  return __cabsf(z);
}
#endif
#elif defined(__GNUC__)
extern  float  __cabsf(struct __fcabs_s);
float    cabsf() __attribute__ ((weak, alias ("__cabsf")));
#endif

float
__cabsf( z )
struct __fcabs_s z;
{

	return __hypotf(z.r, z.i);
}

