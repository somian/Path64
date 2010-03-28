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

#include <alloca.h>
#include "defs.h"
#include "mempool.h"
#include "label_util.h"
#include "be_util.h"
#include "bb.h"

// Map LABEL number to offset and defining BB.
typedef struct {
	mUINT64 offset;
	BB *def_bb;
} label_info;
static label_info *label_info_array;
static INT max_label_info_index = 0;

static void
Realloc_Label_Info (INT min_needed)
{
    if (min_needed >= max_label_info_index) {
	// realloc space
	INT size = MAX(max_label_info_index * 2, min_needed + 8);
	label_info_array =
	    TYPE_MEM_POOL_REALLOC_N (label_info, &MEM_src_pool,
				     label_info_array,
				     max_label_info_index, size);
	max_label_info_index = size;
    }
}

// initialize label_info_array
void
Init_Label_Info (void)
{
	INT last_label_in_PU = LABEL_Table_Size(CURRENT_SYMTAB);
	if (label_info_array == NULL) {
		max_label_info_index = MAX(8,last_label_in_PU);
		label_info_array = (label_info*) Src_Alloc (
			sizeof(label_info) * max_label_info_index);
	}
	else {
		// clear out the values
		INT i;
		for (i = 0; i < max_label_info_index; i++) {
			label_info_array[i].offset = 0;
			label_info_array[i].def_bb = NULL;
		}
		Realloc_Label_Info (last_label_in_PU);
	}
}

// Get the label pc offset for a given label number
UINT64
Get_Label_Offset (LABEL_IDX i)
{
  if (i >= max_label_info_index) {
	// overflowed
	Realloc_Label_Info (i);
  }
  Is_True(i < max_label_info_index, ("Get_Label_Offset still overflows?"));
  return label_info_array[i].offset;
}

void
Set_Label_Offset (LABEL_IDX i, UINT64 offset)
{
  if (i >= max_label_info_index) {
	// overflowed
	Realloc_Label_Info (i);
  }
  Is_True(i < max_label_info_index, ("Set_Label_Offset still overflows?"));
  label_info_array[i].offset = offset;
}

// Get the defining BB for a given label number
BB*
Get_Label_BB (LABEL_IDX i)
{
  if (i >= max_label_info_index) {
	// overflowed
	Realloc_Label_Info (i);
  }
  Is_True(i < max_label_info_index, ("Get_Label_BB still overflows?"));
  return label_info_array[i].def_bb;
}

void
Set_Label_BB (LABEL_IDX i, BB *bb)
{
  if (i >= max_label_info_index) {
	// overflowed
	Realloc_Label_Info (i);
  }
  Is_True(i < max_label_info_index, ("Set_Label_BB still overflows?"));
  label_info_array[i].def_bb = bb;
}

LABEL_IDX Gen_Temp_Label(void)
{
  enum { maxint_digits = 10 };
  char *name;
  LABEL_IDX labx;
  LABEL &label = New_LABEL(CURRENT_SYMTAB, labx);

  name = (char *)alloca(  strlen(".L__")
                        + maxint_digits
                        + maxint_digits
                        + 1);
  // use underscores cause intel's asm doesn't like dots in name
  sprintf(name, ".L_%d_%" SCNd32 , Current_PU_Count(), labx);
  LABEL_Init (label, Save_Str(name), LKIND_DEFAULT);

  return labx;
}
