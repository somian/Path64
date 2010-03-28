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


#pragma ident "@(#) libfi/element/rtc.c	92.1	06/16/99 15:47:23"
#include <fortran.h>

#ifdef _UNICOS

/*
 *      Duplicate names
 *
 *      _RTC_  	- for f90 intrinsic (not used on PVP & MPP systems) 
 *      RTC    	- if called as a subroutine on PVP & MPP
 *      $RTC   	- for cf77 intrinsic
 *      rtc_   	- if called as a subroutine on non-UNICOS
 */

#pragma _CRI duplicate _RTC_ as RTC
#pragma _CRI duplicate _RTC_ as $RTC

_f_real
_RTC_(_f_real *result)
{
	_f_real	rt;

	rt	= (_f_real) _rtc();

	if (_numargs() > 0)
		*result	= rt;

	return(rt);
}

#elif	defined(__mips)

extern double _nowrap_cycles_per_sec;
extern long long _sysclock_nowrap(void);

_f_real8
_RTC_(void)
{
	return (_f_real8) _sysclock_nowrap();
}

void
rtc_(_f_real8 *result)
{
	*result = (_f_real8) _sysclock_nowrap();
}

#else	/* ! _UNICOS */

#include <unistd.h>
#include <sys/time.h>
#include <sys/times.h>

_f_real
_RTC_(void)
{
	clock_t ret;
	struct tms buf;

	ret = times(&buf);
	return( (_f_real) ret);
}

void
rtc_(_f_real *result)
{
	*result = _RTC_();
}

#endif	/* ! _UNICOS */
