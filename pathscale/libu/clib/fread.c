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

#pragma ident "@(#) libu/clib/fread.c	92.1	07/01/99 13:42:20"

#include <fortran.h>
#include <stdio.h>
#include <liberrno.h>

/*
 *	FREAD - Fortran callable interface to fread(3)
 * 
 *   	Call from Fortran:
 *					(non-64 bit address systems only)
 *		CHARACTER * (*) PTR
 *		INTEGER SIZE, N, FP
 *		CALL FREAD(PTR, SIZE, N, FP)
 *		
 *			-or-		(non-64 bit address systems only)
 *		CHARACTER * (*) PTR
 *		INTEGER SIZE, N, FP, FREAD
 *		I = FREAD(PTR, SIZE, N, FP)
 *
 *			-or-
 *		INTEGER PTR, SIZE, N, FP
 *		CALL FREAD(PTR, SIZE, N, FP)
 *		
 *			-or-
 *		INTEGER PTR, SIZE, N, FP, FREAD
 *		I= FREAD(PTR, SIZE, N, FP)
 */

_f_int
FREAD(ptr, size, n, fp)
long	ptr;
long	*size, *n, **fp;
{
	char	*cptr;
	int	ret;

	if (_numargs() * sizeof(long) != 
	    sizeof(long) + sizeof(long*) + sizeof(long*) + sizeof(long**))
		_lerror(_LELVL_ABORT,FEARGLST,"FREAD");

#ifndef	_ADDR64
	if (_isfcd(ptr)) {	/* If character */
		_fcd	fcdptr;

		fcdptr	= *(_fcd *) &ptr;
		cptr	= _fcdtocp(fcdptr);
	}
	else			/* Hollerith */
#endif
		cptr	= (char *) ptr;

	ret	= fread(cptr, *size, *n, (FILE *) (*fp));

	return ( (_f_int) ret);
}

/*
 *	FREADC - Fortran callable interface to fread(3).  Accepts an argument
 *		 of type character.
 * 
 *   	Call from Fortran:
 *
 *		CHARACTER * (*) PTR
 *		INTEGER SIZE, N, FP
 *		CALL FREAD(PTR, SIZE, N, FP)
 *		
 *			-or-
 *		CHARACTER * (*) PTR
 *		INTEGER SIZE, N, FP, FREAD
 *		I = FREAD(PTR, SIZE, N, FP)
 */

_f_int
FREADC(ptr, size, n, fp)
_fcd	ptr;
long	*size, *n, **fp;
{
	int	ret;

	if (_numargs() * sizeof(long) != 
	    sizeof(_fcd) + sizeof(long*) + sizeof(long*) + sizeof(long**))
		_lerror(_LELVL_ABORT,FEARGLST,"FREADC");


	ret	= fread(_fcdtocp(ptr), *size, *n, (FILE *) (*fp));

	return ( (_f_int) ret);
}
