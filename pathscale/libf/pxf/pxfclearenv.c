/*
 * Copyright (C) 2007. PathScale, LLC. All Rights Reserved.
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


#pragma ident "@(#) libf/pxf/pxfclearenv.c	92.1	06/29/99 11:36:06"

#include <fortran.h>
#include <errno.h>
#include <liberrno.h>
#include <stdlib.h>

#ifndef _UNICOS
#include <stddef.h>
#endif

#if defined(BUILD_OS_DARWIN)
extern char **environ;
#else /* defined(BUILD_OS_DARWIN) */
extern char **_environ;
#endif /* defined(BUILD_OS_DARWIN) */

/*
 *  PXFCLEARENV  -- clear all environment variables
 *  (section 4.6.1 of Posix 1003.9-1992)
 *
 *  Call from Fortran:
 *
 *      SUBROUTINE PXFCLEARENV(IERROR)
 *      INTERGER IERROR
 *
 *  Where:
 *
 *  IERROR      is an output integer variable that will contain
 *              the status:
 *
 *               zero    - all environment variables were cleared.
 *
 */

#ifdef _UNICOS
void
PXFCLEARENV(
#else
void
_PXFCLEARENV(
#endif
	     _f_int *IERROR
)
{
  int i;

#if defined(BUILD_OS_DARWIN)
  for(i=0; environ[i] != NULL; i++) {
    environ[i] = NULL;
  }
#else /* defined(BUILD_OS_DARWIN) */
  for(i=0; _environ[i] != NULL; i++) {
    _environ[i] = NULL;
  }
#endif /* defined(BUILD_OS_DARWIN) */

  *IERROR = 0;
}

#ifndef _UNICOS
void
pxfclearenv_(
	     _f_int *IERROR
)
{
  _PXFCLEARENV(IERROR);
}
#endif





