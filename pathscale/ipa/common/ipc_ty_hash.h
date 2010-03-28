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
#ifndef ipc_ty_hash_INCLUDED
#define ipc_ty_hash_INCLUDED

// we steal a TY_IDX bit for temporary marking of merge state
const UINT TY_INDEX_IS_VISITED	= 0x1;	// for loop detection
const UINT TY_INDEX_IS_TEMP	= 0x2;	// TY_IDX is being validated
const UINT TY_INDEX_IS_INSERTED = 0x4;	// inserted into the Ty_Table but
					// not in the hash table

static inline BOOL
Valid_TY_IDX (TY_IDX ty_idx)
{
    return ((ty_idx & 0xff) == 0 && ty_idx != 0);
}
	
static inline void
Set_TY_Merging (TY_IDX& ty_idx)
{
    ty_idx = TY_INDEX_IS_VISITED;
}

static inline BOOL
TY_Merging (TY_IDX ty_idx)
{
    return ty_idx == TY_INDEX_IS_VISITED;
}

static inline void
Set_TY_Temp_Idx (TY_IDX& ty_idx, TY_IDX temp)
{
    ty_idx = (temp &  ~0xff) | TY_INDEX_IS_TEMP | (ty_idx & 0xff);
}

static inline void
Clear_TY_Temp_Idx (TY_IDX& ty_idx)
{
    ty_idx &= (~TY_INDEX_IS_TEMP & 0xff);
}

static inline BOOL
Is_TY_Temp_Idx (TY_IDX ty_idx)
{
    return (ty_idx & TY_INDEX_IS_TEMP);
}

static inline TY_IDX
TY_Temp_Idx (TY_IDX_MAP& ty_map, TY_IDX old_idx)
{
    TY_IDX ty_idx = ty_map.map_[TY_IDX_index (old_idx)];
    return (ty_idx & TY_INDEX_IS_TEMP) ? (ty_idx & ~0xff) : 0;
}


static inline void
Set_TY_Inserted (TY_IDX& ty_idx, TY_IDX new_idx)
{
    ty_idx = Replace_TY_IDX_index (TY_INDEX_IS_INSERTED, new_idx);
}

static inline BOOL
TY_Inserted (TY_IDX ty_idx)
{
    return ty_idx & TY_INDEX_IS_INSERTED;
}


static inline void
Clean_TY_IDX (TY_IDX& ty_idx)
{
    ty_idx &= ~0xff;
}

static inline TY_IDX
TY_IDX_Attributes (TY_IDX ty_idx)
{
    return (ty_idx & 0xff);
}


extern BOOL
Partial_Compare_Fld (FLD_HANDLE merged_fld, const FLD* new_fld);

extern BOOL
Partial_Compare_Arb (ARB_HANDLE merged_arb, const ARB* new_arb);

extern void
Initialize_Type_Merging_Hash_Tables (MEM_POOL* pool);

extern void
Setup_Type_Merging_Hash_Tables (const IPC_GLOBAL_TABS& original_tabs,
				IPC_GLOBAL_IDX_MAP& idx_map);

extern TY_IDX
Insert_Unique_Ty (const TY& ty);

extern void
Insert_Allocated_Ty (TY& ty, TY_IDX ty_idx);


void
Initialize_New_Recursive_Type (TY_IDX ty_idx);

void
Finalize_New_Recursive_Type ();

typedef vector<TY_IDX> TY_IDX_VEC;

extern void
Find_Matching_Ty (const TY& ty, TY_IDX_VEC& matched_list);

#endif /* ipc_ty_hash_INCLUDED */
