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

#pragma ident "@(#) libu/clib/getenv.c	92.1	07/01/99 13:42:20"
#include <fortran.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>

/*
 *  GETENV  -- return value for environment name to the Fortran program
 *
 *  Call from Fortran:
 *
 *              CHARACTER * (*) NAME
 *              CHARACTER * (*) VALUE
 *              INTEGER GETENV
 *		I = GETENV(NAME, VALUE)
 *
 *			-or-
 *              CHARACTER * (*) NAME
 *              INTEGER VALUE(VALUESZ)
 *              INTEGER GETENV
 *		I = GETENV(NAME, VALUE, VALUESZ)
 *
 *		where
 *
 *		NAME	is a character variable containing the name of the
 *			environment variable for which GETENV searches in the 
 *			environment.  NAME can alternatively be an integer 
 *			variable or array containing the left justified name
 *			terminated by a 0 byte.
 *
 *		VALUE	is a character variable or integer variable or  
 *			array element to receive the value of the environment
 *			variable NAME.  
 *
 *		VALUESZ	is the length in words of VALUE if VALUE is of type
 *			integer.
 *
 */

_f_int
GETENV (name, value, mlv)   
long	name, value;
long	*mlv;      
{
	char		*envptr, *nameptr, *valptr;
	unsigned int	len, length, pad;
	int		ret;

	if (_numargs() < 2)
		return( (_f_int) -1);

	if (_isfcd(name)) {	/* If Fortran character */
		_fcd	fcdname;

		fcdname	= *(_fcd *) &name;
		nameptr	= _f2ccpy(fcdname);

		if (nameptr == NULL)
			return( (_f_int) 0);
	}
	else			/* Hollerith */
		nameptr	= (char *) name;

	/* Get environment variable */

	envptr	= getenv(nameptr);

	if (envptr != NULL)
		ret	= 1;
	else {
		envptr	= "";
		ret	= 0;
	}

	if (_isfcd(name))	/* If Fortran character */
		free(nameptr);

	if (_isfcd(value)) {	/* If destination is a Fortran character */
		_fcd	fcdval;

		fcdval	= *(_fcd *) &value;
		valptr	= _fcdtocp(fcdval);
		length	= _fcdlen (fcdval);
		pad	= (int) ' ';
	}
	else {
		if (_numargs() < 3)
			return( (_f_int) -1);

		valptr	= (char *) value;
		length	= *mlv * sizeof(long);
		pad	= 0;
	}

	(void) strncpy(valptr, envptr, length);

	/* Pad with blanks for character and nulls for hollerith */

	len	= strlen(valptr);
	valptr	= valptr + len;
	len	= length - len;

	if (len > 0)
		(void) memset(valptr, pad, len);

	return( (_f_int) ret);
}
