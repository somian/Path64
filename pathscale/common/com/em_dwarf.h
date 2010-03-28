/*
 *  Copyright (C) 2007. QLogic Corporation. All Rights Reserved.
 */

/*
 * Copyright 2004, 2005, 2006 PathScale, Inc.  All Rights Reserved.
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


#ifndef em_dwarf_INCLUDED
#define em_dwarf_INCLUDED

#include "dwarf_stuff.h"
#include "srcpos.h"
#include "targ_em_dwarf.h"

#if defined(__cplusplus)
extern Dwarf_P_Debug Em_Dwarf_Begin (BOOL is_64bit,
				     BOOL dwarf_trace,
				     BOOL is_cplus,
				     symbol_index_recorder record_symidx = NULL);

extern void Em_Dwarf_Write_Scns (Cg_Dwarf_Sym_To_Elfsym_Ofst translate_elfsym = NULL);
#else
extern Dwarf_P_Debug Em_Dwarf_Begin (BOOL is_64bit,
				     BOOL dwarf_trace,
				     BOOL is_cplus);

extern void Em_Dwarf_Write_Scns (void);
#endif

#if defined(__cplusplus)
extern "C" {
#endif

extern INT data_alignment_factor;

extern void Em_Dwarf_End (void);

extern pSCNINFO Em_Dwarf_Find_Dwarf_Scn (size_t scndx);

extern pSCNINFO Em_Dwarf_Find_Dwarf_Scn_By_Name (char *name);

extern INT Em_Dwarf_Prepare_Output (void);

extern void Em_Dwarf_Add_Line_Entry ( INT code_address, SRCPOS srcpos);

extern void Em_Dwarf_Add_Include (UINT16 incl_idx, const char *name);

extern void Em_Dwarf_Add_File (
    UINT16 file_idx,
    const char *name,
    UINT16 incl_idx,
    Dwarf_Unsigned modification_time,
    Dwarf_Unsigned file_size);


/* Given a file_index, return the filename and the path for the file. */
extern void Em_Dwarf_File_Index_To_Name (
    INT file_index, 
    char **filename, 
    char **path);

/* eh_offset should be -1 if no eh region */
extern void Em_Dwarf_Process_PU (Dwarf_Unsigned begin_label,
				 Dwarf_Unsigned end_label,
				 INT32          begin_offset,
				 INT32          end_offset,
				 Dwarf_P_Die    PU_die,
				 Dwarf_P_Fde    fde,
#ifdef KEY
				 Dwarf_P_Fde    eh_fde,
#endif
				 Elf64_Word     eh_symindex,
				 INT            eh_offset);
#ifdef TARG_X8664
/* To add FDEs for ALT ENTRY points for a PU (if any) */
extern void Em_Dwarf_Add_PU_Entries (Dwarf_Unsigned begin_label,
				     Dwarf_Unsigned end_label,
				     INT32          begin_offset,
				     INT32          end_offset,
				     Dwarf_P_Die    PU_die,
				     Dwarf_P_Fde    fde);
#endif

extern void Em_Dwarf_Start_Text_Region (pSCNINFO scninfo, INT start_offset);

extern void Em_Dwarf_Start_Text_Region_Semi_Symbolic (pSCNINFO,
						      INT,
						      Dwarf_Unsigned,
						      Dwarf_Addr);

extern void Em_Dwarf_End_Text_Region (pSCNINFO scninfo, INT end_offset);

extern void Em_Dwarf_End_Text_Region_Semi_Symbolic (pSCNINFO,
						    INT,
						    Dwarf_Unsigned,
						    Dwarf_Addr);
#if defined(__cplusplus)
}
#endif
#endif /* em_dwarf_INCLUDED */
