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
 * Module: acosf.c
 * $Revision: 1.5 $
 * $Date: 04/12/21 14:58:21-08:00 $
 * $Author: bos@eng-25.internal.keyresearch.com $
 * $Source: /home/bos/bk/kpro64-pending/libm/mips/SCCS/s.acosf.c $
 *
 * Revision history:
 *  09-Jun-93 - Original Version
 *
 * Description:	source code for acosf function
 *
 * ====================================================================
 * ====================================================================
 */

static char *rcs_id = "$Source: /home/bos/bk/kpro64-pending/libm/mips/SCCS/s.acosf.c $ $Revision: 1.5 $";

#ifdef _CALL_MATHERR
#include <stdio.h>
#include <math.h>
#include <errno.h>
#endif

#include "libm.h"

#if defined(mips) && !defined(__GNUC__)
extern	float	facos(float);
extern	float	acosf(float);

#pragma weak facos = __acosf
#pragma weak acosf = __acosf
#endif

#if defined(BUILD_OS_DARWIN) /* Mach-O doesn't support aliases */
extern float __acosf(float);
#pragma weak acosf
float acosf( float x ) {
  return __acosf(x);
}
#elif defined(__GNUC__)
extern  float  __acosf(float);
float    acosf(float) __attribute__ ((weak, alias ("__acosf")));
#endif

/* coefficients for polynomial approximation of asin on +/- 0.5    */

static const du	P[] =
{
{D(0x3ff00000, 0x00000000)},
{D(0x3fc5555c, 0x88342cb0)},
{D(0x3fb3301e, 0x4681bb77)},
{D(0x3fa747e4, 0xa38c190b)},
{D(0x3f98c283, 0xc04a0db6)},
{D(0x3fa596d2, 0x8a9d07d2)},
};

/* coefficients for polynomial approximation of asin on +/- sqrt(2 - sqrt(3))/2 */

static const du	P2[] =
{
{D(0x3ff00000, 0x00000000)},
{D(0x3fc55564, 0xfd6bdcbc)},
{D(0x3fb32843, 0xb5c74590)},
{D(0x3fa90146, 0xd5d24e0c)},
};

static const du	half =
{D(0x3fe00000, 0x00000000)};

static const du	one =
{D(0x3ff00000, 0x00000000)};

static const du	piby4 =
{D(0x3fe921fb, 0x54442d18)};

static const du	piby2 =
{D(0x3ff921fb, 0x54442d18)};

static const du	threepiby4 =
{D(0x4002d97c, 0x7f3321d2)};

static const du	pi =
{D(0x400921fb, 0x54442d18)};

static const	fu	Qnan = {QNANF};

static const fu	f_half = {0x3f000000};

static const fu	f_one = {0x3f800000};

static const fu	f_m_one = {0xbf800000};

static const fu	f_piby2 = {0x3fc90fdb};

static const fu	f_pi = {0x40490fdb};

static const int root3by2 = 0x3f5db3d7;

#ifndef	_HDW_SQRT

#define	MAGIC	0x5fe6eb3b

#endif


/* ====================================================================
 *
 * FunctionName		acosf
 *
 * Description		computes arccosine of arg
 *
 * ====================================================================
 */

float
__acosf( float x )
{
int	n;
int	ix, xpt;
unsigned int	iabsx;
float	absx;
double	dx, xsq, poly;
double	result;
double	y, ysq;
double	w;
double	q, Q;
double	ysqby2;
#ifdef _CALL_MATHERR
struct exception	exstruct;
#endif

	FLT2INT(x, ix);		/* copy arg to an integer	*/
	iabsx = (ix << 1);
	iabsx >>= 1;
	xpt = (iabsx >> MANTWIDTH);

	if ( xpt < 0x7e )	
	{
		/* |x| < 0.5 */

		if ( xpt >= 0x65 )
		{
			/* |x| >= 2^(-26) */

			dx = x;

			xsq = dx*dx;

			poly = ((((P[5].d*xsq + P[4].d)*xsq + P[3].d)*xsq +
				     P[2].d)*xsq + P[1].d)*(xsq*dx) + dx;

			result = piby2.d - poly;

			return ( (float)result );
		}

		return ( f_piby2.f );
	}

	if ( xpt < 0x7f )	
	{
		/* |x| < 1.0 */

		if ( iabsx < root3by2 )
		{
			/* |x| < sqrt(3)/2 */
	
			dx = x;

			xsq = dx*dx;
			y = xsq + xsq - one.d;
			ysq = y*y;

			poly = ((((P[5].d*ysq + P[4].d)*ysq + P[3].d)*ysq +
				   P[2].d)*ysq + P[1].d);

			poly = y + (y*ysq)*poly;

			if ( ix > 0 )
				return ( (float)(piby4.d - half.d*poly) );
			else
				return ( (float)(threepiby4.d + half.d*poly) );
		}

		absx = fabsf(x);

		ysq = f_half.f*(f_one.f - absx);

#ifdef	_HDW_SQRT

		y = sqrt(ysq);
#else

		/* Compute y = sqrt(ysq) using Newton-Raphson method.
		*/

		/* First, approximate 1/sqrt(ysq) using a "magic"
		   constant.
		*/

		y = 0.0;
		DBLHI2INT(ysq, n);
		n >>= 1;
		n = MAGIC - n;
		INT2DBLHI(n, y);
		ysqby2 = half.d*ysq;
	
		/* Improve estimate by iterating twice. */

		y = y*(1.5 - ysqby2*y*y);
		y = y*(1.5 - ysqby2*y*y);

		/* Do one final iteration, multiplying by ysq. */

		q = ysqby2*y;
		Q = q + q;
		y = Q*(1.5 - q*y);
#endif

		/* Now compute asin(y) using a polynomial approximation. */

		poly = ((P2[3].d*ysq + P2[2].d)*ysq + P2[1].d);

		w = y + (y*ysq)*poly;

		if ( ix > 0 )
			return ( (float)(w + w) );
		else
			return ( (float)(pi.d - (w + w)) );
	}

	if ( x != x )
	{
		/* x is a NaN; return a quiet NaN */

#ifdef _CALL_MATHERR

                exstruct.type = DOMAIN;
                exstruct.name = "acosf";
                exstruct.arg1 = x;
                exstruct.retval = Qnan.f;

                if ( matherr( &exstruct ) == 0 )
                {
                        fprintf(stderr, "domain error in acosf\n");
                        SETERRNO(EDOM);
                }

                return ( exstruct.retval );
#else
		NAN_SETERRNO(EDOM);
		
		return ( Qnan.f );
#endif
	}

	if ( x == f_one.f )
		return ( 0.0f );

	if ( x == f_m_one.f )
		return ( f_pi.f );

	/*  |x| > 1.0; return a NaN */

#ifdef _CALL_MATHERR

        exstruct.type = DOMAIN;
        exstruct.name = "acosf";
        exstruct.arg1 = x;
        exstruct.retval = Qnan.f;

        if ( matherr( &exstruct ) == 0 )
        {
                fprintf(stderr, "domain error in acosf\n");
                SETERRNO(EDOM);
        }

        return ( exstruct.retval );
#else
	SETERRNO(EDOM);

	return ( Qnan.f );
#endif
}

