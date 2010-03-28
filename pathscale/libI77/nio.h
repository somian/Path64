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


#ifndef __NIO_H__
#define __NIO_H__


#ident "$Revision$"

#include <stdio.h>
#include <cmplrs/fio.h>

#if defined(_LANGUAGE_C) || defined(_LANGUAGE_C_PLUS_PLUS)

typedef union {
   char           *pchar;
   signed char    *pbyte;
   short          *pshort;
   int            *pint;
   long long      *plonglong;
   float          *pfloat;
   double         *pdouble;
   long double    *plongdouble;
   char          **pptr;
}               Pointer;	/* pointer to all sorts of things */

/* PROTOTYPES */
extern int s_rsne(cilist *);
extern int s_rsNe(cilist *);
extern int s_rsNe_mp(cilist *, unit **);
extern int s_wsne(cilist *);
extern int s_wsNe(cilist *);
extern int s_wsne_mp (cilist *pnlarg, unit **fu);
extern int s_wsNe_mp(cilist *, unit **);
/*  int c_nle(cilist *, unit **); internal only */
extern int __kai_s_rsne(cilist *);
extern int __kai_s_wsne(cilist *);
extern int __kai_s_rsne_mp(cilist *, unit **);
extern int __kai_s_wsne_mp(cilist *, unit **);

#if 11
extern int s_rsNe64(cilist64 *pnlarg);
extern int s_rsNe64_mp(cilist64 *pnlarg, unit **fu);
extern int s_wsNe64(cilist64 *pnlarg);
extern int s_wsNe64_mp(cilist64 *pnlarg, unit **fu);
extern int __kai_s_rsne64(cilist64 *pnlarg);
extern int __kai_s_rsne64_mp(cilist64 *pnlarg, unit **fu);
extern int __kai_s_wsne64(cilist64 *pnlarg);
extern int __kai_s_wsne64_mp(cilist64 *pnlarg, unit **fu);
#endif

#endif /* C || C++ */

#endif /* !__NIO_H__ */





