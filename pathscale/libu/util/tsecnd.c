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


#pragma ident "@(#) libu/util/tsecnd.c	92.1	07/07/99 13:18:33"
#include <fortran.h>

/*
 *	TSECND returns the elapsed CPU time in floating point 
 *	seconds for the calling task.
 *
 *	Called from Fortran:
 *
 *		REAL TSECND
 *
 *		x = TSECND()	
 *	or	CALL TSECND(x)			UNICOS systems only
 */

#ifdef	_UNICOS

extern	int	__hertz;	/* _rtc() hertz rate */

_f_real
TSECND(_f_real *time)
{
	double	timeval;
	long	_tskclks();	/* returns clocks for current task */

	timeval	= (double) _tskclks() / (double)__hertz;

	if (_numargs() > 0)
		*time	= (_f_real) timeval;

	return( (_f_real) timeval);
}

#elif	_SOLARIS

#include <unistd.h>
#include <sys/time.h>
#include <sys/times.h>

/*
 *	On Solaris systems, TSECND uses gethrvtime() to get light-weight
 *	process (LWP) CPU time.  This is valid because every multitasking task 
 *	is mapped to exactly one thread which is bound to an LWP.
 *
 *	1/23/94 kaf - this routine not yet activated for Solaris because
 *	gethrvtime() seems to be returning invalid numbers for non-multitasked
 *	programs.
 */
_f_real
tsecnd_(void)
{
	hrtime_t ret;
	ret = gethrvtime();
	return( (_f_real) ((double)ret * 10e-9) ); /* nanoseconds to seconds */
}

#endif	/* _SOLARIS */
