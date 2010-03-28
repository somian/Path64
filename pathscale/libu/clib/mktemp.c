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

#pragma ident "@(#) libu/clib/mktemp.c	92.1	07/01/99 13:42:20"
#include <fortran.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

_f_int
MKTEMP(template)
long	template;
{
	int	ret;
	char	*tempptr;
	_fcd	fcdtemp;

	if (_numargs() < 1)
		return( (_f_int) -1);

#ifdef	_ADDR64
	if (_numargs() * sizeof(long) == sizeof(_fcd)) {
#else
	if (_isfcd(template)) {
#endif
		fcdtemp	= *(_fcd *) &template;
		tempptr	= _f2ccpy(fcdtemp);

		if (tempptr == NULL)
			return( (_f_int) -1);
	}
	else			/* Hollerith */
		tempptr	= (char *) template;

	ret	= (mktemp(tempptr) == NULL) ? -1 : 0;
	if (ret != 0)
		return( (_f_int) ret);

#ifdef	_ADDR64
	if (_numargs() * sizeof(long) == sizeof(_fcd)) {
#else
	if (_isfcd(template)) {
#endif
		unsigned int	lenfcd, lenstr;
		char		*ptrfcd;

		lenstr	= strlen(tempptr);
		lenfcd	= _fcdlen (fcdtemp);
		ptrfcd	= _fcdtocp(fcdtemp);

		(void) strncpy(ptrfcd, tempptr, lenfcd);

		if (lenfcd > lenstr)	/* Pad character template */
			(void) memset(ptrfcd + lenstr, (int) ' ',
					lenfcd - lenstr);

		free(tempptr);
	}

	return( (_f_int) ret);
}
