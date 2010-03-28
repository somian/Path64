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


#pragma ident "@(#) libf/pxf/pxfgetppid.c	92.1	06/29/99 11:36:06"

#include <fortran.h>
#include <errno.h>
#include <liberrno.h>
#include <sys/types.h>
#include <unistd.h>

/*
 *  PXFGETPPID  -- get parent process ID
 *  (section 4.1.1 of Posix 1003.9-1992)
 *
 *  Call from Fortran:
 *
 *      SUBROUTINE PXFGETPPID(IPID, IERROR)
 *      INTERGER IPID, IERROR
 *
 *  Where:
 *
 *  IPID    is an output integer variable that will contain
 *          the parent process ID of the current process.
 *
 *  IERROR  is an output integer variable that will contain
 *          the status:
 *
 *           zero    - parent process ID sucessful acquired.
 *
 *
 */

#ifdef _UNICOS
void
PXFGETPPID(
#else
void
_PXFGETPPID(
#endif
	   _f_int *IPID,
	   _f_int *IERROR
)
{
  *IERROR = 0;
  *IPID = getppid();
}

#ifndef _UNICOS
void
pxfgetppid_(
	   _f_int *IPID,
	   _f_int *IERROR
)
{
  _PXFGETPPID(IPID, IERROR);
}
#endif







