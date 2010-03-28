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

#pragma ident"@(#) libu/clib/fopen.c	92.1	07/01/99 13:42:20"
#include <fortran.h>
#include <malloc.h>
#include <stdio.h>
#include <liberrno.h>

/*
 *	FOPEN - Fortran interface to fopen(3) routine.
 *
 *	Call from Fortran:
 *
 *		CHARACTER * (*), NAME, MODE
 *              CALL FOPEN(NAME, MODE)
 *
 *			-or-
 *
 *		CHARACTER * (*), NAME, MODE
 *		INTEGER FOPEN
 *		I = FOPEN(NAME, MODE)
 *
 *			-or (on NON-ADDR64 systems only) -
 *
 *		INTEGER FOPEN, NAME, MODE
 *		I = FOPEN(NAME, MODE)
 *
 *			-or (on NON-ADDR64 systems only) -
 *
 *		INTEGER  NAME, MODE
 *		CALL FOPEN(NAME, MODE)
 *
 *	Return value:
 *
 *		 0 on error.
 *		>0 for successful completion.
 */

_f_int
FOPEN(name, mode)
#ifdef _ADDR64
_fcd name, mode;
#else
long	name, mode;
#endif
{
	char	*modeptr, *nameptr;
	int	ret;

	if (_numargs() != 2*sizeof(_fcd)/sizeof(long))
		_lerror(_LELVL_ABORT,FEARGLST,"FOPEN");

	/* Process name parameter */

#ifndef _ADDR64
	if (_isfcd(name)) {	/* If name is a Fortran character */
		_fcd	fcdname;

		fcdname	= *(_fcd *) &name;
		nameptr	= _f2ccpy(fcdname);

		if (nameptr == NULL)
			return( (_f_int) 0);
	}
	else			/* Name is a hollerith */
		nameptr	= (char *) name;

	/* Process mode parameter */

	if (_isfcd(mode)) {	/* If mode is a Fortran character */
		_fcd	fcdmode;

		fcdmode	= *(_fcd *) &mode;
		modeptr	= _f2ccpy(fcdmode);

		if (modeptr == NULL) {

			if (_isfcd(name))
				free(nameptr);

			return( (_f_int) 0);
		}
	}
	else			/* Mode is a hollerith */
		modeptr	= (char *) mode;
#else
	nameptr	= _f2ccpy(name);

	if (nameptr == NULL)
		return( (_f_int) 0);
	modeptr	= _f2ccpy(mode);

	if (modeptr == NULL) {

		free(nameptr);

		return( (_f_int) 0);
	}
#endif
	/* Do fopen() */

	ret	= (int) fopen(nameptr, modeptr);

	/* Free memory */

#ifndef _ADDR64
	if (_isfcd(name))
#endif
		free(nameptr);

#ifndef _ADDR64
	if (_isfcd(mode))
#endif
		free(modeptr);

	return( (_f_int) ret);
}
