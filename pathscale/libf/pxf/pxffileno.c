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


#pragma ident "@(#) libf/pxf/pxffileno.c	92.1	06/29/99 11:36:06"
#include <fortran.h>
#include <errno.h>
#include "fio.h"
/*
 * Description:
 *	Returns in ifildes the file descriptor to which the
 *	unit identified by iunit is connected. 
 * Standard:
 *	Section 8.5.2 of Posix 1003.9-1992
 * Parameters:
 *	iunit   (input)  unit number
 *	ifildes (output) file descriptor
 *	ierror	(output) error code
 */
#ifdef _UNICOS
void
PXFFILENO(
#else
void
_PXFFILENO(
#endif
_f_int *iunit, _f_int *ifildes, _f_int *ierror)
{
	unit *cup;
	if ((cup = _get_cup(*iunit)) == NULL)
		*ierror = EINVAL;
	else {
		if (cup->usysfd == -1)
			*ierror = EBADF;
		else {
			*ifildes = cup->usysfd;
			*ierror = 0;
		}
		_release_cup(cup);
	}
	return;
}

/*
 *	PXFFILENO:
 *	Returns in ifildes the file descriptor to which the
 *	unit identified by iunit is connected. 
 *	Allow on non-UNICOS systems.
 */

#if	!defined(_UNICOS)
void
pxffileno_(_f_int *iunit, _f_int *ifildes, _f_int *ierror)
{
	_PXFFILENO(iunit, ifildes, ierror);
	return;
}
#endif
