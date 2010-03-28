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


#ifndef __OPEN_H__
#define __OPEN_H__


#ident "$Revision$"

#include <stdio.h>
#include <cmplrs/fio.h>

#if defined(_LANGUAGE_C) || defined(_LANGUAGE_C_PLUS_PLUS)
/* PROTOTYPES */
 
extern int f_open1(olist *, int *);
extern int f_open0(olist *, int *);
extern int fk_open(int, int, ftnint);
extern int absent(char *, int);
extern int inc_var(ftnintu *, ftnint, int);
extern int set_var(ftnintu *, ftnint, int, long long);
extern int f_dfnf(struct dfnf_struct *);
extern int f_dfnf1(struct dfnf_struct *, ftnint *);
#if 11
extern int f_duped (olist64 *, unit *, unit **);
#else
extern int f_duped (olist *, unit *, unit **);
#endif

#if 11
extern int f_open064 (olist64 *a, int *mask);
extern int f_open064x (olist64 *a, XINT xmask);
extern int f_open064x_mp (olist64 *a, XINT xmask);
#endif


#endif /* C || C++ */

#endif /* !__OPEN_H__ */
