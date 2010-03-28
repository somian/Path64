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


#pragma ident "@(#) libfi/element/irtc.c	92.1	06/16/99 15:47:23"

#include <fortran.h>           

/*
 *	Duplicate names
 *
 *	_IRTC_	- for f90 intrinsic (not used on PVP & MPP)
 *	IRTC	- if called as a subroutine on PVP & MPP systems
 *	$IRTC	- for cf77 intrinsic
 *	irtc_	- if called as a subroutine on non-UNICOS systems
 */
#ifdef _UNICOS			/*****************************************/

extern long __hertz;
_f_int8
IRTC_RATE(void)
{
	return (_f_int8) __hertz;
}

#pragma _CRI duplicate _IRTC_ as IRTC
#pragma _CRI duplicate _IRTC_ as $IRTC
#pragma _CRI duplicate _IRTC_ as _IRTC

_f_int8
_IRTC_(long *result)
{
	long rt = (long)_rtc();

	if (_numargs() == 1)
		*result = rt;
	return(rt);
}

#elif	defined(__mips)		/*****************************************/

extern double _nowrap_cycles_per_sec;
extern long long _sysclock_nowrap(void);


_f_int8
irtc_rate_(void)
{
	if (_nowrap_cycles_per_sec == 0)
		_init_hw_clock();
	return (_f_int8) _nowrap_cycles_per_sec;
}

_f_int8
_IRTC_(void)
{
	return (_f_int8) _sysclock_nowrap();
}

void
irtc_(_f_int8 *result)
{
	*result = _sysclock_nowrap();
}

#else				/*****************************************/

#include <unistd.h>           
#include <sys/time.h>           
#include <sys/times.h>

_f_int
_IRTC_(void)
{
	clock_t ret;
	struct tms buf;
	ret = times(&buf);
	return( (_f_int) ret);
}

void
irtc_(long *result)
{
	*result = _IRTC_();
}

#endif
