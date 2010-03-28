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


#ifndef __IDXIO_H__
#define __IDXIO_H__


#ident "$Revision$"

#include <stdio.h>
#include <cmplrs/fio.h>

#if defined(_LANGUAGE_C) || defined(_LANGUAGE_C_PLUS_PLUS)
/* PROTOTYPES */
extern int idxopen(unit *, char *, int, flag);
extern int idxclose(unit *, flag);
extern int idxread(unit *);
extern int iscleanup(void);
extern void s_idxwrite(unit *);
extern int idxwrite(unit *);
extern int idxrewrite(unit *);
extern int dokey(int, int);
extern int f_del(alist *);
extern int f_del_mp (alist *a);
extern int f_unl(alist *);
extern int f_unl_mp (alist *a);
extern int s_xsue(cilist *);
extern int s_xsue_mp(cilist *, unit **);
extern int s_xsle(cilist *);
extern int s_xsle_mp(cilist *, unit **);
extern int s_xsfe(cilist *);
extern int s_xsfe_mp(cilist *, unit **);
extern int e_xsue(void);
extern int e_xsue_mp(unit **);
#if 11
extern int f_del64(alist *);
extern int f_del64_mp (alist *a);
extern int f_unl64(alist *);
extern int f_unl64_mp (alist *a);
extern int s_xsue64(cilist64 *);
extern int s_xsue64_mp(cilist64 *, unit **);
extern int s_xsle64(cilist64 *);
extern int s_xsle64_mp(cilist64 *, unit **);
extern int s_xsfe64(cilist64 *);
extern int s_xsfe64_mp(cilist64 *, unit **);
extern int e_xsue64(void);
extern int e_xsue64_mp(unit **);
#endif

#endif /* C || C++ */

#endif /* !__IDXIO_H__ */





