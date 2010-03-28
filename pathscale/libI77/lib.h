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


#ifndef __LIB_H__
#define __LIB_H__


#ident "$Revision$"

#include <stdio.h>
#include <cmplrs/fio.h>

#if defined(_LANGUAGE_C) || defined(_LANGUAGE_C_PLUS_PLUS)
/* PROTOTYPES */
extern int setcilist(cilist *, int, char *, int, int, int);
extern int setolist(olist *, int, char *, char *, char *, int, char *, int);
extern int stcllist(cllist *, int, char *, int);
extern int setalist(alist *, int, int);

#if 11
extern int setcilist64(cilist64 *, ftnint, char *, XINT64, int, int);
extern int setolist64(olist64 *, ftnint, char *, char *, char *, int, char *, int);
/* extern int setcllist64(cllist64 *, ftnint, char *, int); */
/* extern int setalist64(alist64 *, ftnint, int); */
#endif

#endif /* C || C++ */

#endif /* !__LIB_H__ */





