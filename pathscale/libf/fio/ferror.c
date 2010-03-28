/*
 * Copyright 2004, 2005, 2006 PathScale, Inc.  All Rights Reserved.
 */

/*

  Copyright (C) 2000, 2001, Silicon Graphics, Inc.  All Rights Reserved.

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



#pragma ident "@(#) libf/fio/ferror.c	92.2	06/18/99 18:41:40"

#include <liberrno.h>
#include <errno.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include "fio.h"

#ifdef	_CRAYMPP
static DECL_MPP_LOCK(ferrlock)	/* MPP: lock for _ferr() */
#endif

/*
 *	_ferr - Fortran statement run-time error processor
 *
 *	The funny ifdef'ing is used if _ferr is defined as a macro for
 *	debugging purposes.
 */
void
_ferr(FIOSPTR fiosp, int errn, ...)
{
	va_list	args;		/* Variable argument list */

#ifdef	_UNICOS
	if (_numargs() < 2)
		abort();
#endif

	MPP_LOCK(&ferrlock);		/* MPP: allow only 1 PE in _ferr() */

	fflush(stdout);
	fflush(stderr);			/* just in case */

	va_start(args, errn);

	_lmessage(errn, NULL, args);	/* Print error message */

	_fcontext(fiosp);		/* Print error context */

	va_end(args);

#if	defined(__mips) || defined(_LITTLE_ENDIAN)
	/* we don't have atabort() on Irix, but we do want to clean up */
	/* the Fortran units before aborting */
	_fcleanup();
#endif
	abort();
}
