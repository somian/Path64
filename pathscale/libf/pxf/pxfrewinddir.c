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


#pragma ident "@(#) libf/pxf/pxfrewinddir.c	92.1	06/29/99 11:36:06"

#include <errno.h>
#include <liberrno.h>
#include <fortran.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdlib.h> /* pick up NULL definition */
#include "table.h"

/*  PXFREWINDDIR  -- rewind directory
 *  (section 5.1.1 of Posix 1003.9-1992)
 *
 *  Synopsis:
 *     SUBROUTINE PXFREWINDDIR(IDIRID,IERROR)
 *     INTEGER IDIRID,IERROR
 *
 *  Function description:
 *  The PXFREWINDDIR routine uses rewinddir(3C) to reset the position in the
 *  directory stream to the first entry of directory stream while updating
 *  the directory stream to the current state of the directory, as a call
 *  PXFOPENDIR would do.
 *
 *  Description of arguments:
 *  IDIRID is an input integer variable containing the unique ID for a
 *         directory opened by PXFOPENDIR.
 *
 *  IERROR is an output integer variable that will contain the status:
 *
 *	   EBADID    The IDIRID argument is invalid.
 *
 */

#ifdef _UNICOS
void
PXFREWINDDIR(
#else
void
_PXFREWINDDIR(
#endif
	      _f_int *IDIRID,
	      _f_int *IERROR
)
{
  DIR *dirp;

  dirp = _table_lookup(&_pxfdir_table, *IDIRID);
  if (dirp == NULL) {
    *IERROR = EBADID;
  } else {
    rewinddir(dirp);
    *IERROR = 0;
  }
}

#ifndef _UNICOS
void
pxfrewinddir_(
	      _f_int *IDIRID,
	      _f_int *IERROR
)
{
  _PXFREWINDDIR(IDIRID,IERROR);
}
#endif
