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

#pragma ident "@(#) libu/clib/stat.c	92.1	07/01/99 13:42:20"

#include <fortran.h>
#include <malloc.h>
#include <unistd.h>
#include <sys/stat.h>

/*
 *	STAT - Fortran interface to stat(2) system call
 *
 *	Call from Fortran:
 *		
 *		CHARACTER * (*) PATH
 *		INTEGER BUF(n)
 *		CALL STAT(PATH, BUF)
 *
 *			-or-
 *
 *		CHARACTER * (*) PATH
 *		INTEGER BUF(n), STAT
 *		I = STAT(PATH, BUF)
 *
 *			-or-
 *
 *		INTEGER PATH, BUF(n), STAT
 *		I = STAT(PATH, BUF)
 *
 *			-or-
 *
 *		INTEGER PATH, BUF(n)
 *		CALL STAT(PATH, BUF)
 */

_f_int
STAT(path, buf)
_fcd	path;
void	*buf;
{
	if (_numargs() * sizeof(long) != sizeof(_fcd) + sizeof(void*))
		return( (_f_int) -1);
 
	return ((_f_int) _clib_call(stat, path, buf));
}
