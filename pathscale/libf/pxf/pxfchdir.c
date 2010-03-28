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


#pragma ident "@(#) libf/pxf/pxfchdir.c	92.1	06/29/99 11:36:06"
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
 *      PXFCHDIR        Change from the current directory to the
 *                      specified directory.
 *
 *      Call from Fortran:
 *
 *              SUBROUTINE PXFCHDIR (PATH, ILEN, IERROR)
 *              CHARACTER * (*) PATH
 *              INTEGER ILEN, IERROR
 *
 *      Where:
 *
 *      PATH    is an input character variable or array element
 *              containing the name of the new directory entry.
 *
 *      ILEN    is an input integer variable containing the length
 *              of PATH in characters.  If ILEN is zero, any and
 *              all trailing blanks are removed.
 *
 *      IERROR  is an output integer variable that will contain the
 *              status:  Zero if PXFCHDIR is successful; otherwise
 *              nonzero.
 *
 *              In addition to the errors returned by the chdir(2)
 *              system call, PXFCHDIR may return the following errors:
 *
 *              EINVAL  If ILEN < 0 or ILEN > LEN(PATH)
 *
 *              ENOMEM  If PXFCHDIR is unable to obtain memory to
 *                      copy PATH.
 */

#ifdef _UNICOS
void
PXFCHDIR(
#else
void
_PXFCHDIR(
#endif
	_fcd	PATH,		/* Character variable of new dir path */
	_f_int	*ILEN,		/* Length of pathname */
	_f_int	*IERROR)	/* Error Status */
{
	_f_int	arglen, errsts, length;
	char	*argstr, *pthstr;
	errsts	= 0;
	argstr	= _fcdtocp(PATH);
	arglen	= _fcdlen (PATH);
	length  = *ILEN;
	if (length < 0 || length > arglen)
		errsts  = EINVAL;
	else {
		/*
		 * If length is zero, strip trailing blanks. Otherwise,
		 * malloc memory and copy the string; adding a NULL
		 * terminator.
		 */
		if (length == 0) {

			pthstr  = _fc_acopy(PATH);

		} else {
			pthstr  = (char *) malloc(length + 1);
		}
		if (pthstr == NULL)     /* If no memory allocated */
			errsts  = ENOMEM;
		else {
			if (length != 0) {      /* Copy argument */
				(void) memcpy(pthstr, argstr, length);
				pthstr[length]  = '\0';
			}
			/* Change directories through chdir() to path. */
			if (chdir(pthstr) == -1)
				errsts  = errno;
			free(pthstr);
		}
	}
	*IERROR = errsts;
	return;
}

#ifndef _UNICOS
/*
 *      PXFCHDIR        Change from the current directory to the
 *                      specified directory.
 *      Allow on SPARC systems with its f77 subroutine interface
 */

void
pxfchdir_(
	char	*PATH,		/* Character ptr to new dir path */
	_f_int	*ILEN,		/* Length of pathname */
	_f_int	*IERROR,	/* Error Status */
	int	lenpath)
{
	_PXFCHDIR( _cptofcd(PATH, lenpath), ILEN, IERROR);
	return;
}
 
#endif
