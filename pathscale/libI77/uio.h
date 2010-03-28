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


#ifndef __UIO_H__
#define __UIO_H__


#ident "$Revision$"

#include <stdio.h>
#include <cmplrs/fio.h>

#if defined(_LANGUAGE_C) || defined(_LANGUAGE_C_PLUS_PLUS)
/* PROTOTYPES */
extern int do_us(unit *, XINT *, char *, ftnlen);
extern int s_usrecsize(int);
extern int do_uio64_mp_1dim( char *ptr, flex *do_idx,
                 XINT *lb, XINT *ub,
                 XINT *step, unit **fu,
                 ftnlen len, ftnlen idxlen);
extern int do_uio_1dim(char *, flex *, ftnint *, ftnint *, ftnint *, ftnlen, ftnlen);
extern void set_do_idx( flex *idx, ftnlen len, ftnll val );
extern int do_uio(ftnint *, char *, ftnlen);
extern int do_uio_mp(ftnint *, char *, unit **, ftnlen);
extern int do_ud(unit *, XINT *, char *, ftnlen);
extern int check_buflen(unit *, XINT);
extern int est_reclen;

extern int do_uio64_1dim(char *, flex *, XINT *, XINT *, XINT *, ftnlen, ftnlen);
extern int do_uio64 (XINT *number, char *ptr, ftnlen len);
extern int do_uio64_mp (XINT *number, char *ptr, unit **fu, ftnlen len);

extern int   do_uioxa4(char *, XINT);
extern int   do_uioxa8(char *, XINT);
extern int   do_uioxh1(char *, XINT, XINT);
extern int   do_uioxi1(char *, XINT);
extern int   do_uioxi2(char *, XINT);
extern int   do_uioxi4(char *, XINT);
extern int   do_uioxi8(char *, XINT);
extern int   do_uioxl1(char *, XINT);
extern int   do_uioxl2(char *, XINT);
extern int   do_uioxl4(char *, XINT);
extern int   do_uioxl8(char *, XINT);
extern int   do_uioxr4(char *, XINT);
extern int   do_uioxr8(char *, XINT);
extern int   do_uioxr16(char *, XINT);
extern int   do_uioxc4(char *, XINT);
extern int   do_uioxc8(char *, XINT);
extern int   do_uioxc16(char *, XINT);
extern int   do_uioxa4v(ftnint);
extern int   do_uioxa8v(ftnll);
extern int   do_uioxh1v(char);
extern int   do_uioxi1v(char);
extern int   do_uioxi2v(short);
extern int   do_uioxi4v(ftnint);
extern int   do_uioxi8v(ftnll);
extern int   do_uioxl1v(char);
extern int   do_uioxl2v(short);
extern int   do_uioxl4v(ftnint);
extern int   do_uioxl8v(ftnll);
extern int   do_uioxr4v(float);
extern int   do_uioxr8v(double);
extern int   do_uioxr16v(long double);
extern int   do_uioxc4v(float, float);
extern int   do_uioxc8v(double, double);
extern int   do_uioxc16v(long double, long double);

extern int   do_uioxa4_mp(char *, XINT, unit **);
extern int   do_uioxa8_mp(char *, XINT, unit **);
extern int   do_uioxh1_mp(char *, XINT, XINT, unit **);
extern int   do_uioxi1_mp(char *, XINT, unit **);
extern int   do_uioxi2_mp(char *, XINT, unit **);
extern int   do_uioxi4_mp(char *, XINT, unit **);
extern int   do_uioxi8_mp(char *, XINT, unit **);
extern int   do_uioxl1_mp(char *, XINT, unit **);
extern int   do_uioxl2_mp(char *, XINT, unit **);
extern int   do_uioxl4_mp(char *, XINT, unit **);
extern int   do_uioxl8_mp(char *, XINT, unit **);
extern int   do_uioxr4_mp(char *, XINT, unit **);
extern int   do_uioxr8_mp(char *, XINT, unit **);
extern int   do_uioxr16_mp(char *, XINT, unit **);
extern int   do_uioxc4_mp(char *, XINT, unit **);
extern int   do_uioxc8_mp(char *, XINT, unit **);
extern int   do_uioxc16_mp(char *, XINT, unit **);
extern int   do_uioxa4v_mp(ftnint, unit **);
extern int   do_uioxa8v_mp(ftnll, unit **);
extern int   do_uioxh1v_mp(char, unit **);
extern int   do_uioxi1v_mp(char, unit **);
extern int   do_uioxi2v_mp(short, unit **);
extern int   do_uioxi4v_mp(ftnint, unit **);
extern int   do_uioxi8v_mp(ftnll, unit **);
extern int   do_uioxl1v_mp(char, unit **);
extern int   do_uioxl2v_mp(short, unit **);
extern int   do_uioxl4v_mp(ftnint, unit **);
extern int   do_uioxl8v_mp(ftnll, unit **);
extern int   do_uioxr4v_mp(float, unit **);
extern int   do_uioxr8v_mp(double, unit **);
extern int   do_uioxr16v_mp(long double, unit **);
extern int   do_uioxc4v_mp(float, float, unit **);
extern int   do_uioxc8v_mp(double, double, unit **);
extern int   do_uioxc16v_mp(long double, long double, unit **);

#endif /* C || C++ */

#endif /* !__UIO_H__ */

