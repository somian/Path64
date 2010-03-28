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
* \file cg_swp_allocator.cxx                                           *
* \brief Implementation file for the Register Allocator                *
***********************************************************************/

/* Basic compiler include files */
#include "defs.h"
#include "tracing.h"
#include "cxx_memory.h"

/* Code Generator (CG) related include files */
#include "cgtarget.h"

/* Software Pipelinig (SWP) related include files */
#include "cg_swp_tracing.h"
#include "cg_swp_allocator.h"

/* Standard Template Library (STL) related include files */
#include <cmath>
#include <functional>

using namespace std;



SWP_DIST::SWP_DIST (const SWP_LIFETIME& lt,
                    const SWP_LOOP& swp_loop,
                    MEM_POOL *m) :
  _m         (m),
  _num_lt    (lt.size ()),
  _dist      (lt.size () * lt.size (), 0, SWP_INT_ALLOC (m))
{
  INT kii = swp_loop.ii * 
            swp_loop.kernel_unrollings * 
            SWP_SUBCYCLES_PER_CYCLE;
  
  for (INT i = 0; i < _num_lt; ++i) {
    if (TN_is_dedicated (lt(i).Get_TN (0))) continue;

    for (INT j = i+1; j < _num_lt; ++j) {
      INT d1 = lt(i).Get_End_Sub_Cycle () - lt(j).Get_Start_Sub_Cycle ();
      INT d2 = lt(j).Get_End_Sub_Cycle () - lt(i).Get_Start_Sub_Cycle ();
      if (   (d1 > 0 && d2 > 0)
          || (d1 > kii)
          || (d2 > kii))
      {
        (*this)(i, j) = (*this)(j, i) = 1;
      }
    }
  }

  return;
}


void 
SWP_DIST::Print (FILE *f) const {
  fprintf (f, 
           "%sDist %dx%d\n%s", 
           DBar, 
           _num_lt, 
           _num_lt, 
           DBar);
  if (_num_lt == 0) return;
  
  fprintf (f, "  |");
  for (INT y = 0; y < _num_lt; ++y) {
    fprintf (f, " %2d", y);
  }
  fprintf (f, "\n");
  for (INT y = 0; y < _num_lt; ++y) {
    fprintf (f, "---");
  }
  fprintf (f, "\n");

  for (INT x = 0; x < _num_lt; ++x) { 
    fprintf (f, "%2d|", x);
    for (INT y = 0; y < _num_lt; ++y) {
      fprintf (f, " %2d", (*this)(x, y));
    }
    fprintf (f, "\n");
  }
  
  fprintf (f, "%s", DBar);

  return;
}


// TODO
void 
SWP_DIST::Verify (void) const {

  return;
}



SWP_ALLOCATOR::SWP_ALLOCATOR (const SWP_OP_VECTOR& mve_ops,
			      const SWP_MVE& mve,
                              const SWP_LIFENESS& lifeness,
                              const SWP_LOOP& swp_loop,
                              SWP_ORDER_ALGO order_algo,
                              SWP_ALLOC_ALGO alloc_algo,
                              MEM_POOL *m) :
  _m               (m),
  _ln              (lifeness),
  _swp_loop        (swp_loop),
  _lt              (0),
  _dist            (0),
  _order_algo      (order_algo),
  _alloc_algo      (alloc_algo),
  _lt_register_set (0),
  _reg_assign      (less <TN *> (), SWP_TN2REG_ALLOC (m)),
  _successful      (FALSE),
  _trace           (Get_Trace (TP_SWPIPE, REGISTER_FLAG))
{
  ISA_REGISTER_CLASS rclass;
  FOR_ALL_ISA_REGISTER_CLASS (rclass) {
    /* Get the allocatable register set for the current register class
     */
    _free_register = REGISTER_CLASS_allocatable (rclass);
    REGISTER_SET used_reg = _free_register;

    /* Calculate the lifetimes for the current register class */
    INT kii = swp_loop.ii * swp_loop.kernel_unrollings;
    _lt = CXX_NEW (SWP_LIFETIME (mve_ops, &mve, _ln, kii, _m, rclass), _m);
    
    /* First allocate the loop invariants. This will reduce the 
     * free_register set
     */
    if (!Allocate_Loop_Invariant (rclass)) {
      _successful = FALSE;
      return;
    }

    /* Now try to allocate the loop variants with the remaining free 
     * registers
     */
    if (!Allocate_Loop_Variant (rclass)) {
      _successful = FALSE;
      return;
    }

    /* Calculate how many registers we used from this register class */
    used_reg = REGISTER_SET_Difference (used_reg, _free_register);
    _register_count[rclass] = REGISTER_SET_Size (used_reg);
  } /* for (INT i = 0; i < ISA_REGISTER_CLASS_COUNT; ++i) */
  
  /* Now we do the actual assignment of registers to TNs */
  SWP_TN2REG_MAP::const_iterator itr;
  for (itr = _reg_assign.begin (); itr != _reg_assign.end (); ++itr) {
    TN_Allocate_Register (itr->first, itr->second);
  }
  _successful = TRUE;

  return;
} /* SWP_ALLOCATOR::SWP_ALLOCATOR */


BOOL
SWP_ALLOCATOR::Allocate_Loop_Invariant (ISA_REGISTER_CLASS rclass) {
  const TN_SET *invariant = _ln.Get_Invariant_TNs ();

  if (_trace) {
    fprintf (TFile, 
             "%sLoop Invariant Allocation (%s):\n%s", 
             SBar,
             REGISTER_CLASS_name (rclass),
             SBar);
    fprintf (TFile, "Free registers:\n");
    REGISTER_SET_Print (_free_register, TFile);
    fprintf (TFile, "\n");
  }

  TN *tn;
  FOR_ALL_TN_SET_members (invariant, tn) {
    /* ignore dedicated registers */
    if (TN_is_dedicated (tn)) continue;
    if (TN_register_class (tn) != rclass) continue;

    /* get the next register from the free register set */
    REGISTER reg;
    reg = REGISTER_SET_Choose_Intersection (
            _free_register, 
            REGISTER_CLASS_caller_saves (rclass));

    if (reg == REGISTER_UNDEFINED) {
      reg = REGISTER_SET_Choose (_free_register);
    }

    if (reg == REGISTER_UNDEFINED) {
      /* we run out of registers */
      if (_trace) {
        fprintf (TFile, "Run out of registers\n");
      }

      return FALSE;
    }

    /* assign the register to the tn */
    _reg_assign[tn] = reg;
    if (_trace) {
      fprintf (TFile, 
               "Assign %s to TN%d\n",
               REGISTER_name (rclass, reg), 
               TN_number (tn));
    }
    /*and remove it from the free register set */
    _free_register = REGISTER_SET_Difference1 (_free_register, reg);
    if (_trace) {
      fprintf (TFile, "Free registers (after registers assignment):\n");
      REGISTER_SET_Print (_free_register, TFile);
      fprintf (TFile, "\n");
    }
  } /* FOR_ALL_TN_SET_members (invariant, tn) */

  /* loop invariant register allocation was successful */
  return TRUE;
} /* SWP_ALLOCATOR::Allocate_Loop_Invariant */


BOOL 
SWP_ALLOCATOR::Allocate_Loop_Variant (ISA_REGISTER_CLASS rclass) {
  switch (_order_algo) {
    case Order_By_Start_Time:
      Sort_By_Start_Time ();
      break;

    case Order_By_Conflict:
      Fail_FmtAssertion ("NOT YET IMPLEMENTED");
      break;
    
    case Order_By_Conflict_And_Start_Time:
      Fail_FmtAssertion ("NOT YET IMPLEMENTED");
      break;

    case Order_By_Adjacency:
      Fail_FmtAssertion ("NOT YET IMPLEMENTED");
      break;

    case Order_By_Adjacency_And_Start_Time:
      Fail_FmtAssertion ("NOT YET IMPLEMENTED");
      break;

    case Order_By_Adjacency_And_Conflict:
      Fail_FmtAssertion ("NOT YET IMPLEMENTED");
      break;

    case Order_By_Adjacency_And_Conflict_And_Start_Time:
      Fail_FmtAssertion ("NOT YET IMPLEMENTED");
      break;

    default:
      Fail_FmtAssertion ("SWP::ALLOCATOR: Unknown order algorithm.");
      break;
  }

  /* Calculate DIST matrix (this has to be done after we have sorted
   * the lifetimes, otherwise the index will not match anymore to the
   * correct lifetime)
   */
  _dist = CXX_NEW (SWP_DIST (*_lt, _swp_loop, _m), _m);
  
  SWP_LIFETIME& lt = *_lt;
  SWP_DIST& dist   = *_dist;

  if (_trace) {
    lt.Print (TFile);
    dist.Print (TFile);
  }

  /* allocate a temporary register set for all lifetimes in the current
   * lifetime set
   */
  _lt_register_set = CXX_NEW_ARRAY (REGISTER_SET, lt.size (), _m);
  
  /* and initialize it with the remaining free registers */
  for (SWP_TN_LIFETIME_VECTOR::size_type i = 0; i < lt.size (); ++i) {
    _lt_register_set[i] = _free_register;
  }

  if (_trace) {
    fprintf (TFile, 
             "%sRegister Set: After initialization\n%s", 
             DBar, 
             DBar);
    for (SWP_TN_LIFETIME_VECTOR::size_type i = 0; i < lt.size (); ++i) {
      fprintf (TFile, "VTN%d: ", lt(i).Get_Idx ());
      REGISTER_SET_Print( _lt_register_set[i], TFile);
      fprintf (TFile, "\n");
    }
    fprintf (TFile, "%s", DBar);
  }
 
  INT last_lt = -1;

  for (INT selected_lt = 0; 
       selected_lt < lt.size ();
       ++selected_lt) 
  {
    if (TN_is_dedicated (lt(selected_lt).Get_TN (0))) continue;

    /* selected_loc is actually a register, but Rau called it location
     * in his algorithm, so we keep the naming schema to be consistent 
     * with his paper.
     */
    REGISTER selected_loc;
    
    switch (_alloc_algo) {
      case Best_Fit_Algo:
        Fail_FmtAssertion ("NOT YET IMPLEMENTED");
        break;

      case First_Fit_Algo:
        selected_loc = First_Fit (selected_lt, rclass);
        break;

      case End_Fit_Algo:
        Fail_FmtAssertion ("NOT YET IMPLEMENTED");
        break;

      default:
        Fail_FmtAssertion ("SWP::ALLOCATOR: Unknown " \
                           "allocation algorithm.");
        break;
        
    }

    if (selected_loc == REGISTER_UNDEFINED) {
      return FALSE;
    }

    for (SWP_TN_VECTOR::size_type i = 0; 
         i < lt(selected_lt).Get_TN_Size (); 
         ++i)
    {
      _reg_assign[lt(selected_lt).Get_TN (i)] = selected_loc;
    }
    Update_Free_Register_Set (selected_lt, selected_loc);
    last_lt = selected_lt;
  } /* for (INT selected_lt = 0 ... */

  return TRUE;
} /* SWP_ALLOCATOR::Allocate_Loop_Variant */


REGISTER 
SWP_ALLOCATOR::Best_Fit (INT selected_lt,
                         ISA_REGISTER_CLASS rclass) const
{

}


REGISTER 
SWP_ALLOCATOR::First_Fit (INT selected_lt, 
                          ISA_REGISTER_CLASS rclass) const 
{
  REGISTER reg;
  reg = REGISTER_SET_Choose_Intersection (
          _lt_register_set[selected_lt], 
          REGISTER_CLASS_caller_saves (rclass));

  if (reg == REGISTER_UNDEFINED) {
    reg = REGISTER_SET_Choose (_lt_register_set[selected_lt]);
  }

  return reg;
}


REGISTER 
SWP_ALLOCATOR::End_Fit (INT selected_lt,
                        ISA_REGISTER_CLASS rclass) const
{

}


void 
SWP_ALLOCATOR::Update_Free_Register_Set (INT selected_lt, 
                                         REGISTER selected_loc) 
{
  const SWP_LIFETIME& lt   = *_lt;
  const SWP_DIST&     dist = *_dist;

  /* Remove all registers, except the allocated one, from the register
   * set of the currently allocated lifetime.
   * This is actually not neccessary, but it makes the trace file easier
   * to read.
   */
  _lt_register_set[selected_lt] = 
    REGISTER_SET_Intersection1 (_lt_register_set[selected_lt], 
                                selected_loc);


  /* Now remove the just allocated register from all not allocated
   * lifetimes which would interfere
   */
  for (INT j = selected_lt + 1; j < lt.size (); ++j) {
    if (dist (selected_lt, j) != 0) {
      _lt_register_set[j] = 
        REGISTER_SET_Difference1 (_lt_register_set[j], selected_loc);
    }
  }

  if (_trace) {
    fprintf (TFile, 
             "%sRegister Set: After allocation of register %d\n%s", 
             DBar, 
             selected_loc,
             DBar);
    for (SWP_TN_LIFETIME_VECTOR::size_type i = 0; i < lt.size (); ++i) {
      fprintf (TFile, "VTN%d: ", lt(i).Get_Idx ());
      REGISTER_SET_Print( _lt_register_set[i], TFile);
      fprintf (TFile, "\n");
    }
    fprintf (TFile, "%s", DBar);
  }

  _free_register =
    REGISTER_SET_Difference1 (_free_register, selected_loc);


  return;
} /*  SWP_ALLOCATOR::Update_Free_Register_Set (INT selected_lt, ... */


void 
SWP_ALLOCATOR::Print (FILE *f) const {
  
  return;
}


void 
SWP_ALLOCATOR::Verify (void) const {

  return;
}

