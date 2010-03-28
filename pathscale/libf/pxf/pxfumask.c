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


#pragma ident "@(#) libf/pxf/pxfumask.c	92.1	06/29/99 11:36:06"

#include <errno.h>
#include <liberrno.h>
#include <fortran.h>
#include <sys/types.h>
#include <sys/stat.h>

/*  PXFUMASK  -- set file creation mask
 *  (section 5.3.3 of Posix 1003.9-1992)
 *
 *  Synopsis:
 *     SUBROUTINE PXFUMASK(ICMASK,IPREVCMASK,IERROR)
 *     INTEGER ICMASK,IPREVCMASK,IERROR
 *
 *  Function description:
 *  The routine PXFUMASK uses the umask(2) system call to change
 *  the file mode creation mask of the calling process. Only the
 *  file permission bits of ICMASK are used.
 *
 *  Description of arguments:
 *  ICMASK     is an input integer variable for the new file mode
 *             creation mask.
 *
 *  IPREVCMASK is an output integer variable for the old file mode
 *             creation mask.
 *
 *  IERROR     is an output integer variable that will contain
 *             the status:
 *
 *              zero    - PXFUMASK was successful.
 *
 */

#ifdef _UNICOS
void
PXFUMASK(
#else
void
_PXFUMASK(
#endif
	  _f_int *ICMASK,
	  _f_int *IPREVCMASK,
	  _f_int *IERROR
)
{
  *IERROR = 0;
  *IPREVCMASK = umask(((mode_t)*ICMASK));
}

#ifndef _UNICOS
void
pxfumask_(
 	  _f_int *ICMASK,
	  _f_int *IPREVCMASK,
	  _f_int *IERROR
)
{
  _PXFUMASK(ICMASK, IPREVCMASK, IERROR);
}
#endif

