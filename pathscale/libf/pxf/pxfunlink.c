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


#pragma ident "@(#) libf/pxf/pxfunlink.c	92.1	06/29/99 11:36:06"

#include <errno.h>
#include <fortran.h>
#include <liberrno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#ifndef _UNICOS
#include <stddef.h>
#endif

extern char *_fc_acopy(_fcd f);

/*
 *	PXFUNLINK	Remove a directory entry
 *
 *	Call from Fortran:
 *
 *		SUBROUTINE PXFUNLINK (PATH, ILEN, IERROR)
 *		CHARACTER * (*) PATH
 *		INTEGER ILEN, IERROR
 *
 *	Where:
 *
 *	PATH	is an input character variable or array element
 *		containing the name of the directory entry to be
 *		removed.
 *
 *	ILEN	is an input integer variable containing the length
 *		of PATH in characters.  If ILEN is zero, any and
 *		all trailing blanks are removed.
 *
 *	IERROR	is an output integer variable that will contain the
 *		status:  Zero if PXFUNLINK is successful; otherwise
 *		nonzero.
 *
 *		In addition to the error statuses returned by the
 *		unlink(2) system call, PXFUNLINK may return the
 *		following error statuses:
 *
 *		EINVAL	If ILEN < 0 or ILEN > LEN(PATH)
 *
 *		ENOMEM	If PXFUNLINK is unable to obtain memory to
 *			copy PATH.
 */

#ifdef _UNICOS
void
PXFUNLINK(
#else
void
_PXFUNLINK(
#endif
	_fcd	PATH,		/* Character variable containing argument */
	_f_int	*ILEN,		/* Significant length of argument */
	_f_int	*IERROR		/* Error status */
)
{
	int	arglen, errsts, length;
	char	*argstr, *pthstr;

	errsts	= 0;
	argstr	= _fcdtocp(PATH);
	arglen	= _fcdlen (PATH);
	length	= *ILEN;

	if (length < 0 || length > arglen)
		errsts	= EINVAL;
	else {

		/*
		 * If length is zero, user wants trailing blanks stripped.
		 * Otherwise, malloc memory and copy the string; adding a
		 * NULL terminator.
		 */

		if (length == 0)
			pthstr	= _fc_acopy(PATH);
		else
			pthstr	= (char *) malloc(length + 1);

		if (pthstr == NULL)	/* If no memory allocated */
			errsts	= ENOMEM;
		else {

			if (length != 0) {	/* Copy argument */
				(void) memcpy(pthstr, argstr, length);
				pthstr[length]	= '\0';
			}

			/* Unlink the file */

			if (unlink(pthstr) == -1)
				errsts	= errno;

			free(pthstr);
		}
	}

	*IERROR	= errsts;

	return;
}

#ifndef _UNICOS
void
pxfunlink_ (
	char	*PATH,		/* Character variable containing argument */
	_f_int	*ILEN,		/* Significant length of argument */
	_f_int	*IERROR,	/* Error status */
	_f_int  pathlen
)
{
  _PXFUNLINK(_cptofcd(PATH, pathlen), ILEN, IERROR);
}
#endif
