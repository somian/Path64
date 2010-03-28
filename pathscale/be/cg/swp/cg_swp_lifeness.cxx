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
* \file cg_swp_lifeness.cxx                                            *
* \brief Implementation of LIFENESS.                                   *
***********************************************************************/

/* Basic compiler include files */
#include "defs.h"
#include "tracing.h"

/* Code Generator (CG) related include files */
#include "op.h"
#include "tn.h"
#include "tn_set.h"

/* Software Pipelining (SWP) related include files */
#include "cg_swp_op_vector.h"
#include "cg_swp_lifeness.h"

/* Standard Template Library (STL) related include files */
#include <functional>

using namespace std;



SWP_LIFENESS::SWP_LIFENESS (const SWP_OP_VECTOR::const_iterator begin, 
                            const SWP_OP_VECTOR::const_iterator end,
                            MEM_POOL *m) :
  _m (m)
{
  Calculate_Lifeness (begin, end);

  return;
}


SWP_LIFENESS::SWP_LIFENESS (const SWP_OP_VECTOR& ops,
                            MEM_POOL *m) :
  _m (m)
{
  Calculate_Lifeness (ops.begin (), ops.end ());

  return;
}


void 
SWP_LIFENESS::Calculate_Lifeness (
  const SWP_OP_VECTOR::const_iterator begin,
  const SWP_OP_VECTOR::const_iterator end)
{
  _tn_defined   = TN_SET_Create_Empty (Last_TN + 1, _m);
  _tn_used      = TN_SET_Create_Empty (Last_TN + 1, _m);
  _tn_dedicated = TN_SET_Create_Empty (Last_TN + 1, _m);

  SWP_OP_VECTOR::const_iterator itr;
  for (itr = begin; itr != end; ++itr) {
    const SWP_OP& swp_op = *itr;
    const OP *op         = swp_op.Get_OP ();
    
    for (INT opr = 0; opr < OP_opnds (op); ++opr) {
      const TN *tn = OP_opnd (op, opr);
      /* only check registers - not constants */
      if (!TN_is_register (tn)) continue;

      if (TN_is_dedicated (tn)) {
        _tn_dedicated = TN_SET_Union1D (_tn_dedicated, tn, _m);
      }

      _tn_used = TN_SET_Union1D (_tn_used, tn, _m);
    }

    for (INT res = 0; res < OP_results (op); ++res) {
      const TN *tn = OP_result (op, res);
      /* only check registers - not constants */
      if (!TN_is_register (tn)) continue;

      if (TN_is_dedicated (tn)) {
        _tn_dedicated = TN_SET_Union1D (_tn_dedicated, tn, _m);
      }

      _tn_defined = TN_SET_Union1D (_tn_defined, tn, _m);
    }
    
  }

  _tn_invariant = TN_SET_Difference (_tn_used, _tn_defined, _m);
  _tn_variant   = TN_SET_Intersection (_tn_used, _tn_defined, _m);

  return;
}


void 
SWP_LIFENESS::Print (FILE *f) const {
  fprintf (f, "%sLifeness:\n%s", DBar, DBar);
  fprintf (f,   "TNs defined in loop: ");
  TN_SET_Print (_tn_defined, f);
  fprintf (f, "\nTNs used in loop:    ");
  TN_SET_Print (_tn_used, f);
  fprintf (f, "\nLoop invariant TNs:  ");
  TN_SET_Print (_tn_invariant, f);
  fprintf (f, "\nLoop variant TNs:    ");
  TN_SET_Print (_tn_variant, f);
  fprintf (f, "\nDedicated TNs:       ");
  TN_SET_Print (_tn_dedicated, f);
  fprintf (f, "\n%s\n", DBar);

  return;
}


void 
SWP_LIFENESS::Verify (void) const {

  return;
}

