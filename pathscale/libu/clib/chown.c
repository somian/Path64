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

#pragma ident "@(#) libu/clib/chown.c	92.1	07/01/99 13:42:20"
#include <fortran.h>
#include <malloc.h>
#include <unistd.h>

/*
 *	CHOWN - Fortran interface to chown(2) system call
 *
 *	Call from Fortran:
 *		CHARACTER * (*), PATH
 *		INTEGER OWNER, GROUP
 *		CALL CHOWN(PATH, OWNER, GROUP)
 *
 *			-or-
 *		CHARACTER * (*), PATH
 *		INTEGER CHOWN, OWNER, GROUP
 *		I = CHOWN(PATH, OWNER, GROUP)
 *
 *			-or-
 *		INTEGER PATH, OWNER, GROUP
 *		CALL CHOWN(PATH, OWNER, GROUP)
 *
 *			-or-
 *		INTEGER CHDIR, PATH, OWNER, GROUP
 *		I = CHOWN(PATH, OWNER, GROUP)
 */

_f_int
CHOWN(path, owner, group)
long	path;
long	*owner, *group;
{
	if (_numargs() < 3)
		return( (_f_int) -1);

	return ((_f_int)_clib_call(access, path, *owner, *group));
}
