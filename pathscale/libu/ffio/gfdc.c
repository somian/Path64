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


#pragma ident "@(#) libu/ffio/gfdc.c	92.1	06/29/99 13:16:47"

#include <stdio.h>
#include <string.h>
#include <ffio.h>

/*
 * Extract the FDC specification words from the option
 *	string in the file characteristics.
 * This routine allocates memory from the heap and
 * returns a pointer to the array of spec words terminated
 * by a zero word.
 */
union spec_u *
_g_fdc_spec(entry)
char *entry;
	{
	char *opts, *specstr;
	extern char *_g_fchar_opts();
	extern char *_g_fchar_opt();
	extern union spec_u *_dec_fdc_spec();

	opts = _g_fchar_opts(entry);
	if (opts == NULL)
		return(NULL);

	specstr = _g_fchar_opt(opts, 'F');
	if (specstr == NULL)
		return(NULL);
	return(_dec_fdc_spec(specstr));
	}

union spec_u *
_dec_fdc_spec(specstr)
char *specstr;
	{
	int wl, toklen, i;
	union spec_u *words;
	long _hex2bin();
/*
 *	Spec string is terminated by a trailing space.  Find the space
 *	to determine the length
 */
	toklen = strchr(specstr, ' ') - specstr;
	wl = toklen / 17;		/* number of words in spec */
	words = (union spec_u *)malloc(wl*8 + 8);
	i = 0;
	do
		{
		words[i].wword = _hex2bin(specstr);
		i++;
		specstr += 17;	/* skip '+' too */
		} while(i < wl);
	words[i].wword = 0;
	return(words);
	}
