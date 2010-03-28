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


#pragma ident "@(#) libfi/char/len_trim.c	92.1	07/08/99 10:41:51"
#include <fortran.h>

/*
 *	Duplicate names
 *
 *	_LEN_TRIM_	- for f90 
 *	_LEN_TRIM	- for f90 3.0? and previous on PVP systems 
 */
#ifdef _UNICOS
#pragma _CRI duplicate _LEN_TRIM_ as _LEN_TRIM
#endif

_f_int
_LEN_TRIM_(_fcd str)
{
	long	len;
	char	*cptr;
	long	i;

	len = _fcdlen (str);
	cptr = _fcdtocp (str);

	for (i = len - 1; i >= 0; i--) {
	    if (cptr[i] != ' ')
		return (i+1);
	}

	return (0);
}


#ifdef	_F_INT4
/*
 *	Duplicate names
 *
 *	_LEN_TRIM_4_	- for f90 
 *	_LEN_TRIM_4	- for f90 3.0? and previous on PVP systems 
 */
#ifdef _UNICOS
#pragma _CRI duplicate _LEN_TRIM_4_ as _LEN_TRIM_4
#endif

_f_int4
_LEN_TRIM_4_(_fcd str)
{
	long	len;
	char	*cptr;
	long	i;

	len = _fcdlen (str);
	cptr = _fcdtocp (str);

	for (i = len - 1; i >= 0; i--) {
	    if (cptr[i] != ' ')
		return ((_f_int4) i+1);
	}

	return (0);
}
#endif


#ifdef	_F_INT8
/*
 *	Duplicate names
 *
 *	_LEN_TRIM_8_	- for f90 
 *	_LEN_TRIM_8	- for f90 3.0? and previous on PVP systems 
 */
#ifdef _UNICOS
#pragma _CRI duplicate _LEN_TRIM_8_ as _LEN_TRIM_8
#endif

_f_int8
_LEN_TRIM_8_(_fcd str)
{
	long	len;
	char	*cptr;
	long	i;

	len = _fcdlen (str);
	cptr = _fcdtocp (str);

	for (i = len - 1; i >= 0; i--) {
	    if (cptr[i] != ' ')
		return ((_f_int8) i+1);
	}

	return (0);
}
#endif
