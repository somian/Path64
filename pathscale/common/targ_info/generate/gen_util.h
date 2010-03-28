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


// gen_util.h
////////////////////////////////////
//
// Utility functions to help in generating .c and .h files.
// These functions are only to be used by the generators.
//
//  void Emit_Header (FILE *hfile, 
//		      const char *name, 
//		      const char * const *interface_desc)
//     Write out the standard h file header to <hfile>. The basename
//     of the header being created is specified by <name>. An optional
//     interface description is specified by <interface_desc> (pass NULL
//     if it is not desired).
//
//  void Emit_Footer (FILE *file)
//     Write out the standard h file footer to <hfile>.
//
//  void Emit_Definitions (FILE *hfile, const char *prefix)
//     Write out client specified definitions (specified with Define_xxx)
//     to <hfile>. The definition names will all be prefxed by <prefix>.
//
////////////////////////////////////

//  $Revision$
//  $Date$
//  $Author$
//  $Source$

#ifndef gen_util_INCLUDED
#define gen_util_INCLUDED

#include "gen_util_gen.h"

#ifdef __cplusplus
extern "C" {
#endif

extern void Emit_Header (FILE *hfile, 
			 const char *name, 
			 const char * const *interface_desc);
extern void Emit_Footer (FILE *hfile);
extern void Emit_Definitions (FILE *hfile, const char *prefix);

#ifdef __cplusplus
}
#endif
#endif /* gen_util_INCLUDED */
