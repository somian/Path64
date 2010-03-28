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


#if 11

extern void get_cilist64( cilist64 *a64, cilist *a );
extern void get_inlist64(inlist64 *dst, inlist *src);
extern void get_icilist64(icilist64 *dst, icilist *src);
extern void get_olist64(olist64 *dst, olist *src);

#endif	/* -n32 or -64 */

#ifdef NO_XXX64_IN_LIBC
extern long long lseek64(int a, long long b, int c);
extern int fseek64(FILE * a, long long b, int c);
extern long long ftell64(FILE *a);
extern int ftruncate64(int a, long long b);
extern int truncate64(const char *a, long long b);
extern int fstat64(int a , struct STAT_TAG *b);
extern int stat64(const char *a, struct STAT_TAG *b);
#endif
