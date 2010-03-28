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


#pragma ident "@(#) libu/util/c1/getlpp.c	92.1	07/07/99 13:22:26"

#include <stdio.h>

/*
 * GETLPP()
 *	Get the LPP environment variable, which determines
 *	the lines-per-page on some applications
 *	Note that decimal is the default interpretation, but a leading
 *	zero, (as in LPP=077) is interpreted as octal and 0x77 is
 *	interpreted as hex.
 *
 *	If LPP is not set, 60 is returned.
 *
 *	Negative LPP values are interpreted as an error, and the
 *	default value of 60 is returned
 */
int
GETLPP()
	{
	int res;
	char *cp;
	extern char *getenv();

	cp = getenv("LPP");
	if (cp == NULL)
		res = 60;
	else
		{
		res = strtol(cp, (char **)NULL, 0);
		if (res < 0)
			res = 60;
		}
	return(res);
	}
