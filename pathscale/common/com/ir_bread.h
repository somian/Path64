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


#ifndef __IR_BREAD_H__
#define __IR_BREAD_H__

#include "wn_map.h"

#define REVISION_MISMATCH 0
#define READER_ERROR -1
#define ABI_MISMATCH -2
				       

struct OFFSET_AND_SIZE
{
    INT64 offset;
    UINT64 size;

    OFFSET_AND_SIZE (INT64 o, UINT64 s) : offset (o), size (s) {}
};


/*
 * Exported interface for reading binary WHIRL from a file.
 */

OFFSET_AND_SIZE
get_section (void *handle, Elf64_Word type, Elf64_Word info);    

/* Enable/disable verbose info for debugging. */
extern void Set_Verbose_Info (BOOL val);

/* Check the Elf headers (called from elfdump) */
extern INT WN_massage_input (char *baseaddr, Elf64_Word size, char*);

/* Find the beginning of a particular section */
extern void *WN_get_section_base (void *handle, INT sect);

/* return size of elf section */
extern Elf64_Word Get_Elf_Section_Size (void *handle, Elf64_Word type, Elf64_Word info);


/* Call these once at the beginning and end, respectively. */
extern void *WN_inline_open_file (char* file_name, off_t *mapped_size,
				  char* file_revision); 

extern void *
WN_open_input (char *filename, off_t *mapped_size);


/*
 * Read the PU subsection headers into a tree of PU_Info structures.
 * Returns a pointer to the root of the tree or -1 on error.  The
 * number of PUs is returned through the p_num_PUs parameter.
 */

extern PU_Info *WN_get_PU_Infos (void *handle, INT32 *p_num_PUs);


/*
 * Read the PU subsections.  These return -1 for errors.
 */

extern WN *WN_get_tree (void *handle, PU_Info *pu);
extern INT WN_get_symtab (void *handle, PU_Info *pu);
extern void *WN_get_depgraph (void *handle, PU_Info *pu);
extern INT WN_get_prefetch (void *handle, PU_Info *pu);

extern INT WN_get_INT32_map(void *handle, PU_Info *pu,
			    INT32 subsection_type, WN_MAP value_map);

extern ST *WN_get_proc_sym (PU_Info *pu);


/*
 * Read the global tables.  These are usually called right after opening
 * the input file.  They return -1 for errors.
 */


extern INT WN_get_global_symtab (void *handle);
extern INT WN_get_strtab (void *handle);


extern INT WN_get_dst (void *handle);
extern INT WN_get_localmap (void *handle);
extern INT WN_get_feedback (void* handle, PU_Info*pu, MEM_POOL* pool);    

/*
 * Read the command line flags used to compile the input file.  The
 * return value is argc (or 0 if the flags are missing) and the argv
 * array is returned through the argv parameter.
 */

extern INT WN_get_flags (void *handle, char ***argv);


#ifndef OWN_ERROR_PACKAGE
/*
 * Define common routines for reading all the whirl sections.
 * These routines use the standard compiler error reporting mechanism.
 */

/* Open_Input_Info opens a single file for global and local info;
 * Open_Global_Input only opens the global file,
 * Open_Local_Input only opens the local file.
 */
extern void* Open_Input_Info (char *input_file);
extern void* Open_Global_Input (char *input_file);
extern void* Open_Local_Input (char *input_file);
extern PU_Info *Read_Global_Info (INT32 *p_num_PUs);
extern void Read_Local_Info (MEM_POOL *pool, PU_Info *pu);
extern void Free_Local_Info (PU_Info *pu);
extern void Free_Dep_Graph (void);
extern void Free_Local_Input(void);
extern void Free_Input_Info (void);

#endif

#endif /* __IR_BREAD_H__ */
