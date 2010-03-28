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


#pragma ident "@(#) libu/util/c1/mttimes.c	92.1	07/07/99 13:22:26"

#include  <stddef.h>
#include  <stdio.h>
#include  <time.h>
#include  <sys/types.h>
#include  <NEW/mtimes.h>
#include  <sys/param.h>
#include  <sys/machd.h>

/*
 *	_MAXMAXCPU evaluates to the size of the mtms_mutime[] array in 
 *	struct mtms.
 */

#define _MAXMAXCPU	((sizeof(struct mtms) -	 \
			 offsetof(struct mtms,mtms_mutime[0])) / sizeof(time_t))

MTTIMES()
{
	register long	 n, t1, t3, t5, t6;
	register float	f0, f1, f2, f3, f4, f5, f6;
        long		t2;
	float		fpr1[_MAXMAXCPU], fpr2[_MAXMAXCPU];
	struct mtms	cptimes;
	extern long	_rtstart;
	extern int	__maxcpu;

	n = $mtused(&cptimes,&t2);
	if ( n == -1  ) {
		printf("\n MTTIMES - data structure corrupted\n\n");
		return;
	}

	f0 = ( t2-_rtstart )/(float) CLK_TCK;
	n = cptimes.mtms_conn-1;
	cptimes.mtms_mutime[n] += ( t2-cptimes.mtms_update );

	printf("\n\n\t\t     CPU Utilization\n\t\t    =================\n\n");

	t1 = 0;
	f1 = 0.0;
	for (n = 0;  n < __maxcpu;  ++n)  {
		t2 = cptimes.mtms_mutime[n];
		fpr1[n] = f2 = t2/(float)CLK_TCK;
		f1 += f2;
		t3 = t2*(n+1);
		fpr2[n] = t3/(float)CLK_TCK;
		t1 += t3;
	}

	if (f1 <= 0.0)  { /* Bad data in struct mtms */
		printf("\n MTTIMES - data structure corrupted\n\n");
		return;
	}
/*
 *	f0 = 	Total wallclock time
 *	f1 =	Total time with some CPUs
 *	f2 =	Total CPtimes
 *	f3 =	Overlap ( when some CPUs connected )
 *	f4 =	Total time with no CPUs
 *
 *	Some constants that are used below:
 *
 *	0.05	=	Don't print percents less than 0.05%
 *	60.0	=	Seconds/minute, used as a threshhold
 *	100.0	=	Used to cast values into percentages
 *	1000.0	=	No stats may be printed
 *	3600.0	=	Seconds/hour, another threshhold value
 *
 */

	f2 = t1/(float)CLK_TCK;
	f3 = f2/f1;
	f4 = f0-f1;
	f5 = 0.0;

	if ( (f4 >= 0.0) && (f4 < (1000.0*f1)) && (f0 > 0.0))  {
		if (((f5 = (100.0*f2/(f0*__maxcpu))) > 0.05) && (f5 < 100.0))  {
			if (f0 > 3600.0)  {
				f6 = f0/3600.0;
				t5 = f6;
				f6 = f0-(t5*3600.0);
				f6 = f6/60.0;
				t6 = f6;
				f6 = f0-(60.0*(t6+(60.0*t5)));
				printf("   CP: %.3fs,  Wallclock: %dh %dm %.1fs,  %.1f%% of %d-CPU Machine\n\n\n", f2,t5,t6,f6,f5,__maxcpu);
		} else printf("   CP: %.3fs,  Wallclock: %.3fs,  %.1f%% of %d-CPU Machine\n\n\n", f2,f0,f5,__maxcpu);
	  } else printf("   CP: %.3fs,  Wallclock: %.3fs\n\n\n", f2,f0);
	}

	printf("\t\tCPUs Time(sec)     Total\n\t\t---- ---------   ---------\n");

	if (f5)
		printf("\t\t%2d x %9.3f = %9.3f\n",0,f4,0.0);

	for (n = 0;  n < __maxcpu;  ++n)  {
		printf("\t\t%2d x %9.3f = %9.3f\n",n+1,fpr1[n],fpr2[n]);
	}
	if (f3 < 1.05)
		printf("\t\t                 ---------\n\
\t\t         Total = %9.3fs\n", f2);
  	else	printf("\t\t---- ---------   ---------\n\
\t    %6.2f x %9.3f = %9.3f\n", f3,f1,f2);

}

/*
 * The MTTIMEX routine should not be used and is not documented, however,
 * it will not be removed at this time due to the danger of users
 * depending upon it.  At some point it should be removed.
 */
MTTIMEX(xtimes)
float	*xtimes;
	{
	return(MTIMESX(xtimes));
	}

MTIMESX(xtimes)
float	*xtimes;
{
	register long	 n, t1, t3;
	register float	f0, f1, f2, f3;
        long		t2;
	struct mtms	cptimes;
	extern long	_rtstart;
	extern int	__maxcpu;

	n = $mtused(&cptimes,&t2);
	if ( n == -1  ) {
		printf("\n MTTIMES - data structure corrupted\n\n");
		return;
	}

	f0 = ( t2-_rtstart )/(float) CLK_TCK;
	n = cptimes.mtms_conn-1;
	cptimes.mtms_mutime[n] += ( t2-cptimes.mtms_update );

	t1 = 0;
	f1 = 0.0;
	for (n = 0;  n < __maxcpu;  ++n)  {
		t2 = cptimes.mtms_mutime[n];
		f2 = t2/(float)CLK_TCK;
		f1 += f2;
		t3 = t2*(n+1);
		f3 = t3/(float)CLK_TCK;
		t1 += t3;
		*(xtimes+n) = f3;
	}

}

MTIMESCN()
{
	long	 n, tmp;
	struct mtms	cptimes;

	n = $mtused(&cptimes,&tmp);
	if ( n == -1  ) {
		printf("\n MTIMESCN - data structure corrupted\n\n");
		return(-1);
	}

	return(cptimes.mtms_conn);
}

MTIMESUP()
{
	long	 n, tmp;
	struct mtms	cptimes;

	n = $mtused(&cptimes,&tmp);
	if ( n == -1  ) {
		printf("\n MTIMESUP - data structure corrupted\n\n");
		return(-1);
	}

	return(cptimes.mtms_update);
}

