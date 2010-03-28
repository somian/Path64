/*
 * Copyright (C) 2008 PathScale, LLC. All Rights Reserved.
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


#define __STDC_LIMIT_MACROS
#include <stdint.h>
#define USE_STANDARD_TYPES
#include <map>
#include "defs.h"
#include "mempool.h"
#include "tn.h"
#include "tn_set.h"
#include "bb.h"
#include "op.h"
#include "op_list.h"
#include "op_map.h"
#include "cgexp.h"
#include "cgtarget.h"
#include "register.h"
#include "cg_loop.h"
#include "cg_swp_options.h"
#include "cg_swp_target.h"
#include "tracing.h"
#include "pf_cg.h"
#include "cg_loop.h"
#include "calls.h"
#include "tag.h"

#ifdef TARG_IA64
  #include "cg_swp.h"
#else
  #include "cg_swp_op_vector.h"
#endif

#include <cmath>

using namespace std;



static BOOL mops_critical;
static BOOL flops_critical;
static BOOL iops_critical;


void 
SWP_Exp_COPY (TN *result, TN *opnd, OPS *ops) {
  Exp_COPY (result, opnd, ops);

  return;
}


INT32 
SWP_Max_Slots_Per_Cycle () {
  
  return 2;
}


void 
SWP_Loop_Init_Fini (BOOL is_doloop,
                    INT stage_count,
                    OPS *prolog_ops,
                    OPS *body_ops,
                    OPS *epilog_ops) 
{

  return;
}


BOOL 
Prepare_Loop_For_SWP_1 (CG_LOOP& cl, bool trace) {

  return TRUE;
}


BOOL 
Prepare_Loop_For_SWP_2 (CG_LOOP& cl, bool trace) {

  return TRUE;
}


void 
Convert_While_Loop_to_Fully_Predicated_Form (CG_LOOP& cl) {

  return;
}


BOOL 
Remove_Non_Definite_Dependence (CG_LOOP &cl, bool cg_loop_init, bool trace) {
  
  return TRUE;
}


BASE_UPDATE 
OP_base_update_kind (OP *op) {
    
  return NO_BASE_UPDATE;
}


// Returns the address base opnd
INT 
OP_base_opnd_num (OP *op) {

  return -1;
}


// Returns the address base opnd
INT 
OP_base_res_num (OP *op) {

  return -1;
}


INT32 
OP_incr_opnd_num (OP *op)
{
  // the last operand
  return (OP_opnds(op) - 1);
}


BOOL 
Imm_Value_In_Range (OP *op, INT64 imm) {
    
  return FALSE;
}


void 
Gen_SWP_Branch (CG_LOOP &cl, bool is_doloop) {

  return;
}


void 
Gen_SWP_Branch_Predict (BB *body, BB *prolog, BB *epilog) {

  return;
}


void 
Undo_SWP_Branch (CG_LOOP &cl, bool is_doloop) {

  return;
}


void 
Gen_Implicit_Prefetches (CG_LOOP &cl, bool trace) {
  // TODO: For now we will delete all prefetches
  // WARNING: If we don't do this for MIPS, the software
  // pipeliner will seg. fault or assert due to BAD II

  /* PathScale Bug 14873
   * Don't delete prefetches for Twice9.
   */
  if (Is_Target_twc9a ()) return;

  
  LOOP_DESCR *loop = cl.Loop ();
  BB *body = LOOP_DESCR_loophead (loop);
  OP *op;

  OP *next_op;
  for (op = BB_first_op (body); op != NULL;  op = next_op) {
    next_op = OP_next(op);
    if (OP_prefetch (op)) {
      if (trace) {
        fprintf (TFile, "Delete prefetch OP:\n");
        Print_OP (op, TFile);
      }
      DevWarn ("Implement handling of prefetches in SWP");

      BB_Remove_Op (body, op);
    }    
  }

  return;
}


TN* 
Base_update_tn (OP *op) { 
  
  return 0;
}


void
SWP_TARGET_Calculate_Critical_Resources (const SWP_OP_VECTOR& ops,
                                         INT ii)
{
  INT num_mops  = 0;
  INT num_flops = 0;
  INT num_iops  = 0;

  for (INT i = 1; i < ops.size () - 1; ++i) {
    OP *op = ops(i).Get_OP ();
            
    if (OP_memory (op) || OP_fp_load (op) || OP_fp_store (op)) {
      ++num_mops;
    }
    
    if (OP_flop (op)) {
      ++num_flops;
    }
    else if (!OP_br (op)) {
     ++num_iops;
    }
  }

  mops_critical  = 
       (static_cast<float> (num_mops) / (ii - 1)) 
    >= SWP_Options.Critical_Threshold / 100.0;
  
  flops_critical = 
       (static_cast<float> (num_flops) / (ii - 1)) 
    >= SWP_Options.Critical_Threshold / 100.0;
  
  iops_critical  = 
       (static_cast<float> (num_iops) / (ii - 1)) 
    >= SWP_Options.Critical_Threshold / 100.0;
  
  return;
}


float
SWP_TARGET_Get_Scale_Factor (OP *op) {
  float scale = 1.0f;

  if (   mops_critical 
      && (OP_memory (op) || OP_fp_load (op) || OP_fp_store (op))) 
  {
    scale *= 0.5;
  } 
  else if (flops_critical && OP_flop (op)) {
    scale *= 0.5;
  }
  else if (iops_critical) {
    scale *= 0.5;
  }

  TOP topcode = OP_code (op);
  switch (topcode) {
    /* latency = 15, repeat rate = 10 */
    case TOP_recip_s:
      scale *= 1.0f / 10.0f;
      break;
    
    /* latency = 23, repeat rate = 18 */
    case TOP_recip_d:
      scale *= 1.0f / 18.0f;
      break;
 
    /* latency = 19, repeat rate = 14 */
    case TOP_rsqrt_s:
      scale *= 1.0f / 14.0f;
      break;
    
    /* latency = 31, repeat rate = 26 */
    case TOP_rsqrt_d:
      scale *= 1.0f / 26.0f;
      break;
    

    /* latency = 17, repeat rate = 14 */
    case TOP_div_s:
    case TOP_sqrt_s:
      scale *= 1.0f / 14.0f;
      break;
 
    /* latency = 32, repeat rate = 29 */
    case TOP_div_d:
    case TOP_sqrt_d:
      scale *= 1.0f / 29.0f;
      break;
     

    default:
      break;
  }
  
  return scale;
}


INT
SWP_TARGET_Get_Branch_Cycle (INT ii) { 

  return (ii - 2);
}


INT 
SWP_TARGET_Scheduling_Constrain_For_OP (const SWP_OP_VECTOR& ops,
                                        INT candidate,
                                        INT cycle,
                                        INT ii)
{
  INT ret;
  OP *op      = ops(candidate).Get_OP ();
  TOP topcode = OP_code (op);

  switch (topcode) {
    case TOP_beq:
    case TOP_bgez:
    case TOP_bgezal:
    case TOP_bgtz:
    case TOP_blez:
    case TOP_bltz:
    case TOP_bltzal:
    case TOP_bne:     
      ret = ii - 2;
      break;

    default:
      ret = cycle;
 
      // Don't schedule an OP in the same cycle as the branch op. This
      // would be fixed anyway the next time the branch op is scheduled,
      // but it reduces unneccessary backtracking.
      if (cycle % ii == ii-2) {
        ret = ++cycle;
      }
     
      
      // Only issue one OP in the branch delay slot.
      if (cycle % ii == ii-1) {
        for (INT i = 0; i < ops.size (); ++i) {
          if (i == ops.Get_Start_Idx ()) continue;
          if (i == ops.Get_Stop_Idx ()) continue;

          if (   ops(i).Is_Scheduled ()
              && (ops(i).Get_Cycle () % ii == ii-1))
          {
            // There is already an OP scheduled in this cycle
            ret = ++cycle;
            break;
          }
        }
      }
      
      // to ensure forward progress
      if (ret == ops(candidate).Get_Cycle ()) {
        ret++;
      }

      break;
  }

  return ret;
}


void
SWP_TARGET_Adjust_LoadStore_Offset (SWP_OP_VECTOR& ops, 
                                    INT start,
                                    TN *tn,
                                    INT offset)
{
  INT stage = ops(start).Get_Stage ();

  for (INT i = 0; i < ops.size (); ++i) {
    OP *op = ops(i).Get_OP ();

    if (!OP_memory (op)) continue;

    INT base_num   = OP_find_opnd_use (op, OU_base);
    INT offset_num = OP_find_opnd_use (op, OU_offset);
    
    if (offset_num < 0) continue;
    
    TN *base_tn    = OP_opnd (op, base_num);
    TN *offset_tn  = OP_opnd (op, offset_num);

    if (base_tn != tn) continue;

    INT omega = ops(i).Get_Omega (base_num);

    if (i < start) {
      omega = omega - 1;
    }

    if (omega == 0) continue;

    INT new_offset_value = TN_value (offset_tn) - offset * omega;
    
    if (!ISA_LC_Value_In_Class (new_offset_value, LC_simm16)) {
      continue;
    }

    TN *new_offset_tn = 
      Gen_Literal_TN (new_offset_value,
                      TN_size (offset_tn));

    Set_OP_opnd (op, offset_num, new_offset_tn);
    
    INT diff = stage - ops(i).Get_Stage ();
    if (i < start) {  
      ops(i).Set_Omega (base_num, 1);
      ops(i).Set_Original_Omega (base_num, diff+1);
    }
    else {
      ops(i).Set_Omega (base_num, 0);
      ops(i).Set_Original_Omega (base_num, diff);
    }
    
    ops(start).Set_Pointer_Update ();
  }

  return;
}


void
SWP_TARGET_Adjust_LoadStore_Offset (SWP_OP_VECTOR& ops, BOOL trace) {
  INT offset_index = 1;
  for (INT i = 0; i < ops.size (); ++i) {
    OP *op = ops(i).Get_OP ();

    TOP top = OP_code (op);
    if (top != TOP_addiu && top != TOP_daddiu) continue;

    TN *result = OP_result (op, 0);
    TN *opnd   = OP_opnd   (op, 0);   

    if (result != opnd) continue;

    TN *offst = OP_opnd (op, 1);
    
    if (!(TN_is_constant (offst) && TN_has_value (offst))) continue;

    if (   offset_index < SWP_Options.Offset_Skip_Before
        || offset_index > SWP_Options.Offset_Skip_After
        || offset_index == SWP_Options.Offset_Skip_Equal)
    {
      if (trace) {
        fprintf (TFile, 
                 "Skip Load/Store Offset Adjustment for TN%d (index=%d)\n",
                 TN_number (opnd),
                 offset_index
                );
      }
    }
    else {
      if (trace) {
        fprintf (TFile, 
                 "Attempting Load/Store Offset Adjustment for TN%d (index=%d)\n",
                 TN_number (opnd),
                 offset_index
                );
      }
      SWP_TARGET_Adjust_LoadStore_Offset (ops, i, opnd, TN_value (offst));
    }
  
    offset_index += 1;
  }

  return;
}

