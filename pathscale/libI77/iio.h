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


#ifndef __IIO_H__
#define __IIO_H__


#ident "$Revision$"

#include <stdio.h>
#include <cmplrs/fio.h>

#if defined(_LANGUAGE_C) || defined(_LANGUAGE_C_PLUS_PLUS)
/* PROTOTYPES */
extern int z_ungetc(unit *, int);
extern int z_getc(unit *);
extern int z_gets(unit *, char *, int, char);
extern int z_putc(unit *, XINT, char, char *);
extern int z_wnew(unit *);
extern int z_rnew(unit *);
extern int s_wsfi(icilist *);
extern int s_wsfi_mp(icilist *, unit**);
extern int z_rSL(unit *);
extern int z_wSL(unit *);
extern int y_ierr(unit *);
extern int e_rsfi();
extern int e_wsfi();
extern int e_rsfi_mp(unit **);
extern int e_wsfi_mp(unit **);

#if 11
extern int s_wsfi64(icilist64 *);
extern int s_wsfi64_mp(icilist64 *, unit**);
extern int e_rsfi64();
extern int e_wsfi64();
extern int e_rsfi64_mp(unit **);
extern int e_wsfi64_mp(unit **);
extern int c_si(icilist64 *, unit *);
#else
extern int c_si(icilist *, unit *);
#endif

extern XINT icnum, icpos;
extern char *icptr, *icend;
#endif /* C || C++ */

#endif /* !__IIO_H__ */
