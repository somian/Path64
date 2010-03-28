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

#pragma ident "@(#) libu/clib/putenv.c	92.1	07/01/99 13:42:20"
#include <fortran.h>
#include <stdlib.h>
#include <string.h>

_f_int
PUTENV (string)
long	string;
{
	char	*strptr;

	if (_numargs() < 1)
		return( (_f_int) -1);

#ifdef	_ADDR64
	if (_numargs() * sizeof(long) == sizeof(_fcd)) { /* If Fortran char */ 
#else
	if (_isfcd(string)) {	/* If Fortran character */
#endif
		_fcd	fcdstr;

		fcdstr	= *(_fcd *) &string;
		strptr	= _f2ccpy(fcdstr);

		if (strptr == NULL)
			return( (_f_int) -1);
	}
	else {			/* Hollerith */
		char	*tmpptr;

		tmpptr	= (char *) string;
		strptr	= malloc(strlen(tmpptr) + 1);

		if (strptr == NULL)
			return( (_f_int) -1);

		(void) strcpy(strptr, tmpptr);
	}

	return( (_f_int) putenv(strptr) );
}
