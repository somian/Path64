/*
  Copyright (C) 2006, 2007. PathScale Inc. All Rights Reserved.
 */

/*

   Path64 is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by
   the Free Software Foundation version 3

   Path64 is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
   or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
   License for more details.

   You should have received a copy of the GNU General Public License
   along with Path64; see the file COPYING.  If not, write to the Free
   Software Foundation, 51 Franklin Street, Fifth Floor, Boston, MA
   02110-1301, USA.

*/

#include "gspin-tree.h"
#include "gspin-list.h"

gs_count_t gs_length (gs_t list) 
{
  GS_ASSERT (list != (gs_t) NULL, "got null list.");
  int n;
  for (n=0;gs_code(list) != EMPTY; n++, list = gs_operand(list, 1));
  return n; 
}

gs_t gs_index (gs_t list, gs_count_t index)
{
  GS_ASSERT (list != (gs_t) NULL, "got null list.");
  GS_ASSERT (gs_length(list) > index, "index >= length of list too large.");

  for (; index != 0; index--, list = gs_operand(list, 1));
  return gs_operand(list, 0);
}

#ifdef FE_GNU_4_2_0
void gs_set_index (gs_t list, gs_count_t index, gs_t value)
{
  GS_ASSERT (list != (gs_t) NULL, "got null list.");
  GS_ASSERT (gs_length(list) > index, "index >= length of list too large.");

  for (; index != 0; index--, list = gs_operand(list, 1));
  gs_set_operand(list, 0, value);
}
#endif
