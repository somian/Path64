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
*** This file contains defintions for functions which implement general 
*** SNL transformations.
***
**/

#ifndef snl_gen_INCLUDED
#define snl_gen_INCLUDED "snl_gen.h"

extern void SNL_Peel_Iteration(WN* wn, BOOL first_iter); 

extern SNL_REGION SNL_GEN_Protect_Nest_With_Conditionals(const SNL_NEST_INFO* 
  ni, BOOL* failed);

extern SNL_REGION SNL_GEN_Distribution(WN* wn_outer, IMAT* unimodular,
  SNL_TILE_INFO* ti, INT nloops, BOOL find_split_depth, SX_PLIST* plist,
  BOOL above_is_distributable, BOOL below_is_distributable);

extern SNL_REGION SNL_GEN_U_Ctiling(WN* wn_outer, INT nloops, IMAT* u,
  SNL_TILE_INFO* t, SNL_BOUNDS_INFO* bi, SX_PLIST* plist,
  EST_REGISTER_USAGE est_register_usage, BOOL warn_lexneg, BOOL from_genperm);

extern WN* SNL_GEN_Permute_Loops(WN* wn_outer, INT permutation[], INT nloops,
  BOOL warn_lexneg);

extern SNL_REGION SNL_GEN_2D_Regtile(SNL_NEST_INFO* ni, INT tilesz);

#endif
