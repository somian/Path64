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


#ifndef __SUE_H__
#define __SUE_H__


#ident "$Revision$"

#include <stdio.h>
#include <cmplrs/fio.h>

#if defined(_LANGUAGE_C) || defined(_LANGUAGE_C_PLUS_PLUS)
/* PROTOTYPES */
extern int do_ui(unit *, XINT *, char *, ftnlen);
extern int s_rsue(cilist *);
extern int s_rsue_mp(cilist *, unit**);
extern int s_wsue(cilist *);
extern int s_wsue_mp(cilist *, unit **);
extern int e_rsue(void);
extern int e_rsue_mp(unit**);
extern int unf_position(FILE *, unit *);

#if 11
extern int wsue (cilist64 *a, unit **fu);
extern int c_sue (cilist64 *a, unit **fu);
#else
extern int wsue (cilist *a, unit **fu);
extern int c_sue (cilist *a, unit **fu);
#endif

#if 11
extern int s_rsue64 (cilist64 *a);
extern int s_rsue64_mp (cilist64 *a, unit **fu);
extern int s_wsue64 (cilist64 *a);
extern int s_wsue64_mp (cilist64 *a, unit **fu);
extern int e_rsue64(void);
extern int e_rsue64_mp(unit**);
#endif


#endif /* C || C++ */

#endif /* !__SUE_H__ */
