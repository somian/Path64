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


#pragma ident "@(#) libf/pxf/pxfsetuid.c	92.1	06/29/99 11:36:06"

#include <errno.h>
#include <liberrno.h>
#include <fortran.h>
#include <sys/types.h>
#include <unistd.h>

/*  PXFSETUID  -- set user ID
 *  (section 4.2.2 of Posix 1003.9-1992)
 *
 *  Call from Fortran:
 *
 *     SUBROUTINE PXFSETUID(IUID, IERROR)
 *     INTEGER IUID, IERROR
 *
 *  Where:
 *
 *  IUID   is an input integer variable used to replace the
 *         current user ID for the calling process.
 *
 *  IERROR is an output integer variable that will contain
 *         the status:
 *
 *          zero    - PXFLOCALTIME was successful.
 *
 *          nonzero - PXFLOCALTIME was unsuccessful.
 *
 *         PXFSETUID may return any of the following
 *
 *          EINVAL If the value of the IUID argument is out of range.
 *
 *          EPERM  If the process does not have the appropriate
 *                 privileges and IUID does not match the real
 *                 user ID.
 *
 *
 */

#ifdef _UNICOS
void
PXFSETUID(
#else
void
_PXFSETUID(
#endif
	   _f_int *IUID,
	   _f_int *IERROR
)
{
  uid_t ciuid = *IUID;

  if (setuid(ciuid) == -1) {
    *IERROR = errno;
    return;
  }

  *IERROR = 0;
}

#ifndef _UNICOS
void
pxfsetuid_(
	   _f_int *IUID,
	   _f_int *IERROR
)
{
  _PXFSETUID(IUID, IERROR);
}
#endif
