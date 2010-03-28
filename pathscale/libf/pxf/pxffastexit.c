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

#include <unistd.h>

#pragma ident "@(#) libf/pxf/pxffastexit.c	92.1	06/29/99 11:36:06"


/*
 *  PXFFASTEXIT -- Terminate a process
 *  (section 3.2.2 of Posix 1003.9-1992)
 *
 *  Synopsis:
 *
 *     SUBROUTINE PXFFASTEXIT(istatus)
 *     INTEGER istatus
 *
 *  Where:
 *
 *	istatus - default integer input variable containing the
 *	          status to be returned to the parent process of
 *	          the process.
 *
 *  PXFFASTEXIT never returns.
 *
 *  DESCRIPTION:
 *
 *  The subroutine procedure PXFFASTEXIT uses _exit() to
 *  terminate a process.
 *
 */

#include <fortran.h>
#include <stdlib.h>

#ifdef _UNICOS
void
PXFFASTEXIT(
#else	/* _UNICOS */
void
_PXFFASTEXIT(
#endif	/* _UNICOS */
	_f_int *istatus
)
{
	_exit(*istatus);
}

#ifndef _UNICOS
void
pxffastexit_(
	_f_int *istatus
)
{
	_PXFFASTEXIT(istatus);
}
#endif	/* not _UNICOS */
