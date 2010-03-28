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


#pragma ident "@(#) libf/pxf/pxfsysconf.c	92.1	06/29/99 11:36:06"

#include <errno.h>
#include <fortran.h>
#include <unistd.h>

/*
 *	PXFSYSCONF	PXF Interface to the sysconf(2) system call.
 *
 *	Call from Fortran:
 *
 *		SUBROUTINE PXFSYSCONF (NAME, IVAL, IERROR)
 *		INTEGER NAME, IVAL, IERROR
 *
 *	Where:
 *
 *	NAME	is the input integer variable containing the integer
 *              value of the symbolic constant for one or more of the
 *              following configurable system variables:
 *
 *			ARG_MAX
 *			CHILD_MAX
 *			CLK_TCK			(CRI extension)
 *			NGROUPS_MAX
 *			OPEN_MAX
 *			STREAM_MAX
 *			TZNAME_MAX
 *			_POSIX_JOB_CONTROL
 *			_POSIX_SAVED_IDS
 *			_POSIX_VERSION
 *
 *
 *	IVAL	is the output integer variable which receives the value
 *		of the system variable being queried.
 *
 *	IERROR	is an output integer variable that will contain the
 *		status:
 *
 *		  Zero    - PXFSYSCONF is successful.
 *
 *		  Nonzero - Error condition (errno) code.
 *
 */

#if	defined(_UNICOS)
void
PXFSYSCONF(
#else
void
_PXFSYSCONF(
#endif
	_f_int	*NAME,
	_f_int	*IVAL,
	_f_int	*IERROR
)
{
	*IVAL = sysconf(*NAME);
	if (*IVAL == -1)
		*IERROR = errno;
	else
		*IERROR = 0;
}

#if	!defined(_UNICOS)
/*
 *	PXFSYSCONF	PXF Interface to the sysconf(2) system call.
 *	Allow on non-UNICOS systems.
 */

void
pxfsysconf_(
	int	*NAME,
	int	*IVAL,
	int	*IERROR
)
{
	_PXFSYSCONF( NAME, IVAL, IERROR);
	return;
}
#endif
