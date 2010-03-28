/*
 * Copyright (C) 2007, 2008. PathScale, LLC. All Rights Reserved.
 */
/*
 * Copyright 2002, 2003, 2004, 2005, 2006 PathScale, Inc.  All Rights Reserved.
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



#ifndef cgemit_INCLUDED
#define cgemit_INCLUDED

#ifdef _KEEP_RCS_ID
static char *cgemit_rcs_id = "$Source: /home/bos/bk/kpro64-pending/be/cg/SCCS/s.cgemit.h $ $Revision: 1.7 $";
#endif /* _KEEP_RCS_ID */

#include "dwarf_DST_mem.h"
#include "symtab.h"

/* Initialize for emitting of a source file: */
extern void EMT_Begin_File (
  char *process_name,	/* Back end process name */
  char *options 	/* Option string used for compilation */
);

/* Finalize for emitting of a source file: */
extern void EMT_End_File ( void );

/* Emit the contents of a PU: */
extern void EMT_Emit_PU ( ST *pu, DST_IDX pu_dst, WN *rwn);

/* put symbol in elf symbol table */ 
#if defined(BUILD_OS_DARWIN)
/* Generally, global-scope symbols require a preceding underscore
 * Calls to external functions require a jump to a stub
 * Other references to external functions require an indirect pointer
 */
typedef enum { DO_UNDERSCORE, DO_STUB, DO_NON_LAZY_PTR } darwin_indirect_t;
extern mINT32 EMT_Put_Elf_Symbol (ST *sym,
  darwin_indirect_t indirect = DO_UNDERSCORE);
#else /* defined(BUILD_OS_DARWIN) */
extern mINT32 EMT_Put_Elf_Symbol (ST *sym);
#endif /* defined(BUILD_OS_DARWIN) */

/* change existing elf symbol to undefined */
extern void EMT_Change_Symbol_To_Undefined (ST *sym);

/* change existing elf symbol to weak binding */
extern void EMT_Change_Symbol_To_Weak (ST *sym);

/*
 * Check if bb should be aligned,
 * and return number of instructions it should be aligned with.
 */
extern INT Check_If_Should_Align_BB (BB *bb, INT32 curpc);

extern BOOL CG_emit_asm_dwarf;
extern BOOL CG_emit_unwind_info;
extern BOOL CG_emit_unwind_directives;
#ifdef TARG_X8664
extern BOOL CG_emit_unwind_info_Set;
#endif
#ifdef KEY
extern BOOL CG_emit_non_gas_syntax;
extern BOOL CG_inhibit_size_directive;
#endif

/* Write a qualified name to a file. */
extern void EMT_Write_Qualified_Name (FILE *f, ST *st);

#if defined(BUILD_OS_DARWIN)
/* Return log base two of an integer (used for .align) */
extern int logtwo(int value);
extern const char *map_section_name(const char *section_name);
extern int is_debug_section(const char *section_name);
#endif /* defined(BUILD_OS_DARWIN) */

#endif /* cgemit_INCLUDED */
