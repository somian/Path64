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


/* =======================================================================
 * =======================================================================
 *
 *  Module: cg_dep_graph_update.h
 *  $Revision$
 *  $Date$
 *  $Author$
 *  $Source$
 *
 *  Revision comments:
 *
 *  3-Aug-1995 - Initial version
 *
 *  Description:
 *  ============
 *
 * =======================================================================
 * =======================================================================
 */

#ifndef CG_DEP_GRAPH_UTIL
#define CG_DEP_GRAPH_UTIL


inline OP_LIST* CG_DEP_Get_Defs(TN *tn, TN_MAP usage_map){
  return (OP_LIST *) TN_MAP_Get(usage_map, tn);
}

inline void CG_DEP_Add_Def(OP *op, UINT8 pos, TN_MAP usage_map, MEM_POOL *pool){
  OP_LIST *defs_list = CG_DEP_Get_Defs(OP_result(op, pos), usage_map); 
  TN_MAP_Set(usage_map, OP_result(op, pos), OP_LIST_Push(op, defs_list, pool));
}

#endif /* CG_DEP_GRAPH_UTIL */
