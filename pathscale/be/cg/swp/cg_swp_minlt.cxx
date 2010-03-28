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


/* Basic compiler include files */
#include "defs.h"

/* Code Generator (CG) related include files */
#include "cg_dep_graph.h"

/* Software Pipelinig (SWP) related include files */
#include "cg_swp.h"
#include "cg_swp_op_vector.h"
#include "cg_swp_mindist.h"
#include "cg_swp_minlt.h"

/* STL related include files */
#include <algorithm>

using namespace std;



SWP_MINLT::SWP_MINLT (const SWP_OP_VECTOR& ops, 
                      const SWP_MINDIST& mindist,
                      INT ii,
                      MEM_POOL *m) :
  _m     (m),
  _ops   (ops),
  _minlt (ops.size (), 0, SWP_INT_ALLOC (m))
{
  /* Ignore first and last OP, because they are 
   * fake "START" and "STOP" OPs
   */
  for (INT d = 1; d < _ops.size () - 1; ++d) {
    OP *op = _ops(d).Get_OP ();
    FmtAssert (op != 0, ("SWP_MINLT: Invalid OP pointer"));

    for (ARC_LIST *succs = OP_succs (op); 
         succs; 
         succs = ARC_LIST_rest (succs)) 
    {
      const ARC *succs_arc = ARC_LIST_first (succs);
        
      /* only consider REGIN */
      if (ARC_kind (succs_arc) == CG_DEP_REGIN) {
        OP *succs_op  = ARC_succ (succs_arc);
        INT u         = _ops.OP_Idx (succs_op);

        Is_True (
          OP_result (op, 0) == OP_opnd (succs_op, ARC_opnd (succs_arc)),
          ("SWP_MINLT: Unexpected TN/register dependency")); 
        
        (*this)(d) = max ((*this)(d), 
                           ARC_omega (succs_arc) * ii + mindist(d, u));
      }
    }
  }

  return;
}


void 
SWP_MINLT::Print (FILE *f) const {
  fprintf (f, "%sSWP_MINLT:\n%s", DBar, DBar);
  for (INT i = 0; i < _minlt.size (); ++i) {
    fprintf (f, "%3d: %4d ", i, (*this)(i));
    
    OP *op = _ops(i).Get_OP ();
    if (OP_results (op) > 0) {
      Print_TN (OP_result (op, 0), FALSE, f);
    }

    fprintf (f, "\n");
  }
  fprintf (f, "%s", DBar);

  return;
}


void 
SWP_MINLT::Verify (void) const {
  
  return;
}

