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


#ifndef __IR_BCOM_H__
#define __IR_BCOM_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

extern const char *Whirl_Revision;

/* This variable is used by IPAA to pass its local map information
 * to ir_bwrite.c, and by ir_bread.c to pass it to WOPT:
 */
extern void *IPAA_Local_Map;

extern BOOL Doing_mmapped_io;

inline off_t
ir_b_align (off_t offset, UINT32 addralign, UINT32 padding)
{
     if (addralign-- > 1)
	return ((offset + padding + addralign) & ~(off_t)addralign) - padding;
    else
	return offset;
}

extern Elf64_Word
ir_b_save_buf (const void *buf, Elf64_Word size, UINT32 align,
	       UINT32 padding, Output_File *fl);

extern Elf64_Word
ir_b_copy_file (const void *buf, Elf64_Word size, void *fl);

extern char*
ir_b_grow_map (Elf64_Word min_size, Output_File *fl);

extern char *
ir_b_create_map (Output_File *fl);

#if defined(KEY) && !defined(FRONT_END) && !defined(IR_TOOLS)
extern Elf64_Word
ir_b_write_tree (WN *, off_t, Output_File *, WN_MAP, PU_Info *);
#else
extern Elf64_Word
ir_b_write_tree (WN *node, off_t base_offset, Output_File *fl, WN_MAP off_map);
#endif

extern Elf64_Word
ir_b_write_dst (DST_TYPE dst, off_t base_offset, Output_File *fl);


extern Elf64_Word
ir_b_write_global_symtab (off_t base_offset, Output_File *fl);

extern Elf64_Word
ir_b_write_local_symtab (const SCOPE& pu, off_t base_offset, Output_File *fl);


extern void
IPA_irb_write_summary(Output_File *fl);

extern void
IPA_irb_write_mod_ref_info(Output_File *);

#ifdef __cplusplus
}
#endif /* __cplusplus */
    
#endif /* __IR_BCOM_H__ */
