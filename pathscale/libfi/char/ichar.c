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


#pragma ident "@(#) libfi/char/ichar.c	92.1	07/08/99 10:41:51"
#include <fortran.h>

/*
 *	Duplicate names
 *
 *	_ICHAR_		- for f90 intrinsic on MPP and PVP
 *	$ICHAR		- for cf77 intrinsic
 *	_ICHAR		- for f90 3.0? and previous
 */
#ifdef _UNICOS
#pragma	_CRI duplicate _ICHAR_ as $ICHAR
#pragma	_CRI duplicate _ICHAR_ as _ICHAR
#endif

_f_int
_ICHAR_(_fcd f)
{
	return ((_f_int) *(_fcdtocp(f)));
}


#ifdef	_F_INT4
/*
 *	Duplicate names
 *
 *	_ICHAR_4_		- for f90 intrinsic on MPP and PVP
 *	$ICHAR_4		- for cf77 intrinsic
 *	_ICHAR_4		- for f90 3.0? and previous
 */
#ifdef _UNICOS
#pragma	_CRI duplicate _ICHAR_4_ as $ICHAR_4
#pragma	_CRI duplicate _ICHAR_4_ as _ICHAR_4
#endif

_f_int4
_ICHAR_4_(_fcd f)
{
	return ((_f_int4) *(_fcdtocp(f)));
}
#endif


#ifdef	_F_INT8
/*
 *	Duplicate names
 *
 *	_ICHAR_8_		- for f90 intrinsic on MPP and PVP
 *	$ICHAR_8		- for cf77 intrinsic
 *	_ICHAR_8		- for f90 3.0? and previous
 */
#ifdef _UNICOS
#pragma	_CRI duplicate _ICHAR_8_ as $ICHAR_8
#pragma	_CRI duplicate _ICHAR_8_ as _ICHAR_8
#endif

_f_int8
_ICHAR_8_(_fcd f)
{
	return ((_f_int8) *(_fcdtocp(f)));
}
#endif
