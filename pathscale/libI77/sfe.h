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


#ifndef __SFE_H__
#define __SFE_H__


#ident "$Revision$"

#include <stdio.h>
#include <cmplrs/fio.h>

#if defined(_LANGUAGE_C) || defined(_LANGUAGE_C_PLUS_PLUS)
/* PROTOTYPES */
extern int e_rsfe(void);
extern int e_rsfe_mp(unit **);
#if 11
extern int c_sfe (cilist64 *a, unit **fu);
#else
extern int c_sfe (cilist *a, unit **fu);
#endif
extern int e_wsfe(void);
extern int e_wsfe_mp(unit **);
extern int e_xsfe(void);
extern int e_xsfe_mp(unit **);
extern int e_wsue(void);
extern int e_wsue_mp(unit **);

#if 11
extern int e_rsfe64(void);
extern int e_rsfe64_mp(unit **);
extern int e_wsfe64(void);
extern int e_wsfe64_mp(unit **);
extern int e_xsfe64(void);
extern int e_xsfe64_mp(unit **);
extern int e_wsue64(void);
extern int e_wsue64_mp(unit **);
#endif

#endif /* C || C++ */

#endif /* !__SFE_H__ */
