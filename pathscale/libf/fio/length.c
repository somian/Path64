/*

  Copyright (C) 2000, 2001, Silicon Graphics, Inc.  All Rights Reserved.

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



#pragma ident "@(#) libf/fio/length.c	92.1	06/21/99 10:37:21"
#include <foreign.h>
#include <fortran.h>
#include <liberrno.h>
#include <sys/param.h>
#include <errno.h>
#include "fio.h"

/*
 *	_LENGTH_
 *
 *		Intrinsic LENGTH function returns the amount of data 
 *		transferred by the preceding BUFFER IN or BUFFER OUT request.
 *
 *	Return Value
 *
 *		The size in words of the preceding BUFFER IN/OUT statement.
 *		Partial words are counted.
 *
 *		Returns 0 if of an end-of-file or an error occurs, or after a
 *		read or write of a zero-length record.
 *
 *		Undocumented feature:  S2 is assigned on exit the 
 *		"unused bit count".  This is the number of bits untransferred 
 *		in the last word of the previous I/O request.
 *
 *	Define duplicate entry points
 *
 *              _LENGTH_ - if user declares it INTRINSIC with CF90 
 *              LENGTH   - if user declaris it EXTERNAL or CALL's it
 *              $LENGTH  - if user declares it INTRINSIC with CF77
 *              _LENGTH  - if user declares it INTRINSIC with CF77 6.0.0.3 or
 *                         previous on the T3D (obsolete)
 */
#ifdef	_UNICOS
#pragma _CRI duplicate _LENGTH_ as LENGTH
#pragma _CRI duplicate _LENGTH_ as $LENGTH
#ifdef _CRAYMPP
#pragma _CRI duplicate _LENGTH_ as _LENGTH
#endif
#endif	/* _UNICOS */

_f_int
_LENGTH_(_f_int *unump)
{
	unum_t	unum;
	unit	*cup;
	int	ret, s2ret;
	struct fiostate	cfs;

	unum	= *unump;

	STMT_BEGIN(unum, 0, T_LENGTH, NULL, &cfs, cup); /* lock the unit */
/*
 *	If not connected, do an implicit open.  Abort if the open fails.
 */
        if (cup == NULL)
                cup	= _imp_open(&cfs, SEQ, UNF, unum, 0, NULL);

	WAITIO(cup, { cup->uerr = 1; } );	/* await outstanding I/O */

	if (cup->uerr) {
		ret	= 0;
		s2ret	= 0;
	}
	else {
		ret	= (cup->ulrecl + BITS_PER_WORD - 1) / BITS_PER_WORD;
		s2ret	= (ret * BITS_PER_WORD) - cup->ulrecl;
	}

	STMT_END(cup, T_LENGTH, NULL, &cfs);	/* unlock the unit */

#ifdef	_CRAY1
	(void) _sets2(s2ret);
#endif

	return( (_f_int) ret);
}
