/*
 * Copyright (C) 2009 Advanced Micro Devices, Inc.  All Rights Reserved.
 */

/*
 * Copyright 2005, 2006 PathScale, Inc.  All Rights Reserved.
 *

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

 */

#ifndef be_ipa_util_INCLUDED
#define be_ipa_util_INCLUDED

#include "segmented_array.h"

struct BE_SUMMARY_HEADER
{
  mUINT64 offset;           // offset from beginning of section
  mUINT64 size;             // size in bytes
  mUINT32 entsize;          // size of each entry
};

struct pu_mod_ref_info
{
  PU_IDX pu_idx;            // pu id
  mUINT32 size;             // size in bytes of mod info (must equal ref)
  mUINT8 * mod;             // bit-vector for mod
  mUINT8 * ref;             // bit-vector for ref
  mUINT8 * same_entry_exit_value_or_1; // bit-vector for global vars whose
                                       // function exit value is the same as
                                       // their entry value, or that value is 1
};

typedef SEGMENTED_ARRAY<pu_mod_ref_info> MOD_REF_INFO_TAB;

extern MOD_REF_INFO_TAB Mod_Ref_Info_Table;

inline pu_mod_ref_info&
New_Mod_Ref_Info (UINT32 &index)
{
  return Mod_Ref_Info_Table.New_entry (index);
}

inline UINT
Mod_Ref_Info_Table_Size (void)  { return Mod_Ref_Info_Table.Size(); }

#endif // be_ipa_util_INCLUDED
