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



#pragma ident "@(#) libf/fort/f90abort.c	92.2	10/06/99 12:11:07"

#include <fortran.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "defalias.h"

static int __abortflag = 0;	/* abort flag set if ABORT has been called */

/*	ABORT - User-callable abort routine
 *
 *	CALL ABORT([s])
 *
 *	where:
 *		s	Optional Fortran Character String
 *
 *	This equates to the following calling sequence:
 *
 *	call abort_(char *msg, int *len)
 *
 *	where:
 *		char* msg	Character pointer to string, Null if
 *				not present.
 *		int* len	Pointer to the length of the string.
 */

#if	!defined(__mips) && !defined(_LITTLE_ENDIAN)
void
abort_(char *msg, _f_int len)
{
	if(__abortflag !=0)
		abort();		/* prevent infinite recursion */
	__abortflag = 1;
	if (msg != NULL) {
		(void)write(fileno(stderr), msg, len);
		(void)write(fileno(stderr), "\n", 1);
	}
	abort();
}

#else
void
abort_msg__(char *msg, _f_int len)
{
	if(__abortflag !=0)
		abort();		/* prevent infinite recursion */
	__abortflag = 1;
	if (msg != NULL) {
		(void)write(fileno(stderr), msg, len);
		(void)write(fileno(stderr), "\n", 1);
	}
	abort();
}

#if defined(BUILD_OS_DARWIN)
/* Mach-O doesn't support aliases */
void abort_msg_(char *msg, _f_int len) {
  abort_msg__(msg, len);
}
#else /* defined(BUILD_OS_DARWIN) */
defalias(abort_msg__, abort_msg_);
#endif /* defined(BUILD_OS_DARWIN) */

void
abort_(void)
{
	abort();
}
#endif
