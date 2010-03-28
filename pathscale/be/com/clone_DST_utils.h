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


/* -*-Mode: c++;-*- (Tell emacs to use c++ mode) */

#ifndef clone_DST_utils_INCLUDED
#define clone_DST_utils_INCLUDED


#include "defs.h"

#ifndef dwarf_DST_mem_INCLUDED
#include "dwarf_DST_mem.h"              // for DST_IDX
#endif

#include "symtab.h"

// To avoid extra includes:
typedef struct mem_pool MEM_POOL;
class  IPO_SYMTAB;

extern mUINT16
DST_get_cross_file_id(DST_IDX parent,
                      DST_IDX inl_routine, 
                      DST_TYPE caller_file_dst,
                      DST_TYPE callee_file_dst);

extern void
DST_enter_inlined_subroutine(DST_IDX parent,
                             DST_IDX inl_routine,
                             LABEL_IDX begin_label,
                             LABEL_IDX end_label,
                             DST_TYPE caller_file_dst,
                             DST_TYPE callee_file_dst,
                             IPO_SYMTAB *symtab,
                             MEM_POOL *caller_file_m,
                             MEM_POOL *callee_file_m,
                             mUINT16 cross_file_id);

extern DST_IDX 
DST_enter_cloned_subroutine(DST_IDX parent, 
                            DST_IDX orig_node, 
                            ST *cloned_st, 
                            DST_TYPE cur_file_dst,
                            IPO_SYMTAB *sym);

#endif /* clone_DST_utils_INCLUDED */
