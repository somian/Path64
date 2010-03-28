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


#pragma ident "@(#) libf/pxf/pxfctermid.c	92.1	06/29/99 11:36:06"


#include <errno.h>
#include <liberrno.h>
#include <fortran.h>
#include <stdio.h>
#include <string.h>

#define BLANK ((int) ' ')

/*
 *  PXFCTERMID  -- Gernerate Terminal Pathname
 *  (section 4.7.1 of Posix 1003.9-1992)
 *
 *  Synopsis:
 *
 *     SUBROUTINE PXFCTERMID(S,ILEN,IERROR)
 *     CHARACTER*(*) S
 *     INTEGER ILEN,IERROR
 *
 *  Function description:
 *
 *     The routine PXFCTERMID uses ctermid() [(3S) on IRIX and Solaris
 *     systems and (3C) on UNICOS systems] to generate a string which
 *     is the pathname for the current process' controlling terminal.
 *     If the pathname for the controlling terminal cannot be determined,
 *     the ILEN variable is set to zero.
 *
 *  Description of Arguments:
 *
 *     S      is an output character array or character element variable
 *            for the pathname for the current process' controlling
 *            terminal.
 *
 *     ILEN   is an output interger variable for the length of S.
 *
 *     IERROR is an output integer variable for the completion status of
 *            PXFCTERMID. IERROR may contain:
 *
 *             zero    - PXFCTERMID was successful.
 *
 *             nonzero - PXFCTERMID was not successful.
 *
 *            PXFCTERMID may return any of the following error
 *            values:
 *
 *             ETRUNC If the output variable S cannot contain the pathname
 *                    for the current process' controlling termina and so
 *                    the pathname was truncated.
 *
 *
 */

#ifdef _UNICOS
void
PXFCTERMID(
#else
void
_PXFCTERMID(
#endif
	    _fcd S,
	    _f_int *ILEN,
	    _f_int *IERROR
)
{
  int sptrlen, slen, copylen;
  char *sptr;

  slen = _fcdlen(S);
  *IERROR = 0;

  if ((sptr = ctermid(NULL)) == NULL) {
    /* cannot determine controlling terminal for current process */
    *ILEN = 0;
  } else {
    /* check for truncation error condition */
    sptrlen = strlen(sptr);
    *ILEN = sptrlen;
    if (sptrlen > slen) {
      *IERROR = ETRUNC;
      copylen = slen;
    } else {
      copylen = sptrlen;
    }

    /* copy the string */
    (void)memcpy(_fcdtocp(S), sptr, copylen*sizeof(char));
    (void)memset(_fcdtocp(S) + sizeof(char)*copylen, BLANK,
		 (slen-copylen)*sizeof(char));
  }
}

#ifndef _UNICOS
void
pxfctermid_(
	    char *S,
	    _f_int *ILEN,
	    _f_int *IERROR,
	    _f_int slen
)
{
  _PXFCTERMID(_cptofcd(S,slen), ILEN, IERROR);
}
#endif



