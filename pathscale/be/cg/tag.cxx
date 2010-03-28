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

#include <alloca.h>
#include "tag.h"
#include "symtab.h"
#include "be_util.h"

OP_MAP OP_Tag_Map;

LABEL_IDX Gen_Tag (void)
{
  enum { maxint_digits = 10 };
  char *name;
  LABEL_IDX labx;
  LABEL &label = New_LABEL(CURRENT_SYMTAB, labx);

  name = (char *)alloca(  strlen(".tag__")
                        + maxint_digits
                        + maxint_digits
                        + 1);
  // use underscores cause intel's asm doesn't like dots in name
  sprintf(name, ".tag_%d_%" SCNd32 , Current_PU_Count(), labx);
  LABEL_Init (label, Save_Str(name), LKIND_TAG);

  return labx;
}

extern void TAG_Finish (void)
{
  if (OP_Tag_Map) {
	OP_MAP_Delete (OP_Tag_Map);
	OP_Tag_Map = NULL;
  }
}
