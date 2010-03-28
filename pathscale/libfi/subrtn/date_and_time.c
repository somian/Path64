/*
 *  Copyright (C) 2007. QLogic Corporation. All Rights Reserved.
 */

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


#pragma ident "@(#) libfi/subrtn/date_and_time.c	92.1	07/13/99 10:47:32"

/*
 *	DATE_AND_TIME - Return data on the real-time clock and date in
 *			a form compatible with the representations defined
 *			in ISO 8601:1988.
 */

#include <string.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <cray/dopevec.h>

void
_DATE_AND_TIME (dat, tim, zon, values)
	_fcd	dat, tim, zon;
	DopeVectorType	*values;
{
	char		*dptr, *tptr, *zptr;
	char		tmp[16];
	int		dlen, tlen, zlen;
	int		retcod;
	struct tm	*timer;
	struct timeval	tv;
	size_t		res;
#ifdef KEY /* Bug 14876 */
	/* The POSIX global "timezone" doesn't seem to work on Darwin. It works
	 * on Linux FC5 only after calling tzset(), and then it needs
	 * correction for daylight time. On FC5, gettimeofday() fails to change
	 * tz_minuteswest along with the rest of the time information when I
	 * change the time zone from the GUI without rebooting. In sum,
	 * (localtime - gmtime) seems most portable and reliable. */
	int timezone = 0;
	struct tm local_tm, gm_tm;
#else /* KEY Bug 14876 */
	extern time_t	timezone;
#endif /* KEY Bug 14876 */
	int		hr, min;
	char		sign;
	_f_int		*vptr;
	_f_int4		*vptr4;
	_f_int8		*vptr8;
	int		i, j;

/*	Initialize character pointers from fortran character descriptors */

	dptr = _fcdtocp (dat);
	tptr = _fcdtocp (tim);
	zptr = _fcdtocp (zon);

/*	Get time to be used for results		*/

	gettimeofday (&tv, 0);
#ifdef KEY /* Bug 14876 */
	timer = &local_tm;
	localtime_r(&(tv.tv_sec), &local_tm);
	gmtime_r(&(tv.tv_sec), &gm_tm);
	timezone = (local_tm.tm_hour - gm_tm.tm_hour) * 3600;
#else /* KEY Bug 14876 */
	timer = localtime (&tv.tv_sec);
#endif /* KEY Bug 14876 */

/*	If date is specified, calculate date values	*/

	if (dptr != NULL) {
	    dlen = _fcdlen (dat);
	    res = strftime (tmp, 9, "%Y%m%d", timer);
	    strncpy (dptr, tmp, 8);
	    if (dlen > 8) 
		for (i = 8; i < dlen; i++)
		    dptr[i] = ' ';
	    dat = _cptofcd (dptr, dlen);
	}

/*	If time is specified, calculate time values	*/

	if (tptr != NULL) {
	    tlen = _fcdlen (tim);
	    res = strftime (tmp, 10, "%H %M %S", timer);
	    tmp[8] = '\0';
#ifdef KEY /* Mac port */
	    sprintf (tmp, "%s.%3.3ld", tmp, (long) tv.tv_usec/1000);
#else /* KEY Mac port */
	    sprintf (tmp, "%s.%3.3ld", tmp, tv.tv_usec/1000);
#endif /* KEY Mac port */
/*
 *	This seemingly useless loop is necessary because SCM expands the
 *	format string into something which is not desired.  The only way
 *	around the problem is to put the blanks in the format string of
 *	the strftime call, and then strip them off afterward.
 */
	    
	    for (i = 0, j = 0; i < 13; i++) {
		if (tmp[i] != ' ' && tmp[i] != '\0')
		    tptr[j++] = tmp[i];
	    }
		tmp[i] = '\0';
		
	    if (tlen > 10)
		for (i = 10; i < tlen; i++)
		    tptr[i] = ' ';
	    tim = _cptofcd (tptr, tlen);
	}

/* 	If timezone is specified, calculate time zone values	*/

	if (zptr != NULL) {
	    zlen = _fcdlen (zon);
 	    hr = (timezone / 3600);
	    min = (timezone % 3600) / 60;
	    /* timezone for Fortran is signed differently than C
	     * and POSIX.  Change the sign for Fortran and return
	     * the result.  This occurs at CrayLibs 3.0 and later.
	     */
#ifdef KEY /* Bug 14876 */
	    /* timezone already has DST and proper sign incorporated */
	    sign = "-+"[timezone >= 0];
#else /* KEY Bug 14876 */
	    if (timezone > 0) {
		sign = '-';
                if (timer->tm_isdst) {
 	            hr--;
                } else {
 	            hr++;
                }
	    } else {
		sign = '+';
	    }
#endif /* KEY Bug 14876 */
	    sprintf (tmp, "%c%02d%02d", sign, hr > 0 ? hr : -hr, min);
	    strncpy (zptr, tmp, 5);
	    if (zlen > 5)
		for (i = 5; i < zlen; i++)
		    zptr[i] = ' ';
	    zon = _cptofcd (zptr, zlen);
	}
	
/*	If Values argument is specified, fill values array	*/

	if (values != NULL) {
	    if (values->type_lens.kind_or_star == 0) {
		if (values->type_lens.int_len == 64) {
		    vptr8 = (_f_int8 *) values->base_addr.a.ptr;
		    vptr8[0] = timer->tm_year + 1900;
		    vptr8[1] = timer->tm_mon + 1;
		    vptr8[2] = timer->tm_mday;
#ifdef KEY /* Bug 14876 */
		    /* timezone already has DST and proper sign incorporated */
		    vptr8[3] = timezone / 60;
#else /* KEY Bug 14876 */
		    if (timer->tm_isdst)
			vptr8[3] = (timezone / 60) - 60;
		    else
			vptr8[3] = timezone / 60;
		    if (timezone > 0)
			vptr8[3] = -(vptr8[3]);
#endif /* KEY Bug 14876 */
		    vptr8[4] = timer->tm_hour;
		    vptr8[5] = timer->tm_min;
		    vptr8[6] = timer->tm_sec;
		    vptr8[7] = tv.tv_usec / 1000;
		} else {
		    vptr4 = (_f_int4 *) values->base_addr.a.ptr;
		    vptr4[0] = timer->tm_year + 1900;
		    vptr4[1] = timer->tm_mon + 1;
		    vptr4[2] = timer->tm_mday;
#ifdef KEY /* Bug 14876 */
		    /* timezone already has DST and proper sign incorporated */
		    vptr4[3] = timezone / 60;
#else /* KEY Bug 14876 */
		    if (timer->tm_isdst)
			vptr4[3] = (timezone / 60) - 60;
		    else
			vptr4[3] = timezone / 60;
		    if (timezone > 0)
			vptr4[3] = -(vptr4[3]);
#endif /* KEY Bug 14876 */
		    vptr4[4] = timer->tm_hour;
		    vptr4[5] = timer->tm_min;
		    vptr4[6] = timer->tm_sec;
		    vptr4[7] = tv.tv_usec / 1000;
		}
	    } else if (values->type_lens.dec_len == 8) {
		vptr8 = (_f_int8 *) values->base_addr.a.ptr;
		vptr8[0] = timer->tm_year + 1900;
		vptr8[1] = timer->tm_mon + 1;
		vptr8[2] = timer->tm_mday;
		if (timer->tm_isdst)
		    vptr8[3] = (timezone / 60) - 60;
		else
		    vptr8[3] = timezone / 60;
		if (timezone > 0)
		    vptr8[3] = -(vptr8[3]);
		vptr8[4] = timer->tm_hour;
		vptr8[5] = timer->tm_min;
		vptr8[6] = timer->tm_sec;
		vptr8[7] = tv.tv_usec / 1000;
	    } else if (values->type_lens.dec_len == 4) {
		vptr4 = (_f_int4 *) values->base_addr.a.ptr;
		vptr4[0] = timer->tm_year + 1900;
		vptr4[1] = timer->tm_mon + 1;
		vptr4[2] = timer->tm_mday;
		vptr4[3] = timezone / 60;
		if (timer->tm_isdst)
		    vptr4[3] = (timezone / 60) - 60;
		else
		    vptr4[3] = timezone / 60;
		if (timezone > 0)
		    vptr4[3] = -(vptr4[3]);
		vptr4[4] = timer->tm_hour;
		vptr4[5] = timer->tm_min;
		vptr4[6] = timer->tm_sec;
		vptr4[7] = tv.tv_usec / 1000;
	    }
	}
}

#ifdef KEY /* Bug 11640 */
#include <math.h>

/* Easy way to init these to zero without knowing internals */
static _fcd dat, tim, zon;

/*
 * G77 intrinsic "secnds"
 * start	Initial time in wall-clock seconds since midnight
 * return	Wall-clock time in seconds since "start"
 */
float secnds_vms(float *start) {
  DopeVectorType dv;
  _f_int4 values[8];

# define N_BITS_PER_CHAR 8
  /* Set dope vector well enough to satisfy DATE_AND_TIME */
  dv.type_lens.kind_or_star = DVD_DEFAULT;
  dv.type_lens.int_len = sizeof(values[0]) * N_BITS_PER_CHAR;
  dv.base_addr.a.ptr = values;
  _DATE_AND_TIME(dat, tim, zon, &dv);

  float result = 3600.0 * values[4] + 60.0 * values[5] + values[6] +
    values[7] / 1000.0;
#define SECPERDAY 86400
  float stemp = fmod(*start, (float) SECPERDAY);
  stemp = (result >= stemp) ? stemp : (stemp - (float) SECPERDAY);
  return result - stemp;
}

#if defined(BUILD_OS_DARWIN)
/* Mach-O doesn't support aliases */
float secnds_(float *start) { return secnds_vms(start); }
#else /* defined(BUILD_OS_DARWIN) */
float secnds_(float *);
#pragma weak secnds_ = secnds_vms
#endif /* defined(BUILD_OS_DARWIN) */

double dsecnds_vms(double *start) {
  float temp = *start;
  float result = secnds_vms(&temp);
  *start = temp;
  return result;
}

#if defined(BUILD_OS_DARWIN)
/* Mach-O doesn't support aliases */
double dsecnds_(double *start) { return dsecnds_vms(start); }
#else /* defined(BUILD_OS_DARWIN) */
double dsecnds_(double *);
#pragma weak dsecnds_ = dsecnds_vms
#endif /* defined(BUILD_OS_DARWIN) */

#endif /* KEY Bug 11640 */
