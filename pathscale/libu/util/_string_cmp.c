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


#pragma ident "@(#) libu/util/_string_cmp.c	92.1	07/07/99 13:18:33"

#include <string.h>
#include <ctype.h>
#include "utildefs.h"

/*
 * Compare a C string (s1) to a Fortran string (s2), treating lower case
 * letters in the Fortran string as upper case letters and ignoring any
 * trailing blanks in the Fortran string.  Returns nonzero (true) if the
 * strings are equal.  Parameter n is length of Fortran string.
 */
int
_string_cmp(const char *s1, const char *s2, int n)
{
	int     i;
 
	while (*(s2 + n-1) == ' ')
		n--;	    	/* back over trailing blanks */
	if (strlen(s1) != n)
		return(0);      /* different length strings aren't equal */
	for(i = 0; i < n; i++)
		if(*s1++ != toupper(*s2++))
			return(0);      /* mismatch */
	return(*s1 == '\0');    /* strings match if no more characters */
}
