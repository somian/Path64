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
 * Module: vlog10f.c
 * $Revision: 1.5 $
 * $Date: 04/12/21 14:58:20-08:00 $
 * $Author: bos@eng-25.internal.keyresearch.com $
 * $Source: /home/bos/bk/kpro64-pending/libm/SCCS/s.vlog10f.c $
 *
 * Revision history:
 *  05-Feb-96 - Original Version
 *
 * Description:	source code for vector common log function
 *
 * ====================================================================
 * ====================================================================
 */

static char *rcs_id = "$Source: /home/bos/bk/kpro64-pending/libm/SCCS/s.vlog10f.c $ $Revision: 1.5 $";

#include "libm.h"

extern	const du        __log_ru[];
extern	const du        _logtab[];

#if defined(mips) && !defined(__GNUC__)
extern	void	vflog10(float *, float *, long, long, long);
extern	void	vlog10f(float *, float *, long, long, long);

#pragma weak vflog10 = __vlog10f
#pragma weak vlog10f = __vlog10f
#endif

#if defined(BUILD_OS_DARWIN) /* Mach-O doesn't support aliases */
extern void __vlog10f( float *x, float *y, long count, long stridex,
  long stridey );
#pragma weak vlog10f
void vlog10f( float *x, float *y, long count, long stridex, long stridey ) {
  __vlog10f(x, y, count, stridex, stridey);
}
#elif defined(__GNUC__)
extern  void  __vlog10f(float *, float *, long, long, long);
void    vlog10f() __attribute__ ((weak, alias ("__vlog10f")));
#endif

static const du	log2 =
{D(0x3fe62e42, 0xfefa39ef)};

static const du	Loge =
{D(0x3fdbcb7b, 0x1526e50e)};

/* coefficients for polynomial approximation of log(1 + t) on +/- 1/256   */

static const du	P[] =
{
{D(0x3fefffff, 0xffff6666)},
{D(0xbfe00006, 0x000349d2)},
{D(0x3fd55561, 0x555d346b)},
};

static const int twop7 = {0x43000000};

static const fu	twopm7 = {0x3c000000};

static const fu	Scaleup = {0x4b000000};

static const	fu	Qnan = {QNANF};

static const fu	Inf = {0x7f800000};

static const fu	Neginf = {0xff800000};

#define MAXEXP	0xffu

#define	MINEXP	0x01u


/* ====================================================================
 *
 * FunctionName		vlog10f
 *
 * Description		computes vector common logarithm of arg
 *
 * ====================================================================
 */

#ifdef _SW_PIPELINE

/* If compiler supports software pipelining, use this algorithm; note that
 * denormal args are not supported.
 */

void
__vlog10f( float *x, float *y, long count, long stridex, long stridey )
{
long	i;
unsigned int j;
unsigned int ix;
int	m;
int	k;
float	u;
double	t;
double	xmu;
double	q;
double	l_lead;
float	w;
float	result;

	/* i = 0, 1, ..., count-1; y[i*stridey] = log10f(x[i*stridex])	*/

	for ( i=0; i<count; i++ )
	{
#ifdef _PREFETCH
#pragma prefetch_ref=*(x+8)
#pragma prefetch_ref=*(y+8)
#endif

		/* extract exponent and sign of x for some quick screening */

		ix = *(unsigned int *)x;	/* copy arg to an int	*/

		m = (ix >> MANTWIDTH);		/* shift off mantissa	*/
		j = m - MINEXP;

		m -= EXPBIAS;

		/* normalize x and compute the nearest 1/128th to x */
	
		ix &= (SIGNMASK & EXPMASK);	/* mask off sign and exponent
						 * bits of x
						 */
		ix |= twop7;	/* set exponent of x to 0x430 */

		INT2FLT(ix, w);	/* copy scaled arg to a float	*/
	
		k = ROUNDF(w);
	
		u = k;
	
		k -= 128;
	
		xmu = twopm7.f*(w - u);
	
		t = __log_ru[k].d*xmu;
	
		/* avoid loss of significance for values of x near two
		   by adjusting index; effectively u is divided by two.
		   The logtable has been adjusted for this.
		*/
	
		if ( k > 64 )
			m++;
	
		q = ((P[2].d*t + P[1].d)*t + P[0].d);
	
		l_lead = _logtab[k].d;
	
		l_lead += m*log2.d;
	
		result = (l_lead + q*t)*Loge.d;

		/* take care of negative args, NaNs, and Infinities     */

		if ( j >= (MAXEXP - MINEXP) )
			result = Qnan.f;

		if ( *x == 0.0f )
			result = Neginf.f;

		if ( *x == Inf.f )
			result = Inf.f;

		*y = result;

		x += stridex;
		y += stridey;
	}
}

#else

void
__vlog10f( float *x, float *y, long count, long stridex, long stridey )
{
long	i;
unsigned int j;
unsigned int ix;
int	m;
int	k;
float	u;
double	t;
double	xmu;
double	q;
double	l_lead;
float	w, z;
float	result;

	/* i = 0, 1, ..., count-1; y[i*stridey] = log10f(x[i*stridex])	*/

	for ( i=0; i<count; i++ )
	{
		/* extract exponent and sign of x for some quick screening */

		w = z = *x;

		FLT2INT(w, ix);

		m = (ix >> MANTWIDTH);		/* shift off mantissa	*/
		j = m;

		if ( m == 0 )
		{
			z *= Scaleup.f;
			w = z;

			FLT2INT(w, ix); /* copy scaled arg to an int   */
			m = (ix >> MANTWIDTH); /* shift off mantissa	*/
			m -= 23;	/* adjust for scaling	*/
		}

		m -= EXPBIAS;

		/* normalize x and compute the nearest 1/128th to x */
	
		ix &= (SIGNMASK & EXPMASK);	/* mask off sign and exponent
						 * bits of x
						 */
		ix |= twop7;	/* set exponent of x to 0x430 */

		INT2FLT(ix, w);	/* copy scaled arg to a float	*/
	
		k = ROUNDF(w);
	
		u = k;
	
		k -= 128;
	
		xmu = twopm7.f*(w - u);
	
		t = __log_ru[k].d*xmu;
	
		/* avoid loss of significance for values of x near two
		   by adjusting index; effectively u is divided by two.
		   The logtable has been adjusted for this.
		*/
	
		if ( k > 64 )
			m++;
	
		q = ((P[2].d*t + P[1].d)*t + P[0].d);
	
		l_lead = _logtab[k].d;
	
		l_lead += m*log2.d;
	
		result = (l_lead + q*t)*Loge.d;

		/* take care of negative args, NaNs, and Infinities     */

		if ( j >= MAXEXP )
			result = Qnan.f;

		if ( z == 0.0f )
			result = Neginf.f;

		if ( z == Inf.f )
			result = Inf.f;

		*y = result;

		x += stridex;
		y += stridey;
	}
}
#endif

