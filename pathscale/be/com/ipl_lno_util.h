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


//* -*-Mode: c++;-*- (Tell emacs to use c++ mode) */
#ifndef ipl_lno_util_INCLUDED
#define ipl_lno_util_INCLUDED

class IPA_LNO_READ_FILE; 
class LINEX; 
class PROJECTED_REGION; 

extern INT32 Formal_Position(const ST* formal_st);
extern TYPE_ID Machine_Type(WN* wn_argument);
extern INT Node_Count(WN* wn_node, INT limit = 0, BOOL symbol_only = FALSE);
extern WN* True_Bound(WN* wn_func, ST_IDX st_idx_exp);
extern BOOL Exp_To_Linex(WN* wn_exp, LINEX* lx_exp, MEM_POOL* mem_pool,
  BOOL negate = FALSE, BOOL Is_LNO = FALSE, IPA_LNO_READ_FILE* 
  IPA_LNO_File = NULL);
extern PROJECTED_REGION* Projected_Region_From_St(WN* wn_func,
  ST* st, MEM_POOL* mem_pool, BOOL Is_LNO = FALSE, 
  IPA_LNO_READ_FILE* IPA_LNO_File = NULL);
extern PROJECTED_REGION* Projected_Region_From_Access_Array(ACCESS_ARRAY* aa,
  MEM_POOL* mem_pool, IPA_LNO_READ_FILE* IPA_LNO_File);

#endif
