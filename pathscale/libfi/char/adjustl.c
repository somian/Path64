/*
 *  Copyright (C) 2007. QLogic Corporation. All Rights Reserved.
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


#pragma ident "@(#) libfi/char/adjustl.c	92.1	07/08/99 10:41:51"
#include <fortran.h>
#include <string.h>

/*
 *	Duplicate names
 *
 *	_ADJUSTL_	- for f90 
 *	_ADJUSTL	- for f90 3.0? and previous on PVP systems 
 */
#ifdef	_CRAY1
#pragma _CRI duplicate _ADJUSTL_ as _ADJUSTL
#endif

void
_ADJUSTL_(
	_fcd	result,
	_fcd	string)
{
	int	lens, lenr;
	int	i;
	char	*sptr, *rptr;
	char	*tptr;

/*	Split fcd of source and result into component parts		*/

	lens = _fcdlen (string);
	sptr = _fcdtocp (string);

	lenr = _fcdlen (result);
	rptr = _fcdtocp (result);

/*
 *	Determine number of bytes without leading 0's.  While doing
 *	it, update temporary pointer to point to first non-blank
 *	character.  Do not dereference tptr if zero length string.
 */

	tptr = sptr;
	for (i = lens ; i > 0 && *tptr == ' '; tptr++, i--) ;

/*
 *	Set result string to all ' ', and copy source (without leading
 *	blanks.
 */

	(void) memset (rptr, (int) ' ', lenr);
#ifdef KEY /* Bug 9723 */
	(void) memmove(rptr, tptr, i);
#else /* KEY Bug 9723 */
	(void) strncpy (rptr, tptr, i);
#endif /* KEY Bug 9723 */

/*	convert result to fcd					*/

	result = _cptofcd (rptr, lenr);

	return;
}
