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

#pragma ident "@(#) libu/clib/rename.c	92.1	07/01/99 13:42:20"
#include <fortran.h>
#include <malloc.h>
#include <stdio.h>

/*
 *	RENAME  - Fortran interface to rename(2) system call
 *   
 *      Call from Fortran:
 *		CHARACTER * (*), OLD, NEW
 *              CALL RENAME(OLD, NEW)
 *
 *                     -or-
 *
 *              CHARACTER * (*), OLD, NEW
 *              INTEGER RENAME
 *              I = RENAME(OLD, NEW)
 *
 *                     -or-
 *
 *              INTEGER OLD,NEW
 *              CALL RENAME(OLD, NEW)
 *
 *                     -or-
 *
 *              INTEGER OLD, NEW, RENAME
 *              I = RENAME(OLD, NEW)
 *      Arguments:
 *              OLD - path name of file to be renamed
 *              NEW - new path name of the file
 *      Returns:
 *              0  - success
 *              -1 - failure (see rename(2) for more information)
 */

_f_int
RENAME(old, new)
long	old, new;
{
	char	*oldptr, *newptr;
	int	ret;

	if (_numargs() < 2)
		return( (_f_int) -1);

	/* Process old parameter */

	if (_isfcd(old)) {	/* If old is a Fortran character */
		_fcd	fcdold;

		fcdold	= *(_fcd *) &old;
		oldptr	= _f2ccpy(fcdold);

		if (oldptr == NULL)
			return( (_f_int) -1);
	}
	else			/* Old is a hollerith */
		oldptr	= (char *) old;

	/* Process new parameter */

	if (_isfcd(new)) {	/* If new is a Fortran character */
		_fcd	fcdnew;

		fcdnew	= *(_fcd *) &new;
		newptr	= _f2ccpy(fcdnew);

		if (newptr == NULL) {

			if (_isfcd(old))
				free(oldptr);

			return( (_f_int) -1);
		}
	}
	else			/* New is a hollerith */
		newptr	= (char *) new;

	/* Do rename() */

	ret	= rename(oldptr, newptr);

	/* Free memory */

	if (_isfcd(old))
		free(oldptr);

	if (_isfcd(new))
		free(newptr);

	return( (_f_int) ret);
}
