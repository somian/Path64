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


#ifndef __ECVT_H__
#define __ECVT_H__


#ident "$Revision$"


#if defined(_LANGUAGE_C) || defined(_LANGUAGE_C_PLUS_PLUS)
/* PROTOTYPES */
/*
#include "synonyms.h"
#include "math_extern.h"
*/

/* declare _dtoa (which is in libc) */
extern int _dtoa(char *buffer, int ndigit, double x, int fflag);
extern int _qtoa(char *, int, long double, int);


extern char *ecvt_mp (double, int, int *, int *, char *);
extern char *fcvt_mp (double, int, int *, int *, char *);
extern char *qecvt_mp (long double, int, int *, int *, char *);
extern char *qfcvt_mp (long double, int, int *, int *, char *);

#endif /* C || C++ */

#endif /* !__ECVT_H__ */





