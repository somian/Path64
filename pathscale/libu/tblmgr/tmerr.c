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


#pragma ident "@(#) libu/tblmgr/tmerr.c	92.1	06/23/99 18:15:00"


/*
 *	TMERR - print table manager error messages
 *
 *	Call from Fortran:
 *		CALL TMERR(ROUTINE, TBERRNO)
 *
 *      Entry:
 *              ROUTINE contains the ascii name of the calling routine.
 *              TBERRNO contains the integer error number.
 *
 *      Exit:
 *              The _lerror routine is called to print the appropriate
 *              error message to stderr, and the job is aborted.
 *
 */

#include <fortran.h>
#include "liberrno.h"

TMERR (routine, tberrno)
_fcd routine;
int *tberrno;
{
	int tmplen;
	char *tmpnam;
	tmplen = _fcdlen(routine);
	tmpnam = _fcdtocp(routine);
	tmpnam[tmplen] = '\0';
	_lerror(_LELVL_EXIT, *tberrno, tmpnam);
}
