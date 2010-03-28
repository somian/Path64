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


// Label Utilities

#include "defs.h"
#include "symtab.h"
#include "bb.h"

// initialize label_info_array
extern void Init_Label_Info (void);

// Get the label pc offset for a given label number
extern UINT64 Get_Label_Offset (LABEL_IDX i);
extern void Set_Label_Offset (LABEL_IDX i, UINT64 offset);

// Get the defining BB for a given label number
extern BB* Get_Label_BB (LABEL_IDX i);
extern void Set_Label_BB (LABEL_IDX i, BB *bb);

extern LABEL_IDX Gen_Temp_Label(void);
