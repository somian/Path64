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


#pragma ident "@(#) libf/pxf/pxfutime.c	92.1	06/29/99 11:36:06"

#include <errno.h>
#include <fortran.h>
#include <liberrno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <utime.h>
#include "pxfstruct.h"
#include "table.h"

#ifndef _UNICOS
#include <stddef.h>
#endif

extern char *_fc_acopy(_fcd f);

/*
 *	PXFUTIME	Sets access and modification times of a file
 *
 *  From section 5.6.6.1 of Posix 1003.9
 *
 *	Call from Fortran:
 *
 *		SUBROUTINE PXFUTIME (PATH, ILEN, JUTIMBUF, IERROR)
 *		CHARACTER * (*) PATH
 *		INTEGER ILEN, JUTIMBUF, IERROR
 *
 *	Where:
 *
 *	PATH	is an input character variable or array element
 *		containing the name of the file.
 *
 *	ILEN	is an input integer variable containing the length
 *		of PATH in characters.  If ILEN is zero, any and
 *		all trailing blanks are removed.
 *
 *	JUTIMBUF	is an input integer variable. It is a handle
 *		for a structure of type utimbuf
 *
 *	IERROR	is an output integer variable that will contain the
 *		status:  Zero if PXFUTIME is successful; otherwise
 *		nonzero.
 *
 *		In addition to the error statuses returned by the
 *		utime(2) system call, PXFUTIME may return the
 *		following error statuses:
 *
 *		EINVAL	If ILEN < 0 or ILEN > LEN(PATH)
 *
 *		ENOMEM	If PXFUTIME is unable to obtain memory to
 *			copy PATH.
 *
 *              EBADHANDLE If JUTIMBUF is invalid.
 */

#ifdef _UNICOS
void
PXFUTIME(
#else
void
_PXFUTIME(
#endif
	_fcd	PATH,		/* Character variable containing argument */
	_f_int	*ILEN,		/* Significant length of argument */
	_f_int	*JUTIMBUF,		/* handle for struct utimbuf */
	_f_int	*IERROR		/* Error status */
)
{
	int	arglen, errsts, length;
	char	*argstr, *pthstr;
	struct utimbuf *times;
	struct pxfhandle pxfhand;

	errsts	= 0;
	argstr	= _fcdtocp(PATH);
	arglen	= _fcdlen (PATH);
	length	= *ILEN;

	if (*JUTIMBUF == 0) {
	  pxfhand.pxfstructptr = NULL;
	} else {
	  pxfhand = _pxfhandle_table_lookup(&_pxfhandle_table, *JUTIMBUF);
	  if (pxfhand.pxfstructptr == NULL || pxfhand.pxftype != PXF_UTIMBUF) {
	    *IERROR = EBADHANDLE;
	    return;
	  }
	}

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
			
			times = pxfhand.pxfstructptr;
			
			/* call utime */
			if (utime(pthstr, times) == -1)
			  errsts	= errno;
			
			free(pthstr);
		}
	}
	
	*IERROR	= errsts;
	
	return;
}

#ifndef _UNICOS
void
pxfutime_(
	char	*PATH,		/* Character variable containing argument */
	_f_int	*ILEN,		/* Significant length of argument */
	_f_int	*JUTIMBUF,	/* handle for struct utimbuf */
	_f_int	*IERROR,	/* Error status */
	_f_int  pathlen
)
{
  _PXFUTIME(_cptofcd(PATH, pathlen), ILEN, JUTIMBUF, IERROR);
}
#endif
