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

#pragma ident "@(#) libu/clib/fgets.c	92.1	07/01/99 13:42:20"

#include <stdio.h>
#include <fortran.h>
#include <stdarg.h>

/*	FGETS 
 *
 *	FGETS - Fortran interface to fgets
 *
 *	Call from Fortran:
 *		INTEGER BUF, N, FP
 *		CALL FGETS(BUF, N, FP)
 *		   -or-
 *		INTEGER BUF, N, FP, FGETS
 *		I = FGETS(BUF, N, FP)
 *		   -or-
 *		CHARACTER * (*) BUF
 *		INTEGER N, FP
 *		CALL FGETS(BUF, N, FP)
 *		   -or-
 *		CHARACTER * (*) BUF
 *		INTEGER  N, FP, FGETS
 *		I = FGETS(BUF, N, FP)
 */

_f_int
FGETS(long *buf, ...)
{
	va_list args;
	long *n;
	long **fp;
#ifdef _ADDR64
	long flen;

	va_start(args, buf);
	if (_numargs() * sizeof(long) == sizeof(_fcd) + sizeof(long *) +
		sizeof(long **)) {
		/* this technique is not recommended. It is used only */
		/* because our documentation says that we allow both */
		/* characters and integers */
		flen = va_arg(args, long);	/* get length of fcd */
	}
#else
	va_start(args, buf);
#endif
	n = va_arg(args, long *);
	fp = va_arg(args, long **);
	return((_f_int)fgets((char *)buf, (int)*n, (FILE *)(*fp)));
}

