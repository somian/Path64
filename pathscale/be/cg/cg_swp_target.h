/*
 * Copyright (C) 2008 PathScale, LLC. All Rights Reserved.
 */

/* 
 * Copyright (C) 2007. QLogic Corporation. All Rights Reserved.
 */
 
/*
 * Copyright (C) 2003, 2004, 2005, 2006 PathScale, Inc.  All Rights Reserved.
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
 *  Module: cg_swp_target.cxx
 *  $Revision: 1.7 $
 *  $Date: 05/10/28 15:58:56-07:00 $
 *  $Author: gautam@jacinth.keyresearch $
 *  $Source: be/cg/SCCS/s.cg_swp_target.h $
 *
 *  Description:
 *  ============
 *
 *  SWP Target Specific 
 *
 *  Here are various miscellaneous functions to provide machine dependent
 *  information.
 *
 *  void SWP_Exp_COPY(result, opnd, OPS *ops)
 *    Generate a copy from opnd to result.   Different from Exp_COPY(),
 *    it supports copy of predicates.
 *
 *  void SWP_Loop_Init_Fini(bool is_doloop,
 *		  	    TN *trip_count_tn,
 *			    INT stage_count,
 *			    TN *label_tn,
 *			    OPS *prolog_ops,
 *			    OPS *body_ops
 *                          OPS *epilog_ops);
 *
 *    Generate the branch operation needed by SWP.  Generate br.ctop for
 *    doloop.  Generate br.wtop for while-loop.  Generate the init/fini
 *    for rotating register bases.
 *  
 *  BOOL Prepare_Loop_For_SWP(CG_LOOP& cl)
 *    Prepare a loop for SWP, including removing all predicate
 *    invariants, and removing non-definite dependence.   If some 
 *    non-definite dependence cannot be removed, then return FALSE.
 *    Convert to postincr form if necessary.
 *
 *  void Convert_While_Loop_to_Fully_Predicated_Form(CG_LOOP& cl, bool trace)
 *    Convert a while-loop into the fully predicated form.
 *    Require CG_LOOP_Info.
 *
 * =======================================================================
 * ======================================================================= */

#ifndef cg_swp_target_INCLUDED
#define cg_swp_target_INCLUDED "cg_swp_target.h" 

#include "tn.h"
#include "op.h"

#ifndef TARG_IA64
  #include "cg_swp_op_vector.h"
#endif

class CG_LOOP;

// Define BASE_UPDATE kind
enum BASE_UPDATE {
  NO_BASE_UPDATE  = 0,  // OP does not have base-update variant
  REG_BASE_UPDATE = 1,  // OP has register base-update variant
  IMM_BASE_UPDATE = 2,  // OP has immediate base-update variant
};

extern void 
SWP_Exp_COPY (TN *result, TN *opnd, OPS *ops);


extern void 
SWP_Loop_Init_Fini (BOOL is_doloop, 
                    INT stage_count, 
                    OPS *prolog_ops,
		    OPS *body_ops,
		    OPS *epilog_ops);

extern BOOL 
Prepare_Loop_For_SWP_1 (CG_LOOP& cl, bool trace);

extern BOOL 
Prepare_Loop_For_SWP_2 (CG_LOOP& cl, bool trace);

extern void 
Convert_While_Loop_to_Fully_Predicated_Form (CG_LOOP& cl);

extern BOOL 
Remove_Non_Definite_Dependence (CG_LOOP &cl, bool cg_loop_init, bool trace);

extern BASE_UPDATE 
OP_base_update_kind (OP *op);

extern INT32 
OP_base_opnd_num (OP *op);

extern INT32 
OP_base_res_num (OP *op);

extern INT32 
OP_incr_opnd_num (OP *op);
 
extern BOOL 
Imm_Value_In_Range (OP *op, INT64 imm);

extern void 
Gen_SWP_Branch (CG_LOOP &cl, bool is_doloop);

extern void 
Gen_SWP_Branch_Predict (BB *body, BB *prolog, BB *epilog);

extern void 
Undo_SWP_Branch (CG_LOOP &cl, bool is_doloop);

extern void 
Gen_Implicit_Prefetches (CG_LOOP &cl, bool trace);

extern TN* 
Base_update_tn (OP *op);

#ifndef TARG_IA64
extern void
SWP_TARGET_Calculate_Critical_Resources (const SWP_OP_VECTOR& ops, 
                                         INT ii);

extern float
SWP_TARGET_Get_Scale_Factor (OP *op);

extern INT
SWP_TARGET_Get_Branch_Cycle (INT ii);

extern INT 
SWP_TARGET_Scheduling_Constrain_For_OP (const SWP_OP_VECTOR& ops,
                                        INT candidate, 
                                        INT cycle,
                                        INT ii);

extern void
SWP_TARGET_Adjust_LoadStore_Offset (SWP_OP_VECTOR& ops, BOOL trace);
#endif

#endif /* cg_swp_target_INCLUDED */

