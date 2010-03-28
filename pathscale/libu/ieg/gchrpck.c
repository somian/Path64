/*
 * Copyright 2004, 2005, 2006 PathScale, Inc.  All Rights Reserved.
 */

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


#pragma ident "@(#) libu/ieg/gchrpck.c	92.1	06/25/99 14:37:13"

#include <fortran.h>

/*
 *	G@CHRPCK	Convert a Fortran character descriptor to a word
 *			address, a bit offset and a bit length.  This is
 *			called by the numeric data conversion routines to
 *			convert CHARACTER variables to word addresses with
 *			bit offsets.  This routine is not intended to be
 *			user-callable.
 *
 *	CHARACTER * (*) FCV
 *	INTEGER BLEN, BOFF, WADD
 *
 *	CALL G@CHRPCK(FCV, WADD, BLEN, BOFF)
 *
 *		FCV	Fortran CHARACTER variable
 *		WADD	Address to receive word address of FCV
 *		BLEN	Address to receive length, in bits, of FCV
 *		WADD	Address to receive word offset, in bits, of FCV
 */

#if	defined(__mips) || defined(_LITTLE_ENDIAN)
void
g$chrpck_(
	char	*cd,		/* Fortran character address */
	_f_int8	*waddr,		/* Word address */
	_f_int8	*blen,		/* Length, in bits */
	_f_int8	*woff,		/* Word offset, in bits */
	int	_len)		/* Length of character item, in bytes */
#else
void
G@CHRPCK(
	_fcd	cd,		/* Fortran character descriptor */
	_f_int8	*waddr,		/* Word address */
	_f_int8	*blen,		/* Length, in bits */
	_f_int8	*woff)		/* Word offset, in bits */
#endif
{
	register _f_int8	cp;

#if	defined(__mips) || defined(_LITTLE_ENDIAN)
	cp	= (_f_int8) (unsigned long) cd;
#else
	cp	= (_f_int8) _fcdtocp(cd);
#endif
#if	defined(_CRAYMPP) || defined(__mips) || defined(_LITTLE_ENDIAN)
	*waddr	= (cp >> 3) << 3;	/* Convert byte address to word address */
#else
	*waddr	= (cp << 6) >> 6;	/* Convert byte address to word address */
#endif
#if	defined(__mips) || defined(_LITTLE_ENDIAN)
	*blen	= _len << 3;	/* Set bit length */
	*woff	= (cd - (char *) (unsigned long) *waddr) << 3;	/* Set bit offset */
#else
	*blen	= _fcdlen(cd) << 3;	/* Set bit length */
	*woff	= (_fcdtocp(cd) - (char *) *waddr) << 3;	/* Set bit offset */
#endif

	return;
}
