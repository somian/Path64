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


// -*-Mode: c++;-*- (Tell emacs to use c++ mode) 

#ifndef symtab_verify_INCLUDED
#define symtab_verify_INCLUDED
#include "config.h" // For LANG_Symtab_Verify_On

//
// Extern interface for Symbol Table Verifiers; 

extern void ST_Verify_Class_Sclass (ST_CLASS, ST_SCLASS );

extern void ST_Verify_Sclass_Export (ST_SCLASS, ST_EXPORT, const ST* st = NULL);

extern void TY_Verify_Kind_Mtype (TY_KIND , mTYPE_ID );

/*ARGSUSED*/ 
inline void TY_Verify_Kind_Function(TY_KIND k, UINT64 s, TYPE_ID t) {
  Is_True( k == KIND_FUNCTION && 
           s == 0 && t == 0,
           ("ty.size/mtype should be 0 for KIND_FUNCTION"));
}
  

extern void  Verify_LOCAL_SYMTAB (const SCOPE& , SYMTAB_IDX );

extern void  Verify_GLOBAL_SYMTAB ();

// ======================================================================
//  Verify_SYMTAB(): inline function
// ======================================================================

inline void
Verify_SYMTAB (SYMTAB_IDX level)
{
  if (!LANG_Symtab_Verify_On) {
    DevWarn ("The NEW SYMTAB Verification is turned OFF; Proceed on your own");
    return;
  }

  // We really do want to verify symbol tables

  if (level > GLOBAL_SYMTAB)
    Verify_LOCAL_SYMTAB (Scope_tab[level], level);
  else
    Verify_GLOBAL_SYMTAB ();
}

#endif // symtab_verify_INCLUDED
