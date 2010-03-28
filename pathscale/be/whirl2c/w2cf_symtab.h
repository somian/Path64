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


#ifndef w2cf_symtab_INCLUDED
#define w2cf_symtab_INCLUDED
/* ====================================================================
 * ====================================================================
 *
 * Module: w2cf_symtab.h
 * $Revision$
 * $Date$
 * $Author$
 * $Source$
 *
 * Revision history:
 *  07-Oct-95 - Original Version
 *
 * Description:
 *  
 *  The symbol table has been implemented as a state-machine maintaining
 *  a stack of symbol-tables.  There are routines to push and pop symbol 
 *  tables, get the name of a given symbol, enter symbols into the 
 *  current symbol table, push and pop symbol tables, etc.
 *
 *  The memory allocated to hold symbol-names and the symbol-table data
 *  structure is never freed up until the use of this package is
 *  terminated or a forced free operation is invoked.
 *
 *  W2CF_Push_Symtab: Pushes a new symbol table onto the stack and
 *     makes it the current symbol-table new symbol entries.
 *
 *  W2CF_Pop_Symtab: Pops the last symbol table entered onto the stack
 *     off the stack, putting all related symbol-table objects onto
 *     free-lists.
 *
 *  W2CF_Symtab_Nameof_St:
 *  W2CF_Symtab_Nameof_St_Pointee:
 *  W2CF_Symtab_Nameof_Ty:
 *  W2CF_Symtab_Nameof_Fld:
 *  W2CF_Symtab_Nameof_Fld_Pointee:
 *  W2CF_Symtab_Nameof_Tempvar:
 *  W2CF_Symtab_Nameof_Preg:
 *     Tries to find the symbol in the symbol table, and if it has been
 *     found, returns its assigned name.  Numerical suffices are added 
 *     to names to disambiguate them from one another.  An attempt will
 *     be made to retain numeric suffices to names, but since such 
 *     suffices are used by this module to disambiguate names they may
 *     sometime be changed.
 *
 *  W2CF_Symtab_Unique_Name:
 *     Adds a name to the symbol table, which matches no other name.
 *     An attempt will be made to keep the name as it is given, but
 *     name-clashes may change it.  There is no way to retrieve this
 *     name again, as its sole purpose is to permanently occupy a
 *     slot in the current symbol-table.
 *
 *  W2CF_Symtab_Unique_Label:
 *     This assumes that label-numbers around 99999 are not used.
 *     Starting with that number, this function returns a number
 *     decremented by one every time it is called.
 *
 *  W2CF_Symtab_Free: Frees up from memory all objects kept on free-
 *     lists, i.e. all symbol-table related information that was used
 *     at some point, but which is not currently in use.
 *
 *  W2CF_Symtab_Terminate: Pops all symbol-tables off the stack and
 *     frees up all memory allocated.  Resets the state to allow
 *     subsequent use of this module.
 *
 * ====================================================================
 * ====================================================================
 */


void W2CF_Symtab_Push(void);
void W2CF_Symtab_Pop(void);

const char * W2CF_Symtab_Nameof_St(const ST *st);
const char * W2CF_Symtab_Nameof_St_Pointee(const ST *st);
const char * W2CF_Symtab_Nameof_Ty(TY_IDX ty);
const char * W2CF_Symtab_Nameof_Fld(FLD_HANDLE fld);
const char * W2CF_Symtab_Nameof_Fld_Pointee(FLD_HANDLE fld);
const char * W2CF_Symtab_Nameof_Tempvar(INT32 tempvar_id);
const char * W2CF_Symtab_Nameof_Preg(TY_IDX preg_ty, PREG_NUM preg_num);

const char * W2CF_Symtab_Unique_Name(const char *name);
UINT32 W2CF_Symtab_Unique_Label(void);

void W2CF_Symtab_Free(void);
void W2CF_Symtab_Terminate(void);


#endif /* w2cf_symtab */
