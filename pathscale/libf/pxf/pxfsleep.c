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


#pragma ident "@(#) libf/pxf/pxfsleep.c	92.1	06/29/99 11:36:06"


/*
 *  PXFSLEEP -- Delay Process Execution.
 *  (section 3.4.3 of Posix 1003.9-1992)
 *
 *  Synopsis:
 *
 *     SUBROUTINE PXFSLEEP(iseconds, isecleft, ierror)
 *     INTEGER iseconds, isecleft, ierror
 *
 *  Where:
 *
 *	iseconds -   default integer input variable containing
 *	             the number of real-time seconds that the
 *	             current process shall be suspended from
 *	             execution.
 *	isecleft -   default integer output variable containing
 *	             the unslept amount in seconds (requested
 *	             amount less amount actually slept).
 *	ierror   -   default integer output variable containing
 *	             status.  The function sleep is always
 *	             successful.
 *
 *           zero	- PXFSLEEP was successful.
 *
 *           nonzero	- PXFSLEEP was interrupted.
 *
 *  On UNICOS and UNICOS/mk systems, EINTR may interrupt the
 *  system call but this does not need to be signaled.  Only
 *  the time left needs to be noted.
 *
 *      EINTR -   System call interrupted.
 *
 *  DESCRIPTION:
 *
 *  The subroutine procedure PXFSLEEP uses the sleep() function
 *  to delay process execution.
 *
 *  NOTE:
 *
 *  Replace the use of subprocedure SLEEP() with the PXFSLEEP()
 *  interface in Fortran programs for portability.
 *
 *  On UNICOS and UNICOS/mk systems, sleep() is a function
 *  reference.  On IRIX systems, sleep() is a subroutine
 *  reference.
 */

#include <errno.h>
#include <fortran.h>
#include <sys/types.h>
#include <unistd.h>

#ifdef _UNICOS
void
PXFSLEEP(
#else	/* _UNICOS */
void
_PXFSLEEP(
#endif	/* _UNICOS */
	_f_int *iseconds,
	_f_int *isecleft,
	_f_int *ierror
)
{
	unsigned int sec, retsec;
	sec = *iseconds;
	*ierror = 0;
	*isecleft = 0;

	/* possible EINTR for interrupt */
	if ((retsec = sleep(sec)) > 0) {
		*isecleft = retsec;
	}
	return;
}

#ifndef _UNICOS
void
pxfsleep_(
	_f_int *iseconds,
	_f_int *isecleft,
	_f_int *ierror
)
{
	_PXFSLEEP(iseconds, isecleft, ierror);
}
#endif	/* not _UNICOS */
