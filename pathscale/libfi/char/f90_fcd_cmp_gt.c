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


#pragma ident "@(#) libfi/char/f90_fcd_cmp_gt.c	92.2	01/20/95 09:56:38"
#include <string.h>

#define	BLANK	((int) ' ')

int _F90_FCD_CMP_GT( char *str1,
		     char *str2,
		     int  len1, 
		     int  len2 )
{
	char		*str3;
	unsigned int	len3, len;
	int		sign, stat;

	/* Find out if strings are of equal length */

	sign	= (len1 < len2);

	if (sign) {
		len	= len1;
		len3	= len2 - len1;
		str3	= str2 + len;
	}
	else {
		len	= len2;
		len3	= len1 - len2;
		str3	= str1 + len;
	}

	/* Compare first part of strings */

	stat	= memcmp(str1, str2, len);

	/*
	 * If the string lengths are unequal and equality has not yet
	 * been resolved, then compare remnant against blanks.
	 */

	while (len3 > 0 && stat == 0) {	/* If further comparision necessary */
		register char	ch;

		ch	= *str3;
		str3	= str3 + 1;
		stat	= sign ? (BLANK - (int) ch) : (((int) ch) - BLANK);
		len3	= len3 - 1;
	}

	return (stat > 0);
}
