/*
 * Copyright (C) 2008 PathScale, LLC.  All Rights Reserved.
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


#include "defs.h"
#include "tracing.h"
#include "cg_flags.h"
#include "cgprep.h"
#include "cg_loop.h"
#include "cg_loop_mii.h"
#include "cg_loop_recur.h"
#include "cg_dep_graph.h"
#include "cg_sched_est.h"
#include "cg_swp.h"
#include "cg_swp_op_vector.h"
#include "cg_swp_mindist.h"
#include "cg_swp_target.h"
#include "cgexp.h"
#include "const.h"
#include "tn_set.h"
#include "ti_res_count.h"
#include "config_opt.h"


#ifdef KEY


// ====================================================================
//
// The code in cg_loop_recur.cxx implements recurrence breaking.
// In a tight loop, recurrence breaking relaxes dependencies by
// converting accumation OPs such as  "x = x[1] + y"  into
// "x = x[new_omega] + new_y", which skips several iteration of the
// loop.  After unrolling by new_omega, this expands to code like:
//
//     x0 = x0 + new_y0;                  x = x + y0;
//     x1 = x1 + new_y1;    instead of    x = x + y1;
//     x2 = x2 + new_y2;                  x = x + y2;
//     x3 = x3 + new_y3;                  x = x + y3;
//
// The transformation can significantly increase register pressure,
// and should be used sparingly.
//
// This file implements three variants of recursion breaking:
//
// RECUR_BACK_SUB_INVARIANT transforms "x = x[1] + INV" (where INV
// is loop invariant) into:
//
//     x[1] = x[0] + INV;
//     x[2] = x[1] + INV;
//     ...
//     x[new_omega - 1] = x[new_omega - 2] + INV;
//     { 
//       x = x[new_omega] + (INV * new_omega)
//
// RECUR_INTERLEAVE transforms "x = x[1] + y" into:
//
//     x[1] = x;
//     x[2] = 0;
//     ...
//     x[new_omega - 1] = 0;
//     {
//       x = x[new_omega] + y;
//     }
//     x = x[0] + x[1] + ... + x[new_omega - 1];
//
// RECUR_BACK_SUB_VARIANT transforms:  "x = x[omega_x] + y"
// (where y is NOT loop invariant) into:
// 
//   y[1] = 0
//   ...
//   y[omega_x] = 0
//   x[omega_x + 1] = x[1]
//   ...
//   x[2 * omega_x] = x[omega_x]
//   {
//     t = y + y[omega_x]
//     x = x[2 * omega_x] + t
//   }
//
// OR (if the Need_copy_variant flag was set), RECUR_BACK_SUB_VARIANT
// transforms "x = x[omega_x] + y[omega_y]" into:
//
//   s[1] = 0
//   ...
//   s[omega_x] = 0
//   x[omega_x + 1] = x[1]
//   ...
//   x[2 * omega_x] = x[omega_x]
//   {
//     s = y[omega_y]
//     t = s + s[omega_x]
//     x = x[2 * omega_x] + t
//   }
//
// The above transformations only apply to simple, one-OP recursions.
// RECUR_NESTED_INTERLEAVE transfroms a nested OP chain such as
//     x = madd(madd(msub(x[1], a, b), c, d), e, f)
// into a sum of shorter latency recursions:
//
//     t1[1] = 0;
//     t2[1] = 0;
//     {
//       t1 = msub(t1[1], a, b);
//       t2 = msub(t2[1], c, d);
//       x  = msub(x[1],  e, f);
//     }
//     x = x[0] + t1[0] + t2[0];
//
// CG invokes recurrence breaking through:
// Fix_Recurrences_Before_Unrolling and Fix_Recurrences_After_Unrolling
//
// ====================================================================


// ====================================================================
//
// class RECUR_OP_DESC calculates and stores the properties of an OP
// needed to perform recurrence breaking.  These properties are:
//
// --> OP *op:  The candidate OP
//
// --> RECUR_ACTION action:  The candidate transformation for this OP;
// one of the following:
//   RECUR_NONE,
//   RECUR_BACK_SUB_INVARIANT,
//   RECUR_INTERLEAVE,
//   RECUR_BACK_SUB_VARIANT
// If action == RECUR_NONE, then other properties may be invalid.
//
// --> INT res_idx:  The result index of the recurring TN.
// --> INT reg_opnd_idx:  The operand index of the recurring TN.
//     So OP_result(op, res_idx) == OP_opnd(op, reg_opnd_idx).
// --> INT invar_opnd_idx: Index of the other operand (which is
//     accumulated onto the recurring TN.
// --> INT second_invar_opnd_idx:  Index of the other other operand.
//     Only valid for float multiply-add
//
// --> INT new_omega;
//
// --> TYPE_ID mtype: Type of recurrence operator; calculated by
//     Mtype_from_opc.
//
// --> bool is_add:  Is the recurrence OP an add or subtract?
// --> bool has_one_invar_opnd: TRUE unless op has
//     second_invar_opnd_idx (float multiply-add has three operands).
// --> bool allow_back_sub_variant;
// --> bool need_copy_variant;
//
// ====================================================================


// Return mtype
TYPE_ID Mtype_from_opc(TOP top)
{
  switch (top) {

  case TOP_add:     case TOP_addu:
  case TOP_addi:    case TOP_addiu:
  case TOP_sub:     case TOP_subu:
    return MTYPE_I4;

  case TOP_dadd:    case TOP_daddi:
  case TOP_daddu:   case TOP_daddiu:
  case TOP_dsub:    case TOP_dsubu:
    return MTYPE_I8;

  case TOP_add_s:   case TOP_madd_s:   case TOP_nmadd_s:
  case TOP_sub_s:   case TOP_msub_s:   case TOP_nmsub_s:
    return MTYPE_F4;

  case TOP_add_d:   case TOP_madd_d:   case TOP_nmadd_d:
  case TOP_sub_d:   case TOP_msub_d:   case TOP_nmsub_d:
    return MTYPE_F8;

  case TOP_add_ps:  case TOP_madd_ps:  case TOP_nmadd_ps:
  case TOP_sub_ps:  case TOP_msub_ps:  case TOP_nmsub_ps:
    return MTYPE_V8F4;

  default:
    Is_True(FALSE, ("Mtype_from_opc: unknown opcode."));
  }

  return MTYPE_UNKNOWN;
}


// ====================================================================
//
// Convert recursive madd chains such as:
//
//  for (i = 0; i < 1000; ++i)
//  {
//    x = madd(madd(x, a[i], b[i]), c[i], d[i]);
//  }
//
//  into:
//
//  x0 = x;
//  x1 = 0;
//  for (i = 0; i < 1000; ++i)
//  {
//    x0 = madd(x0, a[i], b[i]);
//    x1 = madd(x1, c[i], d[i]);
//  }
//  x = x0 + x1;
//
// Including the more general case of nested add, sub, madd, msub,
// nmadd, nmsub:
//   x = add(madd(msub(madd(sub(...(x, ...)))));
//
// ====================================================================

// Initially, loop body looks like:
// t1 = madd x[1], a0, b0
// t2 = msub t1, a1, b1
// t3 =  sub t2, c2
//  x = madd t3, a3, b3
//
// How to detect?  Search for dependence arc x[1] --> x then work forward?




//  Test whether the OPND can be reassociated
//  with the OP.
//
inline BOOL OPND_is_additive(TOP top, INT opnd)
{
  switch (top) {
  case TOP_add:      case TOP_dadd:
  case TOP_addu:     case TOP_daddu:
  case TOP_add_s:    case TOP_add_d:    case TOP_add_ps:
    return (opnd == 0 || opnd == 1);
  case TOP_sub:      case TOP_dsub:
  case TOP_subu:     case TOP_dsubu:
  case TOP_sub_s:    case TOP_sub_d:    case TOP_sub_ps:
  case TOP_madd_s:   case TOP_madd_d:   case TOP_madd_ps:
  case TOP_nmsub_s:  case TOP_nmsub_d:  case TOP_nmsub_ps:
    return (opnd == 0);
  }
  return FALSE;
}



// ====================================================================


enum RECUR_ACTION {
  RECUR_NONE,
  RECUR_BACK_SUB_INVARIANT,
  RECUR_INTERLEAVE,
  RECUR_BACK_SUB_VARIANT,
  RECUR_NESTED_INTERLEAVE
};


class RECUR_OP_DESC {
private:
  // If action == RECUR_NONE, then other properties may not be initialized
  OP *op;			// Candidate OP
  RECUR_ACTION action;		// Candidate transformation for this OP
  INT res_idx;			// Result index of recurring TN
  INT reg_opnd_idx;		// Operand index of recurring TN
  INT invar_opnd_idx;		// Index of non-recurring operand
  INT second_invar_opnd_idx;	// Index of other non-recurring operand
  				// Only valid if has_one_invar_opnd is false
  INT new_omega;
  TYPE_ID mtype;		// Mtype for OP's result
  bool is_add;			// Is the recurrence OP an add or subtract?
  bool has_one_invar_opnd;	// False only for float multiply-add
  bool allow_back_sub_variant;
  bool need_copy_variant;

public:
  OP *Op() { return op; }
  RECUR_ACTION Action() { return action; }
  INT  Res_idx() { return res_idx; }
  INT  Reg_opnd_idx() { return reg_opnd_idx; }
  INT  Invar_opnd_idx() { return invar_opnd_idx; }
  INT  Second_invar_opnd_idx() { return second_invar_opnd_idx; }
  INT  New_omega() { return new_omega; }
  TYPE_ID Mtype() { return mtype; }
  bool Is_add() { return is_add; }
  bool Need_copy_variant() { return need_copy_variant; }

  RECUR_OP_DESC(const RECUR_OP_DESC &r) { *this = r; }

  RECUR_OP_DESC(BB *body, BB *epilog, OP *operation, CG_LOOP_DEF& loop_def,
		TN_SET *multi_def, double estimate_ResMII, bool trace) {

    op = operation;
    action = RECUR_NONE;
    Reset_OP_flag1(op);

    // All recurrence ops have 1 result
    if (OP_results(op) != 1) return;

    // Recurrence ops cannot have its result modified other OPs
    if (TN_SET_MemberP(multi_def, OP_result(op, 0))) return;

    // Fix sgi 768488: Dedicated TNs cannot be assigned omega > 1.
    if (TN_is_dedicated(OP_result(op, 0))) return;

    // These default values may be overruled in switch below.
    // In particular, reg_opnd_idx will be set to a non-negative
    // operand index if op is a candidate for recurrence breaking.
    reg_opnd_idx = -1;
    has_one_invar_opnd = true;
    allow_back_sub_variant = false;
    is_add = false;

    // Quit if op is not a candidate
    switch (OP_code(op)) {

    case TOP_madd_s:
    case TOP_madd_d:
    case TOP_madd_ps:
      is_add = true;
      // FALLTHRU
    case TOP_nmsub_s:
    case TOP_nmsub_d:
    case TOP_nmsub_ps:
      if (! Enable_Cfold_Reassociate) return;
      has_one_invar_opnd = false;
      reg_opnd_idx = 0;
      break;

    case TOP_add_s:
    case TOP_add_d:
    case TOP_add_ps:
      allow_back_sub_variant = true;
      is_add = true;
      if (! Enable_Cfold_Reassociate) return;
      break;

    case TOP_sub_s:
    case TOP_sub_d:
    case TOP_sub_ps:
      if (! Enable_Cfold_Reassociate) return;
      allow_back_sub_variant = true;
      reg_opnd_idx = 0;
      break;

    case TOP_addi:
    case TOP_daddi:
    case TOP_addiu:
    case TOP_daddiu:
      is_add = true;
      // FALLTHRU
    case TOP_sub:
    case TOP_dsub:
    case TOP_subu:
    case TOP_dsubu:
      reg_opnd_idx = 0;
      break;

    case TOP_add:
    case TOP_dadd:
    case TOP_addu:
    case TOP_daddu:
      is_add = true;
      break;

    default:
      return;

    }

    // Check if OP is cyclic; if so, reg_opnd_idx must be set.
    BOOL is_noncyclic = FALSE;
    if (reg_opnd_idx < 0) {
      // Either operand 0 or 1 can work
      if (OP_result(op, 0) == OP_opnd(op, 0)) reg_opnd_idx = 0;
      else if (OP_result(op, 0) == OP_opnd(op, 1)) reg_opnd_idx = 1;
      else is_noncyclic = TRUE;
    } else if (OP_result(op, 0) != OP_opnd(op, reg_opnd_idx)) {
      is_noncyclic = TRUE;
    }

    // For MIPS, reg_opnd_idx and invar_opnd_idx are 0,1 or 1,0.
    invar_opnd_idx = 1 - reg_opnd_idx;
    second_invar_opnd_idx = 2;
    res_idx = 0;
    need_copy_variant = false;
    mtype = Mtype_from_opc(OP_code(op));

    if (is_noncyclic) {
      // Noncyclic OPs are only eligible for RECUR_NESTED_INTERLEAVE.
      // If this OP really is eligible, set OP_flag1.  If this OP is
      // the last/outermost OP, set action to RECUR_NESTED_INTERLEAVE
      // before returning.

      // Triage flags can disable recurrence breaking.
      if (! CG_LOOP_nested_interleave) return;

      // Result must appear in exactly one operand, with omega 0 or 1.
      ARC_LIST *arcs = ARC_LIST_Find(OP_succs(op), CG_DEP_REGIN, DONT_CARE);
      if (arcs == NULL) return;
      ARC *arc = ARC_LIST_first(arcs);
      if (ARC_omega(arc) > 1) return;
      arcs = ARC_LIST_Find(ARC_LIST_rest(arcs), CG_DEP_REGIN, DONT_CARE);
      if (arcs != NULL) return;

      // Can't interleave if the accumulator has a conflicting backpatch;
      // Also, no intermediate should be used outside the loop.
      // TODO: This constraint could be eased in epilog.
      TN *tn_result = OP_result(op, 0);
      CG_LOOP_BACKPATCH *bp;
      for (bp = CG_LOOP_Backpatch_First(epilog, NULL); bp != NULL;
	   bp = CG_LOOP_Backpatch_Next(bp)) {
	if (CG_LOOP_BACKPATCH_body_tn(bp) == tn_result) {
	  if (ARC_omega(arc) == 0 ||
	      CG_LOOP_BACKPATCH_omega(bp) != 0) return;
	}
      }

      // This OP could appear in a chain of nested MADDs
      Set_OP_flag1(op);

      // Is this OP the last/outermost OP in the chain?
      if (ARC_omega(arc) == 0) return;

      // Trace the def-use arcs forward to see if it forms an additive
      // cycle, and determine total latency
      INT latency = 0;
      OP *op0 = NULL;
      do {
	latency += ARC_latency(arc);
	op0 = ARC_succ(arc);
	if (! OP_flag1(op0)) return;
	if (! OPND_is_additive(OP_code(op0), ARC_opnd(arc))) return;
	arc = ARC_LIST_Find_First(OP_succs(op0), CG_DEP_REGIN, DONT_CARE);
      } while (ARC_omega(arc) == 0);
      // Skip this OP if chain is not cyclic or if total latency
      // is already short enough.
      if (op == op0 && latency > estimate_ResMII) {
	action = RECUR_NESTED_INTERLEAVE;
      }
      return;
    }

    // OP is known to be cyclic, something like x = x[omega] + y

    // Fix sgi 796426: Disallow recurrence breaking if the result TN
    // appears in more than one of the operands.  Eg. x = x + x;
    TN *result_tn = OP_result(op, res_idx);
    if (result_tn == OP_opnd(op, invar_opnd_idx)) return;
    if (! has_one_invar_opnd &&
	result_tn == OP_opnd(op, second_invar_opnd_idx)) return;

    // Determine which RECUR_ACTION can be applied to the candidate OP.
    // TODO: Handle omegas larger than one
    if (OP_omega(op, reg_opnd_idx) == 1) {
      if (has_one_invar_opnd &&
	  loop_def.Is_invariant(OP_opnd(op, invar_opnd_idx)))
	action = RECUR_BACK_SUB_INVARIANT;  // See fix 813388 below
      else {
	bool can_interleave = true;
	OP *other_op;
	TN *use = OP_opnd(op, reg_opnd_idx);
	// Can't INTERLEAVE if accumulator TN is read by another OP.
	FOR_ALL_BB_OPs(body, other_op) {
	  if (other_op != op &&
	      OP_Refs_TN(other_op, use)) {
	    can_interleave = false;
	    break;
	  }
	}
	// Can't INTERLEAVE if accumulator TN conflicts with epilog backpatch
	if (can_interleave) {
	  for (CG_LOOP_BACKPATCH *bp = CG_LOOP_Backpatch_First(epilog, NULL);
	       bp != NULL;
	       bp = CG_LOOP_Backpatch_Next(bp)) {
	    if (CG_LOOP_BACKPATCH_body_tn(bp) == use &&
		CG_LOOP_BACKPATCH_omega(bp) != 0) {
	      can_interleave = false;
	      break;
	    }
	  }
	}
	if (can_interleave)
	  action = RECUR_INTERLEAVE;
      }
    }

    if (allow_back_sub_variant && action == RECUR_NONE) {
      // if it is unable to change the prolog backpatch,
      // disable RECUR_BACK_SUB_VARIANT.
      INT old_omega = OP_omega(op, reg_opnd_idx);
      TN *body_tn = OP_opnd(op, reg_opnd_idx);
      CG_LOOP_BACKPATCH *bp;
      for (bp = CG_LOOP_Backpatch_First(CG_LOOP_prolog, NULL);
	   bp != NULL;
	   bp = CG_LOOP_Backpatch_Next(bp)) {
	if (CG_LOOP_BACKPATCH_body_tn(bp) == body_tn &&
	    CG_LOOP_BACKPATCH_omega(bp) > old_omega &&
	    CG_LOOP_BACKPATCH_omega(bp) <= old_omega * 2) break;
      }
      if (bp == NULL) {
	action = RECUR_BACK_SUB_VARIANT;
	TN *body_tn = OP_opnd(op, invar_opnd_idx);
	need_copy_variant = OP_omega(op, invar_opnd_idx) != 0;

	// will need to copy the "variant" if
	// 1) it is conditionally defined, or
	// 2) it is defined multiple times, or
	// 3) it is live-in
	//
	if (!need_copy_variant) {
	  OP *other_op;
	  INT def_count = 0;
	  FOR_ALL_BB_OPs(body, other_op) {
	    if (OP_Defs_TN(other_op, body_tn)) {
	      if (OP_cond_def(other_op) ||
		  def_count++ > 0) {
		need_copy_variant = true;
		break;
	      }
	    }
	  }
	}
	if (!need_copy_variant) {
	  CG_LOOP_BACKPATCH *bp;
	  for (bp = CG_LOOP_Backpatch_First(CG_LOOP_prolog, NULL);
	       bp != NULL; bp = CG_LOOP_Backpatch_Next(bp)) {
	    if (CG_LOOP_BACKPATCH_body_tn(bp) == body_tn) {
	      need_copy_variant = true;
	      break;
	    }
	  }
	}
      }
    }

    // Fix sgi 813388: Abort RECUR_BACK_SUB_INVARIANT if there is a
    // prolog backpatch conflict (just as with RECUR_BACK_SUB_VARIANT
    // above).  TODO: Loop peeling could circumvent this problem.
    if (action == RECUR_BACK_SUB_INVARIANT &&
	CG_LOOP_Backpatch_Find_Non_Body_TN(CG_LOOP_prolog,
					   OP_opnd(op, reg_opnd_idx), 2)) {
      action = RECUR_NONE;
    }

    // Triage flags can disable recurrence breaking.
    BOOL enable_action = TRUE;
    if (action == RECUR_BACK_SUB_INVARIANT)
      enable_action = CG_LOOP_back_substitution;
    else if (action == RECUR_BACK_SUB_VARIANT)
      enable_action = CG_LOOP_back_substitution_variant;
    else if (action == RECUR_INTERLEAVE)
      enable_action = CG_LOOP_interleave_reductions;
    if (! enable_action) action = RECUR_NONE;

    // Heuristics for BACK_SUB_INVARIANT and INTERLEAVE
    if (action != RECUR_NONE && action != RECUR_BACK_SUB_VARIANT) {
      INT latency = CG_DEP_Latency(op, op, CG_DEP_REGIN, reg_opnd_idx);
      for (new_omega = std::max(1, CG_LOOP_recurrence_min_omega);
	   new_omega < CG_LOOP_recurrence_max_omega; ++new_omega) {
	// estimate_ResMII is a double
	if (latency <= new_omega * estimate_ResMII)
	  break;
      }
      if (trace)
	fprintf(TFile, "<recur> new_omega=%d, latency=%d, ResMII=%g\n",
		new_omega, latency, estimate_ResMII);

      if (new_omega <= 1) action = RECUR_NONE;
    }
  }

};


// ====================================================================
//
// Apply_Back_Sub_Invariant performs RECUR_BACK_SUB_INVARIANT,
// which, given an OP and a new omega (in op_desc), transforms
// "x = x[1] + INV" (where INV is loop invariant) into:
//
//     x[1] = x[0] + INV;
//     x[2] = x[1] + INV;
//     ...
//     x[new_omega - 1] = x[new_omega - 2] + INV;
//     { 
//       x = x[new_omega] + (INV * new_omega)
//
// Updates the OP and the loop prolog and epilog.
//
// ====================================================================


void Apply_Back_Sub_Invariant(BB *prolog, RECUR_OP_DESC& op_desc)
{
  OP *op = op_desc.Op();
  INT new_omega = op_desc.New_omega();
  INT invar_opnd_idx = op_desc.Invar_opnd_idx();
  INT reg_opnd_idx = op_desc.Reg_opnd_idx();
  INT res_idx = op_desc.Res_idx();
  TYPE_ID mtype = op_desc.Mtype();

  TN *invar_tn = OP_opnd(op, invar_opnd_idx);
  Is_True(OP_omega(op, reg_opnd_idx) == 1,
	  ("Apply_Back_Sub_Invariant: Expecting omega = 1"));
  Set_OP_omega(op, reg_opnd_idx, new_omega);
  OPS prolog_ops = OPS_EMPTY;

  // Update loop body
  switch (OP_code(op)) {
  case TOP_add_s:
  case TOP_add_d:
  case TOP_add_ps:
  case TOP_sub_s:
  case TOP_sub_d:
  case TOP_sub_ps:
    {
      TN *new_invar_tn = Dup_TN(OP_result(op, res_idx));
      WN *const_tree = Make_Const(Host_To_Targ_Float(mtype, new_omega));
      TYPE_ID mtype = WN_rtype(const_tree);
      TN *const_tn = Build_TN_Of_Mtype(mtype);
      Exp_Load(mtype, mtype, const_tn, WN_st(const_tree), 0,
	       &prolog_ops, V_NONE);
      Exp_MPY(Mtype_from_opc(OP_code(op)), new_invar_tn,
	      invar_tn, const_tn, &prolog_ops);
      Set_OP_opnd(op, invar_opnd_idx, new_invar_tn);
      break;
    }

  case TOP_add:
  case TOP_addu:
  case TOP_dadd:
  case TOP_daddu:
  case TOP_sub:
  case TOP_subu:
  case TOP_dsub:
  case TOP_dsubu:
    {
      TN *new_invar_tn = Dup_TN(OP_result(op, res_idx));
      Exp_MPY(mtype, new_invar_tn, invar_tn, Gen_Literal_TN(new_omega, 4),
	      &prolog_ops);
      Set_OP_opnd(op, invar_opnd_idx, new_invar_tn);
      break;
    }

  case TOP_addi:
  case TOP_addiu:
  case TOP_daddi:
  case TOP_daddiu:
    {
      INT64 imm = TN_value(invar_tn);
      TN *new_imm_tn = Gen_Literal_TN(imm * new_omega, 4);
      if (TOP_Can_Have_Immediate(imm * new_omega, OP_code(op)))
	Set_OP_opnd(op, invar_opnd_idx, new_imm_tn);
      else {
	// generate TOP_add and initialization at prolog
	TOP opc_new;
	switch (OP_code(op)) {
	case TOP_addi:    opc_new = TOP_add;    break;
	case TOP_addiu:   opc_new = TOP_addu;   break;
	case TOP_daddi:   opc_new = TOP_dadd;   break;
	case TOP_daddiu:  opc_new = TOP_daddu;  break;
	}
	OP_Change_Opcode(op, opc_new);
	TN *tn_new = Dup_TN(OP_result(op, res_idx));
	Set_OP_opnd(op, invar_opnd_idx, tn_new);
	Exp_COPY(tn_new, new_imm_tn, &prolog_ops);
      }
      break;
    }

  default:
    Is_True(FALSE, ("Apply_Back_Sub_Invariant: op not supported."));
  }

  // Find non-body TN
  TN *body_tn = OP_result(op, res_idx);
  TN *non_body_tn = CG_LOOP_Backpatch_Find_Non_Body_TN(prolog, body_tn, 1);
  Is_True(non_body_tn, ("Apply_Back_Sub_Invariant: can't find non-body tn"));

  // Update PROLOG BACKPATCH
  for (INT i = 2; i <= new_omega; i++) {
    TN *tn_new = Dup_TN(non_body_tn);
    if (op_desc.Is_add())
      Exp_SUB(mtype, tn_new, non_body_tn, invar_tn, &prolog_ops);
    else
      Exp_ADD(mtype, tn_new, non_body_tn, invar_tn, &prolog_ops);
    CG_LOOP_Backpatch_Add(prolog, tn_new, body_tn, i);
    non_body_tn = tn_new;
  }

  BB_Append_Ops(prolog, &prolog_ops);
}


// ====================================================================
//
// Apply_Back_Sub_Variant performs RECUR_BACK_SUB_INVARIANT,
// which, given an OP and a new omega (in op_desc), transforms
// "x = x[omega_x] + y" (where y is NOT loop invariant) into:
// 
//   prolog backpatches:
//     y[1] = 0
//     ...
//     y[omega_x] = 0
//     x[omega_x + 1] = x[1]
//     ...
//     x[2 * omega_x] = x[omega_x]
//   {
//     t = y + y[omega_x]
//     x = x[2 * omega_x] + t
//   }
//
// OR (if the Need_copy_variant flag was set), it transforms
// "x = x[omega_x] + y[omega_y]" into:
//
//   prolog backpatches:
//     s[1] = 0
//     ...
//     s[omega_x] = 0
//     x[omega_x + 1] = x[1]
//     ...
//     x[2 * omega_x] = x[omega_x]
//   {
//     s = y[omega_y]
//     t = s + s[omega_x]
//     x = x[2 * omega_x] + t
//   }
//
// Given an OP and a new omega, updates the loop body and the prolog.
//
// ====================================================================


bool Apply_Back_Sub_Variant(RECUR_OP_DESC& op_desc, BB *prolog, BB *body,
			    CG_SCHED_EST *loop_se, bool trace)
{
  OP *op = op_desc.Op();
  INT invar_opnd_idx = op_desc.Invar_opnd_idx();
  INT reg_opnd_idx = op_desc.Reg_opnd_idx();
  INT res_idx = op_desc.Res_idx();
  TYPE_ID mtype = op_desc.Mtype();
  bool need_copy_variant = op_desc.Need_copy_variant();

  // Update loop body
  switch (OP_code(op)) {
  case TOP_add_s:
  case TOP_add_d:
  case TOP_add_ps:
  case TOP_sub_s:
  case TOP_sub_d:
  case TOP_sub_ps:
    {
      INT old_omega = OP_omega(op, reg_opnd_idx);
      INT new_omega = old_omega * 2;

      CG_SCHED_EST_Add_Op_Resources(loop_se, TOP_add_s);
      if (need_copy_variant)
	CG_SCHED_EST_Add_Op_Resources(loop_se, TOP_mov_s);

      INT latency = CG_DEP_Latency(op, op, CG_DEP_REGIN, reg_opnd_idx);
      double ResMII = CG_SCHED_EST_Resources_Min_Cycles(loop_se);

      if (trace)
	fprintf(TFile, "<back_sub_variant> latency=%d old_omega=%d ResMII=%g\n",
		latency, old_omega, ResMII);

      if (((double)latency / old_omega) <= ResMII) {
	CG_SCHED_EST_Subtract_Op_Resources(loop_se, TOP_add_s);
	if (need_copy_variant)
	  CG_SCHED_EST_Subtract_Op_Resources(loop_se, TOP_mov_s);
	return false;
      }

      OPS body_ops = OPS_EMPTY;
      OPS copy_ops = OPS_EMPTY;

      TN *invar_tn = OP_opnd(op, invar_opnd_idx);
      if (need_copy_variant) {
	// Insert copy s = y[omega_y] before op
	TN *tn = Dup_TN(invar_tn);
	Exp_COPY(tn, invar_tn, &copy_ops);
	invar_tn = tn;
	BB_Insert_Ops_Before(body, op, &copy_ops);
	OP *mov_op = OP_prev(op);
	Is_True(OP_code(mov_op) == TOP_mov_s,
		("Apply_Back_Sub_Variant: must be mov_s."));
	CGPREP_Init_Op(mov_op);
	CG_LOOP_Init_Op(mov_op);
	INT invar_tn_omega = OP_omega(op, invar_opnd_idx);
	Set_OP_omega(mov_op, 0, invar_tn_omega);
      }

      // Change x = x[omega_x] + y[omega_y] into
      //        x = x[2 * omega_x] + t
      TN *temp_tn = Dup_TN(OP_opnd(op, reg_opnd_idx));
      Set_OP_opnd(op, invar_opnd_idx, temp_tn);
      Set_OP_omega(op, invar_opnd_idx, 0);
      Set_OP_omega(op, reg_opnd_idx, new_omega);

      // Insert t = y + y[omega] before op
      Exp_OP2 (OPCODE_make_op(OPR_ADD, mtype, MTYPE_V),
	       temp_tn, invar_tn, invar_tn, &body_ops);
      BB_Insert_Ops_Before(body, op, &body_ops);
      OP *add_op = OP_prev(op);
      Is_True(OP_code(add_op) == TOP_add_s || OP_code(add_op) == TOP_add_d ||
	      OP_code(add_op) == TOP_add_ps,
	      ("Apply_Back_Sub_Variant:  must be float add."));
      CGPREP_Init_Op(add_op);
      CG_LOOP_Init_Op(add_op);
      Set_OP_omega(add_op, 1, old_omega);

      // Generate backpatches for x:  x[omega + c] = x[c]
      TN *body_tn = OP_result(op, res_idx);
      int i;
      for (i = 1; i <= old_omega; i++) {
	CG_LOOP_BACKPATCH *bp;
	TN *non_body_tn = NULL;
	for (bp = CG_LOOP_Backpatch_First(CG_LOOP_prolog, NULL);
	     bp != NULL; bp = CG_LOOP_Backpatch_Next(bp)) {
	  if (CG_LOOP_BACKPATCH_body_tn(bp) == body_tn &&
	      CG_LOOP_BACKPATCH_omega(bp) == i) {
	    non_body_tn = CG_LOOP_BACKPATCH_non_body_tn(bp);
	    break;
	  }
	}
	Is_True(non_body_tn, ("can't find backpatch for TN%d[%d]\n",
			      TN_number(body_tn), i));
	CG_LOOP_Backpatch_Add(prolog, non_body_tn, body_tn, i + old_omega);
      }

      // Generate backpatches for y (or s): y[1] = ... = y[omega_x] = 0
      for (i = 1; i <= old_omega; i++)
	CG_LOOP_Backpatch_Add(prolog, Zero_TN, invar_tn, i);

      return true; // BB changed
    }

  default:
    Is_True(FALSE, ("Apply_Back_Sub_Variant: op not supported."));
  }
  return false;
}


// ====================================================================
//
// Apply_Interleave performs RECUR_INTERLEAVE, which, given an OP and
// a new omega (in op_desc), transforms "x = x[1] + y" into:
//
//     x[1] = x;
//     x[2] = 0;
//     ...
//     x[new_omega - 1] = 0;
//     {
//       x = x[new_omega] + y;
//     }
//     x = x[0] + x[1] + ... + x[new_omega - 1];
//
// Updates the OP and the loop prolog and epilog.
//
// ====================================================================


void Apply_Interleave(BB *prolog, BB *epilog, RECUR_OP_DESC& op_desc)
{
  OP *op = op_desc.Op();
  INT new_omega = op_desc.New_omega();
  INT reg_opnd_idx = op_desc.Reg_opnd_idx();
  INT res_idx = op_desc.Res_idx();
  TYPE_ID mtype = op_desc.Mtype();

  Is_True(OP_omega(op, reg_opnd_idx) == 1,
	  ("Apply_Interleave: Expecting omega = 1"));
  Set_OP_omega(op, reg_opnd_idx, new_omega);

  // Update prolog backpatches for accumulator TN
  TN *body_tn = OP_result(op, res_idx);
  for (INT i = 2; i <= new_omega; i++)
    CG_LOOP_Backpatch_Add(prolog, Zero_TN, body_tn, i);

  // Update prolog backpatches for accumulator TN
  TN *non_body_tn = CG_LOOP_Backpatch_Find_Non_Body_TN(epilog, body_tn, 0);
  if (!non_body_tn) {

    // Don't need to total backpatches if output is not used.
    DevWarn("Apply_Interleave: can't find non-body tn");

  } else {

    // Insert backpatch TN to retrieve x[1], ..., x[new_omega - 1]
    vector<TN*> backpatch_tns(new_omega);  // TODO: malloc
    backpatch_tns[0] = non_body_tn;
    int i;
    for (i = 1; i < new_omega; ++i) {
      TN *tn_new = Dup_TN(non_body_tn);
      backpatch_tns[i] = tn_new;
      Is_True(CG_LOOP_Backpatch_Find_Non_Body_TN(epilog, body_tn, i) == NULL,
	      ("Apply_Interleave: epilog backpatch existed."));
      CG_LOOP_Backpatch_Add(epilog, tn_new, body_tn, i);
    }

    // Calculate total of non-body backpatch TNs
    // x = x[0] + x[1] + ... + x[new_omega - 1];
    OPS epilog_ops = OPS_EMPTY;
    INT size;
    for (size = new_omega; size > 1; size = (size + 1) / 2) {
      for (i = 0; 2 * i + 1 < size; ++i) {
	TN *dst = backpatch_tns[2 * i];
	TN *src = backpatch_tns[2 * i + 1];
	Exp_ADD(mtype, dst, dst, src, &epilog_ops);
	backpatch_tns[i] = dst;
      }
      backpatch_tns[i] = backpatch_tns[2 * i];  // For odd size
    }
    BB_Append_Ops(epilog, &epilog_ops);
  }

}


// ====================================================================
//
// Apply_Nested_Interleave performs RECUR_NESTED_INTERLEAVE, which,
// given the outermost OP in a chain such as
//     x = madd(madd(msub(x[1], a, b), c, d), e, f)
// transforms it into a sum of shorter latency recursions:
//
//     t1[1] = 0;
//     t2[1] = 0;
//     {
//       t1 = msub(t1[1], a, b);
//       t2 = msub(t2[1], c, d);
//       x  = msub(x[1],  e, f);
//     }
//     x = x[0] + t1[0] + t2[0];
//
// Updates several OPs and the loop prolog and epilog.  Bug 14829.
//
// ====================================================================


void Apply_Nested_Interleave(BB *prolog, BB *epilog, OP *op,
			     INT target_latency)
{
  // Allocate tn_sums big enough to hold all OPs in chain
  INT bb_length = BB_length(OP_bb(op));
  TN **tn_sums = (TN **) alloca (bb_length * sizeof(TN *));
  INT tn_count = 0;

  // Traverse forward (inside to outer OP) through the chain and
  // replace some intermediate TNs with accumulators

  // Find first (innermost) OP in nested chain
  ARC *arc = ARC_LIST_Find_First(OP_succs(op), CG_DEP_REGIN, DONT_CARE);
  ARC *arc_to_break = arc;
  INT latency = 0;
  op = ARC_succ(arc);
  arc = ARC_LIST_Find_First(OP_succs(op), CG_DEP_REGIN, DONT_CARE);
  latency += ARC_latency(arc);

  while (ARC_omega(arc) == 0) {
    OP *op_next = ARC_succ(arc);
    ARC *arc_next = ARC_LIST_Find_First(OP_succs(op_next),
					CG_DEP_REGIN, DONT_CARE);
    if (latency + ARC_latency(arc_next) > target_latency) {
      TN *tn_result = OP_result(op, 0);
      tn_sums[++tn_count] = tn_result;
      Set_OP_opnd(ARC_succ(arc_to_break), ARC_opnd(arc_to_break), tn_result);
      Set_OP_omega(ARC_succ(arc_to_break), ARC_opnd(arc_to_break), 1);
      arc_to_break = arc;
      latency = 0;
    }
    op = op_next;
    arc = arc_next;
    latency += ARC_latency(arc);
  }
  TN *tn_sum = OP_result(op, 0);
  Set_OP_opnd(ARC_succ(arc_to_break), ARC_opnd(arc_to_break), tn_sum);
  Set_OP_omega(ARC_succ(arc_to_break), ARC_opnd(arc_to_break), 1);

  // Add prolog backpatches for accumulator TNs
  for (INT i = 1; i <= tn_count; ++i) {
    CG_LOOP_Backpatch_Add(prolog, Zero_TN, tn_sums[i], 1);
  }

  TN *tn_sum_out = CG_LOOP_Backpatch_Find_Non_Body_TN(epilog, tn_sum, 0);
  if (tn_sum_out == NULL) {
    // Don't need to total epilog backpatches if output is not used.
    DevWarn("Apply_Nested_Interleave: can't find non-body tn");
  } else {

    tn_sums[0] = tn_sum_out;

    // Insert epilog backpatches to retrieve t1[1], ..., tn[1]
    INT i;
    for (i = 1; i <= tn_count; ++i) {
      CG_LOOP_Backpatch_Add(epilog, tn_sums[i], tn_sums[i], 0);
    }

    // Calculate total of non-body backpatch TNs
    // x = x[0] + t1[1] + ... + tn[1];
    TYPE_ID mtype = Mtype_from_opc(OP_code(op));
    OPS epilog_ops = OPS_EMPTY;
    INT size;
    for (size = tn_count + 1; size > 1; size = (size + 1) / 2) {
      for (i = 0; 2 * i + 1 < size; ++i) {
	TN *dst = tn_sums[2 * i];
	TN *src = tn_sums[2 * i + 1];
	Exp_ADD(mtype, dst, dst, src, &epilog_ops);
	tn_sums[i] = dst;
      }
      tn_sums[i] = tn_sums[2 * i];  // For odd size
    }
    BB_Append_Ops(epilog, &epilog_ops);
  }

}


// ====================================================================

// The list of TOPs that will be handled by
// the reassociation algorithm.
//
bool TOP_is_associative(TOP top)
{
  switch (top) {
  case TOP_add:    case TOP_dadd:
  case TOP_addu:   case TOP_daddu:
  case TOP_sub:    case TOP_dsub:
  case TOP_subu:   case TOP_dsubu:
    return true;
  case TOP_add_s:  case TOP_add_d:  case TOP_add_ps:
  case TOP_sub_s:  case TOP_sub_d:  case TOP_sub_ps:
  case TOP_mul_s:  case TOP_mul_d:  case TOP_mul_ps:
    return Enable_Cfold_Reassociate;
  }
  return false;
}


//  Give the opposite form, e.g,  - => +,  + => -.
//
inline TOP Get_Opposite_TOP(TOP top)
{
  return CGTARG_Invert(top);
}


//  Test whether the OPND can be reassociated
//  with the OP.
//
bool OPND_can_be_reassociated(TOP top, int opnd)
{
  switch (top) {
  case TOP_add:    case TOP_dadd:
  case TOP_addu:   case TOP_daddu:
  case TOP_add_s:  case TOP_add_d:  case TOP_add_ps:
  case TOP_mul_s:  case TOP_mul_d:  case TOP_mul_ps:
    return (opnd == 0 || opnd == 1);
  case TOP_sub:    case TOP_dsub:
  case TOP_subu:   case TOP_dsubu:
  case TOP_sub_s:  case TOP_sub_d:  case TOP_sub_ps:
    return (opnd == 0);
  }
  return false;
}


//  Reassociation can happen before these two OPs
//
bool OPs_can_be_reassociated(OP *op1, OP *op2)
{
  TOP top1 = OP_code(op1);
  TOP top2 = OP_code(op2);
  if (top1 == top2 || top2 == Get_Opposite_TOP(top1)) {
    if (OP_opnd(op1, OP_PREDICATE_OPND) == OP_opnd(op2, OP_PREDICATE_OPND))
      return true;
  }
  return false;
}


bool OP_is_multiplication(OP *op)
{
  TOP top = OP_code(op);
  return (top == TOP_mul_s || top == TOP_mul_d || top == TOP_mul_ps);
}


bool OP_is_addition(OP *op)
{
  TOP top = OP_code(op);
  return (top == TOP_add || top == TOP_addu ||
	  top == TOP_add_s || top == TOP_add_d || top == TOP_add_ps);
}


//  The other opnd involved in reassociation
//
inline INT Other_opnd(OP *op, INT this_opnd)
{
  return 1 - this_opnd;
}


//  Swap the operands!
//
void Exchange_Opnd(OP *op1, int op1_opnd_idx, OP *op2, int op2_opnd_idx)
{
  int op1_omega = OP_omega(op1, op1_opnd_idx);
  TN *op1_tn = OP_opnd(op1, op1_opnd_idx);

  int op2_omega = OP_omega(op2, op2_opnd_idx);
  TN *op2_tn = OP_opnd(op2, op2_opnd_idx);

  Set_OP_omega(op1, op1_opnd_idx, op2_omega);
  Set_OP_opnd(op1, op1_opnd_idx, op2_tn);

  Set_OP_omega(op2, op2_opnd_idx, op1_omega);
  Set_OP_opnd(op2, op2_opnd_idx, op1_tn);
}


// Return true if op2 is dependent on op1.
//
bool OPs_Are_Dependent(OP *op1, OP *op2)
{
  for (int i = 0; i < OP_results(op1); i++) {
    TN *tn = OP_result(op1,i);
    if (TN_is_register(tn) && !TN_is_const_reg(tn)) {
      if (OP_Defs_TN(op2, tn) || OP_Refs_TN(op2, tn))
	return true;
    }
  }
  for (int i = 0; i < OP_opnds(op1); i++) {
    TN *tn = OP_opnd(op1,i);
    if (TN_is_register(tn) && !TN_is_const_reg(tn)) {
      if (OP_Defs_TN(op2, tn))
	return true;
    }
  }
  return false;
}


//  Return true if OP can sink to point.
//
bool OP_Can_Sink_Before(OP *sink_op, OP *point)
{
  for (OP *op = OP_next (sink_op); op != point; op = OP_next (op)) {
    if (OPs_Are_Dependent (sink_op, op)) {
      return FALSE;
    }
  }

  return TRUE;
}


//  Return true if OPND is involved in the critical path
//
bool OPND_is_not_critical (OP *succ, 
                           int opnd,
                           const SWP_OP_VECTOR& ops,
			   const SWP_MINDIST& mindist)
{
  ARC_LIST *al;
  for (al = OP_preds (succ) ; al != 0; al = ARC_LIST_rest (al) ) {
    ARC *arc = ARC_LIST_first (al);
    OP *pred = ARC_pred (arc);
    INT pred_idx = ops.OP_Idx (pred);
    if (   ARC_opnd (arc) == opnd 
        && mindist(pred_idx, pred_idx))
    {
      return FALSE;
    }
  }

  return TRUE;
}


//  Reassociation
//
void Reassociate (BB *body,
		  const SWP_OP_VECTOR& ops,
		  int op_idx,
		  int opnd,
		  const SWP_MINDIST& mindist,
		  bool trace)
{
  OP *op = ops(op_idx).Get_OP ();
  if (OPND_can_be_reassociated (OP_code (op), opnd)) {
    INT use_count = 0;
    ARC *critical_arc = NULL;
    for (ARC_LIST *al = OP_succs(op); al != 0; al = ARC_LIST_rest (al)) {
      ARC *arc = ARC_LIST_first (al);
      OP *succ = ARC_succ (arc);
      INT succ_idx = ops.OP_Idx (succ);
      if (ARC_kind (arc) == CG_DEP_REGIN) {
	use_count++;
	if (   ARC_omega (arc) == 0 
            && mindist (succ_idx, succ_idx)) 
        {
	  critical_arc = arc;
        }
      }
    }
    bool succeeded = FALSE;
    INT succ_idx;
    INT succ_opnd;
    if (use_count == 1 && critical_arc) {
      OP *succ = ARC_succ (critical_arc);
      succ_idx = ops.OP_Idx (succ);
      if (   OPs_can_be_reassociated (op, succ) 
          && OP_Can_Sink_Before (op, succ)) 
      {
	//  t = a + b    ==>    t = a + c
	//  d = c + t           d = b + t
	//
	INT critical_opnd = ARC_opnd (critical_arc);
	succ_opnd = Other_opnd (succ, critical_opnd);

	if (   OPND_is_not_critical (succ, succ_opnd, ops, mindist) 
            && OPND_can_be_reassociated (OP_code (succ), critical_opnd))
        {
	  Exchange_Opnd (op, opnd, succ, succ_opnd);
	  if (OP_next (op) != succ) {
	    BB_Sink_Op_Before(body, op, succ);
          }

	  // adjusted for the signed-ness
	  if (!OP_is_multiplication (op)) {
	    if (OP_is_addition (op)) {
	      if (OP_is_addition (succ)) {
		// ++ ==> no change
	      } 
              else {
		// +- ==> -+
		OP_Change_Opcode (op, Get_Opposite_TOP (OP_code (op)));
		OP_Change_Opcode (succ, Get_Opposite_TOP (OP_code (succ)));

		// the new opnd for -ve should not be in associative position!
		if (OPND_can_be_reassociated (OP_code (op), opnd)) {
		  Exchange_Opnd (op, opnd, op, Other_opnd (op, opnd));
                }
	      }
            }
            else {
	      if (OP_is_addition (succ)) {
		// -+ ==> no change
	      } 
              else {
		// -- ==> +-
		OP_Change_Opcode (op, Get_Opposite_TOP (OP_code (op)));
		Exchange_Opnd (succ, succ_opnd, succ, Other_opnd (succ, succ_opnd));
		succ_opnd = Other_opnd (succ, succ_opnd);
	      }
            }
	  }

	  succeeded = TRUE;
	}
      }
    }

    if (trace) {
      fprintf (TFile, "<reassoc> %s reassoc OP%d opnd%d\n",
	       succeeded ? "" : "not", op_idx, opnd);
    }

    if (succeeded) {
      Reassociate (body, ops, succ_idx, succ_opnd, mindist, trace);
    }

    BB_Update_OP_Order (body);
  }
}


// Shorten Critical Recurrence by Reassociation
//
void Shorten_Critical_Recurrence_By_Reassociation(CG_LOOP& cl,
						  BOOL is_doloop,
						  BOOL trace)
{
  CXX_MEM_POOL local_pool("local pool", FALSE);

  BB *body = cl.Loop_header();

  // Ignore the address computations.
  OP *op;
  INT count = 0;
  FOR_ALL_BB_OPs(body, op) {
    Reset_OP_loh(op);
    if ((OP_code(op) == TOP_addi || OP_code(op) == TOP_addiu) &&
	OP_result(op, 0) == OP_opnd(op, 2))
      Set_OP_loh(op);
    if ((OP_code(op) == TOP_add || OP_code(op) == TOP_addu) &&
	(OP_result(op, 0) == OP_opnd(op, 1) ||
	 OP_result(op, 0) == OP_opnd(op, 2)))
      Set_OP_loh(op);
    count++;
  }

  if (count + SWP_OPS_OVERHEAD > SWP_OPS_LIMIT) return;  // loop is too big

  SWP_OP_VECTOR ops (body, local_pool ());

  CG_SCHED_EST *loop_se = CG_SCHED_EST_Create(body, local_pool(),
					      SCHED_EST_FOR_UNROLL |
					      SCHED_EST_IGNORE_LOH_OPS |
					      SCHED_EST_IGNORE_PREFETCH);

  int ii = CG_SCHED_EST_Resource_Cycles(loop_se);

  // invokes CG_DEP_Compute_Graph, deconstructor deletes graph
  CYCLIC_DEP_GRAPH manager (body);

  // Identify critical recurrrences
  const SWP_MINDIST mindist (ops, ii, local_pool ());

  if (trace) {
    fprintf(TFile, "Shorten Critical Recurrence by Reassociation:\n");
    CG_DEP_Trace_Graph(body);
    for (int i = 0; i < ops.size(); i++) {
      if (ops(i).Get_OP ()) {
	fprintf(TFile, "%3d: ", i);
	Print_OP_No_SrcLine(ops(i).Get_OP ());
      }
    }
    fprintf(TFile,"II is %d\n", ii);
    fprintf(TFile, "<ti resource count> ");
    TI_RES_COUNT_Print(TFile, loop_se->res_count);
    fprintf(TFile, "\n");
    mindist.Print (TFile);
  }

  for (int i = 0; i < ops.size (); i++) {
    if (   ops(i).Get_OP () 
        && TOP_is_associative (OP_code (ops(i).Get_OP ())) 
        && mindist(i, i))
    {
      Is_True (OP_results (ops(i).Get_OP ()) == 1,
	       ("associative OP must have 1 result."));

      if (trace)
        fprintf(TFile, "<critical recurrence> OP%d is critical.\n", i);

      ARC_LIST *al;
      for (al = OP_preds ((ops(i).Get_OP ())) ; 
           al != 0; 
           al = ARC_LIST_rest (al) ) {
	ARC *arc = ARC_LIST_first(al);
	OP *pred = ARC_pred(arc);
	INT pred_idx = ops.OP_Idx (pred);
	if (   ARC_kind (arc) == CG_DEP_REGIN 
            && ARC_omega (arc) == 1 
            && mindist (pred_idx, pred_idx)) 
        {
	  Reassociate (body, ops, i, ARC_opnd (arc), mindist, trace);
        }
      }
    }
  }
}

#endif


// ====================================================================


void Fix_Recurrences_Before_Unrolling(CG_LOOP& cl)
{
#ifdef KEY
  if (!CG_LOOP_fix_recurrences) return;

  bool trace = Get_Trace(TP_CGLOOP, 0x800);

  if (CG_LOOP_reassociate)
    Shorten_Critical_Recurrence_By_Reassociation(cl, TRUE, trace);

  BB *body = cl.Loop_header();
  if (BB_length(body) > CG_maxinss) return;

  CXX_MEM_POOL local_pool("fix recurrence pool", FALSE);

  // Create two TN_SETS of non-constant register TNs:
  // tn_def holds every TN used as an OP result in BB body.
  // multi_def holds every TN used more than once as an OP result.
  TN_SET *tn_def = TN_SET_Create_Empty(Last_TN + 1, local_pool());
  TN_SET *multi_def = TN_SET_Create_Empty(Last_TN + 1, local_pool());
  OP *op;
  FOR_ALL_BB_OPs(body, op) {
    for (INT i = 0; i < OP_results(op); i++) {
      TN *res = OP_result(op,i);
      if (TN_is_register(res) && !TN_is_const_reg(res)) {
	if (TN_SET_MemberP(tn_def, res)) {
	  multi_def =  TN_SET_Union1D(multi_def, res, local_pool());
	} else
	  tn_def = TN_SET_Union1D(tn_def, res, local_pool());
      }
    }
  }

  BB *prolog = CG_LOOP_prolog;
  BB *epilog = CG_LOOP_epilog;
  CG_LOOP_DEF loop_def(body);

  // Perform resource estimate using ideal unrolling.
  CG_SCHED_EST *loop_se = CG_SCHED_EST_Create(body, local_pool(),
					      SCHED_EST_FOR_UNROLL |
					      SCHED_EST_IGNORE_BRANCH |
					      SCHED_EST_IGNORE_INT_OPS |
					      SCHED_EST_IGNORE_PREFETCH);
  double estimate_ResMII = CG_SCHED_EST_Resources_Min_Cycles(loop_se);

  // If the loop has no memory and FP ops, then we must count
  // the integer ops for resource utilization.
  if (estimate_ResMII < 0.1) {
    loop_se = CG_SCHED_EST_Create(body, local_pool(),
				  SCHED_EST_FOR_UNROLL |
				  SCHED_EST_IGNORE_BRANCH |
				  SCHED_EST_IGNORE_PREFETCH);
    estimate_ResMII = CG_SCHED_EST_Resources_Min_Cycles(loop_se);
  }

  vector<RECUR_OP_DESC> delay_processing;

  // Invokes CG_DEP_Compute_Graph, deconstructor deletes graph
  // Dependence graph needed for RECUR_NESTED_INTERLEAVE.
  CYCLIC_DEP_GRAPH manager(body);

  // Iterate forward through the body (RECUR_NESTED_INTERLEAVE
  // requires that the outermost OP be visited last).
  FOR_ALL_BB_OPs(body, op) {

    if (OP_has_predicate(op) &&
	! TN_is_true_pred(OP_opnd(op, OP_PREDICATE_OPND))) continue;

    RECUR_OP_DESC op_desc(body, epilog, op, loop_def, multi_def,
			  estimate_ResMII, trace);

    switch (op_desc.Action()) {

    case RECUR_BACK_SUB_INVARIANT:
      Apply_Back_Sub_Invariant(prolog, op_desc);

      if (trace)
	fprintf(TFile, "<back_sub_invar> TN%d, %s\n",
		TN_number(OP_result(op, op_desc.Res_idx())),
		TOP_Name(OP_code(op)));

      break;

    case RECUR_INTERLEAVE:

       Apply_Interleave(prolog, epilog, op_desc);

       if (trace)
	fprintf(TFile, "<interleave> TN%d, %s\n",
		TN_number(OP_result(op, op_desc.Res_idx())),
		TOP_Name(OP_code(op)));

       break;

    case RECUR_BACK_SUB_VARIANT:

      delay_processing.push_back(op_desc);
      if (trace)
	fprintf(TFile, "<back_sub_variant> TN%d, %s\n",
		TN_number(OP_result(op, op_desc.Res_idx())),
		TOP_Name(OP_code(op)));

      break;
  
    case RECUR_NESTED_INTERLEAVE:

      Apply_Nested_Interleave(prolog, epilog, op, (INT) estimate_ResMII);
      if (trace)
	fprintf(TFile, "<nested_interleave> TN%d, %s\n",
		TN_number(OP_result(op, op_desc.Res_idx())),
		TOP_Name(OP_code(op)));

      break;

    }
  }

  // Apply BACK_SUB_VARIANT repeatedly on candidate OPs until no more
  bool changed = true;
  while (changed) {
    changed = false;
    for (int i = 0; i < delay_processing.size(); i++) {
      RECUR_OP_DESC& op_desc = delay_processing[i];
      changed |= Apply_Back_Sub_Variant(op_desc, prolog, body, loop_se, trace);
    }
  }
#endif

  return;
}


void Fix_Recurrences_After_Unrolling(CG_LOOP& cl)
{
  // Nothing to do, since MIPS, unlike ia64, has no "base update" form.
  return;
}
