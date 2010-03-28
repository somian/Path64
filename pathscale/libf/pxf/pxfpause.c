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


#pragma ident "@(#) libf/pxf/pxfpause.c	92.1	06/29/99 11:36:06"


/*
 *  PXFPAUSE -- Suspend Process Execution.
 *  (section 3.4.2 of Posix 1003.9-1992)
 *
 *  Synopsis:
 *
 *     SUBROUTINE PXFPAUSE(ierror)
 *     INTEGER ierror
 *
 *  Where:
 *
 *	ierror   -   default integer output variable containing
 *	             status.  The pause function is never
 *	             successful.
 *
 *           nonzero	- PXFPAUSE returned.
 *
 *  The following error may be returned:
 *
 *      EINTR -   Signal is caught by the calling process and 
 *                control is returned from the signal-catching
 *                function.
 *
 *  DESCRIPTION:
 *
 *  The subroutine procedure PXFPAUSE uses the pause() system call
 *  to suspend process execution.
 *
 *  NOTE:
 *
 *  Replace the use of subprocedure PAUSE() with the PXFPAUSE()
 *  interface in Fortran programs for portability.
 *
 *  On UNICOS and UNICOS/mk systems, pause() is a function or
 *  a subroutine reference.  On IRIX systems, pause() is a
 *  function reference.
 */

#include <errno.h>
#include <fortran.h>
#include <sys/types.h>
#include <unistd.h>

#ifdef _UNICOS
void
PXFPAUSE(
#else	/* _UNICOS */
void
_PXFPAUSE(
#endif	/* _UNICOS */
	_f_int *ierror
)
{
	int stat;
	*ierror = 0;
	/* possible EINTR for interrupt */
	if ((stat = pause()) < 0) {
		*ierror = errno;
	}
	return;
}

#ifndef _UNICOS
void
pxfpause_(
	_f_int *ierror
)
{
	_PXFPAUSE(ierror);
}
#endif	/* not _UNICOS */
