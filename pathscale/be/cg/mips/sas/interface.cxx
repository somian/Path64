/*
 * Copyright (C) 2007 PathScale, LLC.  All Rights Reserved.
 */

/*
 * Copyright (C) 2007. QLogic Corporation. All Rights Reserved.
 */

/*
 * Copyright 2005 PathScale, Inc.  All Rights Reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it would be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Further, this software is distributed without any warranty that it is
 * free of the rightful claim of any third person regarding infringement
 * or the like.  Any license provided herein, whether implied or
 * otherwise, applies only to this software file.  Patent licenses, if
 * any, provided herein do not apply to combinations of this program with
 * other software, or any other product whatsoever.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write the Free Software Foundation, Inc., 59
 * Temple Place - Suite 330, Boston MA 02111-1307, USA.
 */

#include "comm.h"
#include "modulo.h"

#include "bb.h"
#include "tn.h"
#include "op.h"
#include "cg_loop.h"
#include "cg_vector.h"
#include "cg_sas.h"


MEM_POOL sas_pool;


// SBB::Prune_Redundant_Arcs uses STNs to prune redundant arcs.
// Currently, this is the only code that uses TNs.  If that algorithm
// were merged into Recreate_DDG, we could eliminate STNs altogether.
// The only STN flag we use is TN_is_register.

static void Setup_TN (TN * in, STN * tn)
{
  if (TN_is_register (in))
  {
    Set_STN_is_register (tn);
    Set_STN_register (tn, TN_register (in));
  }
  else if (TN_is_constant (in))
  {
    Set_STN_is_constant (tn);
    if (TN_is_symbol (in))
      Set_STN_is_symbol (tn);
    else if (TN_is_label (in))
      Set_STN_is_label (tn);
    else if (TN_is_enum (in))
      Set_STN_is_label (tn); // why ?????????????????
    else if (TN_has_value (in))
    {
      Set_STN_has_value (tn);
      Set_STN_value (tn, TN_value (in));
    }
   else Fail_FmtAssertion ("Setup_TN: Const TN not sym/lab/enum/val");
  }
}


// Recreate_DDG converts the BB into an SBB object (OPs to OPRs, TNs to
// STNs), annotates SBB with dependence arcs and scheduling info, then
// invokes the scheduler, then copies the resulting order numbers back
// to the original BB OPs.

void Recreate_DDG (BB * bb, int nOps, BOOL cyclic,
		   int * res_mii, int * rec_mii, int * mii)
{
 // Allocate memory pool for scheduler.
 if (getenv ("DEBUG_SAS"))
   MEM_POOL_Initialize (&sas_pool, "SAS_pool", FALSE);
 else
   MEM_POOL_Initialize (&sas_pool, "SAS_pool", TRUE);
 MEM_POOL_Push (&sas_pool);

 {
  SBB sbb (MUTE);
  sbb.Init (nOps, cyclic);

  OP *op;
  FOR_ALL_BB_OPs (bb, op)
  {
    // Prepare to convert op to Current_OP
    FmtAssert( BB_id(OP_bb(op)) == BB_id(bb), ("") );
    OPR * Current_OP = (OPR *) sbb.Opr_Blk (OP_info_indx (op));
    OPR_results(Current_OP) = OP_results(op);
    ASSERT( OPR_results(Current_OP) <= 1 );
    
    // Convert result TNs of op into result STNs of Current_OP
    for (int i = 0; i < OP_results (op); i++) {
      STN * tn = OPR_result (Current_OP);
      OPR_results (Current_OP)++;
      TN * in = OP_result (op, i);
      Setup_TN (in, tn);
    }

    // Initialize scheduling info for Current_OP
    Setup_OPR (Current_OP, TOP_Name (OP_code (op)));
    BOOL loop_op = Is_CG_LOOP_Op (op);
    Set_OPR_ncycles (Current_OP, OPR_throughput [OPR_opcode (Current_OP)]);

    // Convert operand TNs of op into operand STNs of Current_OP
    for (int i = 0; i < OP_opnds (op); i++) {
      TN * in = OP_opnd (op, i);
      STN * tn = OPR_opnd (Current_OP, OPR_opnds (Current_OP));
      OPR_opnds (Current_OP)++;
      Setup_TN (in, tn);
      if (loop_op) {
        int omega = TN_is_symbol (in) ? OP_restore_omega (op) :
	                                OP_omega (op, i);
        Set_STN_omega (tn, omega);
      }
    }

    // Convert dependence arcs of op into dependency arcs of Current_OP
    for( ARC_LIST* arcs = OP_succs(op);
	 arcs != NULL; arcs = ARC_LIST_rest(arcs) )
    {
      ARC* arc = ARC_LIST_first(arcs);
      OP* succ_op = ARC_succ(arc);
      uint16_t succ_id = OP_info_indx(succ_op);
      CG_DEP_KIND kind = ARC_kind(arc);

      DEP_TYPE type = DEP_MISC;
      int latency = 0;
      int opnd = MAX_OPNDS;
      if (kind == CG_DEP_REGIN)
      {
        type = DEP_REGIN;
	latency = ARC_latency (arc);
	opnd = ARC_opnd (arc);
      }
      else if (kind == CG_DEP_REGANTI)
      {
        type = DEP_REGANTI;
	opnd = ARC_opnd (arc);
      }
      else if (kind == CG_DEP_REGOUT)
        type = DEP_REGOUT;

      sbb.Add_Arc (Current_OP, (OPR*)sbb.Opr_Blk (succ_id),
		   type, opnd, latency, ARC_omega (arc));
    }
  }

  // Invoke scheduler on sbb
  SCHED sched (&sbb, MUTE);

  // Copy results of scheduler to output parameters
  if (cyclic) {
    *mii = sched.Get_mii();
    *rec_mii = sched.Get_rec_mii();
    *res_mii = sched.Get_res_mii();
  }

  // Copy order and cycle numbers (assigned by scheduler) from sbb to bb.
  sched.Emit_Schedule (bb /* BB* */);
 }

 // Free memory used by scheduler
 MEM_POOL_Pop (&sas_pool);
 MEM_POOL_Delete (&sas_pool);
}
