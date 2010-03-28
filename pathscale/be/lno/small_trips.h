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
***
***  Description: 
*** 
***     This file is intended for functions which optimize loops with 
***	small trip counts or reduce the trip counts of loops.  
***	
***  Exported types and functions:
***
***	extern void Remove_Zero_Trip_Loop(WN* wn_loop)
***
***	  Remove the zero trip loop 'wn_loop'
***
***     extern void Remove_Unity_Trip_Loop(WN* wn_loop, BOOL update_access,
***	  WN** wn_first, WN** wn_last, ARRAY_DIRECTED_GRAPH16* dg, 
***	  DU_MANAGER* du)
***
***	  Remove the unity trip loop 'wdloop'.  If 'update_access',
***	  update the access vectors, and 'update_do_depths' update the do
***	  loop depths.  The firsst and last statements in the new code block
***	  are returned in 'wn_first' and 'wn_last'.
***
***	extern DOLOOP_STACK* Finalizable_Loops(WN* wn_loop,
***	  ARRAY_DIRECTED_GRAPH16* dg, DU_MANAGER* du)
***
***	  Returns a DOLOOP_STACK of loops within 'wn_loop' which can
***	  be replaced by their final iteration.
***
***	extern WN* Finalize_Loops(WN* func_nd)
***
***	  If a loop isn't needed, replace the finalizable loops within 
***	  'func_nd' listed in 'stack_final' with their final value and an 
***	  appropriate guard test. 
***
***	extern void Remove_Unity_Trip_Loop_Dep_Update(WN* wn_loop,
***	  ARRAY_DIRECTED_GRAPH16* dg, BOOL will_not_remove_loop)
***
***	  Update dependence graphs assuming wn_loop becomes bad and
***	  the dependence vectors have to be shortened.
***	  If will_not_remove_loop, that means we are only making the
***	  loop bad so the unused dim will be incremented.
***
***     extern WN* SNL_Finalize_Loops(WN* wn_outer_loop, DOLOOP_STACK* 
*** 	  stack_final, ARRAY_DIRECTED_GRAPH16* dg, DU_MANAGER* du)
***
***	  Replace the finalizable loops within 'func_nd' listed in 
*** 	  'stack_final' with their final value and an appropriate guard test. 
***
***	extern void Optimize_Coupled_Loops(WN* wn_tree, DU_MANAGER* du)
***
***	  Perform Unify_Loop() and Trip_Reduce_Loop() on applicable
***       loop pairs, as described in small_trips.cxx  
***
**/

#ifndef small_trips_INCLUDED
#define small_trips_INCLUDED "small_trips.h"

extern void Remove_Zero_Trip_Loop(WN* wn_loop);

extern void Remove_Unity_Trip_Loop(WN* wn_loop, BOOL update_access,
  WN** wn_first, WN** wn_last, ARRAY_DIRECTED_GRAPH16* dg, DU_MANAGER* du);

extern DOLOOP_STACK* Finalizable_Loops(WN* wn_loop, 
  ARRAY_DIRECTED_GRAPH16* dg, DU_MANAGER* du);

extern WN* Finalize_Loops(WN* func_nd); 

extern void Remove_Unity_Trip_Loop_Dep_Update(WN* wn_loop,
  ARRAY_DIRECTED_GRAPH16* dg,
  BOOL will_not_remove_loop=FALSE);

extern void Optimize_Coupled_Loops(WN* wn_tree, DU_MANAGER* du);
extern WN* SNL_Finalize_Loops(WN* wn_outer_loop, DOLOOP_STACK* stack_final,
  ARRAY_DIRECTED_GRAPH16* dg, DU_MANAGER* du);


#endif /* small_trips__INCLUDED */ 

