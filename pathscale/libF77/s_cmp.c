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


/* $Header$ */

/* Craig Hansen - 24-September-86 */

#include <cmplrs/host.h>
#include <string.h>

int32
s_cmp (register string a, register string b, fsize_t la, fsize_t lb)
/* compare two strings */
{
    register string aend, bend, dend;
    register int32 ahold, bhold;
    register int32 space = ' ';

    aend = a + la;
    bend = b + lb;

    if(la <= lb) {
	dend = a + la;
    } else {
	dend = a + lb;
    }

    while(a != dend) {
	ahold = *a++;
	if(ahold != *b)
	    return( ahold - *b );
	else {
	    ++b;
	}
    }

    while(b != bend) {
	bhold = *b++;
	if(bhold != space)
	    return( space - bhold );
    }

    while(a != aend) {
	ahold = *a++;
	if(ahold != space)
	    return(ahold - space);
    }
    return(0);
}
