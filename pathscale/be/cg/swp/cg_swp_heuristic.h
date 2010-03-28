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
* \file cg_swp_heuristic.h                                             *
* \brief Header file for scheduling heuristic                          *
*                                                                      *
* This heuristic is based on                                           *
* Huff, Richard A., Lifetime-Sensitive Modulo Scheduling, ACM SIGPLAN  *
* Notices, volume 28, pages 258-267, 1993                              *
***********************************************************************/

#ifndef cg_swp_heuristic_INCLUDED
#define cg_swp_heuristic_INCLUDED "cg_swp_heuristic.h"

/* Basic compiler include files */
#include "defs.h"
#include "mempool.h"

/* Software Pipelinig (SWP) related include files */
#include "cg_swp.h"
#include "cg_swp_op_vector.h"
#include "cg_swp_mrt.h"
#include "cg_swp_mindist.h"
#include "cg_swp_slack.h"
#include "cg_swp_minlt.h"



class SWP_HEURISTIC {
  private:
  SWP_OP_VECTOR&     _ops;
  SWP_MRT&           _mrt;
  const SWP_MINDIST& _mindist;
  SWP_SLACK&         _slack;
  const SWP_MINLT&   _minlt;
  SWP_FLOAT_VECTOR   _scale;
  SWP_INT_VECTOR     _adjust;
  BOOL               _trace;
  INT                _ii;
  INT                _num_mops;
  INT                _num_flops;
  INT                _num_iops;

  SWP_SEARCH_DIRECTION 
  Search_Direction (INT) const;
  void 
  Calculate_Critical_Resources (void);

  public:
  SWP_HEURISTIC (SWP_OP_VECTOR& ops, 
                 SWP_MRT& mrt, 
                 const SWP_MINDIST& mindist, 
                 SWP_SLACK& slack, 
                 const SWP_MINLT& minlt,
                 INT ii,
                 MEM_POOL *m);
  ~SWP_HEURISTIC (void) { return; }

  INT
  Choose_OP (void) const;
  pair <BOOL, BOOL> 
  Choose_Cycle (INT candidate) const;
    
  void 
  Eject_Conflicting_Resources (INT candidate);
  void 
  Eject_Dependence_Conflict_OPs (INT candidate);
  void
  Eject_Successors_Of_OP (SWP_OP& swp_op);

  BOOL 
  Update_Dependencies (INT candidate);

  float& 
  Scale (INT);
  float  
  Scale (INT) const;

  INT& 
  Adjust_OP (INT);
  INT  
  Adjust_OP (INT) const;

  void 
  Print (FILE *f = stderr) const;
  void 
  Verify (void) const;
};


inline float& 
SWP_HEURISTIC::Scale (INT x) {
  Is_True ((x >= 0) && (x < _scale.size ()),
           ("SWP_HEURISTIC: Out of bounds!"));
  return _scale[x];
}


inline float 
SWP_HEURISTIC::Scale (INT x) const {
  Is_True ((x >= 0) && (x < _scale.size ()),
           ("SWP_HEURISTIC: Out of bounds!"));
  return _scale[x];
}
  
  
inline INT& 
SWP_HEURISTIC::Adjust_OP (INT x) { 
  Is_True ((x >= 0) && (x < _adjust.size ()),
           ("SWP_HEURISTIC: Out of bounds!"));
  return _adjust[x];
}
  
  
inline INT 
SWP_HEURISTIC::Adjust_OP (INT x) const {
  Is_True ((x >= 0) && (x < _adjust.size ()),
           ("SWP_HEURISTIC: Out of bounds!"));
  return _adjust[x];
}

#endif /* cg_swp_heuristic_INCLUDED */

