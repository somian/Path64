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


#pragma ident "@(#) libf/pxf/pxfisatty.c	92.1	06/29/99 11:36:06"

#include <errno.h>
#include <liberrno.h>
#include <fortran.h>
#include <unistd.h>

#if defined(_SOLARIS) || defined(_MIPSEB)
#include <stdlib.h>
#endif

#if defined(_UNICOS) || defined(_MIPSEB)
#include <unistd.h>
#endif

/*
 *  PXFISATTY  -- determine if file descriptor corresponds
 *                to file descriptor
 *  (section 4.7.2 of Posix 1003.9-1992)
 *
 *  Synopsis:
 *
 *     SUBROUTINE PXFISATTY(IFILDES,ISATTY,IERROR)
 *     INTEGER IFILDES,IERROR
 *     LOGICAL ISATTY
 *
 *  Function definition:
 *  The routine PXFISATTY uses isatty(3C) to determine if
 *  IFILDES is a valid file descriptor for a terminal.
 *
 *  Description of arguments:
 *
 *  IFILDES is an input integer variable containing the
 *          file descriptor to be checked for an associated
 *          terminal.
 *
 *  ISATTY  is an output logical variable which is .TRUE.
 *          when IFILDES is a file descriptor with an
 *          associated terminal, otherwise ISATTY is
 *          .FALSE..
 *
 *  IERROR  is an output integer variable for PXFISATTY's
 *          completion status. IERROR may contain:
 *
 *           zero  - PXFISATTY was successful.
 *
 */

#ifdef _UNICOS
void
PXFISATTY(
#else
void
_PXFISATTY(
#endif
	   _f_int *IFILDES,
	   _f_log *ISATTY,
	   _f_int *IERROR
)
{
  *ISATTY = _btol(isatty((int)*IFILDES));
  *IERROR = 0;
}

#ifndef _UNICOS
void
pxfisatty_(
	   _f_int *IFILDES,
	   _f_log *ISATTY,
	   _f_int *IERROR
)
{
  _PXFISATTY(IFILDES,ISATTY,IERROR);
}
#endif
