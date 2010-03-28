#pragma ident "@(#)92/gen/strnrstrn.c	92.1	06/02/99 16:43:34"

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


#include <string.h>

#define	MAXCHRS	256	/* Size of character set */

/*
 *	strnrstrn	Search for the last occurance of a substring
 *			in another string.  The length of the strings
 *			are arguments and the strings are not necessarily
 *			null-terminated.
 *
 *	Algorithm:  This routine uses the Boyer-Moore algorithm as
 *		described in _Algorithms_ by Robert Sedgewick,
 *		Addison-Wesley, 2d Ed., 1988.  pp. 277-289.
 *
 *	Performance:  If M is the length of the substring and N is
 *		the length of the string; this algorithm's worst case
 *		is M+N character comparisons, and ``average-case'' is
 *		N/M comparisons.  The ``brute-force'' algorithm's
 *		worst case is about N*M comparisons.
 */

char *
strnrstrn(const char *string, size_t lenstr, const char *substr, size_t lensub)
{
	register short	i;
	register unsigned short	j;
	unsigned short	skip[MAXCHRS];

	if (lensub == 0)
		return( (char *) string + lenstr);

	/* Initialize skip array */

	for (i = 0; i < MAXCHRS; i++)	/* Should vectorize */
		skip[i]	= lensub;

	for (i = lensub - 1; i >= 0; i--)
		skip[(int)substr[i]]	= i;

	i	= lenstr - lensub;
	j	= 0;

	do {

		if (string[i] == substr[j]) {
			i	= i + 1;
			j	= j + 1;
		}
		else {
			if (j > skip[(int)string[i]])
				i	= i - (j + 1);
			else
				i	= i - skip[(int)string[i]];

			j	= 0;
		}

	} while (j < (unsigned) lensub && i >= 0);

	if (i < 0)
		return(NULL);
	else
		return( (char *) string + i - lensub);
}
