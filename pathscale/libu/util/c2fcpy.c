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


#pragma ident "@(#) libu/util/c2fcpy.c	92.1	07/07/99 13:18:33"

#include <fortran.h>
#include <memory.h>

/*
 * _c2fcpy -	Copy a C string to a Fortran character variable.  Pad with
 *		blanks.  If there is insufficient space in f for the string
 *		pointed to by c, -1 is returned.  Otherwise, 0 is returned.
 */
int
_c2fcpy(
char *c,	/* source string */
_fcd f)		/* output string */
{
	int  clen,flen;
	char *fptr;

	clen = strlen(c);
	fptr = _fcdtocp(f);
	flen = _fcdlen(f);

	if (clen > flen)
		return(-1);

	(void)memcpy(fptr,c,clen);			/* copy string */
	(void)memset(&fptr[clen],' ',flen-clen);	/* blank pad */
	return(0);
}
