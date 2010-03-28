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



#pragma ident "@(#) libf/fio/fwarn.c	92.1	06/18/99 19:52:04"

#include <errno.h>
#include <liberrno.h>
#include <stdarg.h>
#include <cray/portdefs.h>

/*
 *	_fwarn - Fortran run-time warning processor
 *
 *		Returns:
 *			 0  Warning printed
 *			-1  _fwarn() called with no arguments
 *			-2  _fwarn() called with a warning number outside
 *			    the range of Fortran library message numbers.
 */

int
_fwarn(int errn, ...)
{
	va_list		args;		/* Variable argument list	*/

#ifdef	_UNICOS
	if (_numargs() < 1)
		return(-1);
#endif

	if (((errn < BASE) || (errn > (BASE+999))) &&
	    ((errn < FDC_ERRB) || (errn > (FDC_ERRB+999))))
		return(-2);

	va_start(args, errn);

	_lmessage(errn, "WARNING", args);

	va_end(args);

	return(0);
}
