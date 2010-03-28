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
#include "tracing.h"

/* Software Pipelinig (SWP) related include files */
#include "cg_swp_options.h"
#include "cg_swp_tracing.h"
#include "cg_swp_op_vector.h"
#include "cg_swp_mrt.h"
#include "cg_swp_mindist.h"
#include "cg_swp_slack.h"
#include "cg_swp_minlt.h"
#include "cg_swp_heuristic.h"
#include "cg_swp_target.h"

/* Standard Template Library (STL) related include files */
#include <cmath>

using namespace std;



SWP_HEURISTIC::SWP_HEURISTIC (SWP_OP_VECTOR& ops,
                              SWP_MRT& mrt,
                              const SWP_MINDIST& mindist,
                              SWP_SLACK& slack, 
                              const SWP_MINLT& minlt,
                              INT ii,
                              MEM_POOL *m) :
  _ops     (ops),
  _mrt     (mrt),
  _mindist (mindist),
  _slack   (slack),
  _minlt   (minlt),
  _scale   (ops.size (), 1.0f, SWP_FLOAT_ALLOC (m)),
  _adjust  (SWP_INT_ALLOC (m)),
  _ii      (ii)
{
  INT start  = _ops.Get_Start_Idx ();
  INT stop   = _ops.Get_Stop_Idx ();
  INT branch = _ops.Get_Branch_Idx (); 

  _trace =    Get_Trace (TP_SWPIPE, HEURISTIC_FLAG)
           || Get_Trace (TP_SWPIPE, DETAILED_FLAG)
           || Get_Trace (TP_SWPIPE, SCHEDULER_FLAG);
  
  SWP_TARGET_Calculate_Critical_Resources (ops, _ii);


  /* Schedule fake START and STOP OP */
  _ops(start).Set_Cycle (_slack.Estart(start), _ii);
  _ops(start).Reset_Try_Counter ();  
  _mrt.Reserve_Resources (_ops(start));
  _slack.Update_Slack (start);

  _ops(stop).Set_Cycle (_slack.Lstart (stop), _ii);
  _ops(stop).Reset_Try_Counter ();
  _mrt.Reserve_Resources (_ops(stop));
  _slack.Update_Slack (stop);


  /* Initialize OPs */
  for (INT i = 1; i < _ops.size () - 1; ++i) {
    OP *op = _ops(i).Get_OP ();

    _ops(i).Reset_Scheduled ();
    _ops(i).Set_Cycle (-1, ii);
    _ops(i).Reset_Try_Counter ();
    Scale(i) = SWP_TARGET_Get_Scale_Factor (op);
  }


  /* Schedule branch */
  INT cycle = SWP_TARGET_Get_Branch_Cycle (_ii);
  _ops(branch).Set_Cycle (cycle, ii);
  _ops(branch).Reset_Try_Counter ();
  _mrt.Reserve_Resources (_ops(branch));
  _slack.Update_Slack (branch);

  return;
} /* SWP_HEURISTIC::SWP_HEURISTIC */


INT 
SWP_HEURISTIC::Choose_OP (void) const {
  INT   candidate = -1;
  float highest_priority  = 1000000.0f;
  /* lower values have higher priority */

  for (INT i = 1; i < _ops.size () - 1; ++i) {
    if (_ops(i).Is_Scheduled ()) {
      continue;
    }

    float priority = _slack(i) * Scale(i);
    if (   priority < highest_priority 
        || (   priority == highest_priority
            && _slack.Lstart(i) < _slack.Lstart(candidate))
       )
    {
      highest_priority = priority;
      candidate = i;
    }
  }
  
  if (_trace) {
    if (candidate >= 0) {
      fprintf (TFile, "Next candidate = %i\n", candidate);
      Print_OP_No_SrcLine (_ops(candidate).Get_OP (), TFile);
    }
    else {
      fprintf (TFile, 
               "No more candidates! It seems we are finished.\n");
    }
  }

  return candidate;
}


pair <BOOL, BOOL> 
SWP_HEURISTIC::Choose_Cycle (INT candidate) const {
  SWP_SEARCH_DIRECTION dir = Search_Direction (candidate);

  INT cycle = _mrt.Find_Free_Resources (_ops(candidate),
                                        _slack.Estart(candidate),
                                        _slack.Lstart(candidate),
                                        dir);

  /* Couldn't find an free issue slot */
  if (cycle < 0) {

    cycle = max (_slack.Estart(candidate), 
                 _ops(candidate).Get_Cycle () + 1);  
    
    if (_trace) {
      fprintf (TFile, 
               "Could 'not' find a free issue slot. " \
               "I will try cycle %d\n",
               cycle);
    }
  }

  INT cycle2 = SWP_TARGET_Scheduling_Constrain_For_OP (_ops, 
                                                       candidate, 
                                                       cycle, 
                                                       _ii);
  if (_trace && (cycle != cycle2)) {
    fprintf (TFile, 
             "Scheduling Constrain: Cycle changed to %d\n",
             cycle2);
  }
  
  _ops(candidate).Set_Cycle (cycle2, _ii);

  return pair <BOOL, BOOL> 
    ((   _ops(candidate).Get_Cycle () < _slack.Estart(candidate)
      || _ops(candidate).Get_Cycle () > _slack.Lstart(candidate)
     ),
     !_mrt.Resources_Available (_ops(candidate), cycle2)
    );
}


void 
SWP_HEURISTIC::Eject_Conflicting_Resources (INT candidate) {
  INT cycle = _ops(candidate).Get_Cycle ();

  /*********************************************************************
  * 1. Try to unschedule an OP which is issued in the same cycle and   *
  *    uses exactly the same resources as the OP we would like to      *
  *    schedule.                                                       *
  *********************************************************************/
  for (INT i = 1; i < _ops.size () - 1; ++i) {
    if (   _ops(i).Is_Scheduled ()
        && (_ops(i).Get_Cycle () - cycle) % _ii == 0
        && _mrt.Resources_Equivalent (_ops(i), _ops(candidate))
       )
    {
      _mrt.Unreserve_Resources (_ops(i));
      /* the slack of all unscheduled OPs which preceed or succed this
       * OP needs to be adjusted.
       */
      _adjust.push_back (i);
      
      if (_trace) {
        fprintf (TFile, 
                 "Eject OP %d due to equivalent resource usage\n", 
                 i);
        Print_OP (_ops(i).Get_OP (), TFile);
      }

      return;
    }
  }
  /********************************************************************/


  /*********************************************************************
  * 2. Try to unschedule an OP which uses relevant resources and is    *
  *    not necessarily scheduled in the same cycle.                    *
  *********************************************************************/
  for (INT i = 1; i < _ops.size () - 1; ++i) {
    if (   _ops(i).Is_Scheduled ()
        && _mrt.Resources_Relevant (_ops(i), _ops(candidate))
       )
    {
      _mrt.Unreserve_Resources (_ops(i));
      /* the slack of all unscheduled OPs which preceed or succed this
       * OP needs to be adjusted.
       */
      _adjust.push_back (i);

      if (_trace) {
        fprintf (TFile, 
                 "Eject OP %d due to relevant resource usage\n", 
                 i);
        Print_OP (_ops(i).Get_OP (), TFile);
      }

      return;
    }
  }
  /********************************************************************/

  /*********************************************************************
  * 3. Try to unschedule an OP which might not use any required        *
  *    resources, but it uses the same issue slot.                     *
  *********************************************************************/
  for (INT i = 1; i < _ops.size () - 1; ++i) {
    if (   _ops(i).Is_Scheduled ()
        && (_ops(i).Get_Cycle () - cycle) % _ii == 0
       )
    {
      _mrt.Unreserve_Resources (_ops(i));
      /* the slack of all unscheduled OPs which preceed or succed this
       * OP needs to be adjusted.
       */
      _adjust.push_back (i);
        
      if (_trace) {
        fprintf (TFile, 
                 "Eject OP %d due to same issue cycle usage\n", 
                 i);
        Print_OP (_ops(i).Get_OP (), TFile);
      }

      return;
    }
  }
  /********************************************************************/

  return;
} /* SWP_HEURISTIC::Eject_Conflicting_Resources (INT candidate) */


void 
SWP_HEURISTIC::Eject_Dependence_Conflict_OPs (INT candidate) {
  /* The candidate has been scheduled outside its Estart/Lstart 
   * boundaries and therefore could affect other OPs Estart/Lstart
   * boundaries or even have a conflict with already scheduled OPs.
   */
  INT cycle = _ops(candidate).Get_Cycle ();

  for (INT i = 0; i < _ops.size (); ++i) {
    /* Compute new Estart/Lstart boundaries for OP i */
    INT estart = cycle + _mindist(candidate, i);
    INT lstart = cycle - _mindist(i, candidate);
      
    if (_ops(i).Is_Scheduled ()) {
      if (   _ops(i).Get_Cycle () < estart 
          || _ops(i).Get_Cycle () > lstart)
      {
        /* Scheduling of candidate violates the Estart/Lstart boundaries
         * of OP i. We need to eject Op i, reschedule it and adjust the
         * Estart/Lstart boundaries.
         */
        _mrt.Unreserve_Resources (_ops(i));
        /* the slack of all unscheduled OPs which preceed or succed this
         * OP needs to be adjusted.
         */
        _adjust.push_back (i);
        
        if (_trace) {
          fprintf (TFile, 
                   "Eject OP %d due to dependence constrain.\n", 
                   i);
        }

        Eject_Successors_Of_OP (_ops(i));
      }
    }
    else {
      if (   _slack.Estart(i) < estart
          || _slack.Lstart(i) > lstart)
      {
        _adjust.push_back (i);
      }
    }
  } /* for (INT i = 0; i < _ops.size (); ++i) */

  return;
}

void
SWP_HEURISTIC::Eject_Successors_Of_OP (SWP_OP& swp_op) {
  OP *op = swp_op.Get_OP ();
  if (   OP_code (op) == TOP_swp_start
      || OP_code (op) == TOP_swp_stop ) return;

  ARC_LIST *arc_list;
  for (arc_list = OP_succs (op);
       arc_list != 0;
       arc_list = ARC_LIST_rest (arc_list))
  {
    ARC *arc      = ARC_LIST_first (arc_list);
    
    if (ARC_kind (arc) != CG_DEP_REGIN) {
      continue;
    }

    OP  *succ     = ARC_succ (arc);
    INT  succ_idx = _ops.OP_Idx (succ);
    
    SWP_INT_VECTOR::const_iterator itr;
    itr = find (_adjust.begin (), _adjust.end (), succ_idx);
    if (itr != _adjust.end ()) {
      continue;  
    }

    if (_ops(succ_idx).Is_Scheduled ()) {
      _mrt.Unreserve_Resources (_ops(succ_idx));
      if (_trace) {
        fprintf (TFile, 
                 "Eject OP %d, which is a succsessor of OP %zd.\n", 
                 succ_idx, 
                 _ops.OP_Idx (op));
      }
    }
    
    _adjust.push_back (succ_idx);
    Eject_Successors_Of_OP (_ops(succ_idx));
  }

  return;
}


BOOL 
SWP_HEURISTIC::Update_Dependencies (INT candidate) {
  _slack.Update_Slack (_adjust);
  _slack.Update_Slack (candidate);

  INT start = _ops.Get_Start_Idx ();
  INT stop  = _ops.Get_Stop_Idx ();

  /* Check if START or STOP have been unscheduled */
  if (!_ops(start).Is_Scheduled ()) return FALSE;

  if (!_ops(stop).Is_Scheduled ()) {
    
    if (_trace) {
      fprintf (TFile, "SWP: STOP has been unscheduled!\n");
      _slack.Print (TFile);
    }

    INT sched_len   = _ops(stop).Get_Cycle () + 1;
    INT lstart_stop = (sched_len / _ii + 1) * _ii - 1;

    _slack.Init (lstart_stop);
    _ops(stop).Set_Cycle (_slack.Lstart(stop), _ii);
    _mrt.Reserve_Resources (_ops(stop));
  }
  
  return TRUE;
}


void 
SWP_HEURISTIC::Print (FILE *f) const {
  fprintf (f, "%sHeuristic:\n%s", DBar, DBar);
  fprintf (f, " OP | Scale |\n");
  fprintf (f, "%s", SBar);
  for (INT i = 1; i < _ops.size () - 1; ++i) {
    fprintf (f, "%3d | %1.3f | ", i, Scale(i));
    Print_OP_No_SrcLine (_ops(i).Get_OP (), f);
  }
  fprintf (f, "%s", DBar);

  return;
}


void 
SWP_HEURISTIC::Verify (void) const {

  return;
}


SWP_SEARCH_DIRECTION 
SWP_HEURISTIC::Search_Direction (INT u) const {
  INT inputs  = 0;
  INT outputs = 0;
 
  if (SWP_Options.Sched_Direction == 1) {
    if (_trace) {
      fprintf (TFile, "Search direction is fixed to 'forward'.\n");
    }

    return SWP_Forward; 
  }
  else if (SWP_Options.Sched_Direction == 2) {
    if (_trace) {
      fprintf (TFile, "Search direction is fixed to 'backward'.\n");
    }

    return SWP_Backward;
  }

  if (_slack(u) == 0) {
    if (_trace) {
      fprintf (TFile, "Search direction does not matter. Slack is 0.\n");
    }

    return SWP_Forward;
  }

  OP *op = _ops(u).Get_OP ();

  /* Check inputs/operants */
  for (INT i = 0; i < OP_opnds (op); ++i) {
    /* ignore duplicate inputs */
    if (   ((i + 1) < OP_opnds (op))
        && (OP_opnd (op, i) == OP_opnd (op, i + 1)))
    {
      continue;
    }
    
    ARC *arc = ARC_LIST_Find_First (OP_preds (op), CG_DEP_REGIN, i);
    if (arc == 0) {
      continue;
    }
      
    /* ignore selfe recurrence */
    if (ARC_pred (arc) == op) {
      continue;
    }

    /* only consider scheduled OPs */
    INT d = _ops.OP_Idx (ARC_pred (arc));
    if (!_ops(d).Is_Scheduled ()) {
      continue;
    }


    if (  (_slack.Estart(d) + _minlt(d)) 
        < (ARC_omega(arc) * _ii + _slack.Lstart(u))) 
    {
      inputs += 1;
    }
  } /* for (INT i = 0; i < OP_opnds (op); ++i) */


  /* Check outputs/results */
  ARC_LIST *arc_list;
  for (arc_list = OP_succs (op);
       arc_list != 0;
       arc_list = ARC_LIST_rest (arc_list))
  {
    ARC *arc = ARC_LIST_first (arc_list);

    if (ARC_kind (arc) != CG_DEP_REGOUT) {
      continue;
    }

    BOOL found_duplicate = FALSE;
    ARC_LIST *arc_list2;
    for (arc_list2 = ARC_LIST_rest (arc_list);
         arc_list2 != 0;
         arc_list2 = ARC_LIST_rest (arc_list2))
    {
      ARC *arc2 = ARC_LIST_first (arc_list2);
      if (ARC_kind (arc2) != CG_DEP_REGOUT) {
        continue;
      }

      /* ignore duplicates */
      if (ARC_succ (arc) == ARC_succ (arc2)) {
        found_duplicate = TRUE;
        break;
      }
    }

    if (found_duplicate) continue;

    /* ignore selfe recurrence */
    if (ARC_succ (arc) == op) {
      continue;
    }

    /* only consider scheduled OPs */
    INT d = _ops.OP_Idx (ARC_succ (arc));
    if (!_ops(d).Is_Scheduled ()) {
      continue;
    }

    if (  (_slack.Estart(u) + _minlt(u))
        < (ARC_omega (arc) * _ii + _slack.Lstart(d))) 
    {
      outputs += 1;
    }
  }

  if (inputs > outputs) {
    if (_trace) {
      fprintf (TFile, 
               "Search direction is 'forward', because we " \
               "have more stretchable inputs then outputs.\n");
    }

    return SWP_Forward;
  }
  else if (outputs > inputs) {
    if (_trace) {
      fprintf (TFile, 
               "Search direction is 'backward', because we " \
               "have more stretchable outputs then inputs.\n");
    }

    return SWP_Backward;
  }

  INT preds = 0;
  INT succs = 0;
  for (ARC_LIST *arc_list = OP_preds (op);
       arc_list != 0;
       arc_list = ARC_LIST_rest (arc_list)) 
  {
    ARC *arc     = ARC_LIST_first (arc_list);
    OP *pred     = ARC_pred (arc);
    INT pred_idx = _ops.OP_Idx (pred);
    if (_ops(pred_idx).Is_Scheduled ()) {
      preds += 1;
    }
  }
  
  for (ARC_LIST *arc_list = OP_succs (op);
       arc_list != 0;
       arc_list = ARC_LIST_rest (arc_list)) 
  {
    ARC *arc     = ARC_LIST_first (arc_list);
    OP *succ     = ARC_succ (arc);
    INT succ_idx = _ops.OP_Idx (succ);
    if (_ops(succ_idx).Is_Scheduled ()) {
      succs += 1;
    }
  }

  if (preds > succs) {
    if (_trace) {
      fprintf (TFile, 
               "Search direction is 'forward', because we have " \
               "more scheduled predecessors then successors.\n");
    }

    /* schedule early */
    return SWP_Forward;
  }
  else if (succs > preds) {
    if (_trace) {
      fprintf (TFile, 
               "Search direction is 'backward', because we have " \
               "more scheduled successors then predecessors.\n");
    }

    /* schedule late */
    return SWP_Backward;
  }
  else if (succs == 0) {
    if (_trace) {
      fprintf (TFile, 
               "Search direction is 'forward', because we have " \
               "no scheduled predecessors and successors.\n");
    }

    /* if no succs and no preds have been place, we place early */
    return SWP_Forward;
  }
  else {
    if (_trace) {
      fprintf (TFile, 
               "Search direction is 'backward', because we " \
               "have the same amount of scheduled predecessors " \
               "and successors.\n");
    }

    return SWP_Backward;
  }
} /* SWP_HEURISTIC::Search_Direction */

