/*
 * Copyright (C) 2006, 2007. PathScale Inc. All Rights Reserved.
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

#ifndef __GSPIN_LIST_H__
#define __GSPIN_LIST_H__

static inline gs_t gs_cons (gs_t e, gs_t es)
{
  gs_t node = __gs(CONS);
  gs_set_operand(node, 0, e);
  gs_set_operand(node, 1, es);
  return node;
}

gs_count_t gs_length (gs_t list);
gs_t gs_index (gs_t list, gs_count_t index);
#ifdef FE_GNU_4_2_0
void gs_set_index (gs_t list, gs_count_t index, gs_t value);
#endif

#endif // __GSPIN_LIST_H__
