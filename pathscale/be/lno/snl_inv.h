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


// -*-C++-*-

/**
*** Description:
***
*** This file contains defintions for functions which implement invariant 
*** SNL transformations. 
***
**/ 

#ifndef snl_inv_INCLUDED
#define snl_inv_INCLUDED "snl_inv.h"

extern WN* SNL_INV_Permute_Loops(WN* outer_loop, INT permutation[], 
  INT nloops, BOOL warn_lexneg);

extern WN* SNL_INV_Cache_Block(SNL_NEST_INFO* ni, const SNL_TILE_INFO* t,
  WN* permloop[], LS_IN_LOOP& loop_ls, SNL_REGION* region, 
  SNL_INV_CACHE_BLOCK_REASON reason, SYMBOL* outersym, 
  MEM_POOL* pool, BOOL report_prompf);

extern SNL_REGION SNL_INV_Transforms(WN* wn_outer, INT permutation[],
  SNL_NEST_INFO* ni, INT  nloops, SNL_TILE_INFO* t, INT regstripsz[],
  EST_REGISTER_USAGE est_register_usage, BOOL want_se_and_dist, 
  SNL_REGION* old_region, BOOL hoist_outer_invar, SNL_REGION* rg_kernel);

#endif 
