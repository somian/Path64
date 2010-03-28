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


#ifndef sxlimit_INCLUDED
#define sxlimit_INCLUDED "sxlimit.h"

/**
***	SXLIMIT implements limited tile size scalar expansion and distri-
***	  bution on invariant innermost SNLs. 
***
***     NOTE:
***
***         In the functions below, the argument 'full_dist' is TRUE
***         if the scalar expansion is being provided to transform a
***         SNL into a perfect nest via distribution, FALSE if the
***         scalar expansion is being provided only to enable the
***         permutation.
***
***     WN* SE_Tile_Inner_Loop(WN* loop,
***                            MEM_POOL *pool)
***
***	Tile the 'loop' for scalar expansion, given that temporary memory is 
***	allocated out of 'pool'.  The outer tile loop immediately encloses 
***	'loop' when we are finished, and its value is returned.
***
***	void SE_New_Tile_Infos(WN* wn_outer,
***                            SX_PLIST* plist,
***			       INT permutation[],
***                            INT nloops,
***                            MEM_POOL* pool,
***                            SNL_TILE_INFO **ti_se,
***			       BOOL full_dist)
***
***	For the SNL 'wn_outer' to which we are applying a 'permutation' of 
***	'nloops' loops and list of expandable scalars 'plist', compute a 
***	SNL_TILE_INFO 'ti_se' to describe the limited scalar expansion 
***	tiling which must be performed on it.
***
***	void SE_CT_New_Tile_Infos(WN* wn_outer,
***                               SX_PLIST* plist,
***                               SNL_TILE_INFO *ti,
***				  INT permutation[], 
***                               INT nloops,
***                               MEM_POOL* pool,
***                               SNL_TILE_INFO **ti_se,
***                               SNL_TILE_INFO **ti_ct,
***				  BOOL full_dist) 
***
***	Given the SNL 'wn_outer' of 'nloops' loops to which we are 
***     applying the 'permutation' of length 'nloops', and which originally 
***     had a cache tiling info of 'ti', compute the new cache tiling info
***     ti_ct' and the new scalar expansion tiling info 'ti_se' which will 
***     result in fixed-size scalar expansion tiles for the SNL 'wn_outer' 
***     with scalar expansion variable list 'plist'.
***
***     WN* SNL_INV_Limited_SE_And_Dist(WN* wn_outer,
***                                     SNL_TILE_INFO* ti_se,
***                                     INT permutation[],
***                                     INT nloops,
***                                     SX_PLIST* plist,
***					BOOL full_dist)
***	
***	For the innermost SNL 'wn_outer' of 'nloops' loops, tile it
***	for scalar expansion and distribute the loops according to the infor-
***	mation in 'ti_se'. The 'plist' is a list of expandable scalars made
***	by calling SX_INFO::Make_Sx_Info().  Subscripts in the scalar expanded
***	array are made to correspond to the 'permutation' of length 'nloops'.
***
**/

extern WN* SE_Tile_Inner_Loop(WN* loop,
                              MEM_POOL *pool);

extern void SE_New_Tile_Infos(WN* wn_outer,
                              SX_PLIST* plist,
			      INT permutation[], 
                              INT nloops,
                              MEM_POOL* pool,
                              SNL_TILE_INFO **ti_se,
			      BOOL full_dist);

extern void SE_CT_New_Tile_Infos(WN* wn_outer,
                                 SX_PLIST* plist,
                                 SNL_TILE_INFO *ti,
				 INT permutation[], 
                                 INT nloops,
                                 MEM_POOL* pool,
                                 SNL_TILE_INFO **ti_se,
                                 SNL_TILE_INFO **ti_ct,
				 BOOL full_dist);

extern WN* SNL_INV_Limited_SE_And_Dist(WN* wn_outer,
                                       SNL_TILE_INFO* ti_se,
                                       INT permutation[],
                                       INT nloops,
                                       SX_PLIST* plist,
				       BOOL full_dist);
#endif /* sxlimit_INCLUDED */
