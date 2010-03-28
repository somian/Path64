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


#pragma ident "@(#) libf/pxf/pxfalarm.c	92.1	06/29/99 11:36:06"


/*
 *  PXFALARM -- Schedule alarm.
 *  (section 3.4.1 of Posix 1003.9-1992)
 *
 *  Synopsis:
 *
 *     SUBROUTINE PXFALARM(iseconds, isecleft, ierror)
 *     INTEGER iseconds, isecleft, ierror
 *
 *  Where:
 *
 *	iseconds -  default integer input variable containing
 *	            the number of real-time seconds to wait before
 *	            sending the calling process a SIGALRM signal.
 *	isecleft -  default integer output variable containing
 *	            the number of seconds left until a previous
 *	            request would have generated a SIGALRM signal.
 *	ierror   -  default integer output variable containing
 *	            status:
 *
 *           zero	- PXFALARM was successful.
 *
 *  DESCRIPTION:
 *
 *  The subroutine procedure PXFALARM uses alarm() system call to
 *  wait iseconds before generating SIGALRM signal.  If a previous
 *  PXFALARM has time remaining, isecleft contains the number of
 *  seconds until the SIGALRM would have been generated.
 *
 *  NOTE:
 *
 *  Replace the use of subprocedure ALARM() with the PXFALARM()
 *  interface in Fortran programs for portability.
 *
 *  On UNICOS and UNICOS/mk systems, alarm() can be a subroutine
 *  or a function reference.  On IRIX systems, alarm() is a
 *  function reference that calls a procedure after so many
 *  seconds.
 */

#include <errno.h>
#include <fortran.h>
#include <sys/types.h>
#include <sys/signal.h>
#include <unistd.h>

#ifdef _UNICOS
void
PXFALARM(
#else	/* _UNICOS */
void
_PXFALARM(
#endif	/* _UNICOS */
	_f_int *iseconds,
	_f_int *isecleft,
	_f_int *ierror
)
{
	unsigned int stat;
	*ierror = 0;
	stat = alarm(*iseconds);
	*isecleft = (_f_int) stat;
	return;
}

#ifndef _UNICOS
void
pxfalarm_(
	_f_int *iseconds,
	_f_int *isecleft,
	_f_int *ierror
)
{
	_PXFALARM(iseconds, isecleft, ierror);
}
#endif	/* not _UNICOS */
