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

#pragma ident "@(#) libu/clib/freopen.c	92.1	07/01/99 13:42:20"
#include <fortran.h>
#include <malloc.h>
#include <stdio.h>
#include <liberrno.h>

/*
 *	FREOPEN - Fortran interface to freopen(3)
 *
 *	Call from Fortran:
 *
 *		CHARACTER * (*) NAME, MODE
 *		INTEGER FP
 *		CALL FREOPEN(NAME, MODE, FP)
 *
 *			-or-
 *
 *		CHARACTER * (*) NAME, MODE
 *		INTEGER FP, FREOPEN
 *		I =  FREOPEN(NAME, MODE, FP)
 *
 *			-or (on non-ADDR64 systems only) -
 *
 *		INTEGER NAME, MODE, FP, FREOPEN
 *		I =  FREOPEN(NAME, MODE, FP)
 *
 *			-or (on non-ADDR64 systems only) -
 *
 *		INTEGER NAME, MODE, FP
 *		CALL FREOPEN(NAME, MODE, FP)
 *
 */

_f_int
FREOPEN(name, mode, fp)
#ifndef _ADDR64
long	name, mode;
#else
_fcd name, mode;
#endif
long	**fp;
{
	char	*modeptr, *nameptr;
	int	ret;

	if (_numargs() != 1 + 2 * sizeof(_fcd)/sizeof(long))
		_lerror(_LELVL_ABORT,FEARGLST,"FREOPEN");

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
	/* Do freopen() */

	ret	= (int) freopen(nameptr, modeptr, (FILE *) (*fp));

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
