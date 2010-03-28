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


#pragma ident "@(#) libf/pxf/pxfgeteuid.c	92.1	06/29/99 11:36:06"


#include <fortran.h>
#include <errno.h>
#include <liberrno.h>
#include <sys/types.h>
#include <unistd.h>

/*
 *  PXFGETEUID  -- get effective user ID
 *  (section 4.2.1 of Posix 1003.9-1992)
 *
 *  Call from Fortran:
 *
 *      SUBROUTINE PXFGETEUID(IEUID, IERROR)
 *      INTEGER IEUID, IERROR
 *
 *  Where:
 *
 *  IEUID    is an output integer variable for the effective user
 *           ID for the current process.
 *
 *  IERROR   is an output integer variable that will contain
 *           the status:
 *
 *            zero    - PXFGETEUID was successful. 
 *
 */


#ifdef _UNICOS
void
PXFGETEUID(
#else
void
_PXFGETEUID(
#endif
	    _f_int *IEUID,
	    _f_int *IERROR
)
{
  *IERROR = 0;
  *IEUID = geteuid();
}


#ifndef _UNICOS
void
pxfgeteuid_(
	    _f_int *IEUID,
	    _f_int *IERROR
)
{
  _PXFGETEUID(IEUID, IERROR);
}
#endif
