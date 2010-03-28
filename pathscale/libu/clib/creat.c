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

#pragma ident "@(#) libu/clib/creat.c	92.1	07/01/99 13:42:20"
#include <fcntl.h>
#include <fortran.h>
#include <malloc.h>
#include <unistd.h>

/*	CREAT - Fortran interface to creat(2) system call 
 * 
 *   	Call from Fortran:
 *		CHARACTER * (*) PATH
 *		INTEGER MODE
 *		CALL CREAT(PATH, MODE)
 *		
 *			-or-
 *		CHARACTER * (*) PATH
 *		INTEGER MODE, CREAT
 *		I = CREAT(PATH, MODE)
 *
 *			-or-
 *              INTEGER PATH, MODE
 *		CALL CREAT(PATH, MODE)
 *
 *			-or-
 *              INTEGER PATH, MODE, CREAT
 *		I = CREAT(PATH, MODE)
 */

_f_int
CREAT (path, mode)
_fcd	path;
long	*mode;
{
	if (_numargs() < 2)
		return( (_f_int) -1);

	return ((_f_int)_clib_call(creat, path, *mode));
}
