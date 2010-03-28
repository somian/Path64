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


#ifndef cxx_ipo_main_INCLUDED
#define cxx_ipo_main_INCLUDED


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

extern INT IPO_Total_Inlined;
extern INT IPO_Gprel_Sym_Count;
extern INT IPO_Internal_Sym_Count;
extern INT IPO_Hidden_Sym_Count;
extern INT IPO_Protected_Sym_Count;
extern BOOL one_got;

extern void Perform_Interprocedural_Optimization (void);

#ifdef __cplusplus
}
#endif /* __cplusplus */
    
#endif
