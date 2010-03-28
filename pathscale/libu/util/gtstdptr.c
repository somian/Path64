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


#pragma ident "@(#) libu/util/gtstdptr.c	92.1	07/07/99 13:18:33"
  
#include <stdio.h>
  
/*
 *	GTSTDPTR	Returns pointer to standard file
 *
 *	Call from Fortran:
 *
 *		INTEGER FD, STREAM, GTSTDPTR
 *      	STREAM = GTSTDPTR(FD)
 *
 *              FD	File descriptor, type INTEGER, as follows:
 *
 *			0  Return address of standard input (stdin)
 *			1  Return address of standard output (stdout)
 *			2  Return address of standard error (stderr)
 *
 *	Result:
 *
 *		STREAM	Address of FILE structure (stream pointer)
 *
 *
 *	Refer to the stdio(3) and fopen(3) man pages for a description of
 *	file descriptors and stream pointers.  This routine is intended for
 *	those applications that manipulate such data structures directly.
 *	For example, via the Fortran-callable FREAD(3) and FWRITE routines.
 */

FILE *
GTSTDPTR (fd)
long	*fd;			/* file descriptor */
{
	if (_numargs() == 1) {
		if (*fd >= 0 && *fd <= 2)
			return(_getfp(*fd));
	}
	return(NULL);
}
