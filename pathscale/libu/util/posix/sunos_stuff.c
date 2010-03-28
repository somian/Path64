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


static char USMID[] = "@(#) libu/util/posix/sunos_stuff.c	92.0	10/08/98 14:57:41";

/*
 *	The following POSIX functions are not defined on SunOS.   So we
 *	implement them here.
 */
#ifdef	_SUNOS

void * 
memmove(void *s, void *s0, int n)
{
   	char *s1 = s;
	char *s2 = s0;

	if (s1 <= s2) {
		while (n-- > 0)
			*s1++ = *s2++;
	}
	else {
		s2 += n;
		s1 += n;
		while (n-- > 0)
			*--s1 = *--s2;
	}
	return (s);
}

char *
strerror(int i)
{
	extern char *sys_errlist[];
	return sys_errlist[i];
}

#endif	/* _SUNOS */
