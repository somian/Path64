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


#pragma ident "@(#) libf/pxf/pxfaccess.c	92.1	06/29/99 11:36:06"

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
 *	PXFACCESS	PXF Interface to the access(2) system call
 *	         	to check accessibility of a named file
 *
 *	Call from Fortran:
 *
 *		SUBROUTINE PXFACCESS (PATH, ILEN, IAMODE, IERROR)
 *		CHARACTER * (*) PATH
 *		INTEGER ILEN, IAMODE, IERROR
 *
 *	Where:
 *
 *	PATH	is an input character variable or array element
 *		containing the name of a file.  ACCESS checks the
 *		permissions in IAMODE with the current permissions
 *		for the file in PATH.
 *
 *	ILEN	is an input integer variable containing the length
 *		of PATH in characters.  If ILEN is zero, all trailing
 *		blanks are removed before calling ACCESS.
 *
 *	IAMODE	is an input integer variable containing the integer
 *		value of the symbolic constant for one or more of the
 *		following permissions:  R_OK,  W_OK,  X_OK, or  F_OK.
 *		An integer value for each of these symbolic constants
 *		is retrieved through the use of PXFCONST or IPXFCONST.
 *		The integer values may be combined through the use of
 *		a bitwise inclusive OR function.
 *
 *	IERROR	is an output integer variable that will contain the
 *		status:
 *
 *		  Zero    - PXFACCESS is successful, i.e., the
 *		            requested access is permitted.
 *
 *		  Nonzero - PXFACCESS is not successful.
 *
 *		In addition to the error statuses returned by the
 *		access(2) system call, PXFACCESS may return one of
 *		the following error statuses:
 *
 *		EINVAL	If ILEN < 0 or ILEN > LEN(PATH)
 *
 *		ENOMEM	If PXFACCESS is unable to obtain memory to
 *			copy PATH.
 */

#ifdef _UNICOS
void
PXFACCESS(
#else
void
_PXFACCESS(
#endif
	_fcd	PATH,		/* Character variable containing argument */
	_f_int	*ILEN,		/* Significant length of character argument */
	_f_int	*IAMODE,	/* bitwise inclusive OR of access permissions */
	_f_int	*IERROR		/* Error status */
)
{
	int	arglen, errsts, length;
	char	*argstr, *pthstr;
	int	permis;

	errsts	= 0;
	argstr	= _fcdtocp(PATH);
	arglen	= _fcdlen (PATH);
	length	= *ILEN;
	permis	= *IAMODE;

	if (length < 0 || length > arglen)
		errsts	= EINVAL;
	else {

		/*
		 * If length is zero, user wants trailing blanks stripped.
		 * Otherwise, malloc memory and copy the string; adding a
		 * NULL terminator.
		 */

		if (length == 0)

			pthstr  = _fc_acopy(PATH);

		else
			pthstr	= (char *) malloc(length + 1);

		if (pthstr == NULL)	/* If no memory allocated */
			errsts	= ENOMEM;
		else {

			if (length != 0) {	/* Copy argument */
				(void) memcpy(pthstr, argstr, length);
				pthstr[length]	= '\0';
			}

			/* Check permissions of the file through access() */

			if (access(pthstr,permis) == -1)
				errsts	= errno;
			free(pthstr);
		}
	}

	*IERROR	= errsts;

	return;
}

#ifndef _UNICOS
/*
 *	PXFACCESS	PXF Interface to the access(2) system call
 *	         	to check accessibility of a named file
 *	Allow on SPARC systems with its f77 subroutine interface.
 */

void
pxfaccess_(
	char	*PATH,		/* Character variable containing argument */
	_f_int	*ILEN,		/* Significant length of character argument */
	_f_int	*IAMODE,	/* bitwise inclusive OR of access permissions */
	_f_int	*IERROR,	/* Error status */
	int	lenpath
)
{
	_PXFACCESS( _cptofcd(PATH, lenpath), ILEN, IAMODE, IERROR);
	return;
}
#endif
