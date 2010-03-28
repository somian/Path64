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


#ifndef __WRTFMT_H__
#define __WRTFMT_H__


#ident "$Revision$"

#include <stdio.h>
#include <cmplrs/fio.h>

#if defined(_LANGUAGE_C) || defined(_LANGUAGE_C_PLUS_PLUS)
/* PROTOTYPES */
extern int wrt_I(unit *, uinteger *, int, ftnlen);
extern int wrt_LL(unit *, uinteger *, int);
extern int wrt_L(unit *, uinteger *, int, ftnlen);
extern int wrt_G(unit *, void *, int, int, int, int, ftnlen, ftnint, flag);
extern int w_ed(unit *, struct f77syl *, char *, ftnlen, ftnint);
extern int w_ned(unit *, struct f77syl *);

#endif /* C || C++ */

#endif /* !__WRTFMT_H__ */





