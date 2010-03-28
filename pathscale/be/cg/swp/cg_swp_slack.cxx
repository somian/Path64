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

/* Code Generator related include files */
#include "cg_dep_graph.h"

/* Software Pipelinig (SWP) related include files */
#include "cg_swp.h"
#include "cg_swp_op_vector.h"
#include "cg_swp_mindist.h"
#include "cg_swp_slack.h"

/* Standard Template Library (STL) related include files */
#include <cmath>
#include <algorithm>

using namespace std;



SWP_SLACK::SWP_SLACK (const SWP_OP_VECTOR& ops, 
                      const SWP_MINDIST& mindist,
                      INT ii,
                      MEM_POOL *m) :
  _m       (m),
  _ops     (ops),
  _mindist (mindist),
  _start   (ops.Get_Start_Idx ()),
  _stop    (ops.Get_Stop_Idx ()),
  _estart  (ops.size (), -1, SWP_INT_ALLOC (m)),
  _lstart  (ops.size (), -1, SWP_INT_ALLOC (m))
{
  INT lstart_stop = static_cast<INT> 
    (ceil (static_cast<float> ((_mindist(_start, _stop))+2) / ii)) * 
    ii - 1;

  Init (lstart_stop);

  return;
}


void 
SWP_SLACK::Init (INT sched_length) {
  for (INT x = 0; x < _ops.size (); ++x) {
    Estart(x) = _mindist(_start, x);
    Lstart(x) = sched_length - _mindist(x, _stop);
  }

  return;
}

  
void 
SWP_SLACK::Update_Slack (void) {
  INT lstart_stop = Lstart(_stop);

  for (INT x = 0; x < _estart.size (); ++x) {
    if (!_ops(x).Is_Scheduled ()) {
      Estart(x) = _mindist(_start, x);
      Lstart(x) = lstart_stop - _mindist(x, _stop);

      for (INT y = 0; y < _estart.size (); ++y) {
        if (_ops(y).Is_Scheduled ()) {
          INT cycle = _ops(y).Get_Cycle ();
          Estart(x) = max (Estart(x), cycle + _mindist(y, x));
          Lstart(x) = min (Lstart(x), cycle - _mindist(x, y));
        }
      }
    }
    else {
      Estart(x) = Lstart(x) = _ops(x).Get_Cycle ();
    }
  }
  
  return;
}


void 
SWP_SLACK::Update_Slack (INT y) {
  INT cycle = _ops(y).Get_Cycle ();
    
  for (INT x = 0; x < _estart.size (); ++x) {
    if (!_ops(x).Is_Scheduled ()) {
      Estart(x) = max (Estart(x), cycle + _mindist(y, x));
      Lstart(x) = min (Lstart(x), cycle - _mindist(x, y));
    }
  }
  
  Estart(y) = Lstart(y) = cycle;

  return;    
}


void 
SWP_SLACK::Update_Slack (SWP_INT_VECTOR &adjust) {
  /* Create a list of all OP which boundaries need to be recomputed */
  SWP_BOOL_VECTOR marked (_ops.size (), FALSE, SWP_BOOL_ALLOC (_m));

  while (!adjust.empty ()) {
    INT i = adjust.back ();
    adjust.pop_back ();
    marked[i] = TRUE;

    if (i == _start || i == _stop) {
      continue;
    }

    for (ARC_LIST *arc_list = OP_preds (_ops(i).Get_OP ());
         arc_list != 0;
         arc_list = ARC_LIST_rest (arc_list)) 
    {
      ARC *arc     = ARC_LIST_first (arc_list);
      OP *pred     = ARC_pred (arc);
      INT pred_idx = _ops.OP_Idx (pred);
      if (!_ops(pred_idx).Is_Scheduled () && !marked[pred_idx]) {
        marked[pred_idx] = TRUE;
        adjust.push_back (pred_idx);
      }
    }
    
    for (ARC_LIST *arc_list = OP_succs (_ops(i).Get_OP ());
         arc_list != 0;
         arc_list = ARC_LIST_rest (arc_list)) 
    {
      ARC *arc     = ARC_LIST_first (arc_list);
      OP *succ     = ARC_succ (arc);
      INT succ_idx = _ops.OP_Idx (succ);
      if (!_ops(succ_idx).Is_Scheduled () && !marked[succ_idx]) {
        marked[succ_idx] = TRUE;
        adjust.push_back (succ_idx);
      }
    }
  }

  INT lstart_stop = Lstart(_stop);

  for (INT i = 0; i < marked.size (); ++i) {
    if (!marked[i]) continue;

    Estart(i) = _mindist(_start, i);
    Lstart(i) = lstart_stop - _mindist(i, _stop);

    for (INT j = 0; j < _estart.size (); ++j) {
      if (_ops(j).Is_Scheduled ()) {
        INT cycle = _ops(j).Get_Cycle ();
        Estart(i) = max (Estart(i), cycle + _mindist(j, i));
        Lstart(i) = min (Lstart(i), cycle - _mindist(i, j));
      }
    }
  }

  return;
}


void 
SWP_SLACK::Print (FILE *f) const {
  fprintf (f, "%sSlack:\n%s", DBar, DBar);
  FmtAssert (_estart.size () == _lstart.size (),
             ("SWP_SLACK: Estart and Lstart are of different size."));

  for (INT x = 0; x < _estart.size (); ++x) {
    fprintf (f, 
             "%3d: Estart = %4d, Lstart = %4d, Slack = %4d\n",
             x,
             Estart(x),
             Lstart(x),
             (*this)(x));
  }

  fprintf (f, "%s", DBar);

  return;
}


void 
SWP_SLACK::Verify (void) const {
  FmtAssert (Estart(_start) <= Lstart(_start),
               ("SWP_SLACK: Estart (START) is later then " \
                "Lstart (START)!"));
  FmtAssert (Estart(_start) <= Estart(_stop),
               ("SWP_SLACK: Estart (START) is later then " \
                "Estart (STOP)!"));
  FmtAssert (Lstart(_start) <= Lstart(_stop),
               ("SWP_SLACK: Lstart (START) is later then " \
                "Lstart (STOP)!"));
  FmtAssert (Estart(_stop) <= Lstart(_stop),
               ("SWP_SLACK: Estart (STOP) is later then " \
                "Lstart (STOP)!"));


  for (INT i = 1; i < _ops.size () - 1; ++i) {
    FmtAssert (Estart(_start) <= Estart(i),
               ("SWP_SLACK: Estart (START) is later then " \
                "Estart (%i)!",
                i));
    FmtAssert (Estart(i) <= Lstart(i),
               ("SWP_SLACK: Estart (%i) is later then Lstart (%i)!",
                i,
                i));
    FmtAssert (Lstart(i) <= Lstart(_stop),
               ("SWP_SLACK: Lstart (%i) is later then " \
                "Lstart (STOP)!",
                i));
  }

  return;
}

