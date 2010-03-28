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


#pragma ident "@(#) libf/pxf/pxfuname.c	92.1	06/29/99 11:36:06"

#include <errno.h>
#include <fortran.h>
#include <liberrno.h>
#include <string.h>
#include <sys/utsname.h>
#include "pxfstruct.h"
#include "table.h"

/*
 *	PXFUNAME	PXF interface to the uname(2) system call
 *			to return name(s) of current operating system
 *
 *	Call from Fortran:
 *
 *		SUBROUTINE PXFUNAME (JUNAMBUF, IERROR)
 *		INTEGER JUNAMBUF, IERROR
 *
 *      Where:
 *
 *      JUNAMBUF is an input integer variable or array element
 *               containing the pointer to the utsname structure.
 *
 *      IERROR  is an output integer variable that will contain the
 *              status:
 *
 *                Zero    - PXFUNAME is successful, i.e., the
 *                          requested operating system information
 *                          was returned.
 *
 *                Nonzero - PXFUNAME is not successful.
 *
 *                EBADHANDLE If JUNAMBUF is invalid.
 *
 */

#ifdef _UNICOS
void
PXFUNAME(
#else
void
_PXFUNAME(
#endif
	_f_int *JUNAMBUF,	/* handle for struct utsname	*/
	_f_int *IERROR)		/* error status			*/
{
	struct	utsname *utn;
	int	errsts = 0;
	struct pxfhandle pxfhand;

        pxfhand = _pxfhandle_table_lookup(&_pxfhandle_table, *JUNAMBUF);
        if (pxfhand.pxfstructptr == NULL || pxfhand.pxftype != PXF_UNAMBUF) {
          *IERROR = EBADHANDLE;
          return;
        }
	
	utn = pxfhand.pxfstructptr;
	if (uname (utn) == -1)
	  errsts = errno;
	*IERROR	= errsts;
} 


#ifndef _UNICOS
void
pxfuname_(
	  _f_int *JUNAMBUF,
	  _f_int *IERROR
)
{
  _PXFUNAME(JUNAMBUF, IERROR);
}
#endif
