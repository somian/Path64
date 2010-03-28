/* 
 *  Copyright (C) 2008  PathScale, LLC. All Rights Reserved.
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
* \file cg_swp_mve.cxx                                                 *
* \brief Implementation file for the Modulo Variable Expansion         *
***********************************************************************/

/* Basic compiler include files */
#include "defs.h"
#include "cxx_memory.h"

/* Code Generator (CG) related include files */
#include "wn_util.h"
#include "tn.h"
#include "op.h"
#include "bb.h"
#include "freq.h"
#include "cg_loop.h"
#include "cgprep.h"
#include "cgtarget.h"

/* Software Pipelinig (SWP) related include files */
#include "cg_swp_options.h"
#include "cg_swp_op_vector.h"
#include "cg_swp_lifetime.h"
#include "cg_swp_mve.h"

/* Standard Template Library (STL) related include files */
#include <algorithm>
#include <cmath>

using namespace std;



SWP_MVE::SWP_MVE (const SWP_OP_VECTOR& ops_in, 
                  SWP_OP_VECTOR& ops_out, 
                  const SWP_LIFENESS& lifeness,
                  const SWP_LIFETIME& lifetime,
                  INT ii,
                  MEM_POOL *m) :
  _m           (m),
  _ops         (ops_in),
  _mve_ops     (ops_out),
  _k_min       (0),
  _ln          (lifeness),
  _lt          (lifetime),
  _renamed_tns (less <const TN *> (), SWP_TN2TN_ARRAY_ALLOC (m)),
  _reverse_tns (less <TN *> (), SWP_TN2TN_ALLOC (m)),
  _ii          (ii)
{
  Determine_Unroll_Factor ();
  Unroll_Kernel ();
  Update_Omega ();

  return;
}


void 
SWP_MVE::Determine_Unroll_Factor (void) {
  /* Go through all loop variant registers and get their lifetime. The
   * longest lifetime is the critial one to determine the unrolling
   * factor
   */
  mUINT16 max_lt = 0;

  for (SWP_TN_LIFETIME_VECTOR::size_type i = 0; i < _lt.size (); ++i) {
    if (_lt(i).Get_TN_Lifetime () > max_lt) {
      max_lt = _lt(i).Get_TN_Lifetime ();
    }
  }

  _k_min = static_cast<INT> (ceil (static_cast<float> (max_lt) / _ii));

  return;
}


void 
SWP_MVE::Unroll_Kernel (void) {
  _mve_ops.reserve (_ops.size () * _k_min);

  Generate_Renaming_TNs ();

  for (INT kernel_unroll = 0; kernel_unroll < _k_min; ++kernel_unroll) {
    for (INT i = 0; i < _ops.size (); ++i) {
      if (i == _ops.Get_Start_Idx ()) continue;
      if (i == _ops.Get_Stop_Idx ()) continue;
      
      Duplicate_OP (_ops(i), kernel_unroll); 
    }
  }

  return;
}


void 
SWP_MVE::Generate_Renaming_TNs (void) {
  /*********************************************************************
  * Rename loop invariant TNs                                          *
  *********************************************************************/
  /* Global Register Allocation (GRA) doesn't like it when we use TNs 
   * inside the SWP region with the same name as outside. 
   * Therefore we rename here all loop invariant TNs and create a glue 
   * copy to the renamed TN later during code generation in the prolog.
   * If GRA does a good job, the glue copy will be a noop.
   */
  /* PathScale Bug 14709
   * We always need to make a copy of loop invariant TNs to garantee
   * that they are not assigned a different register by a following
   * SWP phase.
   */
  const TN_SET *invariant = _ln.Get_Invariant_TNs ();

  /* First create a new TN for every loop invariant TN */
  TN *tn;
  FOR_ALL_TN_SET_members (invariant, tn) {
    if (TN_is_dedicated (tn)) continue;

    TN **tns = CXX_NEW_ARRAY (TN *, _k_min, _m);
    
    TN *new_tn;
    new_tn = Dup_TN (tn);
    
    for (INT i = 0; i < _k_min; ++i) {
      tns[i] = new_tn;
    }

    _renamed_tns[tn]     = tns;
    _reverse_tns[new_tn] = tn;
  } /* FOR_ALL_TN_SET_members (invariant, tn) */
  /********************************************************************/

  /*********************************************************************
  * Rename loop variant TNs                                            *
  *********************************************************************/
  for (SWP_TN_LIFETIME_VECTOR::size_type i = 0; i < _lt.size (); ++i) {
    for (SWP_TN_VECTOR::size_type j = 0; j < _lt(i).Get_TN_Size (); ++j)
    {
      TN  *tn  = _lt(i).Get_TN (j);
    
      if (TN_is_dedicated (tn)) break;
    
      TN **tns = CXX_NEW_ARRAY (TN *, _k_min, _m);
    
      for (INT k = 0; k < _k_min; ++k) {
        tns[k] = Dup_TN (tn);
        _reverse_tns[tns[k]] = tn;
      }

      _renamed_tns[tn] = tns;
    }
  }
  /********************************************************************/

  return;
}


void 
SWP_MVE::Duplicate_OP (const SWP_OP& swp_op, INT kernel_unroll) {
  OP *op     = swp_op.Get_OP ();
  OP *new_op = Dup_OP (op);

  SWP_OP new_swp_op (new_op, 
                     swp_op.Get_Cycle () + kernel_unroll * _ii,
                     swp_op.Get_Start_Cycle () + kernel_unroll * _ii,
                     swp_op.Get_Stage (),
                     swp_op.Is_Scheduled (),
                     swp_op.Is_Pointer_Update (),
                     swp_op.Get_Try_Counter (),
                     kernel_unroll,
                     swp_op);

  /* Rename results */
  UINT8 res;
  FOR_ALL_OP_RESULTs (new_op, res) {
   TN *new_res = Get_Result_TN (new_swp_op, res);
   Set_OP_result (new_op, res, new_res);
  }

  /* Rename operands */
  UINT8 opr;
  FOR_ALL_OP_OPNDs (new_op, opr) {
    TN *new_opr = Get_Operand_TN (new_swp_op, opr);
    Set_OP_opnd (new_op, opr, new_opr);
  }

  /* Set OP start cycle */
  OP_scycle (new_op) = new_swp_op.Get_Start_Cycle ();

  _mve_ops.push_back (new_swp_op);
  
  return;
}


void
SWP_MVE::Update_Omega (void) {
  SWP_OP_VECTOR::size_type i;
  FOR_ALL_SWP_OP_VECTOR_IDXs (_mve_ops, i) {
    SWP_OP& swp_op = _mve_ops(i);

    OP *op = swp_op.Get_OP ();
    UINT8 opr;
    FOR_ALL_OP_OPNDs (op, opr) {
      INT omega     = swp_op.Get_Omega (opr);
      INT unroll    = swp_op.Get_Unrolling ();
      INT adjust    = omega + (_k_min - unroll - 1);
      INT new_omega = adjust / _k_min;
      swp_op.Set_Omega (opr, new_omega);
    }
  }

  return;
}


TN * 
SWP_MVE::Get_Result_TN (const SWP_OP& swp_op, UINT8 idx) {
  OP *op = swp_op.Get_OP ();
  Is_True (idx >= 0 && idx < OP_results (op),
           ("SWP_MVE: Out of bounds!"));
  TN *tn = OP_result (op, idx);

  /* if the TN is not a register, we don't have to do any renaming. */
  if (!TN_is_register (tn)) return tn;

  INT unroll = swp_op.Get_Unrolling ();
  INT stage  = swp_op.Get_Stage ();

  INT version = unroll - stage;
  
  version = version % _k_min;
  while (version < 0) {
    version += _k_min;
  }

  return Get_Renamed_TN (tn, 0, version);
}


TN * 
SWP_MVE::Get_Operand_TN (const SWP_OP& swp_op, UINT8 idx) {
  OP *op = swp_op.Get_OP ();
  Is_True (idx >= 0 && idx < OP_opnds (op),
           ("SWP_MVE: Out of bounds!"));
  TN *tn = OP_opnd (op, idx);

  /* if the TN is not a register, we don't have to do any renaming. */
  if (!TN_is_register (tn)) return tn;

  INT omega  = swp_op.Get_Original_Omega (idx);
  INT unroll = swp_op.Get_Unrolling ();
  INT stage  = swp_op.Get_Stage ();

  INT version = unroll - stage;
  
  version = version % _k_min;
  while (version < 0) {
    version += _k_min;
  }
      
  return Get_Renamed_TN (tn, omega, version);
}


void 
SWP_MVE::Print (FILE *f) const {
  fprintf (f, "%sTN mapping:\n%s", DBar, DBar);
  SWP_TN2TN_ARRAY_MAP::const_iterator itr;
  for (itr = _renamed_tns.begin (); itr != _renamed_tns.end (); ++itr) {
    fprintf (f, "TN%d ->", TN_number (itr->first));
    for (INT i = 0; i < _k_min; ++i) {
      fprintf (f, " TN%d", TN_number (itr->second[i])); 
    }
    fprintf (f, "\n");
  }
  fprintf (f, "%s\n", DBar);
  
  fprintf (f, "%sMVE Kernels:\n%s", DBar, DBar);
  _mve_ops.Print ("after modulo variable expansion (MVE)", f);
  fprintf (f, "%s\n", DBar);

  return;
}


void 
SWP_MVE::Verify (void) const {

  return;
}

