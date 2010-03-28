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


#ifndef __DFE_H__
#define __DFE_H__


#ident "$Revision$"

#include <stdio.h>
#include <cmplrs/fio.h>

#if defined(_LANGUAGE_C) || defined(_LANGUAGE_C_PLUS_PLUS)
/* PROTOTYPES */
extern int s_rdfe(cilist *);
extern int s_rdfe_mp(cilist *, unit **);
extern int s_wdfe(cilist *);
extern int s_wdfe_mp(cilist *, unit **);
extern int e_rdfe();
extern int e_rdfe_mp(unit **);
extern int e_wdfe();
extern int e_wdfe_mp(unit **);

#if 11
extern int s_rdfe64(cilist64 *);
extern int s_rdfe64_mp(cilist64 *, unit **);
extern int s_wdfe64(cilist64 *);
extern int s_wdfe64_mp(cilist64 *, unit **);
extern int e_rdfe64();
extern int e_rdfe64_mp(unit **);
extern int e_wdfe64();
extern int e_wdfe64_mp(unit **);
#endif

extern int y_rsk(unit *);
extern int yrd_SL(unit *);
extern int y_ungetc(unit *, int);
extern int y_getc(unit *);
extern int y_gets(unit *, char *, int, char);
extern int y_putc(unit *, XINT, char, char *);
extern int y_rev(unit *);
extern int y_end(unit *);
extern int y_wSL(unit *);

#endif /* C || C++ */

#endif /* !__DFE_H__ */





