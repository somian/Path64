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

#pragma ident "@(#) libu/clib/mknod.c	92.1	07/01/99 13:42:20"

#include <fortran.h>
#include <malloc.h>
#include <unistd.h>
#include <liberrno.h>

/*
 *	MKNOD - Fortran interface to mknod(2) system call
 *
 *	Call from Fortran:
 *		CHARACTER * (*), PATH
 *		INTEGER MODE, DEV
 *		CALL MKNOD(PATH, MODE, DEV)
 *
 *			-or-
 *		CHARACTER * (*), PATH
 *		INTEGER MKNOD, MODE, DEV
 *		I = MKNOD(PATH, MODE, DEV)
 *
 *			-or, on systems that do not have 64-bit addresses: -
 *		INTEGER PATH, MODE, DEV
 *		CALL MKNOD(PATH, MODE, DEV)
 *
 *			-or-
 *		INTEGER MKNOD, PATH, MODE, DEV
 *		I = MKNOD(PATH, MODE, DEV)
 */

_f_int
MKNOD(path, mode, dev)
_fcd	path;
long	*mode, *dev;
{
#ifdef _ADDR64
char *pathptr;
_f_int ret;
	if (_numargs() != (sizeof(_fcd) + 2*sizeof(long *))/sizeof(long)) {
		_lerror(_LELVL_ABORT,FEARGLST,"MKNOD");
	}
	pathptr = _f2ccpy(path);
	if (pathptr == NULL)
		return( (_f_int) -1);
	ret = mknod(pathptr, *mode, *dev);
	free(pathptr);
	return((_f_int)ret);
#else
	if (_numargs() < 3) {
		_lerror(_LELVL_ABORT,FEARGLST,"MKNOD");
	}
	return((_f_int)_clib_call(mknod, path, *mode, *dev));
#endif
}
