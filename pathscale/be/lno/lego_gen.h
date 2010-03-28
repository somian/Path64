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


#ifndef _LEGO_GEN_INCLUDED_
#define _LEGO_GEN_INCLUDED_

/***********************************************************************
 *
 * Generate code for lego pragmas.
 *
 * Exported functions:
 *
 *  extern void Generate_Runtime_Stuff ();
 *      Called before ANY LNO processing, from Lno_Init.
 *      Generate ST and TY entries in the global symbol table
 *      for functions and types in the MP runtime.
 *
 *  extern void Lower_Distr_Pragmas (WN* func_nd);
 *      Generate code for data distribution pragmas.
 *      Called on a per-PU basis at the end of LNO processing.
 *      Generates code into the PU for each data distribution pragma
 *      contained in the global variables da_hash and da_hash_redistr.
 *
 *  extern void Rewrite_Reshaped_Commons (WN* wn)
 *      Walk the entire tree, replacing all references to a reshaped
 *      common array to be to the new ST in the new common block that we
 *      have generated.
 *
 ***********************************************************************/

#include "lego_pragma.h"
#include "lego_util.h"

extern "C" void Generate_Runtime_Stuff ();
extern void Lower_Distr_Pragmas (WN* func_nd);
extern WN*  Get_Array_Dimension_Size (TY* array_ty, INT i); 
extern void Init_Special_Lego_Mp_Call();
extern BOOL Special_Lego_Or_Mp_Call(ST* st_call); 
extern ST *Create_Local_Array_ST(char* array_name, TY_IDX ty, INT num);
extern void Rewrite_Reshaped_Commons (WN* wn);

#endif // _LEGO_GEN_INCLUDED_
