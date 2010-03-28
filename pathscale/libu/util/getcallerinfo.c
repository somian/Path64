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


#pragma ident "@(#) libu/util/getcallerinfo.c	92.1	07/07/99 13:18:33"

#include <errno.h>
#include <fortran.h>
#include <liberrno.h>

#define	BLANK	((int) ' ')
#define	MAXNAME	256

/*
 * NAME
 *
 *	GETCALLERINFO - Returns the name and line number of the calling
 *		routine.
 *
 * SYNOPSIS
 *
 *	INTEGER ILEN, LINENO, IERROR
 *	CHARACTER * (n), NAME
 *
 *	CALL GETCALLERINFO(NAME, ILEN, LINENO, IERROR)
 *
 * DESCRIPTION
 *
 *	The GETCALLERINFO subroutine returns the name of the subroutine
 *	and the line number from which the current routine was called.
 *	The following is a list of arguments to GETCALLERINFO: 
 *
 *	NAME	A variable of type CHARACTER which will receive the name
 *		of the calling subroutine.
 *
 *	ILEN	An integer variable that will receive the actual number
 *		of characters in the caller's name.
 *
 *	LINENO	An integer variable that will receive the line number of
 * 		the calling subroutine.
 *
 *	IERROR	An integer variable that will receive an exit status
 *		(see below).
 *
 * EXIT STATUS
 *
 *	Upon successful completion of GETCALLERINFO, IERROR is set to 0.
 *	If any of the following conditions occur, IERROR is set to the
 *	corresponding value:
 *
 *	[EINVAL]	An internal error prevented recovery of the caller
 *			information.
 *
 *	[ETRUNC]	The caller's name was longer than the length of the
 *			character variable provided by the user.  As much of
 *			the caller's name as will fit is returned.
 */

void
GETCALLERINFO(
	_fcd	name,		/* Variable to receive caller's name	*/
	_f_int	*ilen,		/* Variable to receive name length	*/
	_f_int	*lineno,	/* Variable to receive line number	*/
	_f_int	*ierror		/* Variable to receive error status	*/
)
{
	int		len1, len2;
	register int	i, ierr;
	char		tnam[MAXNAME];
	char		*str;

	ierr	= 0;			/* Assume no error */

	str	= _fcdtocp(name);	/* Character pointer to variable */
	len1	= _fcdlen(name);	/* Length of character variable */

	len2	= _who_called_me(lineno, tnam, MAXNAME, 2);

	if (len2 < 0) {
		ierr	= EINVAL;
		len2	= 0;
	}
	else {
		if (len1 < len2) {
			ierr	= ETRUNC;
			len2	= len1;
		}

		for (i = 0; i < len2; i++)
			str[i]	= tnam[i];
	}

	for (i = len2; i < len1; i++)
		str[i]	= BLANK;

	*ilen	= len2;
	*ierror	= ierr;

	return;
}
