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


#pragma ident "@(#) libfi/element/clock.c	92.1	06/16/99 15:47:23"
#include <fortran.h> 
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>

/*
 *	CLOCK	Returns the current time in "HH:MM:SS" format.
 *		HH = Hour (0 - 23).
 *		MM = Minute (00 - 59).
 *		SS = Second (00 - 59).
 *
 *		May be called as either a function or a subroutine.
 *		If called as a subroutine, the parameter may be
 *		either CHARACTER or INTEGER type.
 */

/*
 *	Duplicate names
 *
 *	_CLOCK_		- for f90 
 *	CLOCK		- for PVP & MPP systems if called as a subroutine
 *	$CLOCK		- for cf77
 *	_CLOCK		- for f90 3.0? and previous on PVP systems 
 */

#if	!defined(__mips) && !defined(_LITTLE_ENDIAN)
#pragma _CRI duplicate _CLOCK_ as CLOCK
#pragma _CRI duplicate _CLOCK_ as $CLOCK
#pragma _CRI duplicate _CLOCK_ as _CLOCK
#endif

#define CLOCK_CHRS	8

#ifdef _UNICOS
_f_int
#else
void
#endif
_CLOCK_(timeofday)
_fcd	timeofday;
{
#ifdef	_UNICOS
	long		clock;
#endif
	struct tm	*sp;
	time_t		now;
	char		str[CLOCK_CHRS + 1];
 
	now	= time((time_t *) NULL);
	sp	= localtime(&now);

	(void) sprintf(str, "%02d:%02d:%02d", sp->tm_hour, sp->tm_min,
			sp->tm_sec);


#ifdef	_UNICOS
	clock	= *(long *) str;

	if (_numargs() > 0) 
#ifdef	_isfcd
		if (! _isfcd(timeofday)) {	/* If Hollerith */
#elif	defined(_UNICOS) && defined(_ADDR64)
		if (_numargs() == 1) {		/* If Hollerith */
#endif
			**(long **) &timeofday	= clock;
		}
		else
#endif	/* _UNICOS */
		{				/* Fortran character */
			unsigned int	len;
			char		*cp;

			cp	= _fcdtocp(timeofday);
			len	= _fcdlen (timeofday);

			(void) strncpy(cp, str, len);

			if (len > CLOCK_CHRS)
				(void) memset(cp + CLOCK_CHRS, (_f_int) ' ',
						len - CLOCK_CHRS);
		}

#ifdef	_UNICOS
	return ( (_f_int) clock );
#endif

}
