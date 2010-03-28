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


#ifndef __UTIL_H__
#define __UTIL_H__


#ident "$Revision$"

#include <stdio.h>
#include <cmplrs/fio.h>

#if defined(_LANGUAGE_C) || defined(_LANGUAGE_C_PLUS_PLUS)
/* PROTOTYPES */
extern int g_char(char *, ftnlen, char *);
extern int b_char(char *, char *, ftnlen);
/* extern int f77mvgbt(int, int, char *, char *); not used anymore */
extern int up_low(char);
extern unit *map_luno(ftnint);
extern unit *find_luno(ftnint);
extern int f77nowreading(unit *);
extern int f77nowwriting(unit *);
extern int f77inode(char *, ino_t *);
extern char *_I90_uppercase( char *, char * );
extern int space_assigned;

#endif /* C || C++ */

#endif /* !__UTIL_H__ */
