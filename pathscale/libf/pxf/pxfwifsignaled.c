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


#pragma ident "@(#) libf/pxf/pxfwifsignaled.c	92.1	06/29/99 11:36:06"


/*
 *  PXFWIFSIGNALED  -- Interpret the ISTAT argument as defined in
 *  (section 3.2.1.2 of Posix 1003.9-1992)
 *
 *  Synopsis:
 *
 *     LOGICAL FUNCTION PXFWIFSIGNALED(istat)
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
 *	Default integer value from system macro WIFSIGNALED(*istat)
 *
 *  DESCRIPTION:
 *
 *  Logical function PXFWIFSIGNALED returns TRUE if the child process
 *  has terminated because of a signal.  Function IPXFWTERMSIG returns
 *  part of the signal that terminated the child process.
 */

#include <sys/types.h>
#include <sys/wait.h>
#include <fortran.h>

#ifdef _UNICOS
_f_log
PXFWIFSIGNALED(
#else	/* _UNICOS */
_f_log
pxfwifsignaled_(
#endif	/* _UNICOS */
	_f_int *istat
)
{
	if (WIFSIGNALED(*istat)) {
		return(_btol(1) );	/* true */
	} else {
		return(_btol(0) );	/* false */
	}
}
