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
//                  	LNO DOACROSS Parallelization
//                  	----------------------------
//

/* ====================================================================
 * ====================================================================
 *
 * Module: doacross.h
 * $Revision$
 * $Date$
 * $Author$
 * $Source$
 *
 * Revision history:
 *  dd-mmm-95 - Original Version
 *
 * Description: Parallelize loops with loop-carried dependences
 *
 * ====================================================================
 * ====================================================================
 */

#ifndef doacross_INCLUDED
#define doacross_INCLUDED

#define NULL_DIST INT_MAX

extern WN* Parallelize_Doacross_Loop(
			WN* processor_loop,
			WN* processor_tile_loop,
			INT32 Doacross_Tile_Size,
			INT32 Sync_Distances[2],
			ARRAY_DIRECTED_GRAPH16* dg,
			DU_MANAGER* du);

extern void Doacross_Init(
			WN* func_nd);

extern void Doacross_Finish();

extern void Compute_Sync_Distances(
			WN* wn_outer,
                        INT nloops,
			INT permutation[],
                        INT parallel_depth,
                        SNL_DEP_MATRIX** sdm_inv,
			BOOL retained[],
                        INT sync_distances[2]);

extern INT Get_Doacross_Tile_Size(
                        INT sync_distances[],
                        WN* wn_outer,
                        INT permutation[],
                        INT nloops,
                        INT parallel_depth,
                        INT num_procs,
                        double work_estimate);

extern double Compute_Doacross_Delay_Cycle(
                        WN* wn_outer,
                        INT permutation[],
                        INT parallel_depth,
                        INT num_proc,
                        INT doacross_tile_size,
			INT sync_distances[],
			double machine_cycles);

extern double Compute_Doacross_Sync_Cycle(
                        WN* wn_outer,
                        INT permutation[],
                        INT parallel_depth,
                        INT doacross_tile_size,
                        INT sync_distances[]);

extern BOOL Dep_Preserved(
                        DEPV* depv,
                        INT doacross_dim,
                        INT sync_distances[2]);

extern BOOL Check_Doacross_Sync_Coverage(
                        WN* doacross_loop,
                        INT sync_distances[2]);

#endif // doacross_INCLUDED
