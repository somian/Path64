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
 * \file cg_swp_lifetime.cxx                                           *
 * \brief Implementation of LIFETIME.                                  *
 **********************************************************************/

/* Basic compiler include files */
#include "defs.h"

/* Code Generator (CG) related include files */
#include "op.h"
#include "tn.h"
#include "cg_loop.h"

/* Software Pipelining (SWP) related include files */
#include "cg_swp_op_vector.h"
#include "cg_swp_lifeness.h"
#include "cg_swp_lifetime.h"
#include "cg_swp_mve.h"

/* Standard Template Library (STL) related include files */
#include <functional>

using namespace std;

mUINT16 SWP_TN_LIFETIME::_num_lt = 0;



SWP_TN_LIFETIME::SWP_TN_LIFETIME (MEM_POOL *m) :
  _start ((mUINT16) -1), // -1 = max value of mUINT16
  _end   (0),
  _omega (0),
  _alpha (0),
  _tn    (SWP_TN_ALLOC (m))
{
  _idx = ++_num_lt;

  return;
}


void 
SWP_TN_LIFETIME::Print (FILE *f) const { 
  FmtAssert (_tn.size () > 0, 
             ("SWP_TN_LIFETIME: Undefined TN."));
  fprintf (f, 
           "VTN%d: Start = %3d(%3d), End = %3d(%3d), Omega = %3d, " \
           "Alpha = %3d, Lifetime  = %3d, TN(%d",
           _idx,
           _start / SWP_SUBCYCLES_PER_CYCLE,
           _start,
           _end / SWP_SUBCYCLES_PER_CYCLE,
           _end,
           _omega,
           _alpha,
           Get_TN_Lifetime (),
           TN_number (_tn[0]));
  for (SWP_TN_VECTOR::size_type i = 1; i < _tn.size (); ++i) {
    fprintf (f, ",%d", TN_number (_tn[i]));
  }
  fprintf (f, ")\n");

  return;
}


void 
SWP_TN_LIFETIME::Verify (void) const {

  return;
}


SWP_LIFETIME::SWP_LIFETIME (const SWP_OP_VECTOR& ops,
			    const SWP_MVE *mve,  /* NULL for original loop */
                            const SWP_LIFENESS & lifeness,
                            INT ii,
                            MEM_POOL *m,
                            INT reg_class) :
  _m            (m),
  _ii           (ii),
  _head         (CG_LOOP_prolog),
  _tail         (CG_LOOP_epilog),
  _ops          (ops),
  _ln           (lifeness),
  _lt           (SWP_TN_LIFETIME_ALLOC (m)),
  _tn2lt        (less <const TN *> (), SWP_TN2INT_ALLOC (m)),
  _reg_class    (reg_class),
  _op_sub_cycle (ops.size (), 0, SWP_mUINT16_ALLOC (_m))
{
  SWP_TN_LIFETIME::_num_lt = 0;

  /* Create a vector with the sub cycle number of each OP */
  mUINT16 last_cycle        = (mUINT16) -1;  // maximum value
  mUINT16 last_modulo_cycle = (mUINT16) -1;
  for (SWP_OP_VECTOR::size_type i = 0; i < _ops.size (); ++i) {
    if (i == _ops.Get_Start_Idx ()) continue;
    if (i == _ops.Get_Stop_Idx ()) continue;

    mUINT16 cycle;
    mUINT16 modulo_cycle = _ops(i).Get_Start_Cycle ();
    if (modulo_cycle == last_modulo_cycle) {
      cycle = last_cycle + 1;
      FmtAssert(cycle % SWP_SUBCYCLES_PER_CYCLE > 0,
		("cg_swp_lifetime.cxx: Too many OPs per cycle."));
    }
    else {
      cycle = modulo_cycle * SWP_SUBCYCLES_PER_CYCLE;
    }

    _op_sub_cycle[i] = cycle;

    last_cycle        = cycle;
    last_modulo_cycle = modulo_cycle;
  }

  UINT8 max_omega = 0;
  SWP_OP_VECTOR::size_type i;
  FOR_ALL_SWP_OP_VECTOR_IDXs (_ops, i) {
    const SWP_OP& swp_op = _ops(i);
    const OP *op = swp_op.Get_OP ();

    FmtAssert (op != 0, ("SWP_LIFETIME: Invalid OP pointer!"));


    if (OP_same_res (op)) {
      SWP_TN2INT_MAP::iterator itr;
      
      UINT8  idx = OP_find_opnd_use (op, OU_same_res);
      TN *res_tn = OP_result (op, 0);
      TN *opr_tn = OP_opnd (op, idx);

      if (   !TN_is_zero_reg (opr_tn)
          && (itr = _tn2lt.find (opr_tn)) != _tn2lt.end ()) 
      {
        SWP_TN_LIFETIME& lt = (*this)(itr->second);
        lt.Add_TN (res_tn);
        _tn2lt[res_tn] = itr->second;
      }
    }

    UINT8 res;
    FOR_ALL_OP_RESULTs (op, res) {
      TN *tn = OP_result (op, res);

      if (!TN_is_register (tn)) continue;
      if (TN_is_dedicated (tn)) continue;

      if (   _reg_class != ISA_REGISTER_CLASS_UNDEFINED
          && _reg_class != TN_register_class (tn)) 
      {
        continue;
      }

      if (_ln.Is_Loop_Defined (tn)) {
	if (OP_uniq_res (op)) {
	  // 15140: To guarantee unique result register, start lifetime
	  // of result one cycle early to overlap operand TN lifetimes. 
	  Update_TN_Lifetime (_op_sub_cycle[i] - 1, 0, tn, SWP_Result);
	  Update_TN_Lifetime (_op_sub_cycle[i], 0, tn, SWP_Operand);
	} else {
	  Update_TN_Lifetime (_op_sub_cycle[i], 0, tn, SWP_Result);
	}
      }
    }
  }
   
  FOR_ALL_SWP_OP_VECTOR_IDXs (_ops, i) {
    const SWP_OP& swp_op = _ops(i);
    const OP *op = swp_op.Get_OP ();

    FmtAssert (op != 0, ("SWP_LIFETIME: Invalid OP pointer!"));
    
    if (OP_same_res (op)) {
      SWP_TN2INT_MAP::iterator itr;
      
      UINT8  idx = OP_find_opnd_use (op, OU_same_res);
      TN *res_tn = OP_result (op, 0);
      TN *opr_tn = OP_opnd (op, idx);
      
      if (   !TN_is_zero_reg (opr_tn)
          && (itr = _tn2lt.find (res_tn)) != _tn2lt.end ()) 
      {
        SWP_TN_LIFETIME& lt = (*this)(itr->second);
        lt.Add_TN (opr_tn);
        _tn2lt[opr_tn] = itr->second;
      }
    }

    UINT opr;
    FOR_ALL_OP_OPNDs (op, opr) {
      TN *tn = OP_opnd (op, opr);

      if (!TN_is_register (tn)) continue;
      if (TN_is_dedicated (tn)) continue;
      
      if (   _reg_class != ISA_REGISTER_CLASS_UNDEFINED
          && _reg_class != TN_register_class (tn)) 
      {
        continue;
      }
      
      if (_ln.Is_Loop_Variant (tn)) {
	const mUINT8 omega = swp_op.Get_Omega (opr);
        Update_TN_Lifetime (_op_sub_cycle[i],
                            omega,
                            tn,
                            SWP_Operand);
	if (max_omega < omega) {
	  max_omega = omega;
	}
      }
    }
  }


  for (CG_LOOP_BACKPATCH *bp = CG_LOOP_Backpatch_First (_tail, 0);
       bp != 0;
       bp = CG_LOOP_Backpatch_Next (bp)) 
  {
    TN *body_tn = CG_LOOP_BACKPATCH_body_tn (bp);
    const INT omega = CG_LOOP_BACKPATCH_omega (bp);
    if (   _reg_class != ISA_REGISTER_CLASS_UNDEFINED
        && _reg_class != TN_register_class (body_tn)) 
    {
      continue;
    }
    
    if (_ln.Is_Loop_Variant (body_tn)) {
      const INT alpha = omega;
      SWP_TN_LIFETIME& lt = (*this)(_tn2lt[body_tn]);

      if (lt.Get_Alpha () < alpha)
        lt.Set_Alpha (alpha);
    }

    /* 15030: A TN defined but not used in a loop may be read
     * by an epilog backpatch, so its lifetime must reach the
     * end of the loop body.
     */
    if (mve == NULL) {  // mve is NULL for original loop
      if (_ln.Is_Loop_Defined (body_tn)) {
	Update_TN_Lifetime (_ii * SWP_SUBCYCLES_PER_CYCLE,
			    omega,
			    body_tn,
			    SWP_Operand);
      }
      if (max_omega < omega) {
	max_omega = omega;
      }
    } else {  // Loop has been unrolled and TNs renamed
      INT num_kernels = mve->Get_Kernel_Unrollings ();
      INT old_ii = _ii / num_kernels;
      for (INT kernel = 0; kernel < num_kernels; ++kernel) {
	TN *kernel_tn = mve->Get_Renamed_TN (body_tn, omega, kernel);
	if (_ln.Is_Loop_Defined (kernel_tn)) {
	  INT end_kernel = kernel + omega;
	  INT new_omega = end_kernel / num_kernels;
	  end_kernel -= num_kernels * new_omega;
	  INT cycle = (end_kernel + 1) * old_ii * SWP_SUBCYCLES_PER_CYCLE;
	  Update_TN_Lifetime (cycle,
			      new_omega,
			      kernel_tn,
			      SWP_Operand);
	  if (max_omega < new_omega) {
	    max_omega = new_omega;
	  }
	}
      }
    }
  }

  FmtAssert(max_omega * _ii < (mUINT16) -1 / SWP_SUBCYCLES_PER_CYCLE,
	    ("cg_swp_lifetime.cxx: Subcycles may overflow mUINT16."));

  return;
}


void 
SWP_LIFETIME::Update_TN_Lifetime (mUINT16 cycle, 
                                  mUINT8 omega, 
                                  TN *tn, 
                                  SWP_TN_TYPE type)
{
  SWP_TN2INT_MAP::const_iterator itr;
  itr = _tn2lt.find (tn);
  if (itr == _tn2lt.end ()) {
    /* The first time we see this TN - lets create a new LIFETIME 
     * object
     */
    SWP_TN_LIFETIME lt (_m);

    if (type == SWP_Result) {
      lt.Set_Start_Sub_Cycle (cycle);
      /* TODO */
      lt.Set_End_Sub_Cycle (cycle);
      lt.Add_TN (tn);
    }
    else {
      lt.Set_End_Sub_Cycle (cycle + omega * _ii * SWP_SUBCYCLES_PER_CYCLE);
      lt.Add_TN (tn);
    }

    lt.Set_Omega (omega);
  
    _tn2lt[tn] = _lt.size ();
    _lt.push_back (lt);
  }
  else {
    /* We already have seen this TN before - lets update it */
    SWP_TN_LIFETIME& lt = (*this)(itr->second);
    
    if (type == SWP_Result) {
      if (lt.Get_Start_Sub_Cycle () > cycle) {
        lt.Set_Start_Sub_Cycle (cycle);
      }
    }
    else {
      UINT16 end = cycle + omega * _ii * SWP_SUBCYCLES_PER_CYCLE;
      if (lt.Get_End_Sub_Cycle () < end) {
        lt.Set_End_Sub_Cycle (end);
      }
    }

    if (lt.Get_Omega () < omega) {
      lt.Set_Omega (omega);
    }
  }

  return;
}


void 
SWP_LIFETIME::Print (FILE *f) const {
  fprintf (f, "%sRegister Lifetime:\n%s", DBar, DBar);
  for_each (_lt.begin (), 
            _lt.end (), 
            bind2nd (mem_fun_ref (&SWP_TN_LIFETIME::Print), f));
  fprintf (f, "%s\n", DBar);

  return;
}


void 
SWP_LIFETIME::Verify (void) const {

  return;
}

