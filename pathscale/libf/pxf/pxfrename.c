/*
 * Copyright 2004, 2005, 2006 PathScale, Inc.  All Rights Reserved.
 */

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


#pragma ident "@(#) libf/pxf/pxfrename.c	92.1	06/29/99 11:36:06"

#include <errno.h>
#include <fortran.h>
#include <liberrno.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

#ifndef _UNICOS
#include <stddef.h>
#endif

extern char *_fc_acopy(_fcd f);

/*
 *	PXFRENAME	Rename a file
 *
 *	Call from Fortran:
 *
 *		SUBROUTINE PXFRENAME(OLDNM, LENOLD, NEWNM, LENNEW, IERROR)
 *		CHARACTER * (*) OLDNM, NEWNM
 *		INTEGER LENOLD, LENNEW, IERROR
 *
 *	Where:
 *
 *	OLDNM	is an input character variable or array element
 *		containing the current name of a file.
 *
 *	LENOLD  is an input integer variable containing the length
 *		of OLDNM in characters.  If LENOLD is zero, all
 *		trailing blanks are removed before calling rename().
 *
 *	NEWNM 	is an input character variable or array element
 *		containing the new name of the file.
 *
 *	LENNEW  is an input integer variable containing the length
 *		of NEWNM in characters.  If LENNEW is zero, all
 *		trailing blanks are removed before calling rename().
 *
 *	IERROR	is an output integer variable that will contain the
 *		status:  Zero if PXFRENAME is successful; otherwise
 *		nonzero.
 *
 *		In addition to the error statuses returned by the
 *		rename(2) system call, PXFRENAME may return the
 *		following error statuses:
 *
 *		EINVAL	If LENOLD < 0 or LENOLD > LEN(OLDNM) or
 *		      	LENNEW < 0 or LENNEW > LEN(NEWNM).
 *
 *		ENOMEM	If PXFRENAME is unable to obtain memory to
 *			copy OLDNM or NEWNM.
 */

#ifdef _UNICOS
void
PXFRENAME(
#else
void
_PXFRENAME(
#endif
	_fcd	OLDNM,		/* Current file Name */
	_f_int	*LENOLD,	/* Length of OLDNM name or zero */
	_fcd	NEWNM,		/* New file Name */
	_f_int	*LENNEW,	/* Length of NEWNM name or zero */
	_f_int	*IERROR		/* Error status */
)
{
	int	arglenold, lengold;
	int	arglennew, lengnew;
	int	errsts;
	char	*argstrold, *pthstrold;
	char	*argstrnew, *pthstrnew;

	errsts	= 0;
	argstrold	= _fcdtocp(OLDNM);
	arglenold	= _fcdlen (OLDNM);
	lengold	= *LENOLD;
	argstrnew	= _fcdtocp(NEWNM);
	arglennew	= _fcdlen (NEWNM);
	lengnew	= *LENNEW;

	if ((lengold < 0 || lengold > arglenold) ||
		((lengnew < 0 || lengnew > arglennew)))
		errsts	= EINVAL;
	else {

		/*
		 * If length is zero, user wants trailing blanks stripped.
		 * Otherwise, malloc memory and copy the string; adding a
		 * NULL terminator.
		 */

		if (lengold == 0)

			pthstrold	= _fc_acopy(OLDNM);

		else
			pthstrold	= (char *) malloc(lengold + 1);

		if (lengnew == 0)

			pthstrnew	= _fc_acopy(NEWNM);

		else
			pthstrnew	= (char *) malloc(lengnew + 1);

		/* if no memory allocated */
		if (pthstrold == NULL || pthstrnew == NULL) {
			errsts	= ENOMEM;
			if (pthstrold != NULL)
				free(pthstrold);
			if (pthstrnew != NULL)
				free(pthstrnew);

		} else {
			if (lengold != 0) {	/* Copy argument */
				(void) memcpy(pthstrold, argstrold, lengold);
				pthstrold[lengold]	= '\0';
			}
			if (lengnew != 0) {	/* Copy argument */
				(void) memcpy(pthstrnew, argstrnew, lengnew);
				pthstrnew[lengnew]	= '\0';
			}

			/* rename the files */

			if (rename(pthstrold,pthstrnew) == -1)
				errsts	= errno;

			free(pthstrold);
			free(pthstrnew);
		}
	}
	*IERROR	= errsts;
	return;
}

#ifndef _UNICOS

void
pxfrename_(
	char	*OBUF,		/* character var of OLD name	*/
	_f_int	*LENOLD,	/* Length of OLDNM name or zero */
	char	*NBUF,		/* character var of NEW name	*/
	_f_int	*LENNEW,	/* Length of NEWNM name or zero */
	_f_int	*IERROR,	/* Error status			*/
	int	lenOL,		/* CHAR Length of OLD name	*/
	int	lenNW		/* CHAR Length of NEW name	*/
)
{
	_PXFRENAME(_cptofcd(OBUF,lenOL), LENOLD, _cptofcd(NBUF,lenNW),
		   LENNEW, IERROR);
	return;
}

#endif
