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


// gen_util_gen.h
////////////////////////////////////
//
// Utility functions to help in generating .c and .h files.
// The functions are for use by generator clients (the specification files)
// or the generators themselves.
//
//  void Define_Macro (const char *name, const char *def)
//     Define a macro.
//
////////////////////////////////////

//  $Revision$
//  $Date$
//  $Author$
//  $Source$

#ifndef gen_util_gen_INCLUDED
#define gen_util_gen_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

extern void Define_Macro (const char *name, const char *def);

#ifdef __cplusplus
}
#endif
#endif /* gen_util_gen_INCLUDED */
