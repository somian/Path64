/*
 * Copyright (C) 2008-2009 Advanced Micro Devices, Inc.  All Rights Reserved.
 */

/*
 *  Copyright (C) 2007, 2008, 2009 PathScale, LLC.  All Rights Reserved.
 */

/*
 *  Copyright (C) 2006, 2007. QLogic Corporation. All Rights Reserved.
 */

/*
 * Copyright 2002, 2003, 2004, 2005, 2006 PathScale, Inc.  All Rights Reserved.
 */

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
 *  Module: ebo.cxx
 *  $Revision: 1.89 $
 *  $Date: 05/11/10 18:45:11-08:00 $
 *  $Author: tkong@hyalite.keyresearch $
 *  $Source: be/cg/SCCS/s.ebo.cxx $
 *
 *  Revision comments:
 *
 *  29-May-1998 - Initial version
 *
 *  Description:
 *  ============
 *
 *  EBO implementation.  See "ebo.h" for interface.
 *
 * =======================================================================
 * =======================================================================
 */

/* =======================================================================
 * =======================================================================
 *
 * This is the Extended Block Optimizer (EBO).
 *
 * It is a peep hole optimizer that works on a sequence of blocks that may
 * contain branches out, but can only be execxuted from the start of the
 * first block in the sequence.  These sequences are recognized during
 * processing.
 *
 * Instructions are processed in the forward direction through each block
 * and in the forward direction through the block's successor list.  New
 * blocks are processed until a "branched to" label is encountered, at
 * which time processing backs up and attempts to take a different path
 * down another successor list.
 *
 * The optimizations performed include forward propagation, common expression
 * elimination, constant folding, dead code elimination and a host of special
 * case transformations that are unique to the arcitecture of a particular
 * machine.
 *
 * In order to perform the optimizations it is nececssary to recognize the
 * definition and use connections between values.  Since this routine may be
 * called several times during compilation, and may have different information
 * available each time it is called, it is necessary to abstract the code
 * representation of the original program so that data values can be easily
 * tracked by common routines.
 *
 * The data structure used to abstract the values is the EBO_TN_INFO.  One is
 * created the first time a value is encoutered.  The EBO_TN_INFO entry is
 * associated with the register, if one has been assigned, or the TN.  This
 * allows us to support special, hard coded registers that are assigned early
 * in the code generation process, provided that references to the value
 * always use the register name to refer to that particular value.
 *
 * The data structure used to abstract each OP is the EBO_OP_INFO.  A unique
 * one is created for each OP that is encountered and contain pointers to
 * the unique EBO_TN_INFO entries that represent the values used and defined
 * by the original OP.
 *
 * These data structures make it simple to track predicated values and allow
 * for the redefintion of TNs and regeisters in a block, while providing
 * access to the previous definition.  It also simplifies the task of tracking
 * predicated code that reuses TN names and registers but define unique values.
 *
 * The construction of this abstraction is done as early s possible, allowing
 * the rest of EBO to work only with these *_INFO entries.  The design supports
 * tracking of constants, even when the original instructions can not directly
 * reference constants in their operand fields.
 *
 */

#ifdef USE_PCH
#include "cg_pch.h"
#endif // USE_PCH
#pragma hdrstop

#ifdef _KEEP_RCS_ID
static const char source_file[] = __FILE__;
#endif /* _KEEP_RCS_ID */

#include <alloca.h>

#include "defs.h"
#include "errors.h"
#include "mempool.h"
#include "tracing.h"
#include "timing.h"
#include "cgir.h"
#include "tn_map.h"
#include "cg_loop.h"
#include "cg.h"
#include "cgexp.h"
#include "register.h"
#include "cg_region.h"
#include "wn.h"
#include "region_util.h"
#include "op_list.h"
#include "cgprep.h"
#include "gtn_universe.h"
#include "gtn_set.h"
#include "cg_db_op.h"
#include "whirl2ops.h"
#include "cgtarget.h"
#include "gra_live.h"
#include "reg_live.h"
#include "cflow.h"
#include "cg_spill.h"
#include "targ_proc_properties.h"

#include "ebo.h"
#include "ebo_info.h"
#include "ebo_special.h"
#include "ebo_util.h"

#ifdef TARG_X8664
#include "targ_sim.h"
#include "config_wopt.h"
#include "config_lno.h"
#include "xstats.h"	// Spill_Var_Cnt
#include "dominate.h"
#include "glob.h"

static void Init_Remove_Dead_LRA_Stores(BS **bs, MEM_POOL *pool);
static void Mark_LRA_Spill_Reference(OP *op, BS **bs, MEM_POOL *pool);
static BOOL Delete_Dead_LRA_Spill(OP *op, BS **bs);
#endif

#ifdef KEY
static void EBO_Adjust_Pred_Branch_Target (BB *bb);
#endif

/* ===================================================================== */
/* Global Data:								 */
/* ===================================================================== */

#ifdef KEY
INT32 EBO_Opt_Level_Default = 6;
INT32 EBO_Opt_Level = 6;
#else
INT32 EBO_Opt_Level_Default = 5;
INT32 EBO_Opt_Level = 5;
#endif
#ifdef KEY
INT32 EBO_Opt_Mask = -1;
#endif
BOOL  CG_skip_local_ebo = FALSE;

INT EBO_tninfo_number = 0;
EBO_TN_INFO *EBO_free_tninfo = NULL;
EBO_TN_INFO *EBO_first_tninfo = NULL;
EBO_TN_INFO *EBO_last_tninfo = NULL;
EBO_OP_INFO *EBO_free_opinfo = NULL;
EBO_OP_INFO *EBO_first_opinfo = NULL;
EBO_OP_INFO *EBO_last_opinfo = NULL;
EBO_OP_INFO *EBO_opinfo_table[EBO_MAX_OP_HASH];

/* Entry point indicators. */
BOOL EBO_in_pre  = FALSE;
BOOL EBO_in_before_unrolling = FALSE;
BOOL EBO_in_after_unrolling = FALSE;
LOOP_DESCR *EBO_loop = NULL;
INT EBO_loop_size = 0;
BOOL EBO_in_peep = FALSE;
BOOL EBO_flow_safe = FALSE;
BOOL EBO_doms_calculated = FALSE;

/* Are OMEGA entries present? */
BOOL EBO_in_loop = FALSE;

#ifdef KEY
BOOL EBO_can_delete_branch_delay_OP = TRUE;
BOOL EBO_no_liveness_info_available = FALSE;
BOOL EBO_optimize_single_BB = FALSE;
#endif

TN_MAP EBO_tninfo_table;
MEM_POOL EBO_pool;

INT EBO_num_tninfo_entries = 0;
INT EBO_tninfo_entries_reused = 0;
INT EBO_num_opinfo_entries = 0;
INT EBO_opinfo_entries_reused = 0;

const char *EBO_trace_pfx;
BOOL EBO_Trace_Execution    = FALSE;
BOOL EBO_Trace_Optimization = FALSE;
BOOL EBO_Trace_Block_Flow   = FALSE;
BOOL EBO_Trace_Data_Flow    = FALSE;
BOOL EBO_Trace_Hash_Search  = FALSE;


/* ===================================================================== */
/* Local Data:								 */
/* ===================================================================== */

static BOOL in_delay_slot = FALSE;
static BOOL rerun_cflow = FALSE;

#ifdef KEY
/* ===================================================================== */
/* Track register pressure within a basic block:                         */
/* ===================================================================== */

// Number of registers available in each register class.
static int avail_regs_count[ISA_REGISTER_CLASS_MAX+1];

// Map OP to the change in the number of registers used at OP.
static OP_MAP Regs_Delta_Map[ISA_REGISTER_CLASS_MAX+1];

// Map TN to EBO_REG_ENTRY for the current BB.
static hTN_MAP regs_map = NULL;

// Based on code in hb_sched.h.
typedef union {
  void *ptr;
  struct {
    unsigned int def_count : 15;
    unsigned int last_use_op_num : 15;	// OP where TN is last used in BB
    unsigned int reg_assigned : 1;
  }s;
} EBO_REG_ENTRY;

#define EBO_REG_ENTRY_ptr(re)			(re.ptr)
#define EBO_REG_ENTRY_def_count(re)		(re.s.def_count)
#define EBO_REG_ENTRY_reg_assigned(re)		(re.s.reg_assigned)
#define EBO_REG_ENTRY_last_use_OP_num(re)	(re.s.last_use_op_num)

// Determine the change in register pressure at each OP in the BB.  Based on
// code in hb_sched.cxx.
static void
Estimate_Reg_Usage (BB *bb, MEM_POOL *pool)
{
  OP *op;
  EBO_REG_ENTRY reginfo;
  INT32 global_regs_used[ISA_REGISTER_CLASS_MAX+1],
	regs_delta[ISA_REGISTER_CLASS_MAX+1];
  ISA_REGISTER_CLASS cl;
  INT i, op_num = 0;

  regs_map = hTN_MAP_Create(pool);

  FOR_ALL_ISA_REGISTER_CLASS(cl) {
    global_regs_used[cl] = 0;
  }

  // Build regs_map.  Count the number of GTNs and dedicated TNs referenced in
  // the basic block.
  FOR_ALL_BB_OPs_FWD (bb, op) {
    op_num++;
    for (i = 0; i < OP_results(op); i++) {
      TN *result_tn = OP_result(op, i);
      EBO_REG_ENTRY_ptr(reginfo) =  hTN_MAP_Get (regs_map, result_tn);
      EBO_REG_ENTRY_def_count(reginfo)++;
      if (TN_is_global_reg(result_tn) || TN_is_dedicated(result_tn)) {
	if (!EBO_REG_ENTRY_reg_assigned(reginfo)) {
	  cl = TN_register_class(result_tn);
	  global_regs_used[cl]++;
	  EBO_REG_ENTRY_reg_assigned(reginfo) = TRUE;
	}
      }
      hTN_MAP_Set (regs_map, result_tn, EBO_REG_ENTRY_ptr(reginfo));
    }
    for (i = 0; i < OP_opnds(op); i++) {
      TN *opnd_tn = OP_opnd(op,i);
      if (TN_is_constant(opnd_tn)) continue;
      if (TN_is_global_reg(opnd_tn) || TN_is_dedicated(opnd_tn)) {
        EBO_REG_ENTRY_ptr(reginfo) = hTN_MAP_Get (regs_map, opnd_tn);
	if (!EBO_REG_ENTRY_reg_assigned(reginfo)) {
	  cl = TN_register_class(opnd_tn);
	  global_regs_used[cl]++;
	  EBO_REG_ENTRY_reg_assigned(reginfo) = TRUE;
	  hTN_MAP_Set (regs_map, opnd_tn, EBO_REG_ENTRY_ptr(reginfo));
	}
      }
    }
  }

  // Determine the change in register pressure at each OP.
  FOR_ALL_BB_OPs_REV (bb, op) {
    FOR_ALL_ISA_REGISTER_CLASS(cl) {
      regs_delta[cl] = 0;
    }
    for (i = 0; i < OP_results(op); i++) {
      TN *result_tn = OP_result(op, i);
      EBO_REG_ENTRY_ptr(reginfo) = hTN_MAP_Get (regs_map, result_tn);
      EBO_REG_ENTRY_def_count(reginfo)--;
      if (EBO_REG_ENTRY_def_count(reginfo) == 0 &&
	  EBO_REG_ENTRY_reg_assigned(reginfo)) 
	{
	  ISA_REGISTER_CLASS cl = TN_register_class(result_tn);
	  regs_delta[cl]++;	// delta for forward BB traversal
	  EBO_REG_ENTRY_reg_assigned(reginfo) = FALSE;
	}
      hTN_MAP_Set (regs_map, result_tn, EBO_REG_ENTRY_ptr(reginfo));
    }
    for (i = 0; i < OP_opnds(op); i++) {
      TN *opnd_tn = OP_opnd(op,i);
      if (TN_is_constant(opnd_tn)) continue;
      EBO_REG_ENTRY_ptr(reginfo) = hTN_MAP_Get (regs_map, opnd_tn);
      if (!EBO_REG_ENTRY_reg_assigned(reginfo)) {
	ISA_REGISTER_CLASS cl = TN_register_class(opnd_tn);
	regs_delta[cl]--;	// delta for forward BB traversal
	EBO_REG_ENTRY_reg_assigned(reginfo) = TRUE;
	if (EBO_REG_ENTRY_last_use_OP_num(reginfo) == 0)
	  EBO_REG_ENTRY_last_use_OP_num(reginfo) = op_num;
	hTN_MAP_Set (regs_map, opnd_tn, EBO_REG_ENTRY_ptr(reginfo));
      }
    }
    // Account for global register usage by adding the globals to the first
    // OP's delta.
    if (op_num == 1) {
      FOR_ALL_ISA_REGISTER_CLASS(cl) {
	regs_delta[cl] += global_regs_used[cl];
      }
    }
    // Record the deltas.
    FOR_ALL_ISA_REGISTER_CLASS(cl) {
      OP_MAP32_Set(Regs_Delta_Map[cl], op, regs_delta[cl]);
    }
    op_num--;
  }
}

// Extend the live range of TN to OP_NUM.
static void
Extend_Live_Range (int *regs_used, int *last_fat_point, int op_num,
		   EBO_TN_INFO *tninfo, EBO_REG_ENTRY reginfo, BB *bb)
{
  int i;
  ISA_REGISTER_CLASS cl = TN_register_class(tninfo->local_tn);
  int regs_avail = avail_regs_count[cl];

  // Determine the OP that previously last uses the live range.
  int last_use_op = (tninfo->in_bb == bb) ? 
		      EBO_REG_ENTRY_last_use_OP_num(reginfo) : 1;
  // Update register usage and fat point.
  for (i = op_num - 1; i >= last_use_op; i--) {
    EBO_REG(regs_used, cl, i)++;
    if (EBO_REG(regs_used, cl, i) > regs_avail) {
      last_fat_point[cl] = i;
      // Don't update register usage before the fat point since we don't care
      // about those usages.
      break;
    }
  }

  // Extend live range to include the OP at op_num.
  EBO_REG_ENTRY_last_use_OP_num(reginfo) = op_num;
  hTN_MAP_Set(regs_map, tninfo->local_tn, EBO_REG_ENTRY_ptr(reginfo));
}

// Update the register usage.  Adjust the fat points to indicate register file
// overflow.  CSE will not extend a TN's live range beyond a fat point.
static void
Update_Reg_Usage (OP *op, int op_num, int *regs_used, int *last_fat_point,
		  BOOL op_is_deleted_load)
{
  ISA_REGISTER_CLASS cl;

  FOR_ALL_ISA_REGISTER_CLASS(cl) {
    int regs_delta_at_op = OP_MAP32_Get(Regs_Delta_Map[cl], op);
    int regs_avail = avail_regs_count[cl];
    int regs_used_after_op =
	  EBO_REG(regs_used, cl, op_num-1) + regs_delta_at_op;
    if (regs_used_after_op > regs_avail)
      last_fat_point[cl] = op_num;
    EBO_REG(regs_used, cl, op_num) = regs_used_after_op;
  }

  // If OP is a deleted load, then no register is needed for the result.
  if (op_is_deleted_load) {
    cl = TN_register_class(OP_result(op, 0));
    EBO_REG(regs_used, cl, op_num)--;
  }
}
#endif	// KEY

/* ===================================================================== */

/* The BB flag: <local_flag1> is overloaded temporarily in this routine
 * as <visited> to keep track of the fact that we have seen this block
 * during processing. The bit will NOT be cleared by the time we exit.
 */
#define BB_visited          BB_local_flag1
#define Set_BB_visited      Set_BB_local_flag1
#define Reset_BB_visited    Reset_BB_local_flag1

inline void clear_bb_flag(BB *first_bb)
{
  BB *bb;
  for (bb = first_bb; bb != NULL; bb = BB_next(bb)) {
    BBLIST *succ_list;

    Reset_BB_visited(bb);

    FOR_ALL_BB_SUCCS(bb, succ_list) { 
      BB *succ = BBLIST_item(succ_list);
      Reset_BB_visited(succ);
    }

  }
}

/* ===================================================================== */

static
BOOL EBO_Fix_Same_Res_Op (OP *op,
                          TN **opnd_tn,
                          EBO_TN_INFO **opnd_tninfo)
{
  if (EBO_in_loop) return FALSE;;

  if (OP_unalign_ld (op)) {
    UINT8 idx = OP_find_opnd_use (op, OU_same_res);
    TN   *res = OP_result (op, 0);
    TN   *tnl = OP_opnd (op, idx);

    if (!TN_is_zero_reg(tnl) && !tn_registers_identical(res, tnl)) {
     /* Allocate a new TN for the result. */
      OPS ops = OPS_EMPTY;
      TN *new_res = Dup_TN (res);
      OP *new_op = Dup_OP (op);
      Exp_COPY(new_res, tnl, &ops);
      Set_OP_result(new_op, 0, new_res);
      Set_OP_opnd(new_op, OP_opnds(op)-1, new_res);
      OPS_Append_Op(&ops, new_op);
      Exp_COPY(res, new_res, &ops);
      BB_Insert_Ops(OP_bb(op), op, &ops, FALSE);
      return TRUE;
    }
  }
  else if (OP_select(op)) {
    TN *res = OP_result(op, 0);
    TN *tn0 = OP_opnd(op, 0);
    TN *tn1;
    TN *tn2;

   /* For special case optimizations, check the OPTIMAL operands. */
    tn1 = opnd_tn[1];
    tn2 = opnd_tn[2];

    if (tn_registers_identical(tn1, tn2)) {
     /* We can optimize this! But return the ACTUAL operand. */
      OPS ops = OPS_EMPTY;
      Exp_COPY(res, OP_opnd(op, 1), &ops);
      BB_Insert_Ops(OP_bb(op), op, &ops, FALSE);
      if (EBO_Trace_Optimization) {
        #pragma mips_frequency_hint NEVER
        fprintf(TFile,"Optimize select - operands are the same\n");
      }
      return TRUE;
    }

    if (TN_is_global_reg(tn1) &&
        (opnd_tninfo[1] != NULL) &&
        (opnd_tninfo[1]->in_op == NULL) &&
        (opnd_tninfo[1]->in_bb != NULL) &&
	(opnd_tninfo[1]->in_bb != OP_bb(op)) &&
        !tn_has_live_def_into_BB(tn1, opnd_tninfo[1]->in_bb)) {
     /* Assume that this value will not be used -
        turn this instruction into a copy of the other operand.
        But return the ACTUAL operand. */
      OPS ops = OPS_EMPTY;
      Exp_COPY(res, OP_opnd(op, 2), &ops);
      BB_Insert_Ops(OP_bb(op), op, &ops, FALSE);
      if (EBO_Trace_Optimization) {
        #pragma mips_frequency_hint NEVER
        fprintf(TFile,"Optimize select - op1 %s can't be used: ",
                EBO_in_peep ? "REG" : "TN");
        Print_TN(OP_opnd(op, 2), FALSE);
        fprintf(TFile,"\n");
      }
      return TRUE;
    }

    if (TN_is_global_reg(tn2) &&
        (opnd_tninfo[2] != NULL) &&
        (opnd_tninfo[2]->in_op == NULL) &&
        (opnd_tninfo[2]->in_bb != NULL) &&
        (opnd_tninfo[2]->in_bb != OP_bb(op)) &&
        !tn_has_live_def_into_BB(tn2, opnd_tninfo[2]->in_bb)) {
    /* Assume that this value will not be used -
        turn this instruction into a copy of the other operand.
        But return the ACTUAL operand. */
      OPS ops = OPS_EMPTY;
      Exp_COPY(res, OP_opnd(op, 1), &ops);
      BB_Insert_Ops(OP_bb(op), op, &ops, FALSE);
      if (EBO_Trace_Optimization) {
        #pragma mips_frequency_hint NEVER
        fprintf(TFile,"Optimize select - op2 %s can't be used: ",
                EBO_in_peep ? "REG" : "TN");
        Print_TN(OP_opnd(op, 1), FALSE);
        fprintf(TFile,"\n");
      }
      return TRUE;
    }

   /* For ensuring that the result operand matches one of the input operands,
      test the ACTUAL operands used in the expression. */
    tn0 = OP_opnd(op, 0);
    tn1 = OP_opnd(op, 1);
    tn2 = OP_opnd(op, 2);
    if ((TN_Is_Constant(tn1) || !tn_registers_identical(res, tn1)) &&
        (TN_Is_Constant(tn2) || !tn_registers_identical(res, tn2))) {
      OPS ops = OPS_EMPTY;
      OP *new_op = Dup_OP (op);

      if (has_assigned_reg(res)) {
       /* Use the existing result TN as the duplicate input. */
        TN *new_res = OP_result(op,0);
        FmtAssert((TN_Is_Constant(tn0) || !tn_registers_identical(res, tn0)),
                  ("Condition code also used as result of select"));
        if (TN_Is_Constant(tn2)) {
          Exp_COPY(new_res, tn2, &ops);
          Set_OP_opnd(new_op, 2, new_res);
        } else {
          Exp_COPY(new_res, tn1, &ops);
          Set_OP_opnd(new_op, 1, new_res);
        }
        OPS_Append_Op(&ops, new_op);
      } else {
       /* Allocate a new TN for the result. */
        TN *new_res = Dup_TN (res);
        Exp_COPY(new_res, tn1, &ops);
        Set_OP_result(new_op, 0, new_res);
        Set_OP_opnd(new_op, 1, new_res);
        OPS_Append_Op(&ops, new_op);
        Exp_COPY(res, new_res, &ops);
      }

      BB_Insert_Ops(OP_bb(op), op, &ops, FALSE);
      if (EBO_Trace_Optimization) {
        #pragma mips_frequency_hint NEVER
        fprintf(TFile,"Rewrite select\n");
      }
      return TRUE;
    }
  }

  return FALSE;

}

BOOL TN_live_out_of(TN *tn, BB *bb)
/* -----------------------------------------------------------------------
 * Requires: global liveness info up-to-date
 * Return TRUE iff <tn> is live out of <bb>.
 * -----------------------------------------------------------------------
 */
{
  if (EBO_Trace_Execution) {
    #pragma mips_frequency_hint NEVER
    fprintf(TFile,"%sEnter TN_live_out_of BB:%d ",EBO_trace_pfx,BB_id(bb));
    Print_TN(tn, FALSE);
    fprintf(TFile,"\n");
  }

#ifdef KEY
  if (EBO_no_liveness_info_available)
    return TRUE;
#endif

  if (EBO_in_peep) {
    if (EBO_Trace_Data_Flow) {
      #pragma mips_frequency_hint NEVER
      fprintf(TFile,"REG_LIVE_Outof_BB %s\n",
         REG_LIVE_Outof_BB (TN_register_class(tn), TN_register(tn), bb)?"TRUE":"FALSE");
    }
    return REG_LIVE_Outof_BB (TN_register_class(tn), TN_register(tn), bb);
  }
  else {
    if (CG_localize_tns) return (TN_is_dedicated(tn) || TN_is_global_reg(tn));
    return GRA_LIVE_TN_Live_Outof_BB (tn, bb);
  }
}



inline BOOL op_is_needed_globally(OP *op)
/* -----------------------------------------------------------------------
 * Requires: global liveness info
 * Return TRUE iff the result of <op> is necessary past the end of its BB.
 * ----------------------------------------------------------------------- */
{
  BB *bb = OP_bb(op);

  if (OP_copy(op)) {
   /* Copies don't have side effects unless a save_reg is involved. */
    if (OP_glue(op) && !EBO_in_peep)
      return TRUE;
    if ((TN_is_save_reg(OP_result(op,0)) || TN_is_save_reg(OP_opnd(op,OP_COPY_OPND))) &&
        !tn_registers_identical(OP_result(op,0), OP_opnd(op,OP_COPY_OPND))) {
      return TRUE;
    }
    return FALSE;
  }
  if (TN_is_save_reg(OP_result(op,0)))
    return TRUE;
  if (OP_glue(op) && !has_assigned_reg(OP_result(op,0)))
    return TRUE;
  if (CGTARG_Is_OP_Intrinsic(op))
   /* Intrinsic ops may have side effects we don't understand */
    return TRUE;
  if (OP_call(op)) 
   /* Calls may have side effects we don't understand */
    return TRUE;
  if (op == BB_exit_sp_adj_op(bb) || op == BB_entry_sp_adj_op(bb))
    return TRUE;
  return FALSE;
}


/* ===================================================================== */


void
tn_info_entry_dump (EBO_TN_INFO *tninfo)
{
  fprintf(TFile,"entry %d\tBB:%d, use count = %d, redefined = %s, same as %d, predicate %d:  ",
          tninfo->sequence_num,tninfo->in_bb?BB_id(tninfo->in_bb):0,
          tninfo->reference_count,
          tninfo->redefined_before_block_end?"TRUE":"FALSE",
          tninfo->same?tninfo->same->sequence_num:0,
          tninfo->predicate_tninfo?tninfo->predicate_tninfo->sequence_num:0);
  Print_TN (tninfo->local_tn, TRUE);
  fprintf(TFile,"[%d]",tninfo->omega);
  if (tninfo->replacement_tn != NULL) {
    fprintf(TFile,"\n\treplace TN with: ");
    Print_TN (tninfo->replacement_tn, TRUE);
    if (tninfo->replacement_tninfo != NULL) {
      fprintf(TFile," (Entry Number %d)",
              tninfo->replacement_tninfo->sequence_num);
    }
  }
  fprintf(TFile,"\n");
  if (tninfo->in_op) {
    fprintf(TFile,"\t");
    Print_OP_No_SrcLine(tninfo->in_op);
  }

  FmtAssert((TN_number(tninfo->local_tn) <= Last_TN),
                  ("TN number exceeds allowed range"));

}


void
tn_info_table_dump ()
{
  EBO_TN_INFO *tninfo = EBO_first_tninfo;

  fprintf(TFile,"\n>>>>>> EBO INFO DUMP <<<<<\n");

  while (tninfo != NULL) {
    tn_info_entry_dump(tninfo);
    tninfo = tninfo->next;
  }

  fprintf(TFile,">>>>>> EBO INFO DUMP COMPLETE <<<<<\n\n");

}



void EBO_Init(void)
/* -----------------------------------------------------------------------
 * See "ebo.h" for interface.
 * -----------------------------------------------------------------------
 */
{

  EBO_Trace_Execution    = FALSE;
  EBO_Trace_Optimization = FALSE;
  EBO_Trace_Block_Flow   = FALSE;
  EBO_Trace_Data_Flow    = FALSE;
  EBO_Trace_Hash_Search  = FALSE;

  MEM_POOL_Initialize(&EBO_pool, "ebo", FALSE);
  MEM_POOL_Push(&EBO_pool);
  EBO_tninfo_table = NULL;

  EBO_tninfo_number = 0;
  EBO_free_tninfo = NULL;
  EBO_first_tninfo = NULL;
  EBO_last_tninfo = NULL;

  EBO_free_opinfo = NULL;
  EBO_first_opinfo = NULL;
  EBO_last_opinfo = NULL;

  bzero(EBO_opinfo_table,sizeof(EBO_opinfo_table));

  EBO_num_tninfo_entries = 0;
  EBO_tninfo_entries_reused = 0;
  EBO_num_opinfo_entries = 0;
  EBO_opinfo_entries_reused = 0;
  EBO_trace_pfx = "<ebo> ";

#ifdef KEY
  // Find the number of registers available in each register class.
  ISA_REGISTER_CLASS cl;
  FOR_ALL_ISA_REGISTER_CLASS(cl) {
    REGISTER_SET reg_set = REGISTER_CLASS_allocatable(cl);
    REGISTER reg;
    avail_regs_count[cl] = 0;
    for (reg = REGISTER_SET_Choose(reg_set);
	 reg != REGISTER_UNDEFINED;
	 reg = REGISTER_SET_Choose_Next(reg_set, reg)) {
      avail_regs_count[cl]++;
    }
    // Tune the number of registers available for CSE.
    if (cl == ISA_REGISTER_CLASS_integer)
      avail_regs_count[cl] += CG_cse_regs;
    else if (cl == ISA_REGISTER_CLASS_float)
      avail_regs_count[cl] += CG_sse_cse_regs;
  }
#endif
}




static void EBO_Start()
/* -----------------------------------------------------------------------
 * -----------------------------------------------------------------------
 */
{
 /* Initialize data structures.  */
  MEM_POOL_Push(&MEM_local_pool);
  EBO_tninfo_table = TN_MAP_Create();

#ifdef TARG_X8664
  if (!EBO_in_peep && !EBO_in_after_unrolling && !Are_Dominators_Calculated()) {
    EBO_doms_calculated = TRUE;
    Calculate_Dominators();
  }
#endif

#ifdef KEY
  ISA_REGISTER_CLASS cl;
  FOR_ALL_ISA_REGISTER_CLASS(cl) {
    Regs_Delta_Map[cl] = OP_MAP32_Create();
  }

  EBO_Special_Start( &MEM_local_pool );
#endif // KEY
}



static void EBO_Finish(void)
/* -----------------------------------------------------------------------
 * -----------------------------------------------------------------------
 */
{
#ifdef KEY
  ISA_REGISTER_CLASS cl;
  FOR_ALL_ISA_REGISTER_CLASS(cl) {
    OP_MAP_Delete(Regs_Delta_Map[cl]);
    Regs_Delta_Map[cl] = NULL;
  }

  EBO_Special_Finish();
#endif

#ifdef TARG_X8664
  if (!EBO_in_peep && !EBO_in_after_unrolling && EBO_doms_calculated) {
    EBO_doms_calculated = FALSE;
    Free_Dominators_Memory();
  }
#endif

  TN_MAP_Delete (EBO_tninfo_table);
  EBO_tninfo_table = NULL;
  MEM_POOL_Pop(&MEM_local_pool);
}




void EBO_Finalize(void)
/* -----------------------------------------------------------------------
 * See "ebo.h" for interface.
 * -----------------------------------------------------------------------
 */
{
  MEM_POOL_Pop(&EBO_pool);
  MEM_POOL_Delete(&EBO_pool);
}


/* ===================================================================== */

  
static INT copy_operand(OP *op)
/* -------------------------------------------------------------------------
 *
 * Return the operand index for the operand that is copied.
 * -------------------------------------------------------------------------
 */
{
  INT opnd;

  if (OP_copy(op)) {
    return OP_COPY_OPND;
  }
  opnd = CGTARG_Copy_Operand(op);
  if (opnd >= 0) {
    return opnd;
  }
  opnd = EBO_Copy_Operand(op);
  if (opnd >= 0) {
    return opnd;
  }
  return -1;
}



static void
find_index_and_offset (EBO_TN_INFO *original_tninfo,
                       TN **base_tn, EBO_TN_INFO **base_tninfo,
                       TN **offset_tn, EBO_TN_INFO **offset_tninfo)
{
 /* Look for an offset descriptor in the index. */
  EBO_OP_INFO *indx_opinfo = locate_opinfo_entry(original_tninfo);
  if ((indx_opinfo != NULL) &&
      (indx_opinfo->in_op != NULL) &&
      (OP_iadd(indx_opinfo->in_op) || 
       EBO_Can_Merge_Into_Offset(indx_opinfo->in_op))) {
#ifdef TARG_X8664
    INT op1_idx = 0;
    INT op2_idx = 1;
#else
    INT op1_idx = TOP_Find_Operand_Use(OP_code(indx_opinfo->in_op),OU_opnd1);
    INT op2_idx = TOP_Find_Operand_Use(OP_code(indx_opinfo->in_op),OU_opnd2);
#endif
    if ((op1_idx >= 0) && (op2_idx >= 0)) {
      EBO_TN_INFO *op1_tninfo = indx_opinfo->actual_opnd[op1_idx];
      EBO_TN_INFO *op2_tninfo = indx_opinfo->actual_opnd[op2_idx];
      TN *op1_tn;
      TN *op2_tn;
      if (op1_tninfo != NULL) {
        if ((op1_tninfo->replacement_tn) &&
            (TN_is_symbol(op1_tninfo->replacement_tn) || TN_Is_Constant(op1_tninfo->replacement_tn))) {
          op1_tn = op1_tninfo->replacement_tn;
          op1_tninfo = op1_tninfo->replacement_tninfo;
        } else {
          op1_tn = op1_tninfo->local_tn;
        }
      } else {
        op1_tn = OP_opnd(indx_opinfo->in_op,op1_idx);
      }
      if (op2_tninfo != NULL) {
        if ((op2_tninfo->replacement_tn) &&
            (TN_is_symbol(op2_tninfo->replacement_tn) || TN_Is_Constant(op2_tninfo->replacement_tn))) {
          op2_tn = op2_tninfo->replacement_tn;
          op2_tninfo = op2_tninfo->replacement_tninfo;
        } else {
          op2_tn = op2_tninfo->local_tn;
        }
      } else {
        op2_tn = OP_opnd(indx_opinfo->in_op,op2_idx);
      }

      if (TN_is_symbol(op1_tn) || TN_Is_Constant(op1_tn)) {
        TN *save = op1_tn;
        op1_tn = op2_tn;
        op2_tn = save;
        op1_tninfo = op2_tninfo;
        op2_tninfo = NULL;
      }

      *base_tn = op1_tn;
      *base_tninfo = op1_tninfo;
      *offset_tn = op2_tn;
      *offset_tninfo = op2_tninfo;

      if (EBO_Trace_Hash_Search) {
        #pragma mips_frequency_hint NEVER
        fprintf(TFile,"%sUse inputs to index: ",EBO_trace_pfx);
        Print_TN(*base_tn,FALSE);
        fprintf(TFile," ");
        Print_TN(*offset_tn,FALSE);
        fprintf(TFile,"\n");
      }
    }
  }
}



static void
merge_memory_offsets( OP *op,
                      INT index_opnd,
                      TN **opnd_tn,
                      EBO_TN_INFO **opnd_tninfo,
                      EBO_TN_INFO **actual_tninfo)
{
  EBO_TN_INFO *index_tninfo = opnd_tninfo[index_opnd];
  OP *index_op = (index_tninfo != NULL) ? index_tninfo->in_op : NULL;
  TN *immed_tn = opnd_tn[index_opnd+1];
  ST *immed_sym = TN_is_symbol(immed_tn) ? TN_var(immed_tn) : NULL;
  INT64 immed_offset = TN_is_symbol(immed_tn) ? TN_offset(immed_tn) : TN_value(immed_tn);
  EBO_OP_INFO *index_opinfo;
  TN *additive_index_tn;
  TN *additive_immed_tn;
  EBO_TN_INFO *additive_index_tninfo;

  TN *adjust_tn = NULL;
  ST *adjust_sym = NULL;
  INT64 adjust_offset = 0;
  TN *new_tn;

  if ((index_tninfo == NULL) ||
      (index_op == NULL) ||
      (!EBO_Can_Merge_Into_Offset (index_op))) {
    return;
  }

  index_opinfo = locate_opinfo_entry (index_tninfo);
  if (index_opinfo == NULL) return;

  additive_index_tn = OP_opnd(index_op,0);
  additive_index_tninfo = index_opinfo->actual_opnd[0];
#ifdef TARG_X8664
  // Handling -fPIC in exp_loadstore.cxx exposes this bug when assembling bug 274
  if ( additive_index_tn == Rip_TN() )
    return;
#endif

  additive_immed_tn = OP_opnd(index_op,1);
  if (!TN_Is_Constant(additive_immed_tn)) return;

 /* Would the new index value be available for use? */
  if (!TN_Is_Constant(additive_index_tn) &&
      !EBO_tn_available(OP_bb(op), additive_index_tninfo)) {
    return;
  }

  if (TN_is_symbol(additive_immed_tn)) {
    if (immed_sym != NULL) return;
    if (OP_isub(index_op)) return;
    if (immed_offset != 0) {
     /* Don't move relocation from an ADD instruction to a memory
        offset field if there is a chance that the (relocation +
        offset) value might overflow the field. */
      return;
    }
    adjust_sym = TN_var(additive_immed_tn);
    adjust_tn = additive_immed_tn;
    adjust_offset = immed_offset + TN_offset(additive_immed_tn);
  } else {
    adjust_sym = immed_sym;
    adjust_tn = immed_tn;
    adjust_offset = TN_value(additive_immed_tn);
    if (OP_isub(index_op)) adjust_offset = -adjust_offset;
    adjust_offset += immed_offset;
  }

  if (!TOP_Can_Have_Immediate( adjust_offset, OP_code(op))) {\
    if (EBO_Trace_Optimization) {
      #pragma mips_frequency_hint NEVER
      fprintf(TFile, "%sin BB:%d combined index expressions do not fit in the offset field\n",
              EBO_trace_pfx, BB_id(OP_bb(op)));
      Print_OP_No_SrcLine(op);
    }
    return;
  }

  if (EBO_Trace_Optimization) {
    #pragma mips_frequency_hint NEVER
    fprintf(TFile, "%sin BB:%d merge memory index expression (from BB:%d) with offset (in BB:%d)\n",
            EBO_trace_pfx, BB_id(OP_bb(op)),BB_id(OP_bb(index_op)),BB_id(OP_bb(op)));
    Print_OP_No_SrcLine(index_op);
    Print_OP_No_SrcLine(op);
  }

 /* Create a combined TN and upated the op and other data structures. */
  if (adjust_sym != NULL) {
    new_tn = Gen_Symbol_TN(adjust_sym, adjust_offset, TN_relocs(adjust_tn));
  } else {
    new_tn = Gen_Literal_TN (adjust_offset, TN_size(adjust_tn));
  }

  if (actual_tninfo[index_opnd] != NULL) {
    dec_ref_count(actual_tninfo[index_opnd]);
  }
  if (additive_index_tninfo != NULL) {
    inc_ref_count(additive_index_tninfo);
  }
  Set_OP_opnd(op, index_opnd, additive_index_tn);
  if (EBO_in_loop) {
    Set_OP_omega (op, index_opnd, (additive_index_tninfo != NULL) ? additive_index_tninfo->omega : 0);
  }
  Set_OP_opnd(op, index_opnd+1, new_tn);
  if (EBO_in_loop) {
    Set_OP_omega (op, index_opnd+1, 0);
  }
  opnd_tn[index_opnd] = additive_index_tn;
  opnd_tn[index_opnd+1] = new_tn;
  opnd_tninfo[index_opnd] = additive_index_tninfo;
  opnd_tninfo[index_opnd+1] = NULL;
  actual_tninfo[index_opnd] = additive_index_tninfo;
  actual_tninfo[index_opnd+1] = NULL;

  if (EBO_Trace_Optimization) {
    #pragma mips_frequency_hint NEVER
    fprintf(TFile, "  to produce the new expression:\n");
    Print_OP_No_SrcLine(op);
  }

}




/* 
 * find_duplicate_mem_op
 *
 * For a given memory op, look for a preceeding memory op to 
 * the same location and attempt to replace one of them.
 * Return TRUE if this memory op is no longer needed.
 */
static BOOL
find_duplicate_mem_op (BB *bb,
                       OP *op,
                       TN **opnd_tn,
                       EBO_TN_INFO **opnd_tninfo,
                       EBO_TN_INFO **actual_tninfo,
		       int op_num,
		       int *regs_used,
		       int *last_fat_point)
/* -----------------------------------------------------------------------
 * Requires: 
 * Returns TRUE if the operands of each OP are identical.
 * -----------------------------------------------------------------------
 */
{
  INT hash_value = 0;
  INT hash_search_length = 0;
  EBO_OP_INFO *opinfo;
  EBO_OP_INFO *intervening_opinfo = NULL;
  EBO_OP_INFO *adjacent_location = NULL;
  INT64 adjacent_offset_pred;
  INT64 adjacent_offset_succ;
#ifdef KEY
  // TRUE if OP is inside the live range of the replacement TN.
  BOOL inside_lr = TRUE;
  // If extending the live range of the replacement TN, this is the replacement
  // TN's tninfo.
  EBO_TN_INFO *extend_lr_tninfo = NULL;
  EBO_REG_ENTRY reginfo;
#endif

  if (EBO_in_loop) {
    if ((CG_LOOP_unroll_level == 2) && (Is_Multibb_Loop(EBO_loop))) {
      // It is possible to have no loop info on the unrolled multi block loop
      if (_CG_LOOP_info(op) == NULL)
        return FALSE;
    }
  }

  if (op == NULL) return FALSE;

#ifdef TARG_X8664
  // Load-exe-store is always needed because it modifies memory.  Bug 15061.
  if (OP_load_exe_store(op))
    return FALSE;
#endif

  if (EBO_Trace_Execution) {
    #pragma mips_frequency_hint NEVER
    fprintf(TFile,"%sEnter find_duplicate_mem_op\n",EBO_trace_pfx);
  }

#ifdef KEY
  // Take OP_prefetch into account.
  if( OP_prefetch(op) ){
    return Combine_L1_L2_Prefetches( op, opnd_tn, opnd_tninfo );
  }
#endif

  if (!(OP_load(op) || OP_store(op))) return FALSE;

 /* Determine the indexes of the address components of this memory op. */
  INT succ_base_idx = TOP_Find_Operand_Use(OP_code(op),OU_base);
  INT succ_offset_idx = TOP_Find_Operand_Use(OP_code(op),OU_offset);

  if ((succ_base_idx >= 0) && (succ_offset_idx >= 0) &&
      TN_Is_Constant(opnd_tn[succ_offset_idx])) {
   /* Look for merge-able expressions. */
    merge_memory_offsets (op, succ_base_idx, opnd_tn, actual_tninfo, opnd_tninfo);
  }

 /* Determine the address components of this memory op. */
  TN *succ_base_tn = (succ_base_idx >= 0) ? opnd_tn[succ_base_idx] : NULL;
  EBO_TN_INFO *succ_base_tninfo = (succ_base_idx >= 0) ? opnd_tninfo[succ_base_idx] : NULL;
  TN *succ_offset_tn = (succ_offset_idx >= 0) ? opnd_tn[succ_offset_idx] : NULL;
  EBO_TN_INFO *succ_offset_tninfo = (succ_offset_idx >= 0) ? opnd_tninfo[succ_offset_idx] : NULL;

#ifdef KEY
  TN *succ_index_tn = NULL;
  TN *succ_scale_tn = NULL;
  EBO_TN_INFO *succ_index_tninfo = NULL;
  EBO_TN_INFO *succ_scale_tninfo = NULL;
#endif

#if defined(TARG_X8664) || defined(TARG_MIPS)
  const INT succ_index_idx = TOP_Find_Operand_Use(OP_code(op),OU_index);
  succ_index_tn = (succ_index_idx >= 0) ? opnd_tn[succ_index_idx] : NULL;
  succ_index_tninfo = (succ_index_idx >= 0) ? opnd_tninfo[succ_index_idx] : NULL;
#endif

#ifdef TARG_X8664
  const INT succ_scale_idx = TOP_Find_Operand_Use(OP_code(op),OU_scale);
  succ_scale_tn = (succ_scale_idx >= 0) ? opnd_tn[succ_scale_idx] : NULL;
  succ_scale_tninfo = (succ_scale_idx >= 0) ? opnd_tninfo[succ_scale_idx] : NULL;

  // For bug 5809, don't CSE "xmm = mov offset(base)" because this increases
  // xmm registers pressure, causing LRA to spill a lot.
  if (CG_sse_load_execute != 0 &&
      OP_load(op) &&
      succ_index_idx == -1 &&
      TN_register_class(OP_result(op, 0)) == ISA_REGISTER_CLASS_float) {
    return FALSE;
  }
#endif

  if ((succ_offset_tn == NULL) && (succ_base_tn != NULL)) {
    find_index_and_offset(succ_base_tninfo,
                          &succ_base_tn, &succ_base_tninfo,
                          &succ_offset_tn, &succ_offset_tninfo);
  }

 /* Determine the proper hash value. */
  hash_value = EBO_hash_op( op, opnd_tninfo);

  if (EBO_Trace_Hash_Search) {
    #pragma mips_frequency_hint NEVER
    fprintf(TFile,"%sLook for redundant memory ops in hash chain %d for\n\t",
                  EBO_trace_pfx,hash_value);
    Print_OP_No_SrcLine(op);
  }

  opinfo = (OP_has_implicit_interactions(op) || OP_unalign_mem(op)) ? NULL : EBO_opinfo_table[hash_value];

  while (opinfo) {
    OP *pred_op = opinfo->in_op;
    INT64 offset_pred = 0;
    INT64 offset_succ = 0;

   /* Identify the address components of the predecessor memory op. */
    INT pred_base_idx = 0;
    INT pred_offset_idx = 0;
    TN *pred_base_tn = NULL;
    EBO_TN_INFO *pred_base_tninfo = NULL;
    TN *pred_offset_tn = NULL;
    EBO_TN_INFO *pred_offset_tninfo = NULL;
#ifdef KEY
    INT pred_index_idx = 0;
    TN* pred_index_tn = NULL;
    EBO_TN_INFO* pred_index_tninfo = NULL;
    INT pred_scale_idx = 0;
    TN* pred_scale_tn = NULL;
    EBO_TN_INFO* pred_scale_tninfo = NULL;
#endif

#ifdef KEY
    if (pred_op && OP_code(pred_op) == TOP_intrncall) break;  // 14955
#endif

#ifdef TARG_X8664
    // Load-exe-store is always needed because it modifies memory.
    // Furthermore, unlike load or store, load-exe-store does not have a
    // register copy of the memory value for reuse.  Bug 15061.
    if (pred_op &&
	OP_load_exe_store(pred_op))
      break;
#endif

    if ((pred_op != NULL) &&
        (OP_load(pred_op) || OP_store(pred_op)
#ifdef TARG_X8664
	 || OP_load_exe(pred_op)
#endif
	 )) {
     /* Determine the address components of the predecessor memory op. */
      pred_base_idx = TOP_Find_Operand_Use(OP_code(pred_op),OU_base);
      pred_offset_idx = TOP_Find_Operand_Use(OP_code(pred_op),OU_offset);
      pred_base_tn = (pred_base_idx >= 0) ? OP_opnd(pred_op,pred_base_idx) : NULL;
      pred_base_tninfo = (pred_base_idx >= 0) ? opinfo->optimal_opnd[pred_base_idx] : NULL;
      pred_offset_tn = (pred_offset_idx >= 0) ? OP_opnd(pred_op,pred_offset_idx) : NULL;
      pred_offset_tninfo = (pred_offset_idx >= 0) ? opinfo->optimal_opnd[pred_offset_idx] : NULL;
#if defined(TARG_X8664) || defined(TARG_MIPS)
      pred_index_idx = TOP_Find_Operand_Use(OP_code(pred_op),OU_index);
      pred_index_tn = (pred_index_idx >= 0) ?
			OP_opnd(pred_op,pred_index_idx) : NULL;
      pred_index_tninfo = (pred_index_idx >= 0) ?
			    opinfo->optimal_opnd[pred_index_idx] : NULL;
#endif
#ifdef TARG_X8664
      pred_scale_idx = TOP_Find_Operand_Use(OP_code(pred_op),OU_scale);
      pred_scale_tn = (pred_scale_idx >= 0) ?
		        OP_opnd(pred_op,pred_scale_idx) : NULL;
      pred_scale_tninfo = (pred_scale_idx >= 0) ?
			    opinfo->optimal_opnd[pred_scale_idx] : NULL;
#endif      
    }

    if ((pred_offset_tn == NULL) && (pred_base_tn != NULL)) {
      find_index_and_offset(pred_base_tninfo,
                            &pred_base_tn, &pred_base_tninfo,
                            &pred_offset_tn, &pred_offset_tninfo);
    }

    BOOL hash_op_matches = ((pred_op != NULL) &&
#ifdef TARG_X8664
                            (OP_memory(pred_op) || OP_load_exe(pred_op)) &&
#else
                            OP_memory(pred_op) &&
#endif
#ifdef TARG_X8664
			    (CGTARG_Is_Thread_Local_Memory_OP(op) ==
			     CGTARG_Is_Thread_Local_Memory_OP(pred_op)) &&
#endif
#ifdef KEY
			    (pred_index_tn == succ_index_tn)         &&
			    (pred_index_tninfo == succ_index_tninfo) &&
			    (pred_scale_tn == succ_scale_tn)         &&
			    (pred_scale_tninfo == succ_scale_tninfo) &&
#endif
                            (pred_base_tn == succ_base_tn) &&           /* The base  index must match */
                            (pred_base_tninfo == succ_base_tninfo) &&   /* The base   info must match */
                            (pred_offset_tninfo == succ_offset_tninfo)) /* The offset info must match */
                            ? TRUE : FALSE;
    BOOL op_is_subset = FALSE;
    BOOL offsets_may_overlap = TRUE;

    hash_search_length++;

    if (hash_op_matches &&
        (pred_offset_tn != succ_offset_tn)) {
     /* The offset tn's need to be looked at in more detail. */

      ST *symbol_pred = ((pred_offset_tn != NULL) && TN_is_symbol(pred_offset_tn)) ?TN_var(pred_offset_tn) : NULL;
      ST *symbol_succ = ((succ_offset_tn != NULL) && TN_is_symbol(succ_offset_tn)) ?TN_var(succ_offset_tn) : NULL;
      mUINT8 relocs_pred = (pred_offset_tn != NULL) ? TN_relocs(pred_offset_tn) : 0;
      mUINT8 relocs_succ = (succ_offset_tn != NULL) ? TN_relocs(succ_offset_tn) : 0;
      offset_pred = (pred_offset_tn != NULL) ? TN_offset(pred_offset_tn) : 0;
      offset_succ = (succ_offset_tn != NULL) ? TN_offset(succ_offset_tn) : 0;

     /* This time, the relocations must be the same. */
      hash_op_matches = (symbol_pred == symbol_succ) && (relocs_pred == relocs_succ);

      if ((OP_prefetch(op) || OP_prefetch(pred_op)) &&
          (offset_pred != offset_succ)) {
        hash_op_matches = FALSE;
      } else if (hash_op_matches) {
       /* If the relocations are the same, we need to examine the offsets and sizes. */
        INT size_pred = CGTARG_Mem_Ref_Bytes(pred_op);
        INT size_succ = CGTARG_Mem_Ref_Bytes(op);

        if ((offset_pred == offset_succ) &&
            (size_pred == size_succ)) {
         /* The perfect match: location and size. */
        }  else if ((offset_pred <= offset_succ) &&
                    ((offset_pred + size_pred) >= (offset_succ + size_succ))) {
         /* The current reference is a subset of the preceeding one. */
          op_is_subset = TRUE;
        } else if (OP_load(op) && OP_load(pred_op) && (size_pred == size_succ) &&
                   (((offset_pred + size_pred) == offset_succ) ||
                    ((offset_succ + size_succ) == offset_pred))) {
          offsets_may_overlap = FALSE;
          if (adjacent_location == NULL) {
            adjacent_location = opinfo;
            adjacent_offset_pred = offset_pred;
            adjacent_offset_succ = offset_succ;
          }
        } else if (((offset_pred + size_pred) <= offset_succ) ||
                   ((offset_succ + size_succ) <= offset_pred)) {
         /* There is no potential overlap. */
          offsets_may_overlap = FALSE;
        } else {
         /* Any other case may be a potential conflict. */
          hash_op_matches = FALSE;
        }
      }
    }

    if (hash_op_matches && !offsets_may_overlap ) {
      if (EBO_Trace_Hash_Search) {
        #pragma mips_frequency_hint NEVER
        fprintf(TFile,"%sMemory overlap not possible with\n\t",
                      EBO_trace_pfx);
        Print_OP_No_SrcLine(pred_op);
      }
      opinfo = opinfo->same;
      continue;
    }

    if (hash_op_matches && OP_has_predicate(op)) {
     /* Check predicates for safety. */

      if (OP_store(op) && OP_store(pred_op)) {
        if (!EBO_predicate_dominates(OP_opnd(op,OP_PREDICATE_OPND),
                                     actual_tninfo[OP_PREDICATE_OPND],
                                     OP_opnd(pred_op,OP_PREDICATE_OPND),
                                     opinfo->optimal_opnd[OP_PREDICATE_OPND]) &&
            !EBO_predicate_complements(OP_opnd(op,OP_PREDICATE_OPND),
                                       actual_tninfo[OP_PREDICATE_OPND],
                                       OP_opnd(pred_op,OP_PREDICATE_OPND),
                                       opinfo->optimal_opnd[OP_PREDICATE_OPND])) {

          if (EBO_Trace_Hash_Search) {
            #pragma mips_frequency_hint NEVER
            fprintf(TFile,"%sStore predicates do not match\n\t", EBO_trace_pfx);
            Print_OP_No_SrcLine(pred_op);
          }

         /* We need to be extra safe with stores.  Assume a conflict. */
          hash_op_matches = FALSE;
          break;
        }
      } else if (!OP_store(op) && !OP_store(pred_op)) {
        if ((intervening_opinfo != NULL) &&
            !EBO_predicate_dominates(OP_opnd(pred_op,OP_PREDICATE_OPND),
                                     opinfo->optimal_opnd[OP_PREDICATE_OPND],
                                     OP_opnd(op,OP_PREDICATE_OPND),
                                     actual_tninfo[OP_PREDICATE_OPND])) {

          if (EBO_Trace_Hash_Search) {
            #pragma mips_frequency_hint NEVER
            fprintf(TFile,"%sRe-load predicates do not match\n\t", EBO_trace_pfx);
            Print_OP_No_SrcLine(pred_op);
          }

         /* A mixture of predicates on the loads may cause problems. */
          hash_op_matches = FALSE;
          break;
        }
        if (!EBO_predicate_dominates(OP_opnd(pred_op,OP_PREDICATE_OPND),
                                     opinfo->optimal_opnd[OP_PREDICATE_OPND],
                                     OP_opnd(op,OP_PREDICATE_OPND),
                                     actual_tninfo[OP_PREDICATE_OPND]) &&
            !EBO_predicate_complements(OP_opnd(op,OP_PREDICATE_OPND),
                                       actual_tninfo[OP_PREDICATE_OPND],
                                       OP_opnd(pred_op,OP_PREDICATE_OPND),
                                       opinfo->optimal_opnd[OP_PREDICATE_OPND])) {

          if (EBO_Trace_Hash_Search) {
            #pragma mips_frequency_hint NEVER
            fprintf(TFile,"%sLoad predicates do not match\n\t", EBO_trace_pfx);
            Print_OP_No_SrcLine(pred_op);
          }

         /* We can't use this load, but we can keep looking. */
          hash_op_matches = FALSE;

        }
      } else if (!OP_store(op) && OP_store(pred_op)) {
        if (!EBO_predicate_dominates(OP_opnd(pred_op,OP_PREDICATE_OPND),
                                     opinfo->optimal_opnd[OP_PREDICATE_OPND],
                                     OP_opnd(op,OP_PREDICATE_OPND),
                                     actual_tninfo[OP_PREDICATE_OPND]) &&
            !EBO_predicate_complements(OP_opnd(op,OP_PREDICATE_OPND),
                                       actual_tninfo[OP_PREDICATE_OPND],
                                       OP_opnd(pred_op,OP_PREDICATE_OPND),
                                       opinfo->optimal_opnd[OP_PREDICATE_OPND])) {

          if (EBO_Trace_Hash_Search) {
            #pragma mips_frequency_hint NEVER
            fprintf(TFile,"%sStore/load predicates do not match\n\t", EBO_trace_pfx);
            Print_OP_No_SrcLine(pred_op);
          }

         /* We need to be extra safe with stores.  Assume a conflict. */
          hash_op_matches = FALSE;
          break;
        }
      }
    }

    if ((pred_op != NULL) &&
        (hash_value == EBO_DEFAULT_MEM_HASH) &&
        (OP_store(pred_op) != OP_store(op)) &&
        (!OP_prefetch(op))) {
     /* Need to be careful about alias issues. */
      WN *pred_wn;
      WN *succ_wn;
      ALIAS_RESULT result;

      if (EBO_Trace_Hash_Search) {
        #pragma mips_frequency_hint NEVER
        fprintf(TFile,"%sPotential Alias problem with\n\t",EBO_trace_pfx);
        Print_OP_No_SrcLine(pred_op);
      }

      result = POSSIBLY_ALIASED;
      if (Alias_Manager != NULL) {
        pred_wn = OP_hoisted(pred_op) ? NULL : Get_WN_From_Memory_OP(pred_op);
        succ_wn = OP_hoisted(op) ? NULL : Get_WN_From_Memory_OP(op);
        if ((pred_wn != NULL) && (succ_wn != NULL)) {
          result = Aliased(Alias_Manager, pred_wn, succ_wn);
          if ((!hash_op_matches) && (result == SAME_LOCATION)) {
           /* This also implies that the size of the items is the same. */
            hash_op_matches = TRUE;
            if (EBO_Trace_Hash_Search) {
              #pragma mips_frequency_hint NEVER
              fprintf(TFile,"%sAlias_Manager identifies SAME_LOCATION\n",
                             EBO_trace_pfx);    
            }
           /* The Alias_Manager may think that the locations are the same,
              but we know that they may not be.  This is because if-conversion
              may create something called a "black hole".  It is hard to believe
              that the optimizations we do will be OK in this situation. */
            if (OP_store(op)) opinfo->op_must_not_be_moved = TRUE;
            break;
          }
        }
      }

      if ((result == POSSIBLY_ALIASED) && (!hash_op_matches)) {
        if ((intervening_opinfo == NULL) && OP_store(pred_op) && OP_load(op)) {
          intervening_opinfo = opinfo;
          if (EBO_Trace_Hash_Search) {
            #pragma mips_frequency_hint NEVER
            fprintf(TFile,"%sContinue looking for duplicate memory op across interving dependency %d.\n",
                          EBO_trace_pfx,result);
          }
          goto do_next;
        }
        if (OP_store(pred_op)) opinfo->op_must_not_be_removed = TRUE;

        if (EBO_Trace_Hash_Search) {
          #pragma mips_frequency_hint NEVER
          fprintf(TFile,"%sAlias information(%d) prevents us from continuing.\n",
                        EBO_trace_pfx,result);
        }
        break;
      }
      if (hash_op_matches && op_is_subset &&
          OP_store(op) && OP_store(pred_op)) {
        opinfo->op_must_not_be_removed = TRUE;
        if (EBO_Trace_Hash_Search) {
          #pragma mips_frequency_hint NEVER
          fprintf(TFile,"%sOverlapping store prevents us from continuing.\n",
                        EBO_trace_pfx);
        }
        break;
      }
      if ((intervening_opinfo != NULL) && (!hash_op_matches)) {
        opinfo->op_must_not_be_removed = TRUE;
        if (EBO_Trace_Hash_Search) {
          #pragma mips_frequency_hint NEVER
          fprintf(TFile,"%sMultiple possible intervening stores prevents us from continuing.\n",
                        EBO_trace_pfx);
        }
        break;
      }

      if (EBO_Trace_Hash_Search) {
        #pragma mips_frequency_hint NEVER
        fprintf(TFile,"%sAlias information allows us to continue.\n",EBO_trace_pfx);
      }
    }

    if (in_delay_slot && !OP_store(op)) {
     /* In general, we can't remove an OP from a delay slot, but
        duplicate stores are removed by noop'ing the pred_op. */
      break;
    }

    if (hash_op_matches && 
        (pred_op != NULL) &&
        (OP_load(pred_op) ||
         (OP_store(pred_op) && !OP_store(op)))) {

      if (OP_store(pred_op)) {
        INT pred_stored_idx = TOP_Find_Operand_Use(OP_code(pred_op),OU_storeval);
        TN *pred_tn = OP_opnd(pred_op,pred_stored_idx);
        if (!TN_Is_Constant(pred_tn)) {
         /* The stored register needs to be available at this point. */
          if (!EBO_tn_available(bb,opinfo->actual_opnd[pred_stored_idx])) {
            opinfo->op_must_not_be_removed = TRUE;
            if (EBO_Trace_Hash_Search) {
              #pragma mips_frequency_hint NEVER
              fprintf(TFile,"%sMemory match found, but stored value is not available\n\t",
                            EBO_trace_pfx);
              Print_OP_No_SrcLine(pred_op);
            }
            break;
          }
#ifdef KEY
	  // See if a register is available to hold the stored value.
	  EBO_REG_ENTRY_ptr(reginfo) = hTN_MAP_Get(regs_map, pred_tn);
	  if (!TN_is_global_reg(pred_tn) &&
	      // OP is not inside the live range of the replacement TN?
	      !(inside_lr = (EBO_REG_ENTRY_last_use_OP_num(reginfo)>=op_num)) &&
	      // No register available to hold the loaded value?
	      opinfo->op_num < last_fat_point[TN_register_class(pred_tn)]) {
            if (EBO_Trace_Hash_Search) {
              #pragma mips_frequency_hint NEVER
              fprintf(TFile,"%sMemory match found, but no register available to keep stored value\n\t",
                            EBO_trace_pfx);
              Print_OP_No_SrcLine(pred_op);
            }
            break;
	  }
	  if (!inside_lr)
	    extend_lr_tninfo = opinfo->actual_opnd[pred_stored_idx];
#endif
        }
      } else {
        TN *pred_tn = OP_result(pred_op,0);
        if (!TN_Is_Constant(pred_tn)) {
         /* The previous result needs to be available at this point. */
          if (!EBO_tn_available(bb,opinfo->actual_rslt[0])) {
            if (EBO_Trace_Hash_Search) {
              #pragma mips_frequency_hint NEVER
              fprintf(TFile,"%sMemory match found, but loaded value is not available\n\t",
                            EBO_trace_pfx);
              Print_OP_No_SrcLine(pred_op);
            }
            break;
          }
#ifdef KEY
	  // See if a register is available to hold the loaded value.
	  EBO_REG_ENTRY_ptr(reginfo) = hTN_MAP_Get(regs_map, pred_tn);
	  if (!TN_is_global_reg(pred_tn) &&
	      // OP is not inside the live range of the replacement TN?
	      !(inside_lr = (EBO_REG_ENTRY_last_use_OP_num(reginfo)>=op_num)) &&
	      // No register available to hold the loaded value?
	      opinfo->op_num < last_fat_point[TN_register_class(pred_tn)]) {
            if (EBO_Trace_Hash_Search) {
              #pragma mips_frequency_hint NEVER
              fprintf(TFile,"%sMemory match found, but no register available to keep loaded value\n\t",
                            EBO_trace_pfx);
              Print_OP_No_SrcLine(pred_op);
            }
            break;
	  }
	  if (!inside_lr)
	    extend_lr_tninfo = opinfo->actual_rslt[0];
#endif
        }
      }
    }

    if (hash_op_matches) {
      BOOL op_replaced = FALSE;

      if (OP_volatile(pred_op)) {
       /* If we match a volatile memory op, this
          one should have been volatile, too. */
        break;
      }

      if (EBO_Trace_Hash_Search) {
        #pragma mips_frequency_hint NEVER
        fprintf(TFile,"%sMatch after searching %d items on hash chain %d\n",
                      EBO_trace_pfx,hash_search_length,hash_value);
      }

      if (intervening_opinfo != NULL) {
        op_replaced = delete_reload_across_dependency (op, opnd_tninfo, opinfo, intervening_opinfo);
      } else if (op_is_subset) {
        op_replaced = delete_subset_mem_op (op, opnd_tninfo, opinfo, offset_pred, offset_succ);
      } else {
        op_replaced = delete_duplicate_op (op, opnd_tninfo, opinfo
#ifdef TARG_X8664
					   , actual_tninfo
#endif
					  );
      }

      if (op_replaced) {
#ifdef KEY
	if (extend_lr_tninfo != NULL)
	  Extend_Live_Range(regs_used, last_fat_point, op_num, extend_lr_tninfo,
			    reginfo, bb);
#endif
        return TRUE;
      } else {
       /* If we matched once and failed to eliminate it,
          we may need to keep both around. */
        if (intervening_opinfo != NULL) {
          opinfo = intervening_opinfo;
          pred_op = intervening_opinfo->in_op;
        }
        if (OP_store(op)) opinfo->op_must_not_be_moved = TRUE;
        if (op_is_subset || (hash_value == EBO_DEFAULT_MEM_HASH)) {
          if (OP_store(pred_op)) opinfo->op_must_not_be_removed = TRUE;
          break;
        }
        if (OP_store(pred_op) || OP_store(op)) {
          break;
        }
      }
    }

do_next:
  opinfo = opinfo->same;
  }

  if (adjacent_location != NULL) {
    BOOL op_replaced = combine_adjacent_loads (op, opnd_tninfo, adjacent_location,
                                               adjacent_offset_pred, adjacent_offset_succ);
    if (op_replaced) {
      return TRUE;
    }
  }

  if (EBO_Trace_Hash_Search) {
    #pragma mips_frequency_hint NEVER
    fprintf(TFile,"%sFail after searching %d items on memory hash chain %d\n",
                  EBO_trace_pfx,hash_search_length,hash_value);
  }

  return FALSE;
}




/* 
 * find_duplicate_op
 *
 * For a given expression op, look for a preceeding indentical
 * expressionn and attempt to replace the new one.
 * Return TRUE if this expression is no longer needed.
 */
static BOOL
find_duplicate_op (BB *bb,
                   OP *op,
                   TN **opnd_tn,
                   EBO_TN_INFO **opnd_tninfo,
                   EBO_TN_INFO **actual_tninfo)
/* -----------------------------------------------------------------------
 * Requires: 
 * Returns TRUE if the operands of each OP are identical.
 * -----------------------------------------------------------------------
 */
{
  INT opcount;
  INT opndnum;
  INT hash_value = 0;
  INT hash_search_length = 0;
  EBO_OP_INFO *opinfo;
  BOOL hash_op_matches = FALSE;

  if (op == NULL) return FALSE;

  if (EBO_Trace_Execution) {
    #pragma mips_frequency_hint NEVER
    fprintf(TFile,"%sEnter find_duplicate_op\n",EBO_trace_pfx);
  }
  
  opcount = OP_opnds(op);

  if (OP_memory(op)) return FALSE;

#ifdef TARG_X8664
  if( OP_load_exe(op) )    return FALSE;

  if( TOP_is_change_rflags( OP_code(op) ) ){
    for( OP* next_op = OP_next(op); next_op != NULL; next_op = OP_next( next_op ) ){
      if( OP_reads_rflags( next_op ) )
	return FALSE;

      if( TOP_is_change_rflags( OP_code(next_op) ) )
	break;
    }
  }
#endif

  if (!OP_results(op)) return FALSE;

  /* Bug#459.
     We cannot replace a "duplicated" cond_def operation by
     a mov operation without knowing the condition.
   */
#if defined(TARG_MIPS) || defined(TARG_X8664)
  // For MIPS, we can not eliminate duplicate conditionally defined ops
  // Example:
  //   TNa :- ld TNz($sp)
  //   TNa :- movn TNy1 TNz
  //   TNa :- movz TNx TNz    // pred_op
  // and,
  //   TNb :- ld TNz($sp)
  //   TNb :- movn TNy2 TNz
  //   TNb :- movz TNx TNz    // op
  // op can not be eliminated by using pred_op
  if (OP_cond_def(op)) return FALSE;
#endif
 /* Compute a hash value for the OP. */
  hash_value = EBO_hash_op( op, opnd_tninfo);

  if (EBO_Trace_Hash_Search) {
    #pragma mips_frequency_hint NEVER
    fprintf(TFile,"%sLook for duplicates at hash value %d of\n\t",EBO_trace_pfx,hash_value);
    Print_OP_No_SrcLine(op);
  }

#ifdef TARG_X8664
  opinfo = (opcount == 0 && !EBO_Can_Eliminate_Zero_Opnd_OP(op)) ?
	      NULL : EBO_opinfo_table[hash_value];
#else
  opinfo = (opcount == 0) ? NULL : EBO_opinfo_table[hash_value];
#endif

  while (opinfo) {
    OP *pred_op = opinfo->in_op;

    hash_search_length++;
    hash_op_matches =    (pred_op != NULL)
#ifdef KEY
      /* bug#480
	 Do not consider a wild op whose owner bb is unknown.
	 TODO:
	 Routines inside ebo_special.cxx should not call BB_Remove_Op() directly.
	 Try using remove_op() and OP_Change_To_Noop() instead.
       */
                      && OP_bb(pred_op) != NULL
#endif
                      && (OP_results(op) == OP_results(pred_op))
		      && (OP_opnds(op) == OP_opnds(pred_op))
		      && (OP_results(op) == 2 || OP_code(op) == OP_code(pred_op));

    if (hash_op_matches) {

      for (opndnum = 0; opndnum < opcount; opndnum++) {
        if (OP_has_predicate(op) && (opndnum == OP_PREDICATE_OPND)) {
         /* Check predicates later. */
          continue;
        }
        if (opinfo->optimal_opnd[opndnum] == opnd_tninfo[opndnum]) {
          if (!TN_Is_Constant(opnd_tn[opndnum])) {
           /* If operands are not constants and the tninfo_entries match,
              then the values represented must also be identical. */
            continue;
          }

         /* Items that are constant (i.e. have a NULL tninfo_entry pointer)
            must be checked to verify that the constants are the same.
            Note that there are several "reasonable" combinations that
            can come up:
              1. The constants have identical TNs.
              2. The OPs have identical TNs and the predecessor hasn't changed.
              3. The TNs have been resolved to the same constant, but the
                 operand of the OP (for some reason) could not be changed to
                 reference a constant.
         */
          if (/* case 1 */(opnd_tn[opndnum] == OP_opnd(pred_op, opndnum)) ||
              /* case 2 */((OP_opnd(op, opndnum) == OP_opnd(pred_op, opndnum)) &&
                           EBO_tn_available(bb, opinfo->actual_opnd[opndnum])) ||
              /* case 3 */((opinfo->actual_opnd[opndnum] != NULL) && 
                           (opnd_tn[opndnum] == opinfo->actual_opnd[opndnum]->replacement_tn))) {
            continue;
          }
        }

       /* Operands don't match - get out of inner loop and try next OP. */
        hash_op_matches = FALSE;
        break;
      }

    }

    if (hash_op_matches && 
        (pred_op != NULL)) {
      int resnum;

      for (resnum = 0; resnum < OP_results(op); resnum++) {
       /* All of the results need to be available at this point. */
        if (!TN_is_const_reg(OP_result(op,resnum)) &&
            !EBO_tn_available(bb,opinfo->actual_rslt[resnum])) {

          if (EBO_Trace_Hash_Search) {
            #pragma mips_frequency_hint NEVER
            fprintf(TFile,"%sExpression match found, but the result[%d] value is not available\n\t",
                           EBO_trace_pfx,resnum);
            Print_OP_No_SrcLine(pred_op);
          }

          hash_op_matches = FALSE;
          break;
        }
      }

    }

    if (in_delay_slot) {
     /* We can't insert or remove items from the delay slot. */
      break;
    }

    if (hash_op_matches && OP_has_predicate(op)) {
     /* Check predicates for safety. */

        if (!EBO_predicate_dominates(OP_opnd(pred_op,OP_PREDICATE_OPND),
                                     opinfo->optimal_opnd[OP_PREDICATE_OPND],
                                     OP_opnd(op,OP_PREDICATE_OPND),
                                     actual_tninfo[OP_PREDICATE_OPND]) &&
            !EBO_predicate_complements(OP_opnd(pred_op,OP_PREDICATE_OPND),
                                       opinfo->optimal_opnd[OP_PREDICATE_OPND],
                                       OP_opnd(op,OP_PREDICATE_OPND),
                                       actual_tninfo[OP_PREDICATE_OPND])) {
          hash_op_matches = FALSE;

          if (EBO_Trace_Hash_Search) {
            #pragma mips_frequency_hint NEVER
            fprintf(TFile,"%sExpression match found, but the predicates do not match\n\t",
                           EBO_trace_pfx);
            Print_OP_No_SrcLine(pred_op);
          }

        }
    }

    if (hash_op_matches) {

      if (EBO_Trace_Hash_Search) {
        #pragma mips_frequency_hint NEVER
        fprintf(TFile,"%sMatch after searching %d items on expression hash chain %d\n",
                      EBO_trace_pfx,hash_search_length,hash_value);
      }

      if (delete_duplicate_op (op, opnd_tninfo, opinfo)) {
        return TRUE;
      } else {
       /* If we matched once and failed to eliminate it,
          we need to keep both around. */
        break;
      }
    }

  opinfo = opinfo->same;
  }

  if (EBO_Trace_Hash_Search) {
    #pragma mips_frequency_hint NEVER
    fprintf(TFile,"%sFail after searching %d items on hash chain %d\n",
                  EBO_trace_pfx,hash_search_length,hash_value);
  }

  return FALSE;
}




/* 
 * find_previous_constant
 *
 * For a given expression op, look for a preceeding indentical
 * expressionn and attempt to replace the new one.
 * Return TRUE if this expression is no longer needed.
 */
static BOOL
find_previous_constant (OP *op,
                        EBO_TN_INFO **actual_tninfo)
{
  TN *const_tn = OP_opnd(op,copy_operand(op));
  EBO_TN_INFO *predicate_tninfo = (OP_has_predicate(op)?actual_tninfo[OP_PREDICATE_OPND]:NULL);
  EBO_TN_INFO *check_tninfo;

  if (!TN_is_constant(const_tn)) return FALSE;
  if (TN_is_const_reg(const_tn)) return FALSE;
  if (OP_cond_def(op)) return FALSE;

  if (EBO_Trace_Execution) {
    #pragma mips_frequency_hint NEVER
    fprintf(TFile,"%sEnter find_previous_constant %d\n",
                  EBO_trace_pfx,(INT32)TN_value(const_tn));
  }

  for (check_tninfo = EBO_last_tninfo;
       check_tninfo != NULL;
       check_tninfo = check_tninfo->prior) {
    if (check_tninfo->replacement_tn == const_tn) {
       /* The asigned register needs to be available at this point. */
      TN *pred_tn = check_tninfo->local_tn;
      OP *pred_op = check_tninfo->in_op;

      if (TN_register_class(OP_result(op, 0)) != TN_register_class(pred_tn)) {
        continue;
      }
      if ((pred_op != NULL) && OP_has_predicate(op) && OP_has_predicate(pred_op)) {
       /* Check predicates for safety. */
        EBO_OP_INFO *opinfo = locate_opinfo_entry(check_tninfo);
        if ((opinfo == NULL) ||
            !EBO_predicate_dominates(OP_opnd(pred_op,OP_PREDICATE_OPND),
                                     opinfo->optimal_opnd[OP_PREDICATE_OPND],
                                     OP_opnd(op,OP_PREDICATE_OPND),
                                     predicate_tninfo)) {
          if ((OP_code(op) == OP_code(pred_op)) &&
              EBO_predicate_complements(OP_opnd(pred_op,OP_PREDICATE_OPND),
                                     opinfo->optimal_opnd[OP_PREDICATE_OPND],
                                     OP_opnd(op,OP_PREDICATE_OPND),
                                     predicate_tninfo) &&
              delete_duplicate_op (op, actual_tninfo, opinfo)) {
            return TRUE;
          }

         /* This previous definition is not always available, keep looking. */
          continue;
        }
      }
      if (EBO_tn_available(OP_bb(op),check_tninfo) &&
          (TN_is_rematerializable(pred_tn))) {
          OPS ops = OPS_EMPTY;

          EBO_Exp_COPY((OP_has_predicate(op)?OP_opnd(op,OP_PREDICATE_OPND):NULL),
                       OP_result(op, 0), pred_tn, &ops);
          if (EBO_in_loop) {
            CG_LOOP_Init_Op(OPS_first(&ops));
            Set_OP_omega (OPS_first(&ops),
                          OP_PREDICATE_OPND,
                          (predicate_tninfo != NULL)?predicate_tninfo->omega:0);
          }
          OP_srcpos(OPS_first(&ops)) = OP_srcpos(op);
          BB_Insert_Ops(OP_bb(op), op, &ops, FALSE);
#ifdef TARG_X8664
	  // On x86, "tn = ld const" can be deleted by folding the const into
	  // the OP that uses the const.  Since the new copy uses the TN, make
	  // sure the TN is defined.
	  inc_ref_count(check_tninfo);
#endif

          if (EBO_Trace_Optimization) {
            #pragma mips_frequency_hint NEVER
            fprintf(TFile,"%sAvoid reloading constant ",EBO_trace_pfx);
            Print_TN(const_tn,FALSE);
            fprintf(TFile," into ");
            Print_TN(OP_result(op, 0),FALSE);
            fprintf(TFile,"\n");
          }
        return TRUE;
      }
      return FALSE;
    }
  }

  return FALSE;
}


/*
 * Check if a load/cmp load exec peep is flow safe
 */
static BOOL
EBO_Load_Flow_Safe(OP *ld_op, BB* ld_bb, BB *cmp_bb)
{
  TN *ld_tn = OP_result(ld_op,0);
  BB *other_bb = NULL;
  BB *search_bb = NULL;
  BBLIST *lst, *slst;
  BOOL flow_safe = FALSE;
  int ld_bb_succ_len = BB_succs_len(ld_bb);
  int cmp_bb_pred_len = BB_preds_len(cmp_bb);

  // exclude handlers
  if (BB_handler(ld_bb) || BB_handler(cmp_bb))
    return FALSE;

  // Since we are motioning a load, no calls may be present
  // in our analysis pass of the related flow, sides effects can
  // exist where the memory location is changed.
  if (BB_call(ld_bb) || BB_call(cmp_bb))
    return FALSE;

  // We are safe if both ops are in the same block and ld_tn does not escape
  if (ld_bb == cmp_bb)
    return (TN_is_global_reg(ld_tn) == FALSE);

  // Only evaluate global tn candidates
  if (TN_is_global_reg(ld_tn) == FALSE)
    return FALSE;

  // Only 1 pred block allowed from cmp block, this ensures
  // nobody else is providing a value for the global ld_tn.
  if (cmp_bb_pred_len != 1)
    return FALSE;
  
  // Now examine the flow we have
  for (lst = BB_preds(cmp_bb); lst != NULL; lst = BBLIST_next(lst)) {
    BB *bb = BBLIST_item(lst);
    if (bb != ld_bb) {
      // iff a single block exists between the ld_bb and the cmp_bb
      // with nothing but ft flow from the ld_bb to it.
      if (ld_bb_succ_len == 1) {
        slst = BB_succs(bb);
        if (BBLIST_item(slst) != cmp_bb) {
          return FALSE;
        } else {
          search_bb = bb;
          flow_safe = TRUE;
        }
      }
    } else {
      flow_safe = TRUE;
      search_bb = ld_bb;
    }
  }
 
  // presence of handlers or calls means we cannot proceed
  if (search_bb && (BB_handler(search_bb) || BB_call(search_bb)))
    return FALSE;

  if (flow_safe == FALSE)
    return FALSE;

  // check for simple flow
  for (lst = BB_succs(search_bb); lst != NULL; lst = BBLIST_next(lst)) {
    BB *bb = BBLIST_item(lst);
    if (bb != cmp_bb)
      other_bb = bb;
  }

  // if only the cmp_bb exists as a successor ld_bb, we need only
  // check if our ld_tn is live out.
  if (other_bb == NULL) {
    // If the tn is live out of the cmp_bb we must fail also
    if (GRA_LIVE_TN_Live_Outof_BB(ld_tn, cmp_bb))
      return FALSE;
    else
      return TRUE;
  }

#ifdef KEY
  if (EBO_no_liveness_info_available)
    return FALSE;
#endif

  // If the tn is live out of the cmp_bb we must fail also
  if (GRA_LIVE_TN_Live_Outof_BB(ld_tn, cmp_bb))
    return FALSE;

  // The final constraint is that the load's tn must not be live in here
  return (GRA_LIVE_TN_Live_Into_BB(ld_tn, other_bb) == FALSE);
}


/* 
 * Iterate through a Basic Block and build EBO_TN_INFO entries.
 */
static BOOL
Find_BB_TNs (BB *bb)
{
  OP *op;
  BOOL no_barriers_encountered = TRUE;
  BOOL scalar_replacement_performed = FALSE;
#ifdef KEY
  OP *op_with_reg_usage_info, *next_op_with_reg_usage_info;
  INT op_count = 0;
#endif

#ifdef TARG_X8664
  if (EBO_in_loop) {
    if ((CG_LOOP_unroll_level == 2) && (Is_Multibb_Loop(EBO_loop))) {
      // Only process blocks in our loop
      if (BB_SET_MemberP(LOOP_DESCR_bbset(EBO_loop),bb) == FALSE)
        return no_barriers_encountered;
    }
  }
#endif

  if (EBO_Trace_Execution) {
    #pragma mips_frequency_hint NEVER
    fprintf(TFile,"%sEnter Find_BB_TNs BB:%d%s\n",
            EBO_trace_pfx,BB_id(bb),EBO_in_peep?" - peep ":" ");
    Print_BB(bb);
  }

#ifdef KEY
  Estimate_Reg_Usage(bb, &MEM_local_pool);
#endif

#ifdef TARG_X8664
  // Delete dead LRA spills.
  BS *LRA_spilled_value_is_used = NULL;
  if (EBO_in_peep) {
    Init_Remove_Dead_LRA_Stores(&LRA_spilled_value_is_used, &MEM_local_pool);
  }

  // x87
  BOOL in_x87_state = FALSE;
  OP *maybe_redundant_EMMS_OP = NULL;

#ifdef TARG_X8664
  BOOL do_load_execute;
  if (CG_cmp_load_exec)
    do_load_execute = ( CG_load_execute > 0 ) && !EBO_in_loop;
  else
    do_load_execute = ( CG_load_execute > 0 ) && !EBO_in_pre && !EBO_in_loop;
#else
  BOOL do_load_execute = ( CG_load_execute > 0 ) && !EBO_in_pre && !EBO_in_loop;
#endif

  if( do_load_execute ){
    Init_Load_Exec_Map( bb, &MEM_local_pool );
  }
#endif

 /* Allocate the dynamic arrays for various operand info. The minimum
    size we allocate is large enough for all OPs with a fixed number
    of operands. If we just allocated the size based on the BB OP with the
    greatest number of operands, then a transformation could produce
    an OP with more operands. It is expected that these new OPs will 
    always have fixed operands, which is why we use OP_MAX_FIXED_OPNDS
    (we of course verify this assumption). */
  INT max_opnds = OP_MAX_FIXED_OPNDS;
  FOR_ALL_BB_OPs (bb, op) {
    INT nopnds = OP_opnds(op);
    if (nopnds > max_opnds) max_opnds = nopnds;
#ifdef KEY
    op_count++;
#endif
  }
  TN **opnd_tn = TYPE_ALLOCA_N(TN *, max_opnds);
  EBO_TN_INFO **opnd_tninfo = TYPE_ALLOCA_N(EBO_TN_INFO *, max_opnds);
  EBO_TN_INFO **orig_tninfo = TYPE_ALLOCA_N(EBO_TN_INFO *, max_opnds);

  in_delay_slot = FALSE;

#ifdef KEY
  int op_num = 0;
  int *regs_used;
  // For CSE purposes, a fat point is where register demand exceeds the number
  // of registers available.  It is represented as a OP number.
  int last_fat_point[ISA_REGISTER_CLASS_MAX+1];
  ISA_REGISTER_CLASS cl;

  // Initialize register usage and fat points.
  regs_used = (int*)alloca((ISA_REGISTER_CLASS_MAX+1)*(op_count+1)*sizeof(int));
  memset (regs_used, 0,
	  (ISA_REGISTER_CLASS_MAX+1) * (op_count+1) * sizeof(int));
  FOR_ALL_ISA_REGISTER_CLASS(cl) {
    last_fat_point[cl] = 0;
  }

  // Identify the next OP that has register usage info generated by
  // Estimate_Reg_Usage.  EBO opts may create replacement OPs, whose register
  // info we haven't compute.
  op_with_reg_usage_info = BB_first_op(bb);
#endif

   FOR_ALL_BB_OPs (bb, op) {
    TN *tn;
    INT opndnum;
    INT resnum;
    EBO_TN_INFO *tninfo;
    TN *tn_replace;
    INT num_opnds = OP_opnds(op);
    TN *rslt_tn = NULL;
    INT rslt_num = 0;
    INT cmp_merge_idx = -1;
    BOOL opnds_constant = TRUE;
    BOOL op_replaced = FALSE;
    BOOL op_is_predicated = OP_has_predicate(op)?TRUE:FALSE;
    BOOL op_is_cmp_merge_cand = FALSE;
    TN *op_predicate_tn = NULL;
    EBO_TN_INFO *op_predicate_tninfo = NULL;
    BOOL check_omegas = (EBO_in_loop && _CG_LOOP_info(op))?TRUE:FALSE;

   /* The assumption is that this can never occur, but make sure it doesn't! */
    FmtAssert(num_opnds <= max_opnds, ("dynamic array allocation was too small!"));

#ifdef KEY
    // Must sync op_num with the op_num in Estimate_Reg_Usage.
    if (op == op_with_reg_usage_info) {
      op_num++;
      next_op_with_reg_usage_info = OP_next(op);
    }
#endif

    if (CGTARG_Is_OP_Barrier(op) || OP_access_reg_bank(op)) {
      if (Special_Sequence(op, NULL, NULL)) {
       /* We were able to restrict propagation of the specific registers. */
        if (EBO_Trace_Execution) {
          #pragma mips_frequency_hint NEVER
          fprintf(TFile,"%sWe were able to restrict propagation of the specific registers in BB:%d\t",
                  EBO_trace_pfx,BB_id(OP_bb(op)));
          Print_OP_No_SrcLine(op);
        }  
      } else {
       /* We could not identify the specific registers involved. */
        if (EBO_Trace_Execution) {
          #pragma mips_frequency_hint NEVER
          fprintf(TFile,"%sFind_BB_TNs Barrier OP encountered\t",EBO_trace_pfx);
          Print_OP_No_SrcLine(op);
        }
        no_barriers_encountered = FALSE;
      }
    }

#ifdef TARG_X8664
    // Remove unnecessary EMMS OPs.  An EMMS is unnecessary if the x87/MMX
    // registers are already in the x87 state (as opposed to MMX state), or if
    // the next OP is an MMX OP (which clobbers the x87 state).
    if (OP_code(op) == TOP_emms) {
      if (in_x87_state)
	OP_Change_To_Noop(op);
      else
	maybe_redundant_EMMS_OP = op;
      in_x87_state = TRUE;
    } else if (OP_x87(op)) {
      in_x87_state = TRUE;
      maybe_redundant_EMMS_OP = NULL;
    } else if (OP_mmx(op)) {
      if (maybe_redundant_EMMS_OP != NULL) {
	OP_Change_To_Noop(maybe_redundant_EMMS_OP);
	maybe_redundant_EMMS_OP = NULL;
      }
      in_x87_state = FALSE;
    }
#endif

    if ((num_opnds == 0) && (OP_results(op) == 0))
#ifdef KEY
      goto finish;
#else
      continue;
#endif

    if (EBO_Trace_Data_Flow) {
      fprintf(TFile,"%sProcess OP\n\t",EBO_trace_pfx); Print_OP_No_SrcLine(op);
    }

   /* Process all the operand TNs. */
    for (opndnum = 0; opndnum < num_opnds; opndnum++) {
      opnd_tn[opndnum] = NULL;
      opnd_tninfo[opndnum] = NULL;
      orig_tninfo[opndnum] = NULL;
    }

    for (opndnum = 0; opndnum < num_opnds; opndnum++) {
      BOOL replace_result = FALSE;
      mUINT8 operand_omega = 0;
#ifdef KEY
      BOOL dont_replace = FALSE;      
#endif

      tn = OP_opnd(op, opndnum);
      tninfo = NULL;
      tn_replace = NULL;
      opnd_tn[opndnum] = tn;
      opnd_tninfo[opndnum] = NULL;
      orig_tninfo[opndnum] = NULL;
      operand_omega = check_omegas ? OP_omega(op,opndnum) : 0;

      if (tn == NULL || TN_is_constant(tn) || TN_is_label(tn)) {
        continue;
      }

#ifdef KEY
      // Don't handle asm OPs where a result may have to be the same as a src
      // operand.  The asm can have multiple such results, but the EBO code
      // expects at most one.
      if (OP_code(op) == TOP_asm) {
	UINT i;
	for (i = 0; i < OP_results(op) && !dont_replace; i ++)	  
	  if (OP_result(op, i) == tn)
	    dont_replace = TRUE;

	// Bug 2408 - asm constraints may force us to use particular registers.
	// To really optimize this case, we have to look at the constraints for
	// each of the operand(s) and result(s) from the ASM WN that generated
	// this asm 'op'.
	ASM_OP_ANNOT *asm_info = (ASM_OP_ANNOT *) OP_MAP_Get(OP_Asm_Map, op);
	for (i = 0; i < OP_opnds(op) && !dont_replace; i ++)
	  if (OP_opnd(op, i) == tn &&
	      // Memory operands don't have special register class requirement.
	      !ASM_OP_opnd_memory(asm_info)[i]) {
	    dont_replace = TRUE;	
	  }
      }

      /* Don't replace a gra homeable gtn; otherwise, gra will do the wrong
	 spilling.  (bug#2913)
      */
      if( !EBO_in_peep     &&
	  !CG_localize_tns &&
	  TN_is_gra_homeable(tn) &&
	  TN_is_global_reg(tn)   &&
	  OP_store( op ) ){
	WN* wn = Get_WN_From_Memory_OP( op );
	if( wn != NULL     &&
	    WN_has_sym(wn) &&
	    /* (Aliased( Alias_Manager, TN_home(tn), wn ) != NOT_ALIASED) */
	    WN_st(wn) == WN_st(TN_home(tn)) ){
	  dont_replace = TRUE;
	}
      }
#endif

#ifdef TARG_MIPS
      if (OP_code(op) == TOP_jalr) {
	dont_replace = TRUE;    // Bug 12505: Don't replace RA_TN in jalr
      }
#endif

      if (tn != True_TN) {
#ifdef TARG_X8664
	TN* tmp_tn = CGTARG_Gen_Dedicated_Subclass_TN( op, opndnum, FALSE );
	if( tmp_tn == NULL )
	  tmp_tn = tn;

        tninfo = tn_info_use ( bb, op, tmp_tn, op_predicate_tn,
			       op_predicate_tninfo, operand_omega );

	// We need to update the tninfo for both TNs if they are different.
	// This is because of cases like the one below:
	// Case I.
	//       TN10      :- copy TN1 # may be an incoming parameter
	//       ...       :- div      # updates TN1 (rax)
	//                 :- TN10     # EBO may propagate the copy TN1
	//
	// Case II.
	//       TN62      :-          # some def
	//       TN63      :-          # some def
	//       TN66      :- sar32 TN62 (0x1f)
	//       TN65 TN64 :- div TN62 TN63 TN66
	// 
	// In case II, the sar32 op may get deleted by EBO because reference 
	// count is not updated for TN62 when processing the div. That is
	// why we need to update the ref count for both TN1 and TN62.

	if (tn != tmp_tn)
	  tninfo = tn_info_use ( bb, op, tn, op_predicate_tn,
				 op_predicate_tninfo, operand_omega );
#else
        tninfo = tn_info_use ( bb, op, tn, op_predicate_tn,
			       op_predicate_tninfo, operand_omega );
#endif

        orig_tninfo[opndnum] = tninfo;
	tn_replace = tninfo->replacement_tn;

#ifdef TARG_X8664
	/* Make sure the low 8-bit part is addressable under -m32
	 */
	if (Is_Target_32bit()  &&
	    tn_replace != NULL &&
	    TN_is_register(tn_replace) &&
	    TN_register(tn_replace) != REGISTER_UNDEFINED &&
	    OP_opnd_size(op, opndnum) == 8) {
	  const REGISTER reg = TN_register(tn_replace);
	  const REGISTER_SET regs =
		 REGISTER_SUBCLASS_members(ISA_REGISTER_SUBCLASS_m32_8bit_regs);
	  Is_True(TN_register_class(tn) == ISA_REGISTER_CLASS_integer,
		  ("Find_BB_TNs: TN not integer register class"));
	  if (!REGISTER_SET_MemberP(regs, reg))
	    tn_replace = NULL;
	}

	if( tn_replace != NULL ){
	  if( EBO_in_peep        &&
	      opndnum == 0       &&
	      OP_x86_style( op ) &&
	      !tn_registers_identical( tn_replace, OP_result(op,0) ) ){
	    tn_replace = NULL;
	  }

	  if( tn_replace != NULL               &&
	      TN_is_register( tn_replace )     &&
	      TN_register( tn_replace ) == RSP &&
	      TOP_Find_Operand_Use( OP_code(op), OU_index ) == opndnum ){
	    tn_replace = NULL;
	  }
	}

	// Before register allocation, do not copy propagate 
	// dedicated TNs.
	// An example:
	//     TN200 :- copy TN8 (%rcx)
	//         ...
	//     TN100 :- shr TN68 TN69
	//     TN201 :- use TN200
	// We can not do copy propagation for the use of TN200.
	// TODO: Look at the live range of the dedicated TNs and see if 
	// there are intervening shift/multiply/divide or any such instructions
	// that need preallocated registers. If there are such instructions, 
	// then we can not do copy propagation.
	if ( tn_replace != NULL &&
	     !EBO_in_peep       &&
	     TN_is_dedicated( tn_replace ) &&
	     TN_register_class(tn_replace) == ISA_REGISTER_CLASS_integer ){
	  const REGISTER reg = TN_register(tn_replace);
	  if( reg == RAX || reg == RCX || reg == RDX ){
	    tn_replace = NULL;
	  }
	}

        // Find cmp merge cands if only neither of the cmp opnds are memops.
        if( Is_Target_32bit() &&
            CG_cmp_load_exec &&
            !EBO_in_peep &&
            !op_is_cmp_merge_cand &&
            do_load_execute &&
            ( num_opnds == 2 ) &&
            ( OP_code(op) == TOP_cmp32 ) && 
            ( tninfo->in_op ) &&
            ( TN_is_register(tn) ) &&
            ( OP_load( tninfo->in_op ) ) &&
            ( tninfo->reference_count == 1 ) &&
            ( OP_code(tninfo->in_op) == TOP_ld32 ) ){
          // If the load cannot be folded into the cmp keep looking.
          // We can only do this step on local loads within the bb,
          // else we rely on the EBO_Load_Flow_Safe.
          if( (tninfo->in_op->bb == bb) &&
              EBO_Not_Load_Exec_Opnd( tninfo->in_op ) ) 
            continue;

          // If the load is from another block apply flow criteria or reject it
          if( !EBO_Load_Flow_Safe( tninfo->in_op, tninfo->in_op->bb, bb ) ) 
            continue;

          // We found a candidate
          cmp_merge_idx = opndnum;
          op_is_cmp_merge_cand = TRUE;
        } else if( Is_Target_64bit() &&
                   CG_cmp_load_exec &&
                   !EBO_in_peep &&
                   !op_is_cmp_merge_cand &&
                   do_load_execute &&
                   ( num_opnds == 2 ) &&
                   ( OP_code(op) == TOP_cmp64 ) && 
                   ( tninfo->in_op ) &&
                   ( TN_is_register(tn) ) &&
                   ( OP_load( tninfo->in_op ) ) &&
                   ( tninfo->reference_count == 1 ) &&
                   ( OP_code(tninfo->in_op) == TOP_ld64 ) ){
          // If the load cannot be folded into the cmp keep looking.
          // We can only do this step on local loads within the bb,
          // else we rely on the EBO_Load_Flow_Safe.
          if( (tninfo->in_op->bb == bb) &&
              EBO_Not_Load_Exec_Opnd( tninfo->in_op ) ) 
            continue;

          // If the load is from another block apply flow criteria or reject it
          if( !EBO_Load_Flow_Safe( tninfo->in_op, tninfo->in_op->bb, bb ) ) 
            continue;

          // We found a candidate
          cmp_merge_idx = opndnum;
          op_is_cmp_merge_cand = TRUE;
        }
#endif

#ifdef KEY
	if( dont_replace ){
	  tn_replace = NULL;
	}
#endif
      }

#ifdef KEY
      if (!dont_replace)
#endif
      if (OP_same_res(op)) {
        INT i;
        for (i=0; i<OP_results(op); i++) {
          if (tn_registers_identical(tn, OP_result(op,i))) {
           /* This logic assumes that only one result matches an operand. */
            replace_result = TRUE;
            rslt_num = i;
            break;
          }
        }
      }

      if ((tn_replace != NULL) &&
          (TN_Is_Constant(tn_replace) ||
           EBO_tn_available(bb,tninfo->replacement_tninfo) ||
           ((tn_registers_identical(tn, tn_replace)) && !check_omegas)) &&
          (TN_Is_Constant(tn_replace) ||
           ((tninfo->replacement_tninfo != NULL) &&
            (tninfo->replacement_tninfo->in_bb == bb)) ||
           ((has_assigned_reg(tn) == has_assigned_reg(tn_replace)) &&
            (EBO_in_peep || (!BB_reg_alloc(bb) && !TN_is_dedicated(tn_replace))))) ) {
       /* The original TN can be "logically" replaced with another TN. */

        if (EBO_Trace_Data_Flow) {
          #pragma mips_frequency_hint NEVER
          fprintf(TFile, "%sin BB:%d propagate value for opnd[%d] ",
                  EBO_trace_pfx, BB_id(OP_bb(op)),opndnum);
          Print_TN(tn, FALSE);
          fprintf(TFile," with ");
          Print_TN(tn_replace, FALSE);
          fprintf(TFile,"\n");
        }

        TN *old_tn = tn;
        tn = tninfo->replacement_tn;
        tninfo = tninfo->replacement_tninfo;

#ifdef KEY
	EBO_REG_ENTRY reginfo;
	BOOL inside_lr = TRUE;
	// opinfo of OP defining the replacement TN
	EBO_OP_INFO *pred_opinfo = locate_opinfo_entry(tninfo);
#endif
        if (!TN_is_constant(tn) &&
            (!OP_store(op) ||
             (opndnum != TOP_Find_Operand_Use(OP_code(op),OU_storeval)) ||
             !TN_has_spill(old_tn)) &&
            (!TN_save_reg(tn)) &&
#if defined(KEY) && defined(TARG_MIPS)
	    // 14863: Don't propagate into a conditional move
	    (!replace_result || OP_code(op) == TOP_asm) &&
#endif
            (!replace_result || tn_registers_identical(old_tn, tn_replace)) &&
            (EBO_in_peep ||
             (has_assigned_reg(old_tn) == has_assigned_reg(tn_replace)) ||
             (TN_is_const_reg(tn)) ||
             (!OP_copy(op))) &&
            (EBO_in_peep ||
             !TN_is_gra_homeable(tn_replace) ||
             (tninfo->in_bb == bb)) &&
            (TN_register_class(old_tn) == TN_register_class(tn_replace)) &&
            (!has_assigned_reg(old_tn) ||
             (ISA_OPERAND_VALTYP_Register_Subclass(ISA_OPERAND_INFO_Operand(ISA_OPERAND_Info(OP_code(op)),opndnum)) == ISA_REGISTER_SUBCLASS_UNDEFINED) ||
	     (has_assigned_reg(tn_replace) &&
		(REGISTER_SET_MemberP(REGISTER_SUBCLASS_members(ISA_OPERAND_VALTYP_Register_Subclass(ISA_OPERAND_INFO_Operand(ISA_OPERAND_Info(OP_code(op)),opndnum))), TN_register(tn_replace))))) &&
            (TN_size(old_tn) <= TN_size(tn_replace)) &&
            (TN_is_float(old_tn) == TN_is_float(tn_replace)) &&
            (TN_is_fpu_int(old_tn) == TN_is_fpu_int(tn_replace)) &&
            ((OP_results(op) == 0) ||
             !OP_uniq_res(op) ||
             !tn_registers_identical(tn, OP_result(op,0)))
#ifdef KEY
            && (TN_is_global_reg(tn_replace) ||
	    	// OP is inside the live range of the replacement TN?
		(EBO_REG_ENTRY_ptr(reginfo) = hTN_MAP_Get(regs_map,tn_replace),
		 inside_lr =
		   (EBO_REG_ENTRY_last_use_OP_num(reginfo) >= op_num)) ||
		// Register available for CSE?
		(pred_opinfo == NULL ||
		 (pred_opinfo->op_num >=
		    last_fat_point[TN_register_class(tn_replace)])))
#endif
	   ) {
         /* The original TN can be "physically" replaced with another TN. */
         /* Put the new TN in the expression,           */
         /* decrement the use count of the previous TN, */
         /* increment the use count of the new TN.      */

          if (EBO_Trace_Optimization) {
            #pragma mips_frequency_hint NEVER
            fprintf(TFile, "%sin BB:%d replace opnd[%d] ",
                    EBO_trace_pfx, BB_id(OP_bb(op)),opndnum);
            Print_TN(OP_opnd(op, opndnum), FALSE);
            fprintf(TFile," with ");
            Print_TN(tn_replace, FALSE);
            if (!TN_Is_Constant(tn_replace) &&
                (tninfo != NULL) &&
                (tninfo->in_bb != NULL)) {
              fprintf(TFile," from BB:%d",BB_id(tninfo->in_bb));
            }
            fprintf(TFile,"\n");
          }

          dec_ref_count(orig_tninfo[opndnum]);
          Set_OP_opnd(op, opndnum, tn);
          if (check_omegas) {
            Set_OP_omega (op, opndnum, (tninfo != NULL) ? tninfo->omega : 0);
          }

          if (tninfo != NULL) {
            inc_ref_count(tninfo);
          }

          if (replace_result) {
           /* This use is also the new result. */
            rslt_tn = tn;
          }

         /* Update information about the actual expression. */
          orig_tninfo[opndnum] = tninfo;

#ifdef KEY
	  // Extend the live range of the replacement TN if OP is outside of
	  // this live range.
	  if (!inside_lr)
	    Extend_Live_Range(regs_used, last_fat_point, op_num, tninfo,
			      reginfo, bb);
#endif
        } /* replace the operand with another TN. */
      }

      opnd_tn[opndnum] = tn;
      opnd_tninfo[opndnum] = tninfo;
      if (!TN_Is_Constant(tn) &&
          (!op_is_predicated || (opndnum != OP_PREDICATE_OPND))) {
        opnds_constant = FALSE;
      }

      if (op_is_predicated && (opndnum == OP_PREDICATE_OPND)) {
        if ((tn == Zero_TN) && !OP_xfer(op)) {
#ifdef TARG_X8664
	  FmtAssert( false, ("X86 has no Zero_TN") );
#endif
         /* The instruction will not be executed - it can be deleted!
            However, Branch instructions should go through
            Resolve_Conditional_Branch so that links between blocks can be updated. */
          op_replaced = Fold_Constant_Expression (op, opnd_tn, opnd_tninfo);
          num_opnds = opndnum + 1;

          if (EBO_Trace_Optimization) {
            #pragma mips_frequency_hint NEVER
            fprintf(TFile, "%sin BB:%d Op can not execute with 0 predicate\n",
                    EBO_trace_pfx, BB_id(OP_bb(op)));
          }
          if (op_replaced) break;
        }

        op_predicate_tn = tn;
        op_predicate_tninfo = tninfo;
      }

    } /* End: Process all the operand TNs. */

#ifdef TARG_X8664
    if (EBO_in_pre && !EBO_in_loop && CG_cmp_load_exec )
      do_load_execute = op_is_cmp_merge_cand;
#endif

    if (OP_memory(op)) {
      if (!op_replaced &&
          OP_same_res(op)) {
        op_replaced = EBO_Fix_Same_Res_Op (op, opnd_tn, opnd_tninfo);
      }
      if (!op_replaced &&
          !in_delay_slot &&
          OP_store(op)) {
        op_replaced = Special_Sequence ( op, opnd_tn, orig_tninfo);
      }
      if (!op_replaced &&
          no_barriers_encountered) {
        op_replaced = find_duplicate_mem_op (bb, op, opnd_tn, opnd_tninfo,
					     orig_tninfo, op_num, regs_used,
					     last_fat_point);
      }

#ifdef TARG_MIPS //bug 14905
     if(!op_replaced && OP_prefetch(op) && !EBO_in_peep)
      op_replaced = Delete_Redundant_Prefetches(op);
#endif   

#ifdef TARG_X8664
      if (WOPT_Enable_Autoaggstr_Reduction_Threshold > 0 && 
	  LNO_Run_Prefetch != AGGRESSIVE_PREFETCH &&
	  !EBO_in_peep && !op_replaced && OP_prefetch(op)) {
	op_replaced = Delete_Unwanted_Prefetches(op);
      }
      if( !op_replaced    &&
	  do_load_execute &&
	  OP_store(op) ){
	Update_op_must_not_be_moved( op, opnd_tninfo );
      }
#endif
#if defined(TARG_X8664) || defined(TARG_MIPS)
      if( !op_replaced && !in_delay_slot ) {
	op_replaced = EBO_Merge_Memory_Addr( op, opnd_tn, opnd_tninfo, orig_tninfo );
      }
      if ( !op_replaced &&
            (CG_LOOP_unroll_level == 2) &&
            OP_load(op) &&
            EBO_in_loop &&
            BB_unrolled_fully(bb) &&
            (EBO_loop != NULL)) {
        op_replaced = EBO_Opt_Const_Array( op, EBO_loop, EBO_loop_size );
        if (op_replaced)
          scalar_replacement_performed = op_replaced;
      }
#endif
    } else if (OP_effectively_copy(op)) {
      if (!op_replaced &&
          opnds_constant && !in_delay_slot) {
        op_replaced = find_previous_constant(op, opnd_tninfo);
      }
      if (!op_replaced) {
        op_replaced = Special_Sequence (op, opnd_tn, opnd_tninfo);
      }
    } else if ( op_is_cmp_merge_cand &&
                !op_replaced ) {
#ifdef TARG_X8664
      /* we already know the cmp is not a memop and that we have a
       * load feeding one of the src operands. Allow load execute to
       * handle the operation. */
      EBO_flow_safe = TRUE;
      op_replaced = EBO_Load_Execution( op, opnd_tn, orig_tninfo, cmp_merge_idx );
      if( op_replaced ){
        /* we need to cleanup here, as we will not be doing so below. */
        OP_Change_To_Noop(orig_tninfo[cmp_merge_idx]->in_op);
      }
      EBO_flow_safe = FALSE;
#endif
    } else if (!op_replaced &&
               !OP_effectively_copy(op) &&
               !OP_glue(op) &&
               !OP_side_effects(op) &&
               !OP_access_reg_bank(op)) {
      if (!in_delay_slot) {
       /* Can we evaluate the expression at compile time? */
        if (opnds_constant && (num_opnds > 1)) {
          if (OP_xfer(op)) {
           /* If we remove a conditional branch and alter the flow, we
              may have created dead code that could cause later processing
              to get into trouble. This needs to be looked into. */
            op_replaced = Resolve_Conditional_Branch (op, opnd_tn);
            rerun_cflow |= op_replaced;
          } else if (OP_results(op) >= 1) {
            op_replaced = Fold_Constant_Expression (op, opnd_tn, opnd_tninfo);
          }
        } else if (num_opnds > 1) {
          if (OP_results(op) > 0) {
           /* Consider special case optimizations. */
            INT o2_idx; /* TOP_Find_Operand_Use(OP_code(op),OU_opnd2) won't work for all the cases we care about */
            INT o1_idx; /* TOP_Find_Operand_Use(OP_code(op),OU_opnd1) won't work for all the cases we care about */
            if (op_is_predicated) {
              if (OP_fadd(op) || OP_fsub(op) || OP_fmul(op) || OP_fdiv(op)) {
               /* The first operand is not wnat we are looking for. */
                o1_idx = (num_opnds > 2) ? 2 : -1;
                o2_idx = (num_opnds > 3) ? 3 : -1;
              } else {
                o1_idx = (num_opnds > 1) ? 1 : -1;
                o2_idx = (num_opnds > 2) ? 2 : -1;
              }
            } else {
              o1_idx = (num_opnds > 0) ? 0 : -1;
              o2_idx = (num_opnds > 1) ? 1 : -1;
            }

            if (OP_same_res(op)) {
              op_replaced = EBO_Fix_Same_Res_Op (op, opnd_tn, opnd_tninfo);
            }
            if (o2_idx >= 0) {
              tn = opnd_tn[o2_idx];
              if (!op_replaced &&
                  (tn != NULL) &&
                  TN_Is_Constant(tn) && TN_Has_Value(tn)) {
                op_replaced = Constant_Operand1 (op, opnd_tn, opnd_tninfo);
              }
            }
            if (o1_idx >= 0) {
              tn = opnd_tn[o1_idx];
              if (!op_replaced &&
                  (tn != NULL) &&
                  TN_Is_Constant(tn) && TN_Has_Value(tn)) {
                  op_replaced = Constant_Operand0 (op, opnd_tn, opnd_tninfo);
              }
            }
          }
          if (!op_replaced) {
            op_replaced = Special_Sequence (op, opnd_tn, orig_tninfo);
          }
        }
#if defined(TARG_X8664) || defined(TARG_MIPS)
	else if (num_opnds == 1) {
	  if (OP_results(op) > 0) {
	    if (!op_replaced) {
	      op_replaced = Special_Sequence (op, opnd_tn, orig_tninfo);
	    }
	  }
	}
#endif
      }

      if (no_barriers_encountered && !op_replaced && !OP_effectively_copy(op)) {
       /* Look for redundant OPs. */
        op_replaced = find_duplicate_op(bb, op, opnd_tn, opnd_tninfo, orig_tninfo);
      }

#ifdef TARG_X8664
      if( do_load_execute  &&
	  !op_replaced     &&
	  !OP_effectively_copy(op) ){
	op_replaced = EBO_Load_Execution( op, opnd_tn, orig_tninfo, cmp_merge_idx );
      }

      if ( !op_replaced &&
            (CG_LOOP_unroll_level == 2) &&
            (OP_code(op) == TOP_leax32) &&
            scalar_replacement_performed &&
            EBO_in_loop &&
            BB_unrolled_fully(bb) &&
            (EBO_loop != NULL)) {
        op_replaced = EBO_Fold_Lea_Const_Component( op );
      }
      if( !op_replaced     &&
	  !OP_effectively_copy(op) ){
	op_replaced = EBO_Lea_Insertion( op, opnd_tn, orig_tninfo );
      }
      if (!op_replaced &&  
	  !OP_effectively_copy(op) ) {
	op_replaced = EBO_Fold_Load_Duplicate( op, opnd_tn, orig_tninfo );
      }
      if (!op_replaced && TOP_is_move_ext( OP_code(op) ))
	op_replaced = Special_Sequence( op, opnd_tn, orig_tninfo );
#endif
    }

#ifdef TARG_X8664
    if (!op_replaced &&
	OP_code(op) == TOP_asm) {
      op_replaced = Special_Asm_Sequence(op, opnd_tn, orig_tninfo);
    }
#endif

    if (op_replaced) {
      if (EBO_Trace_Optimization) {
        fprintf(TFile,"%sin BB:%d remove simplified op - ",EBO_trace_pfx,BB_id(bb));
        Print_OP_No_SrcLine(op);
      }
#ifdef KEY
      // If the deleted OP is a load, then update the source register usage (in
      // case the src live range(s) ends here) but don't reserve a register for
      // the result.
      // If the deleted OP is an ALU OP being replaced by a load-execute, then
      // update the src and dest as usual.
      Update_Reg_Usage(op, op_num, regs_used, last_fat_point, OP_load(op));

      // If the deleted OP is the only restore for a spilled value, then delete
      // the corresponding spill OP.
      if (OP_load(op)) {
      	ST *spill_loc = CGSPILL_OP_Spill_Location(op);
	if (spill_loc != (ST *)0) {		// It's a spill OP.
	  SPILL_SYM_INFO &info = CGSPILL_Get_Spill_Sym_Info(spill_loc);
	  if (info.Restore_Count() == 1 &&
	      info.Spill_Count() == 1 &&
	      !info.Used_By_Load_Exe()) {	// store required for load-exe
	    OP *spill_op = info.Spill_Op();
	    Is_True(spill_op != NULL, ("Find_BB_TNs: missing spill OP"));
	    OP_Change_To_Noop(spill_op);
	  }
	}
      }
#endif
      remove_uses (num_opnds, orig_tninfo);
      OP_Change_To_Noop(op);
    } else {
     /* Add this OP to the hash table and define all the result TN's. */
      add_to_hash_table (in_delay_slot, op, orig_tninfo, opnd_tninfo);

      FmtAssert(((EBO_last_opinfo != NULL) && (EBO_last_opinfo->in_op == op)),
                  ("OP wasn't added to hash table"));

#ifdef KEY
      EBO_last_opinfo->op_num = op_num;
#endif

     /* Special processing for the result TNs */
      resnum = OP_results(op);
      if (OP_effectively_copy(op) || (resnum && OP_glue(op) && !OP_memory(op))
	  ) {
	/* Propagate copy assignements. */
        INT cix = copy_operand(op);
        TN *tnr = OP_result(op, 0);

        if ((tnr != NULL) && (tnr != True_TN) && (tnr != Zero_TN)) {
          tninfo = EBO_last_opinfo->actual_rslt[0];

          if (!OP_glue(op) && (cix >= 0)) {
            tninfo->replacement_tn = opnd_tn[cix];
            tninfo->replacement_tninfo = opnd_tninfo[cix];

#ifdef TARG_X8664
	    /* Do not propagate the constant part here, since most of the time
	       a constant can not serve as a real operand. To enable
	       constant propagation, this constant will be picked up
	       later from opnd_tninfo[].
	    */
	    if( TN_Is_Constant( tninfo->replacement_tn ) &&
		OP_code(op) != TOP_ldc64 &&
		OP_code(op) != TOP_ldc32 ){
	      tninfo->replacement_tn = OP_opnd( op, cix );
	      tninfo->replacement_tninfo = orig_tninfo[cix];
	    }
#endif

            if (EBO_Trace_Data_Flow) {
              #pragma mips_frequency_hint NEVER
              fprintf(TFile,"%sPropagate Copy of ",EBO_trace_pfx);
              Print_TN(tninfo->replacement_tn,FALSE);
              fprintf(TFile,"[%d] into ",(tninfo->replacement_tninfo != NULL)?tninfo->replacement_tninfo->omega:0);
              Print_TN(tnr,FALSE); fprintf(TFile,"\n");
            }
          }
        }

        if ((resnum == 2) && ((tnr=OP_result(op,1)) != NULL) && (tnr != True_TN)  && (tnr != Zero_TN)) {
         /* This logic must be in sync with what ebo_special calls a "copy".       
            This instruction must actually be placing a "FALSE" condition in a predicate. */
          tninfo = EBO_last_opinfo->actual_rslt[1];
          tninfo->replacement_tn = Zero_TN;
#ifdef TARG_X8664
	  FmtAssert( false, ("X86 does not have Zero_TN") );
#endif

          if (EBO_Trace_Data_Flow) {
            #pragma mips_frequency_hint NEVER
            fprintf(TFile,"%sPropagate Copy of ",EBO_trace_pfx);
            Print_TN(tninfo->replacement_tn,FALSE); fprintf(TFile," into ");
            Print_TN(tnr,FALSE); fprintf(TFile,"\n");
          }
        }

      } else if (rslt_tn != NULL) {
#ifdef KEY
        if (op->opr == TOP_asm && TN_is_constant(rslt_tn))
	  ;
	else {
#endif
       /* A result tn needs to be replaced. */
        TN *tnr = OP_result(op, rslt_num);
        tninfo = EBO_last_opinfo->actual_rslt[rslt_num];

       /* This is subtle - yes we do want the replacement_tninfo
          entry to point to the tninfo entry we just created. Yes,
          it does create a circular link in the chain. Code that
          searches the chain will need to be aware of this. */
        tninfo->replacement_tn = rslt_tn;
        tninfo->replacement_tninfo = tninfo;
        Set_OP_result (op, rslt_num, rslt_tn);

        if (EBO_Trace_Data_Flow) {
          #pragma mips_frequency_hint NEVER
          fprintf(TFile,"%sReplace result[%d] tn ",EBO_trace_pfx,rslt_num);
          Print_TN(tnr,FALSE); fprintf(TFile," with ");
          Print_TN(rslt_tn, FALSE); fprintf(TFile,"\n");
        }
#ifdef KEY
        }
#endif

      }
    }

    if (PROC_has_branch_delay_slot()) in_delay_slot = OP_xfer(op);

#ifdef KEY
finish:

#ifdef TARG_X8664
    // If OP is a memory OP that references a spilled LRA value, mark it so.
    // Later we will delete dead LRA spills.
    if (EBO_in_peep &&
	!op_replaced) {
      Mark_LRA_Spill_Reference(op, &LRA_spilled_value_is_used, &MEM_local_pool);
    }
#endif

    // Update the register usage.  Identify the next OP with register usage
    // info.
    Is_True(op_num <= op_count, ("Find_BB_TNs: wrong op_num"));
    if (op == op_with_reg_usage_info) {
      op_with_reg_usage_info = next_op_with_reg_usage_info;
      if (!op_replaced)
	Update_Reg_Usage(op, op_num, regs_used, last_fat_point, FALSE);
    }
#endif
  }

#ifdef TARG_X8664
  // Remove dead LRA stores.  A LRA store becomes dead when all the restores
  // are eliminated by EBO, which can happen when the spilled register is
  // unused between the store and the load, which can happen when intervening
  // instructions are changed from load/use to load-exe.
  if (EBO_in_peep) {
    FOR_ALL_BB_OPs (bb, op) {
      Delete_Dead_LRA_Spill(op, &LRA_spilled_value_is_used);
    }
  }
#endif

  return no_barriers_encountered;
}
  




#ifdef KEY
static int removed_ops = 0;
#endif
static
void EBO_Remove_Unused_Ops (BB *bb, BOOL BB_completely_processed)
/* -----------------------------------------------------------------------
 * -----------------------------------------------------------------------
 */
{
  EBO_OP_INFO *opinfo;
  EBO_TN_INFO *tninfo;
  TN *tn;

  if (EBO_first_opinfo == NULL) return;

  if (EBO_Trace_Execution) {
    #pragma mips_frequency_hint NEVER
    fprintf(TFile,"%sEnter EBO_Remove_Unused_Ops in BB:%d\n",EBO_trace_pfx,BB_id(bb));
    tn_info_table_dump();
  }

  for (opinfo = EBO_last_opinfo; opinfo != NULL; opinfo = opinfo->prior) {
    INT rslt_count = 0;
    INT idx;
    OP *op = opinfo->in_op;

#ifndef KEY
    if (op == NULL) continue;
#else
    // If op was replaced, then op->bb could be NULL. Just skip this op.
    if (op == NULL || op->bb == NULL) continue;

    // Don't delete OP in the branch delay slot if EBO is run after delay slot
    // filling.
    if (!EBO_can_delete_branch_delay_OP &&
	PROC_has_branch_delay_slot() &&
	BB_last_op(bb) == op &&
	OP_prev(op) != NULL &&
	OP_xfer(OP_prev(op))) {
      goto op_is_needed;
    }
#endif

    if (OP_bb(op) != bb) {
      if (EBO_Trace_Block_Flow) {
        #pragma mips_frequency_hint NEVER
        fprintf(TFile,"Stop looking for unused ops, next op is in BB:%d\n",
                OP_bb(op) ? BB_id(OP_bb(op)) : -1);
        Print_OP_No_SrcLine(op);
      }
      break;  /* get out of  loop over opinfo entries. */
    }

    if (EBO_Trace_Data_Flow) {
      #pragma mips_frequency_hint NEVER
      fprintf(TFile,"%sConsider removing OP:\n\t",
              EBO_trace_pfx);
      Print_OP_No_SrcLine(op);
    }

    rslt_count = OP_results(op);
    if (rslt_count == 0) goto op_is_needed;
    if (op_is_needed_globally(op)) goto op_is_needed;

   /* Check that all the result operands can be safely removed. */
    for (idx = 0; idx < rslt_count; idx++) {
      tninfo = opinfo->actual_rslt[idx];

     /* A couple of safety checks. */
      if (tninfo == NULL) continue;
      if (tninfo->in_bb != bb) goto op_is_needed;
      if (tninfo->in_op == NULL) goto op_is_needed;
      tn = tninfo->local_tn;

      if (EBO_Trace_Data_Flow) {
        #pragma mips_frequency_hint NEVER
        fprintf(TFile,"%sTry to remove definition of entry[%d] ",
                EBO_trace_pfx,tninfo->sequence_num);
        Print_TN(tn,FALSE);
        fprintf(TFile,"\n");
      }

     /* Zero_TN or True_TN for a result is a no-op. */
      FmtAssert( tn != Zero_TN, ("NYI") );
      if (tn == Zero_TN) continue;
      if (tn == True_TN) continue;

     /* Copies to and from the same register are not needed. */
      if (EBO_in_peep &&
          OP_effectively_copy(op) &&
          has_assigned_reg(tn) &&
          (copy_operand(op) >= 0) &&
          has_assigned_reg(OP_opnd(op,copy_operand(op))) &&
          (tn_registers_identical(tn, OP_opnd(op,copy_operand(op))))) {
        INT cpo = copy_operand(op);

       /* We may be able to get rid of the copy, but be
          sure that the TN is marked live into this block. */
        if ((opinfo->actual_opnd[cpo] != NULL) &&
            (bb != opinfo->actual_opnd[cpo]->in_bb)) {
          mark_tn_live_into_BB (tn, bb, opinfo->actual_opnd[cpo]->in_bb);
        }

       /* Propagate use count for this TN to it's input TN. */
        if (tninfo->same != NULL) {
          tninfo->same->reference_count += tninfo->reference_count;
        }

        if (!tninfo->redefined_before_block_end &&
            (tninfo->same != NULL) &&
            (tninfo->same->in_bb == bb)) {
         /* Removing the copy causes the previous definition
            of the TN (or reg) to reach the end of the block. */
          tninfo->same->redefined_before_block_end = FALSE;
        }
        goto can_be_removed;
      }

     /* There must be no direct references to the TN. */
      if (!BB_completely_processed) goto op_is_needed;
      if (tninfo->reference_count != 0) goto op_is_needed;
      if (OP_has_implicit_interactions(tninfo->in_op))
	goto op_is_needed;

     /* Check for indirect and global references.   */
      if (!tninfo->redefined_before_block_end &&
          TN_live_out_of(tn, tninfo->in_bb)) goto op_is_needed;

      if (TN_is_pfs_reg(tn)) goto op_is_needed;
      if (TN_is_lc_reg(tn)) goto op_is_needed;
      if (TN_is_ec_reg(tn)) goto op_is_needed;
    }

   /* None of the results are needed. */
    if (opinfo->op_must_not_be_removed) goto op_is_needed;
    if (OP_store(op)) goto op_is_needed;

#ifdef TARG_X8664
    if( TOP_is_change_rflags( OP_code(op) ) ){
      for( OP* next = OP_next(op); next != NULL; next = OP_next(next) ){
	if( OP_reads_rflags( next) )
	  goto op_is_needed;
	if( TOP_is_change_rflags( OP_code(next) ) )
	  break;
      }
    }
#endif 

can_be_removed:

    remove_op (opinfo);

    if (EBO_Trace_Optimization) {
      #pragma mips_frequency_hint NEVER
      fprintf(TFile, "%sin BB:%d removing    ",
              EBO_trace_pfx, BB_id(bb));
      Print_OP_No_SrcLine(op);
    }

    if (opinfo->in_delay_slot) {
      OP_Change_To_Noop(op);
    } else {
#ifdef KEY
      // If deleting the first OP in the BB, make sure all predecessor BBs
      // branch to the correct OP.
      if (BB_first_op(bb) == op &&
	  PROC_has_branch_delay_slot()) {
	EBO_Adjust_Pred_Branch_Target(bb);
      }
#endif
      BB_Remove_Op(bb, op);
    }
    opinfo->in_op = NULL;
    opinfo->in_bb = NULL;

   /* Propagate "reaches block end" information. */
    for (idx = 0; idx < rslt_count; idx++) {
      tninfo = opinfo->actual_rslt[idx];
      if ((tninfo != NULL) &&
          tninfo->redefined_before_block_end &&
          (tninfo->same != NULL) &&
          (tninfo->same->in_bb == bb)) {
        EBO_TN_INFO *next_tninfo = tninfo->same;
        next_tninfo->redefined_before_block_end = TRUE;
      }
    }

#ifdef KEY
    if (EBO_in_peep)
      removed_ops ++;
#endif
    continue;

op_is_needed:

   /* Predicate resolution may not have been possible, and */
   /* preceeding definitions of the same result TN's may   */
   /* need to be saved.                                    */
    for (idx = 0; idx < rslt_count; idx++) {
      tninfo = opinfo->actual_rslt[idx];
      if ((tninfo != NULL) &&
          (tninfo->local_tn != NULL) &&
          (tninfo->same != NULL)) {
        EBO_TN_INFO *next_tninfo = tninfo->same;

        while (next_tninfo != NULL) {
          if ((next_tninfo->in_op != NULL) &&
              (!EBO_predicate_dominates((tninfo->predicate_tninfo != NULL)?tninfo->predicate_tninfo->local_tn:True_TN,
                                         tninfo->predicate_tninfo,
                                         (next_tninfo->predicate_tninfo != NULL)?
                                               next_tninfo->predicate_tninfo->local_tn:True_TN,
                                         next_tninfo->predicate_tninfo)) &&
              (!EBO_predicate_complements((tninfo->predicate_tninfo != NULL)?tninfo->predicate_tninfo->local_tn:True_TN,
                                          tninfo->predicate_tninfo,
                                          (next_tninfo->predicate_tninfo != NULL)?
                                               next_tninfo->predicate_tninfo->local_tn:True_TN,
                                          next_tninfo->predicate_tninfo))) {

           /* A store into an unresolved predicate is a potential problem     */
           /* because the last store might not completely redefine the first. */
           /* The predicates could be completely independant.  But we         */
           /* don't know how to check for that, currently.                    */

           /* Stop searching and preserve the preceeding definition. */
            EBO_OP_INFO *opinfo = locate_opinfo_entry(next_tninfo);
            if (opinfo != NULL) {
              opinfo->op_must_not_be_removed = TRUE;
            } else {
             /* Couldn't find the opinfo entry.  Make sure that the TN has
                a use count so that the defining OP entry will not be deleted. */
              next_tninfo->reference_count += tninfo->reference_count;
            }

            if (EBO_Trace_Data_Flow) {
              #pragma mips_frequency_hint NEVER
              fprintf(TFile,"%sMark result same_tn as needed - original [%d]: ",
                      EBO_trace_pfx,tninfo->sequence_num);
              Print_TN(tninfo->local_tn,FALSE);
              fprintf(TFile," same as [%d]: ",next_tninfo->sequence_num);
              Print_TN(next_tninfo->local_tn,FALSE);
              fprintf(TFile,"\n");
            }

            if (EBO_predicate_dominates((next_tninfo->predicate_tninfo != NULL)?
                                               next_tninfo->predicate_tninfo->local_tn:True_TN,
                                         next_tninfo->predicate_tninfo,
                                        (tninfo->predicate_tninfo != NULL)?tninfo->predicate_tninfo->local_tn:True_TN,
                                         tninfo->predicate_tninfo)) {
              /* If the first store dominates the last, there can be no       */
              /* preceeding definitions that are partially redefined by       */
              /* the last store.  We can stop searching for other dominators. */
              break;
            }
          }
          next_tninfo = next_tninfo->same;
        }

      }
    }

   /* Predicate resolution may not have been possible, and */
   /* preceeding inputs to this instruction may be defined */
   /* within the current extended block, and must be saved.*/
    for (idx = 0; idx < OP_opnds(op); idx++) {
      tninfo = opinfo->actual_opnd[idx];
      if ((tninfo != NULL) &&
          (tninfo->local_tn != NULL) &&
          (tninfo->same != NULL)) {
        EBO_TN_INFO *next_tninfo = tninfo->same;

        while (next_tninfo != NULL) {
          if ((next_tninfo->in_op != NULL) &&
              (next_tninfo->omega == tninfo->omega)) {
            if (EBO_predicate_dominates((next_tninfo->predicate_tninfo != NULL)?
                                               next_tninfo->predicate_tninfo->local_tn:True_TN,
                                         next_tninfo->predicate_tninfo,
                                         (tninfo->predicate_tninfo != NULL)?tninfo->predicate_tninfo->local_tn:True_TN,
                                         tninfo->predicate_tninfo)) {
              /* This predicate dominates the OP we need to save. It's   */
              /* use count should be sufficiant to cause it to be saved. */
              /* We can stop searching for other dominators.             */
              break;
            } else if (EBO_predicate_complements((next_tninfo->predicate_tninfo != NULL)?
                                               next_tninfo->predicate_tninfo->local_tn:True_TN,
                                         next_tninfo->predicate_tninfo,
                                         (tninfo->predicate_tninfo != NULL)?tninfo->predicate_tninfo->local_tn:True_TN,
                                         tninfo->predicate_tninfo)) {
              /* These predicates are mutually exclusive.  There is no   */
              /* need to mark it as used, but se need to keep looking.   */
            } else {
             /* A store into an unresolved predicate is a potential problem. */
             /* The predicates could be completely independant.  But we      */
             /* don't know how to check for that, currently.                 */

             /* Stop searching and preserve the preceeding definition. */
              EBO_OP_INFO *opinfo = locate_opinfo_entry(next_tninfo);
              if (opinfo != NULL) {
                opinfo->op_must_not_be_removed = TRUE;
              } else {
               /* Couldn't find the opinfo entry.  Make sure that the TN has
                  a use count so that the defining OP entry will not be deleted. */
                next_tninfo->reference_count += tninfo->reference_count;
              }

             /* Check for block spanning values and make sure they are marked global. */
              if ((next_tninfo != NULL) &&
                  (bb != next_tninfo->in_bb)) {
                mark_tn_live_into_BB (tninfo->local_tn, bb, next_tninfo->in_bb);
              }

              if (EBO_Trace_Data_Flow) {
                #pragma mips_frequency_hint NEVER
                fprintf(TFile,"%sMark operand same_tn as needed - original [%d]: ",
                        EBO_trace_pfx,tninfo->sequence_num);
                Print_TN(tninfo->local_tn,FALSE);
                fprintf(TFile," same as [%d]: ",next_tninfo->sequence_num);
                Print_TN(next_tninfo->local_tn,FALSE);
                fprintf(TFile,"\n");
              }

            }
          }
          next_tninfo = next_tninfo->same;
        }

      }
    }


   /* Check for newly created references that cross a block
      boundary.  If one is found, mark it as global and carry
      registers in/out of blocks. */
    for (idx = 0; idx < OP_opnds(op); idx++) {
      tninfo = opinfo->actual_opnd[idx];
      if ((tninfo != NULL) &&
          (bb != tninfo->in_bb)) {
        mark_tn_live_into_BB (tninfo->local_tn, bb, tninfo->in_bb);
      }
    }

  } /* end: for each opinfo entry */

 /* Make a quick scan of the OPS in a BB and remove noops. */
  {
    OP *op;
    OP * next_op = NULL;
    in_delay_slot = FALSE;
    TOP noop_top = CGTARG_Noop_Top();

    for (op = BB_first_op(bb); op != NULL; op = next_op) {
      next_op = OP_next(op);
      if ((OP_code(op) == noop_top || OP_code(op) == TOP_noop)
	  && !in_delay_slot)
      {
        if (EBO_Trace_Optimization) {
          #pragma mips_frequency_hint NEVER
          fprintf(TFile, "%sin BB:%d removing noop    ",
                  EBO_trace_pfx, BB_id(bb));
          Print_OP_No_SrcLine(op);
        }
#ifdef KEY
	// If deleting the first OP in the BB, make sure all predecessor BBs
	// branch to the correct OP.
	if (BB_first_op(bb) == op &&
	    PROC_has_branch_delay_slot()) {
	  EBO_Adjust_Pred_Branch_Target(bb);
	}
#endif
        BB_Remove_Op(bb, op);
      } else if (PROC_has_branch_delay_slot()) {
	if (in_delay_slot && OP_code(op) == TOP_noop) {
	   // ugly hack for mips
	   OP_Change_Opcode(op, noop_top);
	}
        in_delay_slot = OP_xfer(op);
      }
    }

  }

  return;
}
  
  
/* ===================================================================== */

  
/* 
 */
static
void
EBO_Add_BB_to_EB (BB * bb)
{
  EBO_TN_INFO *save_last_tninfo = EBO_last_tninfo;
  EBO_OP_INFO *save_last_opinfo = EBO_last_opinfo;
  BBLIST *succ_list;
  BOOL normal_conditions;
#ifdef KEY
  hTN_MAP save_regs_map = regs_map;
#endif

  if (EBO_Trace_Execution) {
    #pragma mips_frequency_hint NEVER
    fprintf(TFile,"%sEnter EBO_Add_BB_to_EB BB:%d. It has %d successors\n",
            EBO_trace_pfx,BB_id(bb),BB_succs(bb)?BB_succs_len(bb):0);
  }

  Set_BB_visited(bb);

  if (   CG_skip_local_ebo 
      && (   (BB_id(bb) < CG_local_skip_before) 
          || (BB_id(bb) > CG_local_skip_after)  
          || (BB_id(bb) == CG_local_skip_equal)
         )
     ) 
  {
    DevWarn ("Skipping EBO for BB=%d", BB_id (bb));
    
    return;
  }

 /* Add this block to the current Extended Block (EB). */
  normal_conditions = Find_BB_TNs (bb);
  if (normal_conditions &&
#ifdef KEY
      !EBO_optimize_single_BB &&
#endif
      !CG_localize_tns) {
   /* Walk through the successors, trying to grow the EB. */
    FOR_ALL_BB_SUCCS(bb, succ_list) { 
      BB *succ = BBLIST_item(succ_list);

      if (EBO_Trace_Block_Flow) {
        #pragma mips_frequency_hint NEVER
        fprintf(TFile,"%sCheck successor BB:%d. It has %d preds and has%s been visited.\n",
               EBO_trace_pfx,BB_id(succ),BB_preds_len(succ),BB_visited(succ)?" ":" not");
      }

      if (!BB_call(bb) &&
#ifdef KEY
	  !BB_asm(bb)  && 	  /* bug#1777 Don't cross asm bb. */
#endif
          (BB_preds_len(succ) == 1) &&
          !BB_visited(succ) &&
          (BB_rid(bb) == BB_rid(succ))) {
        EBO_Add_BB_to_EB (succ);
      }
    }
  }

 /* When we are unable to grow the EB any more, optimize what we have. */
  if (EBO_Trace_Block_Flow) {
    #pragma mips_frequency_hint NEVER
    fprintf(TFile,"%sEBO optimization at BB:%d\n",EBO_trace_pfx,BB_id(bb));
  }

  EBO_Remove_Unused_Ops(bb, normal_conditions);

 /* Remove information about TN's and OP's in this block. */
  backup_tninfo_list(save_last_tninfo);
  backup_opinfo_list(save_last_opinfo);
#ifdef KEY
  regs_map = save_regs_map;
#endif

  return;
}
  
  
/* 
 * Perform EBO
 */
static
void
EBO_Process ( BB *first_bb )
{
  BB *bb;

  rerun_cflow = FALSE;

  EBO_Trace_Execution    = Get_Trace(TP_EBO, 0x001);
  EBO_Trace_Optimization = Get_Trace(TP_EBO, 0x002);
  EBO_Trace_Block_Flow   = Get_Trace(TP_EBO, 0x004);
  EBO_Trace_Data_Flow    = Get_Trace(TP_EBO, 0x008);
  EBO_Trace_Hash_Search  = Get_Trace(TP_EBO, 0x010);

  FmtAssert(((EBO_first_tninfo == NULL) && (EBO_first_opinfo == NULL)),
                  ("Initial pointers not NULL %o %o",EBO_first_tninfo,EBO_first_opinfo));

  EBO_Start();

  if (EBO_Trace_Data_Flow || EBO_Trace_Optimization) {
    #pragma mips_frequency_hint NEVER
    fprintf(TFile,">>>> Before EBO");
    if (EBO_in_peep) {
      fprintf(TFile," - in peep ");
    } else if (EBO_in_before_unrolling) {
      fprintf(TFile," - before unrolling ");
    } else if (EBO_in_after_unrolling) {
      fprintf(TFile," - after unrolling ");
    } else if (EBO_in_pre) {
      fprintf(TFile," - preprocessing ");
    } else {
      fprintf(TFile," - main ");
    }
    fprintf(TFile,"<<<<\n");

    if (EBO_Trace_Data_Flow) {
      Print_All_BBs ();
    }
  }

 /* TEMPORARY - EBO doesn't understand rotating registers, so skip blocks that use them. */
  for (bb = first_bb; bb != NULL; bb = BB_next(bb)) {
    if (BB_rotating_kernel(bb)) Set_BB_visited(bb);
    #ifdef KEY
      if (BB_swp_kernel(bb)) Set_BB_visited(bb);
    #endif
  }

#ifdef TARG_MIPS
  if (EBO_in_peep) // Can do this once after all transformations
    Redundancy_Elimination();
#endif
  for (bb = first_bb; bb != NULL; bb = BB_next(bb)) {
    RID *bbrid;
    if (( bbrid = BB_rid( bb )) &&
        ( RID_level( bbrid ) >= RL_CGSCHED ) ) {
     /*
      * There is no overlap in TN's between the current REGION
      * and REGIONs which have already been through CG
      */
      continue;
    }
    if (!BB_visited(bb)) {
      EBO_Add_BB_to_EB (bb);
#ifdef TARG_X8664
      if (EBO_in_loop) {
        if (CG_LOOP_unroll_level == 2) {
          if (Is_Multibb_Loop(EBO_loop) == FALSE)
            break;
        } else {
          break;
        }
      }
#else
      if (EBO_in_loop) break;
#endif
    }
  }

 /* Clear the bb flag, in case some other phase uses it. */
  clear_bb_flag (first_bb);

  if (rerun_cflow) {
    CFLOW_Optimize(CFLOW_BRANCH | CFLOW_UNREACHABLE, "CFLOW (from ebo)");
  }

  EBO_Finish();

  if (EBO_Trace_Data_Flow) {
    #pragma mips_frequency_hint NEVER
    fprintf(TFile,">>>> After EBO");
    if (EBO_in_peep) {
      fprintf(TFile," - in peep ");
    } else if (EBO_in_before_unrolling) {
      fprintf(TFile," - before unrolling ");
    } else if (EBO_in_after_unrolling) {
      fprintf(TFile," - after unrolling ");
    } else if (EBO_in_pre) {
      fprintf(TFile," - preprocessing ");
    } else {
      fprintf(TFile," - main ");
    }
    fprintf(TFile,"<<<<\n");
    fprintf(TFile,">>>> %d tninfo entries allocated and reused %d times\n",
    EBO_num_tninfo_entries,EBO_tninfo_entries_reused);
    fprintf(TFile,">>>> %d opinfo entries allocated and reused %d times\n",
    EBO_num_opinfo_entries,EBO_opinfo_entries_reused);
    Print_All_BBs ();
  }

}

/* =======================================================================
 * =======================================================================
 *
 * There are a number of different entry points to EBO.  They differ because
 * the specific information available changes throughout the compilation.
 * The flags that are set at each entry are used during EBO if a use is to
 * be made of information that is only available at certain times.
 *
 * =======================================================================
 * =======================================================================
 */

/* 
 * perform EB optimizations right after instruction translation.
 */
void
EBO_Pre_Process_Region ( RID *rid )
{
  BB *first_bb = (rid) ? CGRIN_first_bb(RID_cginfo( rid )) : REGION_First_BB;

  EBO_in_pre  = TRUE;
  EBO_in_before_unrolling = FALSE;
  EBO_in_after_unrolling = FALSE;
  EBO_in_peep = FALSE;

  EBO_in_loop = FALSE;

#ifdef KEY
  EBO_can_delete_branch_delay_OP = TRUE;
  EBO_no_liveness_info_available = FALSE;
  EBO_optimize_single_BB = FALSE;
#endif

  if ((EBO_Opt_Level < 5) && ((EBO_Opt_Level > 0) || (EBO_Opt_Level != -5))) return;

  clear_bb_flag (first_bb);
  EBO_Process (first_bb);
}

  
/* 
 * perform EBO optimizations during unrolling and pipelining
 */
void
EBO_before_unrolling(BB_REGION *bbr )
{
  INT i;
  EBO_in_pre  = FALSE;
  EBO_in_before_unrolling = TRUE;
  EBO_in_after_unrolling = FALSE;
  EBO_in_peep = FALSE;

  EBO_in_loop = TRUE;

#ifdef KEY
  EBO_can_delete_branch_delay_OP = TRUE;
  EBO_no_liveness_info_available = FALSE;
  EBO_optimize_single_BB = FALSE;
#endif

  if ((EBO_Opt_Level < 4) && ((EBO_Opt_Level > 0) || (EBO_Opt_Level != -4))) return;

#if defined(TARG_IA64) || defined(KEY)
  for (i = 0; i < bbr->entries.size(); i++) {
    clear_bb_flag (bbr->entries[i]);
  }
  for (i = 0; i < bbr->exits.size(); i++) {
    Set_BB_visited (bbr->exits[i]);
  }
  EBO_Process (bbr->entries[0]);
#endif
}

  
/* 
 * perform EBO optimizations after unrolling and pipelining
 */
void
EBO_after_unrolling(BB_REGION *bbr, LOOP_DESCR *loop, int loop_iter_size)
{
  INT i;

  EBO_in_pre  = FALSE;
  EBO_in_before_unrolling = FALSE;
  EBO_in_after_unrolling = TRUE;
  EBO_loop = loop;
  EBO_loop_size = loop_iter_size;
  EBO_in_peep = FALSE;

  EBO_in_loop = TRUE;

#ifdef KEY
  EBO_can_delete_branch_delay_OP = TRUE;
  EBO_no_liveness_info_available = FALSE;
  EBO_optimize_single_BB = FALSE;
#endif

  if ((EBO_Opt_Level < 3) && ((EBO_Opt_Level > 0) || (EBO_Opt_Level != -3))) return;

#if defined(TARG_IA64) || defined(KEY)
  for (i = 0; i < bbr->entries.size(); i++) {
    clear_bb_flag (bbr->entries[i]);
  }
  for (i = 0; i < bbr->exits.size(); i++) {
    Set_BB_visited (bbr->exits[i]);
  }
  EBO_Process (bbr->entries[0]);
#endif

  EBO_loop = NULL;
}

  
/* 
 * perform EB optimizations on a region
 */
void
EBO_Process_Region ( RID *rid )
{
  BB *first_bb = (rid) ? CGRIN_first_bb(RID_cginfo( rid )) : REGION_First_BB;

  EBO_in_pre  = FALSE;
  EBO_in_before_unrolling = FALSE;
  EBO_in_after_unrolling = FALSE;
  EBO_in_peep = FALSE;

  EBO_in_loop = FALSE;

#ifdef KEY
  EBO_can_delete_branch_delay_OP = TRUE;
  EBO_no_liveness_info_available = FALSE;
  EBO_optimize_single_BB = FALSE;
#endif

  if ((EBO_Opt_Level < 2) && ((EBO_Opt_Level > 0) || (EBO_Opt_Level != -2))) return;

  clear_bb_flag (first_bb);
  EBO_Process (first_bb);
}


/* 
 * perform EB optimizations after register assignment.
 */
void
EBO_Post_Process_Region ( RID *rid )
{
  BB *first_bb = (rid) ? CGRIN_first_bb(RID_cginfo( rid )) : REGION_First_BB;

  EBO_in_pre  = FALSE;
  EBO_in_before_unrolling = FALSE;
  EBO_in_after_unrolling = FALSE;
  EBO_in_peep = TRUE;

  EBO_in_loop = FALSE;

#ifdef KEY
  EBO_can_delete_branch_delay_OP = TRUE;
  EBO_no_liveness_info_available = FALSE;
  EBO_optimize_single_BB = FALSE;
#endif

  if ((EBO_Opt_Level < 1) && ((EBO_Opt_Level >= 0) || (EBO_Opt_Level != -1))) return;

  /* compute live-in sets for physical registers */
  MEM_POOL_Push(&MEM_local_pool);
  REG_LIVE_Analyze_Region();

  clear_bb_flag (first_bb);
#ifdef KEY
  removed_ops = 0;
#endif
  EBO_Process (first_bb);
  REG_LIVE_Finish();
#ifdef KEY
  // We need to recreate liveness info and run EBO again to delete op
  // that is useless after previous EBO run.
  // Also, we need to run reg life analyze after copy propagation.
  //if (removed_ops)
  {
    REG_LIVE_Analyze_Region();
    clear_bb_flag (first_bb);
    EBO_Process (first_bb);
    REG_LIVE_Finish();
  }
#endif
  MEM_POOL_Pop(&MEM_local_pool);
}


#ifdef KEY
// Run EBO after second instruction scheduling phase (after register
// allocation).
void
EBO_Post_Process_Region_2 ( RID *rid )
{
  BB *first_bb = (rid) ? CGRIN_first_bb(RID_cginfo( rid )) : REGION_First_BB;

  EBO_in_pre  = FALSE;
  EBO_in_before_unrolling = FALSE;
  EBO_in_after_unrolling = FALSE;
  EBO_in_peep = TRUE;

  EBO_in_loop = FALSE;

  EBO_can_delete_branch_delay_OP = FALSE;
  EBO_no_liveness_info_available = TRUE;
  EBO_optimize_single_BB = TRUE;

  if ((EBO_Opt_Level < 6) && ((EBO_Opt_Level >= 0) || (EBO_Opt_Level != -6)))
    return;

  /* compute live-in sets for physical registers */
  MEM_POOL_Push(&MEM_local_pool);
  REG_LIVE_Analyze_Region();

  clear_bb_flag (first_bb);
  removed_ops = 0;
  EBO_Process (first_bb);
  REG_LIVE_Finish();

  MEM_POOL_Pop(&MEM_local_pool);
}

#ifdef TARG_X8664
// If OP references a symbol called lra_spill_temp_xxx, return xxx; else return
// -1.
static int
Get_LRA_Spill_Temp_Number(OP *op)
{
  int base_idx, offset_idx;

  base_idx = OP_find_opnd_use(op, OU_base);
  if (base_idx >= 0 &&
      OP_opnd(op, base_idx) == SP_TN) {
    offset_idx = OP_find_opnd_use(op, OU_offset);
    if (offset_idx >= 0) {
      TN *offset_tn = OP_opnd(op, offset_idx);
      if (offset_tn != NULL &&
	  TN_is_symbol(offset_tn)) {
	ST *st = TN_var(offset_tn);
	char *name = ST_name(st);
	if (strstr(name, "lra_spill_temp_") != NULL) {
	  char *p = name + 15;	// get the number part
	  int id = atoi(p);
	  return id;
	}
      }
    }
  }
  return -1;
}

static void
Init_Remove_Dead_LRA_Stores(BS **bs, MEM_POOL *pool)
{
  // Indexed by xxx in lra_spill_temp_xxx.  Set to 1 if the corrsponding
  // spilled value is used.
  *bs = BS_Create_Empty(Spill_Var_Cnt + 1, pool);
}

static BOOL
Delete_Dead_LRA_Spill(OP *op, BS **bs)
{
  if (!OP_store(op))
    return FALSE;

  int id = Get_LRA_Spill_Temp_Number(op);

  // Delete store if the stored value was never used.
  if (id > 0 &&
      !BS_MemberP(*bs, id)) {
    OP_Change_To_Noop(op);
    return TRUE;
  }
  return FALSE;
}

// If a memory OP loads from lra_spill_temp_xxx, then mark the spilled value
// used.  Delete stores whose spilled value is never used.
static void
Mark_LRA_Spill_Reference(OP *op, BS **bs, MEM_POOL *pool)
{
  if (OP_store(op))
    return;

  // The spilled value is used.  Mark it so.
  int id = Get_LRA_Spill_Temp_Number(op);
  if (id > 0) {
    *bs = BS_Union1D(*bs, id, pool);
  }
}
#endif	// TARG_X8664

#if 0
// We can not say if a particular address will be taken outside of the 
// current extended basic block unless we look at the Symbol Table. 
// Currently, this support is not provided. But, this routine does some 
// useful processing and may be useful at a later point.
// Before a "store" op leaves the opinfo table,
// look for any dependent op and if none exist, then delete
// this "store" op
void
delete_useless_store_op (EBO_OP_INFO *opinfo)
{
  EBO_TN_INFO *save_last_tninfo = EBO_last_tninfo;

  BOOL found_dependent_op = FALSE;

  OP *store_op = opinfo->in_op;

  /* Determine operand TN info */
  BB *bb = opinfo->in_bb; 
  EBO_TN_INFO *tninfo;
  TN *op_predicate_tn = NULL;
  EBO_TN_INFO *op_predicate_tninfo = NULL;
  BOOL check_omegas = FALSE;
  INT max_opnds = OP_MAX_FIXED_OPNDS;
  EBO_TN_INFO **opnd_tninfo = TYPE_ALLOCA_N(EBO_TN_INFO *, max_opnds);
  INT opndnum;
  mUINT8 operand_omega;
  check_omegas = (EBO_in_loop && _CG_LOOP_info(store_op))?TRUE:FALSE;
  TN *tn;
  for (opndnum = 0; opndnum < OP_opnds(store_op); opndnum ++) {
    tn = OP_opnd(store_op, opndnum);
    if (tn == NULL || TN_is_constant(tn) || TN_is_label(tn)) {
      opnd_tninfo[opndnum] = NULL;
      continue;
    }
    operand_omega = check_omegas ? OP_omega(store_op,opndnum) : 0;
    tninfo = tn_info_use (bb, store_op, tn,
			  op_predicate_tn, op_predicate_tninfo,
			  operand_omega);
    opnd_tninfo[opndnum] = tninfo;
  }

  /* Determine the indexes of the address components of this "store" op. */
  INT store_base_idx = TOP_Find_Operand_Use(OP_code(store_op),OU_base);
  INT store_offset_idx = TOP_Find_Operand_Use(OP_code(store_op),OU_offset);

  /* Determine the address components of this "store" op. */
  TN *store_base_tn = 
    (store_base_idx >= 0) ? OP_opnd(store_op, store_base_idx) : NULL;
  EBO_TN_INFO *store_base_tninfo = 
    (store_base_idx >= 0) ? opnd_tninfo[store_base_idx] : NULL;
  TN *store_offset_tn = 
    (store_offset_idx >= 0) ? OP_opnd(store_op, store_offset_idx) : NULL;
  EBO_TN_INFO *store_offset_tninfo = 
    (store_offset_idx >= 0) ? opnd_tninfo[store_offset_idx] : NULL;

  if ((store_offset_tn == NULL) && (store_base_tn != NULL)) {
    find_index_and_offset(store_base_tninfo,
                          &store_base_tn, &store_base_tninfo,
                          &store_offset_tn, &store_offset_tninfo);
  }

  for (; bb != NULL; bb = BB_next(bb)) {
    OP *op;
    FOR_ALL_BB_OPs (bb, op) {
      if (!OP_load(op))
        continue;	
      check_omegas = (EBO_in_loop && _CG_LOOP_info(op))?TRUE:FALSE;
      TN *tn;
      for (opndnum = 0; opndnum < OP_opnds(op); opndnum ++) {
	tn = OP_opnd(op, opndnum);
	if (tn == NULL || TN_is_constant(tn) || TN_is_label(tn)) {
	  opnd_tninfo[opndnum] = NULL;
	  continue;
	}
    	operand_omega = check_omegas ? OP_omega(op,opndnum) : 0;
        tninfo = tn_info_use (bb, op, tn,
                              op_predicate_tn, op_predicate_tninfo,
                              operand_omega);
        opnd_tninfo[opndnum] = tninfo;
      }
      if (EBO_hash_op(op, opnd_tninfo) == opinfo->hash_index) {
	INT base_idx = 0;
	INT offset_idx = 0;
	TN *base_tn = NULL;
	EBO_TN_INFO *base_tninfo = NULL;
	TN *offset_tn = NULL;
	EBO_TN_INFO *offset_tninfo = NULL;

	/* Determine the address components of this "load" op. */
	base_idx = TOP_Find_Operand_Use(OP_code(op),OU_base);
	offset_idx = TOP_Find_Operand_Use(OP_code(op),OU_offset);
	base_tn = (base_idx >= 0) ? OP_opnd(op,base_idx) : NULL;
	base_tninfo = (base_idx >= 0) ? opnd_tninfo[base_idx] : NULL;
	offset_tn = (offset_idx >= 0) ? OP_opnd(op,offset_idx) : NULL;
	offset_tninfo = (offset_idx >= 0) ? opnd_tninfo[offset_idx] : NULL;

	if ((offset_tn == NULL) && (base_tn != NULL)) {
	  find_index_and_offset(base_tninfo,
				&base_tn, &base_tninfo,
				&offset_tn, &offset_tninfo);
	}

	BOOL hash_op_matches = ((base_tn == store_base_tn) &&           /* The base  index must match */
				(base_tninfo == store_base_tninfo) &&   /* The base   info must match */
				(offset_tninfo == store_offset_tninfo)) /* The offset info must match */
	  ? TRUE : FALSE;
	if (!hash_op_matches) // bases do not match
	  continue;
	BOOL op_is_subset = FALSE;
	BOOL offsets_may_overlap = TRUE;
	
 	if (offset_tn == store_offset_tn) {
	  // can not delete this "store" op
	  found_dependent_op = TRUE;
	  break;
	}
	
	/* The offset tn's need to be looked at in more detail. */	
	ST *symbol = ((offset_tn != NULL) && TN_is_symbol(offset_tn)) ?TN_var(offset_tn) : NULL;
	ST *symbol_store = ((store_offset_tn != NULL) && TN_is_symbol(store_offset_tn)) ?TN_var(store_offset_tn) : NULL;
	mUINT8 relocs = (offset_tn != NULL) ? TN_relocs(offset_tn) : 0;
	mUINT8 relocs_store = (store_offset_tn != NULL) ? TN_relocs(store_offset_tn) : 0;
	INT64 offset = 0;
	INT64 offset_store = 0;
	offset = (offset_tn != NULL) ? TN_offset(offset_tn) : 0;
	offset_store = (store_offset_tn != NULL) ? TN_offset(store_offset_tn) : 0;

	/* This time, the relocations must be the same. */
	hash_op_matches = (symbol == symbol_store) && (relocs == relocs_store);
	if (!hash_op_matches) // relocations do not match
	  continue;

	/* If the relocations are the same, we need to examine the offsets and sizes. */
	INT size = CGTARG_Mem_Ref_Bytes(op);
	INT size_store = CGTARG_Mem_Ref_Bytes(store_op);
	
	if ((offset == offset_store) &&
	    (size == size_store)) {
	  /* The perfect match: location and size. */
	  // can not delete
	  found_dependent_op = TRUE;
	  break;
	} else if ((offset >= offset_store) &&
		    ((offset + size) <= (offset_store + size_store))) {
	  /* The current reference is a subset of the preceeding one. */
	  // can not delete
	  found_dependent_op = TRUE;
	  break;
	} else if (((offset_store + size_store) <= offset) ||
		   ((offset + size) <= offset_store)) {
	  /* There is no potential overlap. */
	} else {
	  /* Any other case may be a potential conflict. */
	  hash_op_matches = FALSE;
	}
      }
    }
    if (found_dependent_op)
      break; 
  }
  if (found_dependent_op == FALSE) {
    OP_Change_To_Noop(store_op);
    printf("store op %#x does not have any dep op\n", store_op);
  }
  backup_tninfo_list(save_last_tninfo);
  return;
}
#endif

// For each of BB's predecessors, if it branches to any OP other than the first
// OP in BB, retarget it to branch to one OP earlier.
static void
EBO_Adjust_Pred_Branch_Target (BB *bb)
{
  BB *fall_thru_pred = BB_Fall_Thru_Predecessor(bb);
  BBLIST *edge;

  FOR_ALL_BB_PREDS(bb, edge) {
    BB *pred = BBLIST_item(edge);
    if (pred != fall_thru_pred) {
      OP *xfer_op = BB_last_op(pred);
      if (!OP_xfer(xfer_op))
	xfer_op = OP_prev(xfer_op);
      Is_True(xfer_op != NULL && OP_xfer(xfer_op),
	      ("EBO_Adjust_Pred_Branch_Target: cannot find xfer_op"));
      for (int i = 0; i < OP_opnds(xfer_op); i++) {
	TN *opnd_tn = OP_opnd(xfer_op, i);
	if (TN_is_label(opnd_tn) &&
	    TN_offset(opnd_tn) > 0) {
	  TN *new_label_tn = Gen_Adjusted_TN(opnd_tn, -4);
	  Set_OP_opnd(xfer_op, i, new_label_tn);
	}
      }
    }
  }
}
#endif
