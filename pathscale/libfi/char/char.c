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


#pragma ident "@(#) libfi/char/char.c	92.1	07/08/99 10:41:51"
#include <fortran.h>

/*
 *	Duplicate names
 *
 *	_CHAR_		- for f90 intrinsic on MPP and PVP
 *	$CHAR		- for cf77 intrinsic
 *	_CHAR		- for f90 3.0? and previous
 */
#ifdef _UNICOS
#pragma	_CRI duplicate _CHAR_ as $CHAR
#pragma	_CRI duplicate _CHAR_ as _CHAR
#endif

void
_CHAR_(_fcd C, _f_int *I)
{
	long	len;
	char	*cpr;

	cpr	= _fcdtocp(C);
	len	= _fcdlen (C);

	if (len > 0)
		*cpr	= (char) *I;

	return;
}
