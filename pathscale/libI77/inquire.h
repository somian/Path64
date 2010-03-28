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


#ifndef __INQUIRE_H__
#define __INQUIRE_H__


#ident "$Revision$"

#include <stdio.h>
#include <cmplrs/fio.h>

#if defined(_LANGUAGE_C) || defined(_LANGUAGE_C_PLUS_PLUS)
/* PROTOTYPES */
extern int f_inqu0(inlist *, int *);
extern int f_inqu0_mp(inlist *, int *);

#if 11
extern int f_inqu064(inlist64 *, int *);
extern int f_inqu064_mp(inlist64 *, int *);
extern int f_inqu064x (inlist64 *a, XINT xmask);
extern int f_inqu064x_mp (inlist64 *a, XINT xmask);
#else
extern int f_inqu064(inlist *, int *);
extern int f_inqu064_mp(inlist *, int *);
extern int f_inqu064x (inlist *a, XINT xmask);
extern int f_inqu064x_mp (inlist *a, XINT xmask);
#endif

#endif /* C || C++ */

#endif /* !__INQUIRE_H__ */


