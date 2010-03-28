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


#ifndef __RDFMT_H__
#define __RDFMT_H__


#ident "$Revision$"

#include <stdio.h>
#include <cmplrs/fio.h>

#if defined(_LANGUAGE_C) || defined(_LANGUAGE_C_PLUS_PLUS)
/* PROTOTYPES */
extern int s_rsfi(icilist *);
extern int s_rsfi_mp(icilist *, unit **);
extern int __s_rsfi_com (icilist64 *a, unit **fu, int f90sw);
extern int rd_ed(unit *, struct f77syl *, char *, ftnlen, ftnint);
extern int rd_ned(unit *, struct f77syl *);
extern int rd_I(unit *, uinteger *, long, ftnlen);
extern int rd_OZ(unit *, unsigned char *, long, ftnlen, int);
extern int rd_Q(unit *, uinteger *, ftnlen);
extern int rd_L(unit *, uinteger *, long, ftnlen);
extern int rd_F(unit *, ufloat *, long, long, ftnlen);
extern int rd_A(unit *, char *, ftnlen);
extern int rd_AW(unit *, char *, long, ftnlen);
extern int rd_H(unit *, long);
extern int rd_POS(unit *, char *);
/* extern int c_si(icilist *, unit *); declared in iio.h */

extern int rd_B (unit *ftnunit, unsigned char *n, long w, ftnlen len);


#if 11
extern int s_rsfi64 (icilist64 *a);
extern int s_rsfi64_mp (icilist64 *a, unit **fu);
#endif


#endif /* C || C++ */

#endif /* !__RDFMT_H__ */





