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

#pragma ident "@(#) libu/clib/gethost.c	92.1	07/01/99 13:42:20"
#include <fortran.h>
#include <memory.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define	BLANK	((int) ' ')

/*
 *	GETHOST	Returns the name of the host mainframe
 *
 *	Call from Fortran:
 *
 *		CHARACTER * (*), HOST
 *			...
 *      	CALL GETHOST(HOST)
 *
 *			- or -
 *
 *		CHARACTER * (*), HOST
 *		INTEGER GETHOST, I
 *			...
 *		I = GETHOST(HOST)
 *
 *	Result:
 *
 *		HOST	Variable of type CHARACTER to receive the host
 *			name
 *
 *		I	Return value, type INTEGER, when called as a function
 *
 *			 -2	GETHOST called with no argument or called
 *				with an argument not of type CHARACTER
 *
 *			 -1	gethostname(3W) system request failed.  The
 *				HOST variable will be set to all blanks.
 *
 *			 >0	Number of characters in the host name.
 *
 *		The HOST character variable will receive the name of the
 *		current host, blank filled.  It should be at least as large
 *		as the host name.
 *
 *		Refer to the gethostname(3W) man page for a description of
 *		the gethostname(3W) request.
 */

_f_int
GETHOST (host)
_fcd	host;				/* Fortran character descriptor */
{
	int	rtrn, size;
	char	*cptr;

	if (_numargs() != sizeof(_fcd)/sizeof(long))
		rtrn	= -2;				/* Invalid call */
	else {

		cptr		= _fcdtocp(host);
		size		= _fcdlen(host);
		cptr[size-1]	= '\0';

		if (gethostname(cptr, size) == -1) {
			(void) memset(cptr, BLANK, size);
			rtrn	= -1;			/* Request failed */
		}
		else
			if (cptr[size-1] == '\0') {
				rtrn	= strlen(cptr);
				(void) memset(cptr + rtrn, BLANK, size - rtrn);
			}
			else
				rtrn	= size;
	}

	return( (_f_int) rtrn);
}
