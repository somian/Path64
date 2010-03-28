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
#include "mempool.h"

/* Software Pipelinig (SWP) related include files */
#include "cg_swp_mindist.h"
#include "cg_swp_op_vector.h"

/* Standard Template Library (STL) related include files */
#include <algorithm>

using namespace std;



SWP_MINDIST::SWP_MINDIST (const SWP_OP_VECTOR& ops, 
                          UINT16 ii, 
                          MEM_POOL *m) :
  _m       (m),
  _num_ops (ops.size ()),
  _mindist (ops.size () * ops.size (), -_INFINITY, SWP_mINT16_ALLOC (m))
{
  SWP_OP_VECTOR::size_type start  = ops.Get_Start_Idx ();
  SWP_OP_VECTOR::size_type stop   = ops.Get_Stop_Idx ();
  SWP_OP_VECTOR::size_type branch = ops.Get_Branch_Idx ();

  /* Set arc weights
   * The first OP is always the fake "START" OP and the last element is
   * alway the fake "STOP" OP. Therefore we will ignore the first and 
   * the last OP.
   */
  for (SWP_OP_VECTOR::size_type i = 1; i < ops.size () - 1; ++i) {
    OP *op = ops(i).Get_OP ();
    FmtAssert (op != 0, ("SWP_MINDIST: Invalid OP pointer"));

    for (ARC_LIST *succs = OP_succs (op); 
         succs != 0; 
         succs = ARC_LIST_rest (succs)) 
    {
      SWP_OP_VECTOR::size_type j;
      const ARC *succ_arc = ARC_LIST_first (succs);
      OP        *succ_op  = ARC_succ (succ_arc);
      j                   = ops.OP_Idx (succ_op);

      (*this)(i, j) =   
        max ((*this)(i, j),
             (mINT16)(ARC_latency (succ_arc) - 
                      ARC_omega (succ_arc) * ii));
    } /* for (ARC_LIST *succs = OP_succs (op) ... */

    /* Every OP has a dependence to START and STOP with weight 0 */
    (*this)(start, i) = 0;
    (*this)(i, stop)  = 0;
  } /* for (INT i = 1; i < ops.size () - 1; ++i) */


  /* Floyd-Warshall All-Pairs Shortest Path Algorithm */
  SWP_OP_VECTOR::size_type k, i, j;
  FOR_ALL_SWP_OP_VECTOR_IDXs (ops, k) {
    FOR_ALL_SWP_OP_VECTOR_IDXs (ops, i) {
      FOR_ALL_SWP_OP_VECTOR_IDXs (ops, j) {
        #ifdef Is_True_On
          /* Keep INFINITY at INFINITY to make the trace file more 
           * readable
           */
          if (   (*this)(i, k) != -_INFINITY 
              && (*this)(k, j) != -_INFINITY) 
          {
            (*this)(i, j) =
              max ((*this)(i, j), 
                   (mINT16)((*this)(i, k) + (*this)(k, j)));
          }
        #else
          (*this)(i, j) =
            max ((*this) (i, j), 
                 (mINT16)((*this)(i, k) + (*this)(k, j)));
        #endif
      }
    }
  }

  return;
}


void 
SWP_MINDIST::Remove_Self_Recurrences (void) {
  for (SWP_OP_VECTOR::size_type i = 0; i < _num_ops; ++i) {
    (*this)(i, i) = 0;
  }

  return;
}


void 
SWP_MINDIST::Print (FILE *f) const {
  fprintf (f, "%sSWP_MINDIST %" PRIdPTR "x%" PRIdPTR ":\n%s", DBar, _num_ops, _num_ops, DBar);
  fprintf (f, "     ");
  
  for (SWP_OP_VECTOR::size_type j = 0; j < _num_ops; ++j) {
    fprintf (f,"%5zd", j);
  }
    
  fprintf (f, "\n");
  for (SWP_OP_VECTOR::size_type i = 0; i < _num_ops; ++i) {
    fprintf (f, "%3zd: ", i);
      
    for (SWP_OP_VECTOR::size_type j = 0; j < _num_ops; ++j) {
      if ((*this) (i, j) == -_INFINITY) {
        fprintf (f, " -INF");
      }
      else {
        fprintf (f,"%5d", (*this)(i, j));
      }
    }
    fprintf(f, "\n");
  }

  fprintf (f, "%s", DBar);

  return;
}


void 
SWP_MINDIST::Verify (void) const {
  for (SWP_OP_VECTOR::size_type i = 0; i < _num_ops; ++i) {
    /* If II >= RecMII, then we will get always values <= 0 on the 
     * diagonal. If not, then our RecMII calculation was incorrect.
     */
    FmtAssert ((*this)(i, i) <= 0, 
               ("SWP_MINDIST: Incorrect II."));
  }
  
  return;
}

