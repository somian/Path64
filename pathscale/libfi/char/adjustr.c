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


#pragma ident "@(#) libfi/char/adjustr.c	92.1	07/08/99 10:41:51"
#include <fortran.h>
#include <string.h>

/*
 *	Duplicate names
 *
 *	_ADJUSTR_	- for f90 
 *	_ADJUSTR	- for f90 3.0? and previous on PVP systems 
 */
#ifdef	_CRAY1
#pragma _CRI duplicate _ADJUSTR_ as _ADJUSTR
#endif

void
_ADJUSTR_(
	_fcd	result,
	_fcd	string)
{
	char	*rptr, *sptr;
	int	lenr, lens;
	int	i, j;
	char	*tptr;

/*	Split fcds for source and result into component parts	*/

	lens = _fcdlen(string);
	sptr = _fcdtocp(string);

	lenr = _fcdlen(result);
	rptr = _fcdtocp(result);

/*
 *	Set temporary pointer to end of source string.  Work backwards
 *	until a non-blank character is found.  This will give the number
 *	of characters to copy into result.  Do not dereference tptr if
 *	it is a zero-length string.
 */

	tptr = (char *) sptr;
	tptr += lens - 1;
	for (i = lens; i > 0 && *tptr == ' '; tptr--, i--) ;

/*
 *	Set entire result to ' 's.  Then, set temporary pointer to point
 *	to the first character after the required number of blanks, and
 *	copy source (without trailing blanks) to this spot.
 */

	(void) memset (rptr, ' ', lenr);
	tptr = (char *) rptr;
	tptr += lenr - i;
	(void) strncpy (tptr, sptr, i);

/*	convert result to fcd					*/

	result = _cptofcd (rptr, lenr);

	return;
}
