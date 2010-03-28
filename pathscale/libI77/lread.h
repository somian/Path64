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


#ifndef __LREAD_H__
#define __LREAD_H__


#ident "$Revision$"

#include <stdio.h>
#include <cmplrs/fio.h>

#if defined(_LANGUAGE_C) || defined(_LANGUAGE_C_PLUS_PLUS)
/* PROTOTYPES */
extern int t_gets(unit *, char *, int, char);
extern int t_getc(unit *);
extern int t_ungetc(unit *, int);
extern int e_rsle(void);
extern int e_rsle_mp(unit **);
extern int e_rsli(void);
extern int e_rsli_mp(unit **);
extern int l_read(unit *, XINT *, flex *, ftnlen, ftnint);
extern int s_rsle(cilist *);
extern int s_rsle_mp(cilist *, unit**);
extern int s_rsli(icilist *);
extern int s_rsli_mp(icilist *, unit **);

extern int s_rsle64 (cilist64 *a);
extern int s_rsle64_mp (cilist64 *a, unit **fu);
extern int s_rsli64 (icilist64 *a);
extern int s_rsli64_mp (icilist64 *a, unit **fu);
extern int e_rsle64(void);
extern int e_rsle64_mp(unit **);
extern int e_rsli64(void);
extern int e_rsli64_mp(unit **);


#endif /* C || C++ */

#endif /* !__LREAD_H__ */

