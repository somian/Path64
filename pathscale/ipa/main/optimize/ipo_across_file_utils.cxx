/*
 * Copyright (C) 2007. PathScale, LLC. All Rights Reserved.
 */
/*
 * Copyright 2003, 2004, 2005, 2006 PathScale, Inc.  All Rights Reserved.
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


/* -*-Mode: c++;-*- (Tell emacs to use c++ mode) */
// ====================================================================
// ====================================================================
//
// Module: ipo_across_file_utils.cxx
//
// Revision history:
//  31 July -96 - Original Version
//
// Description:  utilities used by the across file inlining
//
// ===================================================================
//====================================================================
#if defined(BUILD_OS_DARWIN)
#include <darwin_elf.h>
#else /* defined(BUILD_OS_DARWIN) */
#include <elf.h>
#endif /* defined(BUILD_OS_DARWIN) */
#define USE_STANDARD_TYPES          /* override unwanted defines in defs.h */
#include "defs.h"
#include "config_ipa.h"             // for INLINE/IPA options
#include "mempool.h"
#include "stab.h"
#include "wn.h"
#include "dwarf_DST_mem.h"          // for ipc_file.h
#include "ipc_file.h"
#include "cxx_base.h"
#include "cxx_memory.h"             // for CXX_NEW, etc.
#include "clone.h"
#include "ipo_clone.h"
#include "ipo_st_utils.h"
#include "clone.h"
#include "ipo_clone.h"
#include "ipo_across_file_utils.h"

//------------------------------------------------------------
// initialize the mappings
//------------------------------------------------------------
void Init_ty_maps(IP_FILE_HDR *shd_callee, IP_FILE_HDR *shd_caller,
		  SYMTAB *caller_gs)
{

  MEM_POOL* caller_aux_mem_pool = &IP_FILE_HDR_aux_mem_pool(shd_caller);
  MEM_POOL* callee_aux_mem_pool = &IP_FILE_HDR_aux_mem_pool(shd_callee);
	
  if (shd_callee != shd_caller) {
    if (IP_FILE_HDR_global_ty_map(shd_caller) == 0) {
      IP_FILE_HDR_global_ty_map(shd_caller) = (void *)
	CXX_NEW (IPO_TY_HASH (caller_aux_mem_pool),
		 caller_aux_mem_pool);
    }
    if (IPA_Enable_Merge_ty) {
      IPO_TY_HASH *map =
	(IPO_TY_HASH *) IP_FILE_HDR_global_ty_map(shd_caller);
      map->Init_Hash (SYMTAB_types(caller_gs));
      
      if (IP_FILE_HDR_global_ty_map(shd_callee) == 0) {
	IP_FILE_HDR_global_ty_map(shd_callee) = (void *)
	  CXX_NEW (IPO_TY_HASH (callee_aux_mem_pool),
		   callee_aux_mem_pool);
      }
    }
  }
}
