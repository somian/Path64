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


#pragma ident "@(#) libf/pxf/pxfstat.c	92.1	06/29/99 11:36:06"

#include <errno.h>
#include <fortran.h>
#include <liberrno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include "pxfstruct.h"
#include "table.h"

/*
 *	PXFSTAT		PXF interface to the stat(2) system call
 *			to retrieve file status
 *
 *	Call from Fortran:
 *
 *		SUBROUTINE PXFSTAT (PATH, ILEN, JSTAT, IERROR)
 *		INTEGER ILEN, JSTAT, IERROR
 *
 *      Where:
 *
 *      PATH    is an input character variable or array element
 *              containing the name of the file.
 *
 *      ILEN    is an input integer variable containing the length
 *              of PATH in characters.  If ILEN is zero, all trailing
 *              blanks are removed before stat() is called.
 *
 *      JSTAT   is an input integer variable or array element
 *              containing the pointer to the stat structure.
 *
 *      IERROR  is an output integer variable that will contain the
 *              status:
 *
 *                Zero    - PXFSTAT is successful, i.e., the
 *                          requested file status was returned.
 *
 *                Nonzero - PXFSTAT is not successful.
 *
 *              In addition to the error statuses returned by the
 *              stat(2) system call, PXFSTAT may return the
 *              following error statuses:
 *
 *              EINVAL  If ILEN < 0 or ILEN > LEN(PATH)
 *
 *              ENOMEM  If PXFSTAT is unable to obtain memory to
 *                      copy PATH.
 *
 *              EBADHANDLE If JSTAT is invalid.
 *
 */

#ifdef _UNICOS
void
PXFSTAT(
#else
void
_PXFSTAT(
#endif
	_fcd	PATH,		/* file name */
	_f_int	*ILEN,		/* length of name in PATH or zero */
	_f_int  *JSTAT,	        /* handle for struct stat */
	_f_int	*IERROR) 	/* error status	*/
{
	int     arglen, errsts, length;
	char    *argstr, *pthstr;
	struct	stat *jst;
	struct pxfhandle pxfhand;

	errsts	= 0;
	argstr  = _fcdtocp(PATH);
	arglen  = _fcdlen (PATH);
	length  = *ILEN;

        pxfhand = _pxfhandle_table_lookup(&_pxfhandle_table, *JSTAT);
        if (pxfhand.pxfstructptr == NULL || pxfhand.pxftype != PXF_STATBUF) {
          *IERROR = EBADHANDLE;
          return;
        }	

	if (length < 0 || length > arglen)
		errsts  = EINVAL;
	else {
		/*
		 * If length is zero, user wants trailing blanks stripped.
		 * Otherwise, malloc memory and copy the string; adding a
		 * NULL terminator.
		 */
		if (length == 0)
			pthstr  = _fc_acopy(PATH);
		else
			pthstr  = (char *) malloc(length + 1);
		if (pthstr == NULL)     /* If no memory allocated */
			errsts  = ENOMEM;
		else {
			if (length != 0) {      /* Copy argument */
				(void) memcpy(pthstr, argstr, length);
				pthstr[length]  = '\0';
			}
			jst = pxfhand.pxfstructptr;
			if (stat (pthstr, jst) == -1)
				errsts	= errno;
			free(pthstr);
		}
	}
	*IERROR	= errsts;
	return;
}

#ifndef _UNICOS
void
pxfstat_(
         char   *PATH,          /* file name */
         _f_int *ILEN,          /* length of name in PATH or zero */
         _f_int *JSTAT,         /* handle for struct stat */
         _f_int *IERROR,        /* error status */
         _f_int pathlen
)
{
  _PXFSTAT(_cptofcd(PATH,pathlen), ILEN, JSTAT, IERROR);
}
#endif




