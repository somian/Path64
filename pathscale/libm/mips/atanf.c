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
 * Module: atanf.c
 * $Revision: 1.5 $
 * $Date: 04/12/21 14:58:21-08:00 $
 * $Author: bos@eng-25.internal.keyresearch.com $
 * $Source: /home/bos/bk/kpro64-pending/libm/mips/SCCS/s.atanf.c $
 *
 * Revision history:
 *  09-Jun-93 - Original Version
 *
 * Description:	source code for atanf function
 *
 * ====================================================================
 * ====================================================================
 */

static char *rcs_id = "$Source: /home/bos/bk/kpro64-pending/libm/mips/SCCS/s.atanf.c $ $Revision: 1.5 $";

#ifdef _CALL_MATHERR
#include <stdio.h>
#include <math.h>
#include <errno.h>
#endif

#include "libm.h"

#if defined(mips) && !defined(__GNUC__)
extern	float	fatan(float);
extern	float	atanf(float);

#pragma weak fatan = __atanf
#pragma weak atanf = __atanf
#endif

#if defined(BUILD_OS_DARWIN) /* Mach-O doesn't support aliases */
extern float __atanf(float);
#pragma weak atanf
float atanf( float x ) {
  return __atanf( x );
}
#elif defined(__GNUC__)
extern  float  __atanf(float);
float    atanf(float) __attribute__ ((weak, alias ("__atanf")));
#endif

/* coefficients for rational approximation of atan on [-1.0, 1.0]        */

static const du	P[] =
{
{D(0x4033b1f1, 0x5145e7e8)},
{D(0x4036468d, 0x2d72b575)},
{D(0x4016a439, 0x0afa7438)},
{D(0x3fc69c34, 0x97688a95)},
};

static const du	Q[] =
{
{D(0x4033b1f1, 0x5145e7e8)},
{D(0x403cd732, 0xd729eaa7)},
{D(0x4026ab7d, 0x65d9a7a4)},
{D(0x3ff00000, 0x00000000)},
};

/* coefficients for rational approximation of atan(1/x) for |x| > 1.0    */

static const du	P2[] =
{
{D(0x3f825e40, 0xbe79ec0c)},
{D(0x3fd264c4, 0x2019db77)},
{D(0x3ff218ab, 0x3c579116)},
{D(0x3ff00000, 0x00000000)},
};

static const du	Q2[] =
{
{D(0x3fa9ff0f, 0x6b64219d)},
{D(0x3fe26aab, 0x734ccc2a)},
{D(0x3ff76e00, 0x7ae4fafe)},
{D(0x3ff00000, 0x00000000)},
};

static const du	piby2 =
{D(0x3ff921fb, 0x54442d18)};

static const	fu	Qnan = {QNANF};

static const fu	fpiby2 =
{0x3fc90fdb};


/* ====================================================================
 *
 * FunctionName		atanf
 *
 * Description		computes arctangent of arg
 *
 * ====================================================================
 */

float
__atanf( float x )
{
int	ix, xpt;
double	dx, xsq, num, denom;
float	result;
#ifdef _CALL_MATHERR
struct exception	exstruct;
#endif

	FLT2INT(x, ix);		/* copy arg to an integer	*/
	xpt = (ix >> MANTWIDTH);
	xpt &= 0xff;

	if ( xpt < 0x7f )	
	{
		/* |x| < 1.0 */

		dx = x;

		if ( xpt >= 0x73 )
		{
			/* |x| >= 2^(-12) */

			xsq = dx*dx;

			num = ((P[3].d*xsq + P[2].d)*xsq + P[1].d)*xsq + P[0].d;

			denom = ((xsq + Q[2].d)*xsq + Q[1].d)*xsq + Q[0].d;

			result = (x*num)/denom;

			return ( result );
		}

		return (x);
	}

	if ( xpt < 0x98 )
	{
		/* 1.0 <= |x| < 2^24 */

		/* compute atan(|x|) = pi/2 - atan(1/|x|) by rewriting the
		   rational approximation to atan(1/|x|)
		*/

		dx = fabsf(x);

		xsq = dx*dx;

		num = ((xsq + P2[2].d)*xsq + P2[1].d)*xsq + P2[0].d;

		denom = ((xsq + Q2[2].d)*xsq + Q2[1].d)*xsq + Q2[0].d;

		result = piby2.d - num/(dx*denom);

		if ( x < (float)0.0 )
			result = -result;

		return ( result );
	}

	if ( x != x )
	{
		/* x is a NaN; return a quiet NaN */

#ifdef _CALL_MATHERR

                exstruct.type = DOMAIN;
                exstruct.name = "atanf";
                exstruct.arg1 = x;
                exstruct.retval = Qnan.f;

                if ( matherr( &exstruct ) == 0 )
                {
                        fprintf(stderr, "domain error in atanf\n");
                        SETERRNO(EDOM);
                }

                return ( exstruct.retval );
#else
		NAN_SETERRNO(EDOM);
		
		return ( Qnan.f );
#endif
	}

	result = fpiby2.f;

	if ( x < (float)0.0 )
		result = -result;

	return ( result );

}

