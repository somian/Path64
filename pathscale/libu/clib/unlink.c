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

#pragma ident "@(#) libu/clib/unlink.c	92.1	07/01/99 13:42:20"

#include <fortran.h>
#include <malloc.h>
#include <unistd.h>

/*
 *	UNLINK - Fortran interface to unlink(2) system call
 *
 *	Call from Fortran:
 *		CHARACTER * (*), PATH
 *		CALL UNLINK(PATH)
 *
 *			-or-
 *		CHARACTER * (*), PATH
 *		INTEGER UNLINK
 *		I = UNLINK(PATH)
 *
 *			-or-
 *		INTEGER PATH
 *		CALL UNLINK(PATH)
 *			-or-
 *		INTEGER UNLINK, PATH
 *		I = UNLINK(PATH)
 */

_f_int 
UNLINK(path)
_fcd	 path;
{
	if (_numargs() * sizeof(long) != sizeof(_fcd))
		return( (_f_int) -1);

	return ((_f_int) _clib_call(unlink, path));
}
