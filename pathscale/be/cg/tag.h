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


// Tag Utilities

#include "defs.h"
#include "symtab.h"
#include "op_map.h"

extern OP_MAP OP_Tag_Map;

extern LABEL_IDX Gen_Tag (void);

inline void Set_OP_Tag (OP *op, LABEL_IDX tag)
{
  Set_OP_has_tag(op);
  // create map on the fly, so no cost if no tags
  if (OP_Tag_Map == NULL)
	OP_Tag_Map = OP_MAP32_Create();
  OP_MAP32_Set (OP_Tag_Map, op, tag);
}

inline LABEL_IDX Get_OP_Tag (const OP *op)
{
  Is_True(OP_has_tag(op), ("OP doesn't have tag"));
  Is_True(OP_Tag_Map != NULL, ("OP_Tag_Map not created"));
  return (LABEL_IDX) OP_MAP32_Get (OP_Tag_Map, op);
}

// call at end of PU
extern void TAG_Finish (void);
