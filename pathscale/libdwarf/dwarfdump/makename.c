
/* 
  Copyright (C) 2000,2004 Silicon Graphics, Inc.  All Rights Reserved.

   Path64 is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   Path64 is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
   or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
   License for more details.

   You should have received a copy of the GNU General Public License
   along with Path64; see the file COPYING.  If not, write to the Free
   Software Foundation, 51 Franklin Street, Fifth Floor, Boston, MA
   02110-1301, USA.

   Special thanks goes to SGI for their continued support to open source

   makename.c
   $Revision: 1.2 $ 
   $Date: 2001/01/16 17:47:55 $

   This used to be elaborate stuff.
   Now it is trivial, as duplicating names is
   unimportant in dwarfdump (in general).

   And in fact, this is only called for attributes and
   tags etc whose true name is unknown. Not for
   any normal case.

*/

#include <stdio.h>
#if ! defined(BUILD_OS_DARWIN)
#include <malloc.h>
#endif /* defined(BUILD_OS_DARWIN) */
#include <string.h>
#include <stdlib.h>
#include "makename.h"

char * 
makename( char * s)
{
	char *newstr;

	if(!s) {
		return "";
	}

	newstr = strdup(s);
        if(newstr == 0) {
		fprintf(stderr,"Out of memory mallocing %d bytes\n",
			(int)strlen(s));
		exit(1);
	}
	return newstr;
}
