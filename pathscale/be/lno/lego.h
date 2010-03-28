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


/***********************************************************************
 *
 * Top-level routines/variables for Lego processing.
 *
 * extern void Lego_Read_Pragmas (WN* func_nd);
 *      Read the data distribution pragmas and build up internal 
 *      representations.
 *
 * extern void Lego_Lower_Pragmas (WN* func_nd);
 *      Generate code for the data distribution pragmas, and 
 *      lower array accesses to reshaped arrays.
 *
 * extern void Lego_PU_Init ();
 *      Do PU-level initialization of lego-processing data structures.
 *      Called at the beginning of LNO for each PU.
 *
 * extern void Lego_PU_Fini ();
 *      Do PU-level finalization of lego-processing data structures.
 *      Called at the end of LNO for each PU.
 *
 * extern void Lego_Compute_Tile_Peel (WN* func_nd);
 *      Compute the tiling and peeling factors to optimize references
 *      to reshaped arrays, based on a combination of affinity hints
 *      (if any) and analysis of references to reshaped arrays within the loop.
 *
 * extern MEM_POOL LEGO_memory_pool, *LEGO_pool;
 *      Memory pool for lego processing.
 *
 *
 ***********************************************************************/

#ifndef _LEGO_INCLUDED_
#define _LEGO_INCLUDED_

#include "wn.h"
#include "cxx_memory.h"

extern void Lego_OZero_Driver(PU_Info* current_pu, WN* func_nd);
extern void Lego_Read_Pragmas(WN* func_nd);
extern void Lego_Lower_Pragmas(WN* func_nd);
extern void Lego_PU_Init();
extern void Lego_PU_Fini();
extern void Lego_Compute_Tile_Peel(WN* func_nd);

extern MEM_POOL LEGO_memory_pool, *LEGO_pool;

#endif /* _LEGO_INCLUDED_ */
