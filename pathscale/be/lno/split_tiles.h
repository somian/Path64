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

#ifndef split_tiles_INCLUDED
#define split_tiles_INCLUDED "split.h"

extern WN* SNL_SPL_Loop_Is_Inner_Tile(WN* wn_loop, INT* tile_size); 

extern BOOL SNL_SPL_Split_Inner_Tile_Loop(WN* wn_outer, WN* wn_inner,
  INT tile_size, const char prefix[], BOOL cache_annotate); 

extern void SNL_SPL_Split_Inner_Tile_Loops(WN* wn_first, WN* wn_last, 
  INT split_flag, const char prefix[], BOOL cache_annotate);

#endif /* split_tiles_INCLUDED */

