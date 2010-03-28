/*
 *  Copyright (C) 2008, 2009 PathScale, LLC.  All Rights Reserved.
 *
   Path64 is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by
   the Free Software Foundation version 3

   Path64 is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
   or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
   License for more details.

   You should have received a copy of the GNU General Public License
   along with Path64; see the file COPYING.  If not, write to the Free
   Software Foundation, 51 Franklin Street, Fifth Floor, Boston, MA
   02110-1301, USA.
 */


/*******************************************************************//**
* \file cg_swp_op_vector.cxx                                           *
* \brief Implementation of SWP_OP_VECTOR.                              *
***********************************************************************/

/* Basic compiler include files */
#include "defs.h"
#include "cxx_memory.h"
#include "tracing.h"

/* Code Generator (CG) related include files */
#include "op.h"
#include "tag.h"
#include "tn.h"

/* Software Pipelining (SWP) related include files */
#include "cg_swp.h"
#include "cg_swp_op_vector.h"

/* Standard Template Library (STL) related include files */
#include <functional>

using namespace std;



SWP_OP::SWP_OP (OP *op) :
  _op        (op),
  _cycle     (0),
  _scycle    (0),
  _stage     (0),
  _scheduled (FALSE),
  _p_update  (FALSE),
  _num_tries (0),
  _unrolling (0)
{
  // don't use memset, because MAX_OPERANDS is very small
  for (UINT8 i = 0; i < MAX_OPERANDS; ++i) {
    _original_omega[i] = 0;
  }

  for (UINT8 i = 0; i < MAX_OPERANDS; ++i) {
    _omega[i] = 0;
  }

  return;
}


SWP_OP::SWP_OP (const SWP_OP& swp_op) :
  _op        (swp_op._op),
  _cycle     (swp_op._cycle),
  _scycle    (swp_op._scycle),
  _stage     (swp_op._stage),
  _scheduled (swp_op._scheduled),
  _p_update  (swp_op._p_update),
  _num_tries (swp_op._num_tries),
  _unrolling (swp_op._unrolling)
{
  for (UINT8 i = 0; i < MAX_OPERANDS; ++i) {
    _original_omega[i] = swp_op._original_omega[i];
  }

  for (UINT8 i = 0; i < MAX_OPERANDS; ++i) {
    _omega[i] = swp_op._omega[i];
  }

  return;
}


SWP_OP::SWP_OP (OP *op, const SWP_OP& swp_op) :
  _op        (op),
  _cycle     (swp_op._cycle),
  _scycle    (swp_op._scycle),
  _stage     (swp_op._stage),
  _scheduled (swp_op._scheduled),
  _p_update  (swp_op._p_update),
  _num_tries (swp_op._num_tries),
  _unrolling (swp_op._unrolling)
{
  for (UINT8 i = 0; i < MAX_OPERANDS; ++i) {
    _original_omega[i] = swp_op._original_omega[i];
  }

  for (UINT8 i = 0; i < MAX_OPERANDS; ++i) {
    _omega[i] = swp_op._omega[i];
  }

  return;
}
  
SWP_OP::SWP_OP (OP *op, 
                INT16 cycle, 
                INT16 scycle, 
                UINT8 stage, 
                BOOL scheduled,
                BOOL p_update,
                UINT16 num_tries, 
                UINT8 unrolling, 
                const SWP_OP& swp_op) :
  _op        (op),
  _cycle     (cycle),
  _scycle    (scycle),
  _stage     (stage),
  _scheduled (scheduled),
  _p_update  (p_update),
  _num_tries (num_tries),
  _unrolling (unrolling)
{
  for (UINT8 i = 0; i < MAX_OPERANDS; ++i) {
    _original_omega[i] = swp_op._original_omega[i];
  }

  for (UINT8 i = 0; i < MAX_OPERANDS; ++i) {
    _omega[i] = swp_op._omega[i];
  }

  return;
}


void 
SWP_OP::Print (FILE *f) const {
  fprintf (f, 
           "[%4d,%2d] ", 
           Srcpos_To_Line (OP_srcpos (_op)), 
           OP_scycle (_op));
  
  if (OP_has_tag (_op)) {
    LABEL_IDX tag = Get_OP_Tag (_op);
    fprintf (f, "<tag %s>: ", LABEL_name (tag));
  }

  UINT8 res;
  FOR_ALL_OP_RESULTs (_op, res) {
    Print_TN (OP_result (_op, res), FALSE, f);
    fprintf (f, " ");
  }

  fprintf(f, ":- ");
  fprintf(f, "%s ", TOP_Name (OP_code (_op)));
 
  UINT8 opr;
  FOR_ALL_OP_OPNDs (_op, opr) {
    const TN *tn = OP_opnd (_op, opr);
    Print_TN (tn, FALSE, f);
    fprintf (f, "[%d|%d]", Get_Omega (opr), Get_Original_Omega (opr));
    if (OP_Defs_TN (_op, tn)) fprintf(f, "<defopnd>");
    fprintf (f, " ");
  }

  fprintf (f, ";");

  /* print flags */
  if (OP_glue (_op))               fprintf (f, " glue");
  if (OP_no_alias (_op))           fprintf (f, " noalias");
  if (OP_copy (_op))               fprintf (f, " copy");
  if (OP_volatile (_op))           fprintf (f, " volatile");
  if (OP_side_effects (_op))       fprintf (f, " side_effects");
  if (OP_hoisted (_op))            fprintf (f, " hoisted");
  if (OP_cond_def (_op))           fprintf (f, " cond_def");
  if (OP_same_res (_op))           fprintf (f, " same_res");
  if (OP_end_group (_op))          fprintf (f, " end_group");
  if (OP_tail_call (_op))          fprintf (f, " tail_call");
  if (OP_no_move_before_gra (_op)) fprintf (f, " no_move");
  if (OP_spadjust_plus (_op))      fprintf (f, " spadjust_plus");
  if (OP_spadjust_minus (_op))     fprintf (f, " spadjust_minus");

  if (OP_unrolling (_op)) {
    UINT8 unr = OP_unrolling (_op);
    fprintf (f, " %d%s unrolling", unr,
	    unr == 1 ? "st" : unr == 2 ? "nd" : unr == 3 ? "rd" : "th");
  }
  fprintf (f, "\n");
  
  fprintf (f, 
           "cycle = %d, modulo cycle = %d, stage = %d, tries = %d, " \
           "unrolling = %d <%s>\n",
           _cycle,
           _scycle,
           _stage,
           _num_tries,
           _unrolling,
           _scheduled ? "scheduled" : "not scheduled");
  
  return;
}


void 
SWP_OP::Verify (void) const {
  FmtAssert (_op != 0, ("SWP OP: Invalid OP"));
  FmtAssert (_cycle >= 0, ("SWP OP: Invalid scheduling cycle"));
  FmtAssert (_scycle >= 0, ("SWP OP: Invalid scheduling cycle"));

  return;
}



void 
SWP_OP_VECTOR::Recalculate_OP_Indices (void) {
  _op_map.clear ();

  for (size_type i = 0; i < size (); ++i) {
    OP *op = (*this)(i).Get_OP ();
    if (op == _start) {
      _start_idx = i;
    }
    else if (op == _stop) {
      _stop_idx = i;
    }
    else if (op == _branch) {
      _branch_idx = i;
    }

    _op_map[op] = i;
  }

  FmtAssert (_start_idx == 0,
             ("SWP OP: Start is not the first element!"));
  FmtAssert (_stop_idx == size () - 1,
             ("SWP OP: Stop is not the last element!"));

  return;
}


SWP_OP_VECTOR::SWP_OP_VECTOR (MEM_POOL *m) :
  SWP_STL_OP_VECTOR (SWP_STL_OP_ALLOC (m)),
  _m            (m),
  _op_map       (less <const OP *> (), SWP_OP2INT_ALLOC (m)),
  _start        (0),
  _stop         (0),
  _branch       (0),
  _start_idx    ((size_type) -1),
  _stop_idx     ((size_type) -1),
  _branch_idx   ((size_type) -1)
{

  return;
}


SWP_OP_VECTOR::SWP_OP_VECTOR (BB* body, MEM_POOL *m) :
  SWP_STL_OP_VECTOR (SWP_STL_OP_ALLOC (m)),
  _m            (m),
  _op_map       (less <const OP *> (), SWP_OP2INT_ALLOC (m)),
  _start        (0),
  _stop         (0),
  _branch       (0),
  _start_idx    ((size_type) -1),
  _stop_idx     ((size_type) -1),
  _branch_idx   ((size_type) -1)
{
  /* Reserve required memory for the STL vector*/
  reserve (BB_length (body) + 2);

  /* Create fake/dummy "START" OP */
  _start          = Mk_OP (TOP_swp_start);
  _start_idx      = size ();
  push_back (SWP_OP (_start));

  /* Now add all loop body OPs */
  OP *op;
  FOR_ALL_BB_OPs (body, op) {
    if (OP_br (op)) {
      _branch     = op;
      _branch_idx = size ();
    }

    SWP_OP swp_op (op);
    UINT8 opr;
    FOR_ALL_OP_OPNDs (op, opr) {
      INT8 omega = OP_omega (op, opr);
      swp_op.Set_Omega (opr, omega);
      swp_op.Set_Original_Omega (opr, omega);
    }

    push_back (swp_op);
  }
  
  /* Create fake/dummy "STOP" OP */
  _stop          = Mk_OP (TOP_swp_stop);
  _stop_idx      = size ();
  push_back (SWP_OP (_stop));

  return;
}


void
SWP_OP_VECTOR::Update_Omega (void) {
  for (size_type i = 0; i < size (); ++i) {
    if (i == _start_idx) continue;
    if (i == _stop_idx) continue;

    OP    *op     = (*this)(i).Get_OP ();
    UINT8  stage1 = (*this)(i).Get_Stage ();

    UINT8 res;
    FOR_ALL_OP_RESULTs (op, res) {
      TN *cur_tn = OP_result (op, res);
      BOOL alive = TRUE;

      for (size_type j = i+1; j < size () + i; ++j) {
        size_type  idx    = j % size ();
        OP        *op2    = (*this)(idx).Get_OP ();
        UINT8      stage2 = (*this)(idx).Get_Stage ();

        UINT8 opr;
        FOR_ALL_OP_OPNDs (op2, opr) {
          TN *opr_tn = OP_opnd (op2, opr);
          if (opr_tn == cur_tn) {
            INT16 diff      = stage2 - stage1;
            INT8  omega     = (*this)(idx).Get_Omega (opr);
            INT8  new_omega = omega + diff;
            (*this)(idx).Set_Omega (opr, new_omega);
          }
        }

        UINT8 res2;
        FOR_ALL_OP_RESULTs (op2, res2) {
          TN *res_tn = OP_result (op2, res2);
          if (res_tn == cur_tn) {
            alive = FALSE;
            break;
          }
        }

        if (!alive) break;
      }

      if (!alive) break;
    } /* FOR_ALL_OP_RESULTs (op, res) */
  } /* for (INT i = 0; i < size (); ++i) */

  return;
}


void 
SWP_OP_VECTOR::Print (const char *string, FILE *f) const {
  fprintf (f,
           "%sSWP LOOP OPs: %s\n%s\n",
           DBar,
           string,
           DBar);

  for_each (begin (), 
            end (), 
            bind2nd (mem_fun_ref (&SWP_OP::Print), f));

  fprintf (f, "%s\n", DBar);

  return;
}


void 
SWP_OP_VECTOR::Verify (void) const {
  FmtAssert (_start      != 0, ("SWP_OP_VECTOR: Invalid start OP."));
  FmtAssert (_stop       != 0, ("SWP_OP_VECTOR: Invalid stop OP."));
  FmtAssert (_branch     != 0, ("SWP_OP_VECTOR: Invalid branch OP."));
  FmtAssert (_start_idx  == 0, 
             ("SWP_OP_VECTOR: Invalid index for START OP."));
  FmtAssert (_stop_idx   > 0, 
             ("SWP_OP_VECTOR: Invalid index for STOP OP."));
  FmtAssert (_branch_idx > 0 && _branch_idx < _stop_idx, 
             ("SWP_OP_VECTOR: Invalid index for branch OP."));
  
  for_each (begin (), 
            end (), 
            mem_fun_ref (&SWP_OP::Verify));

  for (size_type i = 1; i < size () - 1; ++i) {
    FmtAssert (OP_Idx ((*this)(i).Get_OP ()) == i,
               ("SWP_OP_VECTOR: OP to index mapping not correct."));
    if ((*this)(i).Is_Scheduled ()) {
      FmtAssert (   (    (*this)(i).Get_Cycle () 
                     >= (*this)(_start_idx).Get_Cycle ())
                 && ((*this)(i).Get_Cycle () 
                     <= (*this)(_stop_idx).Get_Cycle ()),
                 ("SWP_OP_VECTOR: Scheduled cycle is out of bounds."));
    }
  }
  
  return;
}


/**********************************************************************/


// Can_Swap_OPs checks the register dependencies between OPs sop1 and
// sop2, to see if we should swap their positions, and returns:
// SWP_REORDER_OPS_NO  if dependencies prevent the swap;
// SWP_REORDER_OPS_YES if dependencies require the swap;
// SWP_REORDER_OPS_MAYBE_YES if the swap is allowable but not required
//   and would improve register pressure;
// SWP_REORDER_OPS_MAYBE_NO if the swap is allowable but not required,
//   but could hurt register pressure.
// SWP_REORDER_OPS_MAYBE if the swap is allowable but is not required
//   nor particularly beneficial.
// NOTE: There is no check for memory dependencies; presumable two OPs
// that access the same memory can't be assigned to the same cycle.

enum SWP_REORDER_OPS {
  SWP_REORDER_OPS_NO,
  SWP_REORDER_OPS_MAYBE_NO,
  SWP_REORDER_OPS_MAYBE,
  SWP_REORDER_OPS_MAYBE_YES,
  SWP_REORDER_OPS_YES
};


static SWP_REORDER_OPS Can_Swap_OPs(const SWP_OP& sop1, const SWP_OP& sop2)
{
  const OP *op1 = sop1.Get_OP ();
  const OP *op2 = sop2.Get_OP ();
  UINT8 res, opr;
  SWP_REORDER_OPS result = SWP_REORDER_OPS_MAYBE;

  FOR_ALL_OP_RESULTs (op1, res) {
    const TN *res_tn = OP_result (op1, res);
    FOR_ALL_OP_OPNDs (op2, opr) {
      if (OP_opnd (op2, opr) == res_tn) {
	if (sop2.Get_Omega (opr) == 0)  {
	  return SWP_REORDER_OPS_NO;
	} else {
	  result = SWP_REORDER_OPS_MAYBE_YES;
	}
      }
    }
  }

  FOR_ALL_OP_RESULTs (op2, res) {
    const TN *res_tn = OP_result (op2, res);
    FOR_ALL_OP_OPNDs (op1, opr) {
      if (OP_opnd (op1, opr) == res_tn) {
	if (sop1.Get_Omega (opr) == 0)  {
	  return SWP_REORDER_OPS_YES;
	} else {
	  result = SWP_REORDER_OPS_MAYBE_NO;
	}
      }
    }
  }

  return result;
}


// SWP_OP_VECTOR::Sort_OPs_Within_Cycles considers OPs that have been
// assigned to the same cycle and reorders them to honor dependencies
// and tries reduce register pressure.

void
SWP_OP_VECTOR::Sort_OPs_Within_Cycles (void)
{
  // Find the start and end points.
  SWP_OP_VECTOR::iterator itr0 = begin ();
  SWP_OP_VECTOR::iterator itr_end = end ();
  if (_start && _stop) {
    ++itr0;
    --itr_end;
  }

#if 0

  // This sort algorithm only considers swapping immediately adjacent OPs.
  // It is simplier, but may miss some cases if more than two OPs are
  // assigned to the same cycle.
  if (itr0 == itr_end) return;
  SWP_OP_VECTOR::iterator itr1 = itr0;
  for (++itr1; itr1 != itr_end; itr0 = itr1, ++itr1) {
    if (itr0->Get_Start_Cycle() < itr1->Get_Start_Cycle()) continue;
    Is_True(itr0->Get_Start_Cycle() == itr1->Get_Start_Cycle(),
	    ("SWP_OP_VECTOR::Sort_OPs_Within_Cycles found out-of-order OPs"));
    SWP_REORDER_OPS can_swap_01 = Can_Swap_OPs(*itr0, *itr1);
    if (can_swap > SWP_REORDER_OPS_MAYBE) {
      std::swap(*itr0, *itr1);
    }
  }

#else

  // This sort algorithm considers three consecutive OPs at a time.
  // It catches more cases when three or more OPs are assigned to
  // the same cycle, but still may miss some cases if more than
  // three OPs somehow land in the same cycle.
  if (itr0 == itr_end) return;
  SWP_OP_VECTOR::iterator itr1 = itr0;
  for (++itr1; itr1 != itr_end; itr0 = itr1, ++itr1) {
    if (itr0->Get_Start_Cycle() < itr1->Get_Start_Cycle()) continue;
    Is_True(itr0->Get_Start_Cycle() == itr1->Get_Start_Cycle(),
	    ("SWP_OP_VECTOR::Sort_OPs found out-of-order OPs"));
    SWP_REORDER_OPS can_swap_01 = Can_Swap_OPs(*itr0, *itr1);
    SWP_REORDER_OPS can_swap_02 = SWP_REORDER_OPS_NO;
    SWP_REORDER_OPS can_swap_12 = SWP_REORDER_OPS_NO;
    SWP_OP_VECTOR::iterator itr2 = itr1;
    ++itr2;
    if (itr2 != itr_end &&
	itr1->Get_Start_Cycle() >= itr2->Get_Start_Cycle()) {
      can_swap_02 = Can_Swap_OPs(*itr0, *itr2);
      can_swap_12 = Can_Swap_OPs(*itr1, *itr2);
    }
    // Identify cases when *itr0 remains in its current position.
    if ((can_swap_01 <= SWP_REORDER_OPS_MAYBE &&
	 can_swap_02 <= SWP_REORDER_OPS_MAYBE) ||
	(can_swap_01 == SWP_REORDER_OPS_NO &&
	 can_swap_12 == SWP_REORDER_OPS_NO)) {
    }
    // Handle cases when *itr1 should be moved before *itr0.
    else if (can_swap_02 == SWP_REORDER_OPS_NO ||
	     can_swap_12 == SWP_REORDER_OPS_NO ||
	     (can_swap_02 <= SWP_REORDER_OPS_MAYBE &&
	      can_swap_12 <= SWP_REORDER_OPS_MAYBE)) {
      std::swap(*itr0, *itr1);
    }
    // Move *itr2 before *itr0 and *itr1.
    else {
      std::swap(*itr0, *itr1);
      std::swap(*itr0, *itr2);
    }
  }
#endif

  Recalculate_OP_Indices ();

  return;
}
