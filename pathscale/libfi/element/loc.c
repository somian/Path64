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


#pragma ident "@(#) libfi/element/loc.c	92.1	07/08/99 15:50:39"
#include <fortran.h>

/*
 *	Duplicate names
 *
 *	_LOC_	- for f90 intrinsic
 *	$LOC	- for cf77 intrinsic
 */
#if	defined(_UNICOS)
#pragma	_CRI duplicate _LOC_ as $LOC
#pragma	_CRI duplicate _LOC_ as _LOC
#endif

int
_LOC_(void *ptr)
{
#if defined(_UNICOS) && (!defined(_ADDR64) && !defined(_WORD32))
	if (_isfcd(ptr))
		return((int) _fcdtocp(ptr));
#endif
	return ((int) ptr);			/* return pointer value */
}
