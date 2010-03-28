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


#pragma ident "@(#) libf/pxf/pxfclosedir.c	92.1	06/29/99 11:36:06"

#include <errno.h>
#include <liberrno.h>
#include <fortran.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdlib.h>
#include "table.h"

/*  PXFCLOSEDIR  -- close directory
 *  (section 5.1.1 of Posix 1003.9-1992)
 *
 *  Synopsis:
 *     SUBROUTINE PXFCLOSEDIR(IDIRID,IERROR)
 *     INTEGER IDIRID,IERROR
 *
 *  Function description:
 *  The PXFCLOSEDIR routine uses closedir(3C) to close the directory
 *  stream referenced by IDIRID. Upon sucessful completion, IDIRID
 *  is undefined and the result of subsequent calls to PXFCLOSEDIR
 *  with IDIRID is not well defined.
 *
 *  Description of arguments:
 *  IDIRID is an input integer variable containing a unique ID for
 *         a directory stream created by PXFOPENDIR.
 *
 *  IERROR is an output integer variable that will contain the status:
 *
 *             zero    - PXFCLOSEDIR was successful.
 *
 *             nonzero - PXFCLOSEDIR was not successful.
 *
 *         PXFCLOSEDIR may return any of the following error
 *         values:
 *
 *          EBADF If, when detected, an invalid, unique directory
 *                stream ID was used for IDIRID.
 *
 *          EBADID The IDIRID argument is invalid.
 */


#ifdef _UNICOS
void
PXFCLOSEDIR(
#else
void
_PXFCLOSEDIR(
#endif
	     _f_int *IDIRID,
	     _f_int *IERROR
)
{
  int cdirid;
  DIR *dirp;

  /* make sure IDIRID is positive */
  if (*IDIRID < 0) {
    cdirid = -*IDIRID;
  } else {
    cdirid = *IDIRID;
  }

  dirp = (DIR *)_table_lookup(&_pxfdir_table, cdirid);

  if (dirp == NULL) {
    *IERROR = EBADID;
    return;
  }

  if (closedir(dirp) == -1) {
    *IERROR = errno;
    return;
  }

  _table_remove(&_pxfdir_table, *IDIRID);

  *IERROR = 0;
}

#ifndef _UNICOS
void
pxfclosedir_(
	     _f_int *IDIRID,
	     _f_int *IERROR
)
{
  _PXFCLOSEDIR(IDIRID,IERROR);
}
#endif
