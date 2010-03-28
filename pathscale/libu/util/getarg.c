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


#pragma ident "@(#) libu/util/getarg.c	92.1	07/07/99 13:18:33"

#include <fortran.h>
#include <string.h>

/*
 * INTEGER FUNCTION GETARG(k, c [,ls])
 *
 * returns the kth unix command argument in fortran character
 * variable argument c.
 * This version of GETARG differs from the baseline source as follows:
 *    -the ls count is in words and is used only when the destination
 *     is not a character variable.
 *    -GETARG returns the number of characters in the field.
 *    -the format of FORTRAN character pointers is different.
 *    -the tail of the destination is padded with spaces or zeros.
 *    -the name of xargc and xargv are different in UNICOS.
 */

extern int _argc;	/* the number of argv fields */
extern char **_argv;	/* pointer to pointer to fields */

#ifndef	_ADDR64			/* not available if _isfcd is not available */

_f_int
GETARG(n, arg, ls)
long	*n;	/* argument number wanted */
_fcd	arg;	/* destination address */
long	*ls;	/* if destination is not a FORTRAN character, this is
                 * the number of 64 bit words in destination field */
{
	char	*src;
	char	*dest;
	char	pad;
	int	count;
	int	len;

	if (_numargs() < 2)
		return( (_f_int) -1);

	/* Determine destination length and address */

	if (_isfcd(arg)) {	/* If destination is a character variable */
		dest	= _fcdtocp(arg);
		count	= _fcdlen(arg);
		pad	= ' ';
	}
	else {			/* Destination is a hollerith variable */
		if (_numargs() < 3)
			return( (_f_int) -1);

		dest	= *(char **) &arg;
		count	= *ls * sizeof(long);
		pad	= '\0';
	}

	/* Pick up address of field needed */

	if (*n >= 0 && *n < _argc)
		src	= _argv[*n];
	else
		src	= "";		/* Null string */

	len	= strlen(src);

	/* Move string until destination full or NULL found */

	if (len < count) {
		(void) memcpy(dest, src, len);
		(void) memset(dest + len, pad, count - len);
	}
	else {
		(void) memcpy(dest, src, count);
		len	= count;
	}

	return( (_f_int) len);
}
#endif	/* _ADDR64 */

/*
 * INTEGER FUNCTION IARGC()
 *
 * returns the number of command line arguments
 */

_f_int
IARGC()
{
	return(_argc - 1);
}
