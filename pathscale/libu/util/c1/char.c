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


#pragma ident "@(#) libu/util/c1/char.c	92.1	07/07/99 13:22:26"

#include <string.h>
#include <fortran.h> 	

_charlen(f)
_fcd *f;
{
	return(_fcdlen(*f));	/* return length */
}
 
/* 
 * This routine copies from f to s. Trailing blanks are eliminated
 */
_charcpy(s, f, len)
char *s;
_fcd *f;		/* fortran character descriptor */
int len;
{
	int wrdlen;
	char *t;
	char *c;

	t = _fcdtocp(*f);
	c = t+len-1;
	for (wrdlen = len; *c == ' '; c-- ){ 
		wrdlen--;
	}
	(void) strncpy( s, t, wrdlen);
	s = s + wrdlen;
	*s = '\0';

}

/*
 * This routine copies from f to s. Trailing blanks are
 * eliminated. If the length of the string is > maxlen,
 * -1 is returned; otherwise 0 is returned.
 */
_charncpy(s, f, len, maxlen)
char *s;
_fcd *f;
int len;
int maxlen;
{
	int wrdlen;
	char *t;
	char *c;

	t = _fcdtocp(*f);
	c = t+len-1;
	for (wrdlen = len; *c == ' '; c-- ){ 
		wrdlen--;
	}
        if (wrdlen > maxlen)
	{
		return(-1);	/* error */
	}
	(void) strncpy( s, t, wrdlen);
	s = s + wrdlen;
	*s = '\0';
	return(0);
}
 
