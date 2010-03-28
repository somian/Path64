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


#ifdef	_UNICOS
#pragma _CRI duplicate _memwcpy as memwcpy
#else
#if defined(BUILD_OS_DARWIN)
/* Mach-O doesn't support aliases */
void _memwcpy(long *s, long *s0, int n );
void memwcpy(long *s, long *s0, int n ) { _memwcpy(s, s0, n); }
#else /* defined(BUILD_OS_DARWIN) */
#pragma weak memwcpy = _memwcpy
#endif /* defined(BUILD_OS_DARWIN) */
#endif

#ifndef	_CRAY
#define memwcpy _memwcpy
#endif

void  
memwcpy(long *s, long *s0, int n )
{
	if (n != 0) {
   		register long *s1 = s;
		register long *s2 = s0;
		if (s1 <= s2) {
			do {
				*s1++ = *s2++;
			} while (--n != 0);
		} else {
			s2 += n;
			s1 += n;
			do {
				*--s1 = *--s2;
			} while (--n != 0);
		}
	}
	return;
}
