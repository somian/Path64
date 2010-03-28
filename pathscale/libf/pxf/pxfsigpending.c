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


#pragma ident "@(#) libf/pxf/pxfsigpending.c	92.1	06/29/99 11:36:06"


/*
 *  PXFSIGSIGPENDING -- Examine pending signals.
 *  (section 3.3.6 of Posix 1003.9-1992)
 *
 *  Synopsis:
 *
 *     SUBROUTINE PXFSIGPENDING(jsigset, ierror)
 *     INTEGER jsigset, ierror
 *
 *  Where:
 *
 *	jsigset is a default integer input variable containing a
 *	     handle created by PXFSTRUCTCREATE('sigset',...)
 *	ierror is a default integer output variable containing
 *	       status:
 *           zero	- PXFSIGPENDING was successful.
 *
 *           nonzero	- PXFSIGPENDING was not successful.
 *
 *  DESCRIPTION:
 *
 *  Subroutine procedure PXFSIGPENDING uses sigpending() function
 *  to retrieve the signal set in the handle jsigset.
 *
 *  PXFSIGPENDING may return one of the following error values:
 *
 *      EFAULT     The jsigset argument points outside the
 *                 allocated space.
 *
 *	EBADHANDLE The jsigset argument is invalid.
 *
 */

#include <errno.h>
#include <fortran.h>
#include <liberrno.h>
#include <string.h>
#include <sys/signal.h>
#include "pxfstruct.h"
#include "pxfstructtable.h"
#include "table.h"

#ifndef _UNICOS
#include <stddef.h>
#endif

#ifdef _UNICOS
void
PXFSIGPENDING(
#else	/* _UNICOS */
void
_PXFSIGPENDING(
#endif	/* _UNICOS */
	_f_int *jsigset,
	_f_int *ierror
)
{
	int	stat;
	sigset_t mask;
	struct	pxfhandle pxfhand;
	*ierror = 0;
	pxfhand = _pxfhandle_table_lookup(&_pxfhandle_table, *jsigset);
	if (pxfhand.pxfstructptr == NULL || pxfhand.pxftype != PXF_SIGSET) {
		*ierror = EBADHANDLE;
		return;
	}
	mask = ((struct sig_set *)(pxfhand.pxfstructptr))->samask;
	if ((stat = (_f_int) sigpending(&mask)) == -1)
		*ierror = errno;
	else
		((struct sig_set *)(pxfhand.pxfstructptr))->samask = mask;
	return;
}

#ifndef _UNICOS
void
pxfsigpending_(
	_f_int *jsigset,
	_f_int *ierror
)
{
	_PXFSIGPENDING(jsigset, ierror);
}
#endif	/* not _UNICOS */
