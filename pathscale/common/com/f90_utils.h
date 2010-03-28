/*
 * Copyright 2003, 2004, 2005, 2006 PathScale, Inc.  All Rights Reserved.
 */

/*

  Copyright (C) 2000, 2001 Silicon Graphics, Inc.  All Rights Reserved.

   Path64 is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

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



#ifndef f90_utils_INCLUDED
#define f90_utils_INCLUDED
#ifdef __cplusplus
extern "C" {
#endif
   
   
#define MAX_NDIM 7 /* Max number of dimensions allowed */
   
extern BOOL F90_Is_Transformational(INT32 intrinsic);
extern BOOL F90_Is_Char_Intrinsic(INT32 intr);
extern INT F90_Get_Dim(WN *dim_wn);
extern BOOL F90_Size_Walk(WN *expr, INT *ndim, WN **sizes);
extern INT  F90_Rank_Walk(WN *expr);
extern WN * F90_Wrap_ARREXP(WN *expr);
extern WN * F90_wrap_cvtl(WN * wn, TYPE_ID ty);

#ifdef __cplusplus
}
#endif
#endif /* f90_utils_INCLUDED */





