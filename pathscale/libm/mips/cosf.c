
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
 * Module: cosf.c
 * $Revision$
 * $Date$
 * $Author$
 * $Source$
 *
 * Revision history:
 *  09-Jun-93 - Original Version
 *
 * Description:	source code for cosf function
 *
 * ====================================================================
 * ====================================================================
 */

static char *rcs_id = "$Source$ $Revision$";

#ifdef _CALL_MATHERR
#include <stdio.h>
#include <math.h>
#include <errno.h>
#endif

#include "libm.h"

#ifdef mips
extern	float	fcos(float);
extern	float	cosf(float);

#pragma weak fcos = __cosf
#pragma weak cosf = __cosf
#endif

#if defined(BUILD_OS_DARWIN) /* Mach-O doesn't support aliases */
extern float __cosf(float);
#pragma weak cosf
float cosf( float x ) {
  return __cosf( x );
}
#elif defined(__GNUC__)
extern  float  __cosf(float);
float    cosf(float) __attribute__ ((weak, alias ("__cosf")));
#endif

/* coefficients for polynomial approximation of sin on +/- pi/4 */

static const du  S[] =
{
{D(0x3ff00000, 0x00000000)},
{D(0xbfc55554, 0x5268a030)},
{D(0x3f811073, 0xafd14db9)},
{D(0xbf29943e, 0x0fc79aa9)},
};

/* coefficients for polynomial approximation of cos on +/- pi/4 */

static const du  C[] =
{
{D(0x3ff00000, 0x00000000)},
{D(0xbfdffffb, 0x2a77e083)},
{D(0x3fa553e7, 0xf02ac8aa)},
{D(0xbf5644d6, 0x2993c4ad)},
};

static const du  rpiby2 =
{D(0x3fe45f30, 0x6dc9c883)};

static const du  piby2hi =
{D(0x3ff921fb, 0x50000000)};

static const du  piby2lo =
{D(0x3e5110b4, 0x611a6263)};

static const	fu	Qnan = {QNANF};

static const fu Inf = {0x7f800000};


/* ====================================================================
 *
 * FunctionName		cosf
 *
 * Description		computes cosine of arg
 *
 * Note:  Routine rcis() computes cosf(arg) + i*sinf(arg) and should
 *	  be kept in sync with this routine and cosf() so that it
 *	  returns the same result as one gets by calling cosf() and
 *	  sinf() separately.
 *
 * ====================================================================
 */

float
__cosf( float x )
{
int  n;
int  ix, xpt;
double  dx, xsq;
double  dn;
double  poly;
float	result;
#ifdef _CALL_MATHERR
struct exception	exstruct;
#endif

	FLT2INT(x, ix);	 /* copy arg to an integer */

	xpt = (ix >> (MANTWIDTH-1));
	xpt &= 0x1ff;

	/* xpt is exponent(x) + 1 bit of mantissa */
	
	if ( xpt < 0xfd )
	{
		/* |x| < .75 */

		dx = x;

		if ( xpt >= 0xe6 )
		{
			/* |x| >= 2^(-12) */

			xsq = dx*dx;

			result = ((C[3].d*xsq + C[2].d)*xsq + C[1].d)*xsq + 1.0;
		}
		else
		{
			result = 1.0f;
		}

		return ( result );
	}

	if (xpt < 0x12a)
	{
		/*  |x| < 2**22  */

		dx = x;
		dn = dx*rpiby2.d;

		n = ROUND(dn);
		dn = n;

		dx = dx - dn*piby2hi.d;
		dx = dx - dn*piby2lo.d;  /* dx = x - n*piby2 */

		xsq = dx*dx;

		if ( n&1 )
		{
			poly = ((S[3].d*xsq + S[2].d)*xsq + S[1].d)*(xsq*dx) + dx;

			if ( n&2 )
			{
				/*
				 *  n%4 = 3
				 *  result = sin(x)
				 */

				result = poly;
			}
			else
			{
				/*
				 *  n%4 = 1
				 *  result = -sin(x)
				 */

				result = -poly;
			}

			return ( result );
		}

		poly = ((C[3].d*xsq + C[2].d)*xsq + C[1].d)*xsq + C[0].d;

		if ( n&2 )
		{
			/*
			 *  n%4 = 2
			 *  result = -cos(x)
			 */

			result = -poly;
		}
		else
		{
			/*
			 *  n%4 = 0
			 *  result = cos(x)
			 */

			result = poly;
		}

		return ( result );
	}

	if ( (x != x) || (fabsf(x) == Inf.f) )
	{
		/* x is a NaN or +/-inf; return a quiet NaN */

#ifdef _CALL_MATHERR

                exstruct.type = DOMAIN;
                exstruct.name = "cosf";
                exstruct.arg1 = x;
                exstruct.retval = Qnan.f;

                if ( matherr( &exstruct ) == 0 )
                {
                        fprintf(stderr, "domain error in cosf\n");
                        SETERRNO(EDOM);
                }

                return ( exstruct.retval );
#else
		NAN_SETERRNO(EDOM);
		
		return ( Qnan.f );
#endif
	}

	/* just give up and return 0.0, setting errno = ERANGE */

#ifdef _CALL_MATHERR

		exstruct.type = TLOSS;
		exstruct.name = "cosf";
		exstruct.arg1 = x;
		exstruct.retval = 0.0f;

		if ( matherr( &exstruct ) == 0 )
		{
			fprintf(stderr, "range error in cosf (total loss \
of significance)\n");
			SETERRNO(ERANGE);
		}

		return ( exstruct.retval );
#else
		SETERRNO(ERANGE);

		return ( 0.0f );
#endif
}

