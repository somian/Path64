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


#ifndef __DUE_H__
#define __DUE_H__


#ident "$Revision$"

#include <stdio.h>
#include <cmplrs/fio.h>

#if defined(_LANGUAGE_C) || defined(_LANGUAGE_C_PLUS_PLUS)
/* PROTOTYPES */
extern int   f_find(flist *);
extern int   s_rdue(cilist *);
extern int   s_wdue(cilist *);
extern int   e_rdue(void);
extern int   e_wdue(void);
extern int   f_find_mp(flist *);
extern int   s_rdue_mp(cilist *, unit **);
extern int   s_wdue_mp(cilist *, unit **);
extern int   e_rdue_mp(unit **);
extern int   e_wdue_mp(unit **);

#if 11
extern int   f_find64(flist64 *);
extern int   s_rdue64(cilist64 *);
extern int   s_wdue64(cilist64 *);
extern int   e_rdue64(void);
extern int   e_wdue64(void);
extern int   f_find64_mp(flist64 *);
extern int   s_rdue64_mp(cilist64 *, unit **);
extern int   s_wdue64_mp(cilist64 *, unit **);
extern int   e_rdue64_mp(unit **);
extern int   e_wdue64_mp(unit **);
#endif

#endif /* C || C++ */

#endif /* !__DUE_H__ */
