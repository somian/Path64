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


#ifndef __RSFE_H__
#define __RSFE_H__


#ident "$Revision$"

#include <stdio.h>
#include <cmplrs/fio.h>

#if defined(_LANGUAGE_C) || defined(_LANGUAGE_C_PLUS_PLUS)
/* PROTOTYPES */
extern int s_rsfe(cilist *);
extern int s_rsfe_mp(cilist *, unit **);
extern int xrd_SL(unit *);
extern int x_ungetc(unit *, int);
extern int x_gets(unit *, char *, int, char);
extern int x_getc(unit *);
extern int x_endp(unit *);
extern int x_rev(unit *);

#if 11
extern int s_rsfe64 (cilist64 *a);
extern int s_rsfe64_mp (cilist64 *a, unit **fu);
#endif	/* -n32 and -64 */

#endif /* C || C++ */

#endif /* !__RSFE_H__ */

