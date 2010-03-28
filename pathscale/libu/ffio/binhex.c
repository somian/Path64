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


#pragma ident "@(#) libu/ffio/binhex.c	92.1	06/29/99 13:16:47"

#include <stdlib.h>
#include <cray/portdefs.h>
#include "spec_parse.h"

#define HEX_DIGITS	(sizeof(int)*2)

static char hexdigits[] =
	{'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
static int digihex[] =
	{0,1,2,3,4,5,6,7,8,9,0,0,0,0,0,0,0,10,11,12,13,14,15};

/*
 * hex2bin converts HEX_DIGITS hex digits to one cray word 
 */
long
_hex2bin(char *str)
	{
	long tword;
	int i;

	tword = 0;
	for (i = 0 ; i < HEX_DIGITS ; i++)
		{
		tword = tword << 4;
		if (str[i] < '0' || str[i] > 'F') abort();
		tword = tword | digihex[str[i] - '0'];
		}
	return(tword);
	}

/*
 * bin2hex converts one cray word to hex format and puts the HEX_DIGITS chars
 * at the location specified
 */
void
_bin2hex(char *str, unsigned long num)
	{
	int i;

	for (i = 0 ; i < HEX_DIGITS ; i++)
		{
		num = _dshiftl(num,num,4);
		*str = hexdigits[num & 0xF];
		str++;
		}
	}

/*
 * Copy a name terminated by space, newline, tab, or null.
 */
int
_cpyname(char *target, char *src)
	{
	int i;

	i = 0;
	while ( *src != ' '  && /* space */
		*src != '\t' && /* tab */
		*src != '\n' && /* newline */
		*src != '\0')   /* NULL */
		{
		*target = *src;
		target++;
		src++;
		i++;
		}
	*target = '\0';
	return(i);
	}


