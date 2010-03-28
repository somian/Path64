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


#pragma ident "@(#) libfi/char/len.c	92.1	07/08/99 10:41:51"
#include <fortran.h>

/*
 *	Duplicate names
 *
 *	_LEN_	- for f90 intrinsic
 *	_LEN_	- for cf77 intrinsic
 *	_LEN	- for f90 3.0? and previous on PVP systems 
 */
#ifdef _UNICOS
#pragma	_CRI duplicate _LEN_ as $LEN
#pragma	_CRI duplicate _LEN_ as _LEN
#endif

_f_int
_LEN_(_fcd f)
{
	return ((_f_int) _fcdlen(f));
}


#ifdef	_F_INT4
/*
 *	Duplicate names
 *
 *	_LEN_4_	- for f90 intrinsic
 *	_LEN_4_	- for cf77 intrinsic
 *	_LEN_4	- for f90 3.0? and previous on PVP systems 
 */
#ifdef _UNICOS
#pragma	_CRI duplicate _LEN_4_ as $LEN_4
#pragma	_CRI duplicate _LEN_4_ as _LEN_4
#endif

_f_int4
_LEN_4_(_fcd f)
{
	return ((_f_int4) _fcdlen(f));
}
#endif


#ifdef	_F_INT8
/*
 *	Duplicate names
 *
 *	_LEN_8_	- for f90 intrinsic
 *	_LEN_8_	- for cf77 intrinsic
 *	_LEN_8	- for f90 3.0? and previous on PVP systems 
 */
#ifdef _UNICOS
#pragma	_CRI duplicate _LEN_8_ as $LEN_8
#pragma	_CRI duplicate _LEN_8_ as _LEN_8
#endif

_f_int8
_LEN_8_(_fcd f)
{
	return ((_f_int8) _fcdlen(f));
}
#endif
