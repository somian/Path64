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


#pragma ident "@(#) libu/util/abort.c	92.1	07/07/99 13:18:33"

#include <fortran.h>
#include <stdio.h>
#include <stdlib.h>

static int _abortflag = 0;	/* abort flag: set if ABORT has been called */

/*
 *	ABORT	User-callable abort routine.
 *
 *	CALL	ABORT([s])
 *
 *		s	Optional Fortran Character string or NULL-terminated
 *			Hollerith.
 */

void
ABORT(s)
_fcd	s;
{
	char	*msg;
	int	len;

	if (_abortflag != 0)
		abort();		/* prevent infinite recursion */

	_abortflag = 1;

	if (_numargs() > 0) {
#ifdef	_ADDR64
		if (_numargs() * sizeof(long) >= sizeof(_fcd)) {
#else
		if (_isfcd(s)) {	/* If Fortran character */
#endif
			msg	= _fcdtocp(s);
			len	= _fcdlen (s);
		}
		else {
			msg	= *(char **) &s;
			len	= strlen(msg);
		}

		(void) write(fileno(stderr), msg, len);

		(void) write(fileno(stderr), "\n", 1);
	}

#ifdef	_CRAY2
	(void) _tracebk();
#endif
	abort();
}
