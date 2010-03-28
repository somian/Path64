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


#pragma ident "@(#) libf/pxf/pxftime.c	92.1	06/29/99 11:36:06"


#include <fortran.h>
#include <errno.h>
#include <liberrno.h>
#include <time.h>

#ifndef _UNICOS
#include <stddef.h>
#endif

/*
 *  PXFTIME  -- get system time (section 4.5.1 of Posix 1003.9-1992)
 *
 *  Call from Fortran:
 *
 *       SUBROUTINE PXFTIME(ITIME, IERROR)
 *       INTEGER ITIME, IERROR
 *
 *  Where:
 *
 *  ITIME	is an output integer variable for the number of
 *	        seconds since 00:00:00 UTC, January 1, 1970.
 *
 *  IERROR	is an output integer variable that will contain
 *	        the status:
 *
 *	         zero    - environment varible was changed.
 *
 *	         nonzero - PXFTIME was not successful.
 */

#ifdef _UNICOS
void
PXFTIME(
#else
void
_PXFTIME(
#endif
	_f_int *ITIME,
        _f_int *IERROR)
{
  time_t t;
  
  *IERROR = 0;

  if (time(&t) == -1)
    *IERROR = errno;

  *ITIME = t;
  return;
}


#ifndef _UNICOS

void
pxftime_(
	_f_int *ITIME,
        _f_int *IERROR)
{
	_PXFTIME(ITIME, IERROR);
}

#endif

