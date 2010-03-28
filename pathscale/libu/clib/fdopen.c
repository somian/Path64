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

#pragma ident "@(#) libu/clib/fdopen.c	92.1	07/01/99 13:42:20"
#include <fortran.h>
#include <malloc.h>
#include <stdio.h>

/*
 *	FDOPEN - Fortran interface to fdopen(3) routine.
 *
 *	Call from Fortran:
 *
 *		INTEGER FILDES
 *		CHARACTER * (*) TYPE
 *		CALL FDOPEN(FILDES, TYPE)
 *
 *			-or-
 *		INTEGER FILDES, FDOPEN
 *		CHARACTER * (*) TYPE
 *		I = FDOPEN(FILDES, TYPE)
 *
 *			-or-
 *		INTEGER FILDES,TYPE
 *		CALL FDOPEN(FILDES, TYPE)
 *
 *			-or-
 *		INTEGER FILDES, FDOPEN, TYPE
 *		I = FDOPEN(FILDES, TYPE)
 */

_f_int
FDOPEN(fildes, type)
long	*fildes;
long	type;
{
	char	*typeptr;
	int	ret;
	int isfcd = 0;

	if (_numargs() < 2)
		return( (_f_int) 0);

#ifdef _ADDR64
	if (_numargs() *sizeof(long) == 1 + sizeof(_fcd)) { 
#else
	if (_isfcd(type)) {	/* If Fortran character */
#endif
		_fcd	fcdtype;
	
		isfcd = 1;
		fcdtype	= *(_fcd *) &type;
		typeptr	= _f2ccpy(fcdtype);

		if (typeptr == NULL)
			return( (_f_int) 0);
	}
	else			/* Hollerith */
		typeptr	= (char *) type;

	ret	= (int) fdopen(*fildes, typeptr);

	if (isfcd)
		free(typeptr);

	return( (_f_int) ret);
}
