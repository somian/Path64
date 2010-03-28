/*
 *  Copyright 2009 SiCortex, Inc.
 */

/*
 *  Copyright (C) 2007, 2008, 2009 PathScale, LLC.  All Rights Reserved.
 */

/*
 *  Copyright (C) 2006, 2007. QLogic Corporation. All Rights Reserved.
 */

/*
 * Copyright 2003, 2004, 2005, 2006 PathScale, Inc.  All Rights Reserved.
 */

/*

  Copyright (C) 2000, 2001 Silicon Graphics, Inc.  All Rights Reserved.

  This program is free software; you can redistribute it and/or modify it
  under the terms of version 2 of the GNU General Public License as
  published by the Free Software Foundation.

  This program is distributed in the hope that it would be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  

  Further, this software is distributed without any warranty that it is
  free of the rightful claim of any third person regarding infringement 
  or the like.  Any license provided herein, whether implied or 
  otherwise, applies only to this software file.  Patent licenses, if 
  any, provided herein do not apply to combinations of this program with 
  other software, or any other product whatsoever.  

  You should have received a copy of the GNU General Public License along
  with this program; if not, write the Free Software Foundation, Inc., 59
  Temple Place - Suite 330, Boston MA 02111-1307, USA.

  Contact information:  Silicon Graphics, Inc., 1600 Amphitheatre Pky,
  Mountain View, CA 94043, or:

  http://www.sgi.com

  For further information regarding this notice, see:

  http://oss.sgi.com/projects/GenInfo/NoticeExplan

*/


// =======================================================================
// =======================================================================
//
//  Module: igls.cxx
//  $Revision: 1.14 $
//  $Date: 04/12/21 14:57:07-08:00 $
//  $Author: bos@eng-25.internal.keyresearch.com $
//  $Source: /home/bos/bk/kpro64-pending/be/cg/SCCS/s.igls.cxx $
//
//  Description:
//  ============
//
//  Integrated Global and Local Scheduling Framework (IGLS). The main driver 
//  controls the execution of the local scheduling phase (LOCS), the
//  hyperblock scheduling phase (HBS) and the global scheduling phase (GCM).
//
// =======================================================================
// =======================================================================

#define __STDC_LIMIT_MACROS
#include <stdint.h>
#include <alloca.h>
#include <math.h>
#include "defs.h"
#include "config.h"
#include "config_targ_opt.h"
#include "mempool.h"
#include "bb.h"
#include "bb_set.h"
#include "tracing.h"
#include "timing.h"
#include "cgir.h"
#include "glob.h"
#include "tn_map.h"
#include "cg.h"
#include "cg_flags.h"
#include "ercg.h"
#include "cgtarget.h"
#include "cg_vector.h"
#include "dominate.h"
#include "findloops.h"
#include "note.h"
#include "lra.h"
#include "gcm.h"
#include "ti_res.h"
#include "ti_res_res.h"
#include "ti_latency.h"
#include "ti_errors.h"
#include "cg_region.h"
#include "gtn_universe.h"
#include "gtn_set.h"
#include "cxx_memory.h"
#include "hb_sched.h"
#include "hb_hazards.h"
#include "targ_proc_properties.h"

#ifdef KEY
#include "whirl2ops.h"

// Delete prefetches that are dropped by the scheduler.
static void
Delete_Unscheduled_Prefetches (BB *bb)
{
  OP *op, *next_op;
  int offset = -1;

  for (op = BB_first_op(bb); op != NULL; op = next_op) {
    next_op = OP_next(op);
    if (OP_prefetch_deleted(op)) {
      FmtAssert(OP_prefetch(op),
		("Delete_Unscheduled_Prefetches: OP not a prefetch"));
      BB_Remove_Op(bb, op);
    } else {
      // The new cycle 0 is at the first non-deleted instruction.
      if (offset == -1) {
	offset = OP_scycle(op);
      }

      // Adjust scycle to account for the deleted cycles.
      OP_scycle(op) = OP_scycle(op) - offset;
    }
  }

  // Note the new schedule length.
  if (Assembly)
    Add_Scheduling_Note(bb, (void*) 0);
}

// Fabricate a BB that looks like OLD_BB for the purpose of scheduling.
static BB *
Gen_BB_For_Sched (BB *old_bb)
{
  OP *op;
  OPS new_ops;
  OPS_Init(&new_ops);

  OP *entry_sp_adj_op = NULL;
  OP *exit_sp_adj_op = NULL;

  BB *new_bb = Gen_BB_Like(old_bb);

  // Copy over the minimum amount of info needed by the scheduler.
  BB_bbregs(new_bb) = BB_bbregs(old_bb);
  BB_flag(new_bb) = BB_flag(old_bb);

  // Create entryinfo/exitinfo for new BB.  A new entryinfo/exitinfo is needed
  // to hold the new BB_entry_sp_adj_op/BB_exit_sp_adj_op.
  if (BB_entry(old_bb)) {
    entry_sp_adj_op = BB_entry_sp_adj_op(old_bb);
    ANNOTATION *old_annot = ANNOT_Get(BB_annotations(old_bb), ANNOT_ENTRYINFO);
    ENTRYINFO *old_info = ANNOT_entryinfo(old_annot);
    ENTRYINFO *new_info = TYPE_P_ALLOC(ENTRYINFO);
    // Copy over everything except ENTRYINFO_sp_adj.
    ENTRYINFO_name(new_info) = ENTRYINFO_name(old_info);
    ENTRYINFO_entry_wn(new_info) = ENTRYINFO_entry_wn(old_info);
    ENTRYINFO_srcpos(new_info) = ENTRYINFO_srcpos(old_info);
    BB_Add_Annotation(new_bb, ANNOT_ENTRYINFO, new_info);
  }
  if (BB_exit(old_bb)) {
    exit_sp_adj_op = BB_exit_sp_adj_op(old_bb);
    ANNOTATION *old_annot = ANNOT_Get(BB_annotations(old_bb), ANNOT_EXITINFO);
    EXITINFO *old_info = ANNOT_exitinfo(old_annot);
    EXITINFO *new_info = TYPE_P_ALLOC(EXITINFO);
    // Copy over everything except EXITINFO_sp_adj.
    EXITINFO_srcpos(new_info) = EXITINFO_srcpos(old_info);
    BB_Add_Annotation(new_bb, ANNOT_EXITINFO, new_info);
  }

  FOR_ALL_BB_OPs(old_bb, op) {
    OP *new_op = Dup_OP(op);
    Copy_WN_For_Memory_OP(new_op, op);
    OP_srcpos(new_op) = OP_srcpos(op);
    OPS_Append_Op(&new_ops, new_op);

    if (op == entry_sp_adj_op)
      Set_BB_entry_sp_adj_op(new_bb, new_op);
    else if (op == exit_sp_adj_op)
      Set_BB_exit_sp_adj_op(new_bb, new_op);
  }
  BB_Append_Ops(new_bb, &new_ops);
  return new_bb;
}

static BOOL
Run_One_Sched (HB_Schedule *Sched, BOOL is_fwd, BB *bb, HBS_TYPE hbs_type,
	       const char *name, INT32 *best_cycles,
	       const char **best_name, BOOL *best_is_fwd, BB **best_bb = NULL)
{
  INT32 max_cycles = *best_cycles;

  // Make a copy of the original BB for the scheduler to work with, so that the
  // scheduler can add side effects, such as copy insertion and TN renaming,
  // without disturbing the original BB.
  if (best_bb != NULL) {
    bb = Gen_BB_For_Sched(bb);
    // Force the scheduler to create an initial schedule for the BB.
    max_cycles = INT32_MAX;
  }

  Sched->Init(bb, hbs_type, max_cycles, NULL, NULL);
  BOOL optimal = Sched->Schedule_BB(bb, NULL, is_fwd);
  if (BB_length(bb) > 0 &&
      (OP_scycle(BB_last_op(bb)) + 1) < *best_cycles) {
    *best_cycles = OP_scycle(BB_last_op(bb)) + 1;
    *best_name = name;
    *best_is_fwd = is_fwd;

    // Save the new OPs ordering.
    if (best_bb != NULL) {
      *best_bb = bb;
    }
  }
  return optimal;
}

// Run the BB scheduler.  Run it multiple times if necessary to pick the best
// schedule.  Return TRUE if the schedule was generated using forward
// scheduling.
static BOOL
Run_Sched (HB_Schedule *Sched, BB *bb, HBS_TYPE hbs_type, INT32 max_sched)
{
  INT32 best_cycles = max_sched;
  const char *best_name = "none";
  BOOL best_is_fwd = FALSE;
  BOOL optimal = FALSE;
  int length = BB_length(bb);

  // Save a copy of the original OPs if scheduler is to be run mulitple times
  // with side effects, such as copy insertion.
  BB **best_bb = NULL;
  BB *the_best_bb = NULL;

  if ((hbs_type & HBS_RELAX_REGANTI) &&
      (LOCS_Best ||
       LOCS_Scheduling_Algorithm == 2 /* backward and forward */)) {
    best_bb = &the_best_bb;
  }

  if (BB_keep_prefetch(bb))
    hbs_type = hbs_type & ~HBS_DROP_UNSCHED_PREFETCHES;

  if (LOCS_Best) {

    // Run scheduler many times.
 
    HBS_TYPE base_hbs_type = hbs_type
			     & ~HBS_BALANCE_READY_TYPES
			     & ~HBS_BALANCE_UNSCHED_TYPES;
    
    optimal = Run_One_Sched(Sched, 0, bb, base_hbs_type, "backward",
			    &best_cycles, &best_name, &best_is_fwd, best_bb);

    if (length > 1) {
      // Backward scheduling.

      if (!optimal)
        optimal = Run_One_Sched(Sched, 0, bb,
				base_hbs_type | HBS_BALANCE_READY_TYPES,
				"backward balance_ready_types",
				&best_cycles, &best_name,
				&best_is_fwd, best_bb);

      if (!optimal)
	optimal = Run_One_Sched(Sched, 0, bb,
				base_hbs_type | HBS_BALANCE_UNSCHED_TYPES,
				"backward balance_unsched_types",
				&best_cycles, &best_name,
				&best_is_fwd, best_bb);

      if (!optimal)
	optimal = Run_One_Sched(Sched, 0, bb,
				base_hbs_type | HBS_BALANCE_UNSCHED_TYPES
				  | HBS_BALANCE_READY_TYPES,
				"backward balance_unsched_types balance_ready_types",
				&best_cycles, &best_name,
				&best_is_fwd, best_bb);

#ifndef TARG_MIPS
      // Forward scheduling. Dropping prefetches is not supported.
      if (!optimal)
        optimal = Run_One_Sched(Sched, 1, bb,
				base_hbs_type & ~HBS_DROP_UNSCHED_PREFETCHES,
				"forward",
				&best_cycles, &best_name,
				&best_is_fwd, best_bb);
#endif
    }
  } else if (LOCS_Scheduling_Algorithm == 2) {

    // Do backward and forward scheduling.

    optimal = Run_One_Sched(Sched, 0, bb, hbs_type, "backward",
			    &best_cycles, &best_name, &best_is_fwd, best_bb);

    if (length > 1) {
      if (!optimal)
	optimal = Run_One_Sched(Sched, 1, bb, hbs_type, "forward",
				&best_cycles, &best_name,
				&best_is_fwd, best_bb);
    }

  // Run scheduler once.
  } else {
    Sched->Init(bb, hbs_type, max_sched, NULL, NULL);
    Sched->Schedule_BB(bb, NULL, LOCS_Scheduling_Algorithm);
    best_is_fwd = (LOCS_Scheduling_Algorithm == 1) ? TRUE : FALSE;
  }

  // Transfer the best schedule to the BB.
  if (best_bb != NULL &&
      *best_bb != NULL) {
    BB_Remove_All(bb);
    BB_Append_All(bb, *best_bb);
    OP *entry_sp_adj_op = BB_entry_sp_adj_op(*best_bb);
    OP *exit_sp_adj_op = BB_exit_sp_adj_op(*best_bb);
    if (entry_sp_adj_op != NULL)
      Set_BB_entry_sp_adj_op(bb, entry_sp_adj_op);
    if (exit_sp_adj_op != NULL)
      Set_BB_exit_sp_adj_op(bb, exit_sp_adj_op);
    // Copy the register requests computed indirectly through scheduling.
    LRA_Copy_Register_Request(bb, *best_bb);
  }

  // Delete prefetches that didn't fit into idle issue slots.
  if (hbs_type & HBS_DROP_UNSCHED_PREFETCHES)
    Delete_Unscheduled_Prefetches(bb);

  if (Trace_HB &&
      best_name != NULL) {
    fprintf(TFile, "Best scheduling heuristic: %s\n", best_name);
  }

  return best_is_fwd;
}
#endif

// ======================================================================
// IGLS_Schedule_Region 
//
// The main driver for invoking all the scheduling phases in CG. They mainly
// include HBS (for single-BBs and hyperblocks) and GCM. The data-speculation
// phase is also invoked here since it's tied very closely with the 
// scheduling phase. 
// The <before_regalloc> parameter indicates whether the scheduler is being 
// invoked before or after register allocation. The amount of work done by 
// the various phases depends on the optimization level. 
//
// -O0 : insert noops to remove hazards.
// -O1 : perform HBS scheduling for local BBs (ONLY) after register allocation.
//       fill branch delay slot nops (for MIPS).
// -O2 : perform hyperblock(s) scheduling before register allocation.
//       provide accurate register estimates for GRA.
//       invoke post-GRA global scheduling (post-GCM) phase
//       invoke THR phase to perform data-speculation (after register-
//       allocation).
// -O3 : perform hyperblock(s) scheduling before register allocation.
//	 provide accurate register estimates for GRA/GCM.
//	 invoke pre-GRA global scheduling (pre-GCM) phase.
//	 invoke post-GRA global scheduling (post-GCM) phase
//
// ======================================================================
void
IGLS_Schedule_Region (BOOL before_regalloc)
{
  BB *bb;
  BOOL should_we_local_schedule;  // controls local scheduling (single BBs).
  BOOL should_we_global_schedule; // controls HB scheduling and GCM.
  BOOL should_we_schedule;        // controls all scheduling (LOCS,  HBS, GCM)
  BOOL should_we_do_thr;          // controls the THR phase in CG.

  RID *rid;
  HBS_TYPE hbs_type;
  HB_Schedule *Sched = NULL;
  CG_THR      *thr = NULL;

  Set_Error_Phase ("Hyperlock Scheduler");
  Start_Timer (T_Sched_CU);
  Trace_HB = Get_Trace (TP_SCHED, 1);
  should_we_schedule = IGLS_Enable_All_Scheduling;
  should_we_do_thr = CG_enable_thr;
  L_Save();

  // 12581: In "main" entry, always keep spadjust at top, so that debugging
  // info and ctrl register setup occur in correct order.
  if (!strcmp(Cur_PU_Name, "MAIN__") || !strcmp(Cur_PU_Name, "main")) {
    Set_BB_scheduled(REGION_First_BB);
  }

  if (before_regalloc) {

    // schedule if (-O > O1) and
    // -CG:local_sched=on && -CG:pre_local_sched=on.
    should_we_local_schedule = (   CG_opt_level > 1
				   && LOCS_Enable_Scheduling
				   && LOCS_PRE_Enable_Scheduling);

    // global schedule if (-O > O2) and either of the following below are true.
    // -CG:hb_sched=on && -CG:pre_hb_sched=on (hyperblock scheduling).
    // -CG:gcm=on && -CG:pre_gcm=on for GCM.
    should_we_global_schedule = ( CG_opt_level > 2 &&
				  ((IGLS_Enable_HB_Scheduling &&
				    IGLS_Enable_PRE_HB_Scheduling) ||
				   (GCM_PRE_Enable_Scheduling &&
				    GCM_Enable_Scheduling)));

    hbs_type = HBS_BEFORE_GRA | HBS_BEFORE_LRA | HBS_DEPTH_FIRST;
#ifdef KEY
    if (LOCS_Balance_Ready_Types)
      hbs_type |= HBS_BALANCE_READY_TYPES;
    if (LOCS_Balance_Unsched_Types)
      hbs_type |= HBS_BALANCE_UNSCHED_TYPES;
    if (LOCS_Add_Copy ||
	(!LOCS_Add_Copy_set && CG_opt_level > 2)) {
      hbs_type |= HBS_RELAX_REGANTI;
    }
#endif

    if (Trace_HB) {
      #pragma mips_frequency_hint NEVER
      fprintf (TFile, "***** HYPERBLOCK SCHEDULER (before GRA) *****\n");
    }
  }
  else {

    // schedule if (-O > O0) and
    // -CG:local_sched=on && -CG:post_local_sched=on.
    should_we_local_schedule = (   CG_opt_level > 0
				   && LOCS_Enable_Scheduling
				   && LOCS_POST_Enable_Scheduling);

    // global schedule if (-O > O1) and either of the following below are true.
    // -CG:hb_sched=on && -CG:post_hb_sched=on (hyperblock scheduling).
    // -CG:gcm=on && -CG:post_gcm=on for GCM.
    should_we_global_schedule = ( CG_opt_level > 1 &&
				  ((IGLS_Enable_HB_Scheduling &&
				   (IGLS_Enable_POST_HB_Scheduling ||
				    IGLS_Enable_PRE_HB_Scheduling)) ||
				   (GCM_Enable_Scheduling &&
				    GCM_POST_Enable_Scheduling)));
    hbs_type = HBS_CRITICAL_PATH;
    if (PROC_has_bundles()) hbs_type |= HBS_MINIMIZE_BUNDLES;

#ifdef KEY
    if (LOCS_Balance_Ready_Types)
      hbs_type |= HBS_BALANCE_READY_TYPES;
    if (LOCS_Balance_Unsched_Types)
      hbs_type |= HBS_BALANCE_UNSCHED_TYPES;
#endif

    // allow data-speculation if (-O > O1) and -OPT:space is turned off.
    should_we_do_thr = should_we_do_thr && (CG_opt_level > 1) && !OPT_Space;

    if (Trace_HB) {
      #pragma mips_frequency_hint NEVER
      fprintf (TFile, "***** HYPERBLOCK SCHEDULER (after GRA) *****\n");
    }
  }

  // Before register allocation:
  // - Do hyperblock scheduling first to get perfect schedules at each
  //   hyperblock level (register-sensitive). 
  // - Do GCM next to extract global parallelism. Some work needs to be
  //   done, so that it strictly enforces hyperblock boundaries.
  // - Do local scheduling for BBs which are not part of any hyperblocks.

  if (before_regalloc) {
    if (!should_we_schedule) return;

    // Do HB scheduling for all HBs generated (before register allocation).
    if (IGLS_Enable_HB_Scheduling && IGLS_Enable_PRE_HB_Scheduling &&
	should_we_global_schedule) {
      HB_Remove_Deleted_Blocks();
      std::list<HB*>::iterator hbi;
      FOR_ALL_BB_STLLIST_ITEMS_FWD(HB_list, hbi) {
	if (!Sched) {
	  Sched = CXX_NEW(HB_Schedule(), &MEM_local_pool);
	}

	// Check to see if not SWP'd.
	std::list<BB*> hb_blocks;
	Get_HB_Blocks_List(hb_blocks,*hbi);
	if (Can_Schedule_HB(hb_blocks)) {
	  Sched->Init(hb_blocks, hbs_type, NULL);
	  Sched->Schedule_HB(hb_blocks);
	}
      }
    }

    // Try GCM (before register allocation).
    if (GCM_Enable_Scheduling && should_we_global_schedule) {
	Stop_Timer (T_Sched_CU);

	GCM_Schedule_Region (hbs_type);

	Set_Error_Phase ("Hyperblock Scheduler (HBS)");
	Start_Timer (T_Sched_CU);
    }

    if (!should_we_local_schedule) return;

    // Do local scheduling for BBs which are not part of HBs. 
    // (before register allocation).
    for (bb = REGION_First_BB; bb != NULL; bb = BB_next(bb)) {
      
      if (    ( rid = BB_rid(bb) )
	      && ( RID_level(rid) >= RL_CGSCHED ) )
	continue;
      
      if (!BB_scheduled(bb)) {
	if (!Sched) {
	  Sched = CXX_NEW(HB_Schedule(), &MEM_local_pool);
	}
#ifdef KEY
	Run_Sched(Sched, bb, hbs_type, INT32_MAX);
#else
	Sched->Init(bb, hbs_type, INT32_MAX, NULL, NULL);
	Sched->Schedule_BB(bb, NULL, LOCS_Scheduling_Algorithm);
#endif
      }
    }
  }
  else {

    // After register allocation:
    // - Perform data-speculation first, since it will expose more 
    //   parallelism and scheduling opportunities at the block level.
    // - Do hyperblock scheduling next to get perfect schedules at each
    //   hyperblock level (parallelism-driven).
    // - Do GCM next to extract global parallelism. Some work needs to be
    //   done, so that it strictly enforces hyperblock boundaries.
    // - Do local scheduling for BBs which are not part of any hyperblocks.

    // Perform data-speculation first, since it will expose parallelism
    // and scheduling opportunities at the block level.
    // TODO: Invoke data-speculation phase before register allocation,
    // requires GRA spill support, and conditionally invoke the phase
    // after register allocation.

#ifdef KEY
    // Drop prefetches that can't be scheduled into an unused issue slot.
    if (LOCS_Reduce_Prefetch)
      hbs_type |= HBS_DROP_UNSCHED_PREFETCHES;

    // Raise priority of some OPs and retry scheduling.
    if (LOCS_Retry)
      hbs_type |= HBS_RETRY;
#endif

    if (should_we_do_thr) {
      Stop_Timer (T_Sched_CU);
      Set_Error_Phase ("Tree-Height Reduction (THR)");
      Start_Timer (T_THR_CU);

      for (bb = REGION_First_BB; bb != NULL; bb = BB_next(bb)) {
	if (    ( rid = BB_rid(bb) )
		&& ( RID_level(rid) >= RL_CGSCHED ) )
	  continue;

	// Perform data-speculation (if profitable).
	// Avoid processing SWP scheduled blocks, all other scheduled blocks
	// are still considered as candidates for THR.

	if (BB_scheduled(bb) && !BB_scheduled_hbs(bb)) continue;
	if (!thr) {
	  thr = CXX_NEW(CG_THR(), &MEM_local_pool);
	}
	thr->Init(bb, THR_DATA_SPECULATION_NO_RB, FALSE);
	thr->Perform_THR();
	
      } /* for (bb= REGION_First_BB).. */

      Stop_Timer (T_THR_CU);
      Check_for_Dump (TP_THR, NULL);
      Start_Timer (T_Sched_CU);

    } /* should_we_do_thr */

    // Do HB scheduling for all HBs generated (after register allocation).
    if (IGLS_Enable_HB_Scheduling && IGLS_Enable_POST_HB_Scheduling &&
	should_we_schedule && should_we_global_schedule) {

      HB_Remove_Deleted_Blocks();
      std::list<HB*>::iterator hbi;
      FOR_ALL_BB_STLLIST_ITEMS_FWD(HB_list, hbi) {
	if (!Sched) {
	  Sched = CXX_NEW(HB_Schedule(), &MEM_local_pool);
	}
	// Check to see if not SWP'd.
	std::list<BB*> hb_blocks;
	Get_HB_Blocks_List(hb_blocks,*hbi);
	if (Can_Schedule_HB(hb_blocks)) {
	  Sched->Init(hb_blocks, hbs_type, NULL);
	  Sched->Schedule_HB(hb_blocks);
	}
      }
    }

    // Try GCM for the region (after register allocation).
    if (GCM_Enable_Scheduling && should_we_schedule &&
	should_we_global_schedule) {
	Stop_Timer (T_Sched_CU);

 	GCM_Schedule_Region (hbs_type);

        Set_Error_Phase ("Hyperblock Scheduler (HBS)");
	Start_Timer (T_Sched_CU);
    }

    // Do local scheduling for BBs which are not part of HBs. 
    // (after register allocation).
    for (bb = REGION_First_BB; bb != NULL; bb = BB_next(bb)) {
      if (    ( rid = BB_rid(bb) )
	      && ( RID_level(rid) >= RL_CGSCHED ) )
	continue;

      BOOL skip_bb = BB_scheduled(bb) && !BB_scheduled_hbs(bb);

      if (should_we_do_thr && !skip_bb) Remove_Unnecessary_Check_Instrs(bb);

#ifdef KEY_1873
      /* The original code with Reschedule_BB is meanlingless. I think the original
	 author meant BB_scheduled(bb), not Reschedule_BB(bb).
      */
      const BOOL resched = FALSE;
#else
      BOOL resched = !skip_bb && Reschedule_BB(bb); /* FALSE; */      
#endif // KEY
      if (should_we_schedule && should_we_local_schedule &&
	  (!skip_bb || resched)) {

	// TODO: try locs_type = LOCS_DEPTH_FIRST also.
	INT32 max_sched = (resched) ?  OP_scycle(BB_last_op(bb))+1 : INT32_MAX;

#ifdef KEY
	// Reschedule if new OPs were added since the last time the BB was
	// scheduled.  This includes spill OPs added by the register allocator.
	if (max_sched < INT32_MAX) {
	  OP *op;
	  FOR_ALL_BB_OPs_FWD(bb, op) {
	    if (OP_scycle(op) == 0) {
	      max_sched = INT32_MAX;
	      break;
	    }
	  }
	}
#endif
	if (LOCS_Enable_Scheduling) {
	  if (!Sched) {
	    Sched = CXX_NEW(HB_Schedule(), &MEM_local_pool);
	  }
#ifdef KEY
	  BOOL is_fwd = Run_Sched(Sched, bb, hbs_type, max_sched);

	  // Regenerate the schedule in the forward direction to make the
	  // scycle numbers match the way the processor multi-issue the OPs.
	  // (SiCortex 5734)
	  if (!is_fwd &&
	      LOCS_Renumber_Cycles) {
	    Sched->Init(bb, HBS_NO_REORDER, INT32_MAX, NULL, NULL);
	    Sched->Schedule_BB(bb, NULL, 1);
	  }
#else
	  Sched->Init(bb, hbs_type, max_sched, NULL, NULL);
	  Sched->Schedule_BB(bb, NULL, LOCS_Scheduling_Algorithm);
#endif
	}
      }
      Handle_All_Hazards (bb);
    } /* for (bb= REGION_First_BB).. */

#ifdef TARG_X8664
    {
      extern void CG_Sched( MEM_POOL*, BOOL );
      CG_Sched( &MEM_local_pool, Get_Trace( TP_SCHED, 1 ) );
    }
#endif

    // Do branch optimizations here.
    if (should_we_schedule && should_we_local_schedule) {
      if (GCM_Enable_Scheduling) GCM_Fill_Branch_Delay_Slots ();
      if (Assembly) Add_Scheduling_Notes_For_Loops ();
    }
  }

  // need to explicitly delete Sched and thr
  // so that destructors are called.
  if (Sched) {
	CXX_DELETE(Sched, &MEM_local_pool);
  }
  if (thr) {
	CXX_DELETE(thr, &MEM_local_pool);
  }
  L_Free();

  Check_for_Dump (TP_SCHED, NULL);
  Stop_Timer (T_Sched_CU);
}


