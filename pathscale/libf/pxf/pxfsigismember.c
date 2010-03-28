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


#pragma ident "@(#) libf/pxf/pxfsigismember.c	92.1	06/29/99 11:36:06"


/*
 *  PXFSIGISMEMBER -- Test whether an individual signal specified
 *                    by isigno is a member of the jsigset signal
 *                    set. This is one of the sigsetops primitives.
 *  (section 3.3.3 of Posix 1003.9-1992)
 *
 *  Synopsis:
 *
 *     SUBROUTINE PXFSIGISMEMBER(jsigset, isigno, ismember, ierror)
 *     INTEGER jsigset, isigno, ierror
 *     LOGICAL ismember
 *
 *  Where:
 *
 *	jsigset  -  default integer input variable containing a
 *	            handle created by PXFSTRUCTCREATE('sigset',...)
 *	isigno   -  default integer input variable containing a
 *	            signal to be added to the jsigset signal set.
 *	ismember -  default logical output variable containing
 *	            TRUE if isigno is a member of the jsigset
 *	            signal set.  Otherwise, it contains FALSE.
 *	ierror is a default integer output variable containing
 *	       status:
 *
 *           zero	- PXFSIGISMEMBER was successful.
 *
 *           nonzero	- PXFSIGISMEMBER was not successful.
 *
 *  PXFSIGISMEMBER may return one of the following error values:
 *
 *	EFAULT     The jsigset argument points outside the
 *                 allocated space.
 *
 *	EINVAL     The value of isigno is an invalid or unsupported
 *                 signal number.
 *
 *	EBADHANDLE The jsigset argument is invalid.
 *
 *  DESCRIPTION:
 *
 *  Subroutine procedure PXFSIGISMEMBERET uses sigismember()
 *  function to test the membership of signal isigno in the
 *  signal set pointed to by handle jsigset.
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

#ifdef _UNICOS
void
PXFSIGISMEMBER(
#else	/* _UNICOS */
void
_PXFSIGISMEMBER(
#endif	/* _UNICOS */
	_f_int *jsigset,
	_f_int *isigno,
	_f_int *ismember,
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
	if ((stat = (_f_int) sigismember(&mask, *isigno)) == -1) {
		*ierror = errno;
		*ismember = _btol(0);
	} else {
		*ismember = _btol(stat);
	}
	return;
}

#ifndef _UNICOS
void
pxfsigismember_(
	_f_int *jsigset,
	_f_int *isigno,
	_f_int *ismember,
	_f_int *ierror
)
{
	_PXFSIGISMEMBER(jsigset, isigno, ismember, ierror);
}
#endif	/* not _UNICOS */
