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


#pragma ident "@(#) libf/pxf/ipxfwstopsig.c	92.1	06/29/99 11:36:06"


/*
 *  PXFWSTOPSIG  -- Interpret the ISTAT argument as defined in
 *  (section 3.2.1.2 of Posix 1003.9-1992)
 *
 *  Synopsis:
 *
 *     INTEGER FUNCTION IPXFWSTOPSIG(istat)
 *     INTEGER istat
 *
 *  Where:
 *
 *	istat -   default integer input variable containing
 *	          the istat argument returned by PXFWAIT or
 *	          PXFWAITPID.
 *
 *  FUNCTION RESULT VALUE:
 *
 *	Default integer value from system macro WSTOPSIG(*istat)
 *
 *  DESCRIPTION:
 *
 *  Integer function IPXFWSTOPSIG returns part of the bits of the
 *  signal number that caused the child process to stop.  Logical
 *  function PXFWIFSTOPPED returns TRUE to indicate that the child
 *  process has stopped.  IPXFWSTOPSIG should only be used if
 *  PXFWIFSTOPPED returns the value TRUE.
 */

#include <sys/types.h>
#include <sys/wait.h>
#include <fortran.h>

#ifdef _UNICOS
_f_int
IPXFWSTOPSIG(
#else	/* _UNICOS */
_f_int
ipxfwstopsig_(
#endif	/* _UNICOS */
	_f_int *istat
)
{
	return((_f_int) WSTOPSIG(*istat) );
}
