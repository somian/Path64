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
#ifndef ipc_type_merge_INCLUDED
#define ipc_type_merge_INCLUDED

#ifndef ipc_symtab_merge_INCLUDED
#include "ipc_symtab_merge.h"
#endif

extern void
Initialize_Type_Merging_Hash_Tables (MEM_POOL* pool);

extern void
Merge_All_Types (const IPC_GLOBAL_TABS& original_tabs,
		 IPC_GLOBAL_IDX_MAP& idx_map);
#endif /* ipc_type_merge_INCLUDED */
