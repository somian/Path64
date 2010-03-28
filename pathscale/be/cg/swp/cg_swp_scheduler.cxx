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
#include "cg_swp_tracing.h"
#include "cg_swp_op_vector.h"
#include "cg_swp_mrt.h"
#include "cg_swp_mindist.h"
#include "cg_swp_minlt.h"
#include "cg_swp_slack.h"
#include "cg_swp_heuristic.h"
#include "cg_swp_scheduler.h"
#include "cg_swp_target.h"



SWP_SCHEDULER::SWP_SCHEDULER (SWP_OP_VECTOR& ops, 
                              INT ii,
                              INT budget,
                              MEM_POOL *m) :
  _m          (m),
  _ii         (ii),
  _ops        (ops),
  _successful (FALSE)
{
  BOOL trace_mrt       = Get_Trace (TP_SWPIPE, MRT_FLAG);
  BOOL trace_mindist   = Get_Trace (TP_SWPIPE, MINDIST_FLAG);
  BOOL trace_minlt     = Get_Trace (TP_SWPIPE, MINLT_FLAG);
  BOOL trace_slack     = Get_Trace (TP_SWPIPE, SLACK_FLAG);
  BOOL trace_heuristic = Get_Trace (TP_SWPIPE, HEURISTIC_FLAG);
  BOOL trace_scheduler = Get_Trace (TP_SWPIPE, SCHEDULER_FLAG);

  MEM_POOL_Push (_m);
  {
    SWP_MRT mrt (ops, _ii, _m);
    if (trace_mrt) {
      fprintf (TFile, "SWP_MRT after initialization:\n");
      mrt.Print (TFile);
    }
    mrt.Verify ();

    SWP_MINDIST mindist (ops, _ii, _m);
    if (trace_mindist) {
      fprintf (TFile, "SWP_MINDIST after initialization:\n");
      mindist.Print (TFile);
    }
    mindist.Verify ();
    /* Important: Remove_Self_Recurrences must be called after Verify
     * otherwise the use of Verify is useless.
     */
    mindist.Remove_Self_Recurrences ();

    SWP_MINLT minlt (ops, mindist, _ii, _m);
    if (trace_minlt) {
      fprintf (TFile, "SWP_MINLT after initialization:\n");
      minlt.Print (TFile);
    }
    minlt.Verify ();

    SWP_SLACK slack (ops, mindist, _ii, _m);
    if (trace_slack) {
      fprintf (TFile, "SWP_SLACK after initialization:\n");
      slack.Print (TFile);
    }
    slack.Verify ();

    SWP_HEURISTIC heur (ops, mrt, mindist, slack, minlt, _ii, _m);
    if (trace_heuristic) {
      fprintf (TFile, "SWP_HEURISTIC after initialization:\n");
      heur.Print (TFile);
    }
    heur.Verify ();

    while (TRUE) {
      /*****************************************************************
      * 1. Choose a good operation to place.                           *
      *****************************************************************/
      INT candidate = heur.Choose_OP ();
      if (candidate == -1) {
	/* No more OPs to place - we are finished */
	_successful = TRUE;
	break;
      }

      if (ops(candidate).Get_Try_Counter () > budget) {
	_successful = FALSE;

	if (trace_scheduler) {
	  fprintf (TFile, "Scheduling failed: Run out of budget.\n");
	}
	break;
      }
      /****************************************************************/

      /*****************************************************************
      * 2. Search for a good issue cycle                               *
      *****************************************************************/
      pair <BOOL, BOOL> conflict = heur.Choose_Cycle (candidate);
      /****************************************************************/

      /*****************************************************************
      * 3. Eject conflicting OPs if neccessary                         *
      *****************************************************************/
      /*****************************************************************
      * 3.1 Eject resource conflicting OPs                             *
      *****************************************************************/
      if (conflict.second) {
        /* Try to free resources once at a time until we can allocate
         * the required resources for the OP we want to schedule.
         */
	for (INT i = 1; i < ops.size () - 1; ++i) {
	  if (!mrt.Resources_Available (ops(candidate),
					ops(candidate).Get_Cycle ())) {
	    heur.Eject_Conflicting_Resources (candidate);
	  }
	  else {
	    break;
	  }
	}

	/* If we were not able to unschedule all our resources by now
	 * we have done something seriously wrong.
	 */
	FmtAssert (mrt.Resources_Available (ops(candidate),
					    ops(candidate).Get_Cycle ()),
		   ("SWP_SCHEDULER: Could not unschedule all "
		    "resources!"));
      } /* if (conflict.second == TRUE) */
      /****************************************************************/

      /*****************************************************************
      * 3.2 Eject dependence conflicting OPs                           *
      *****************************************************************/
      if (conflict.first) {
	heur.Eject_Dependence_Conflict_OPs (candidate);
      }
      /****************************************************************/

      /*****************************************************************
      * 3.3 Update dependencies                                        *
      *****************************************************************/
      if (conflict.first || conflict.second) {
	if (!heur.Update_Dependencies (candidate)) {
	  _successful = FALSE;
	  break;
	}
      }
      /****************************************************************/

      /*****************************************************************
      * 4. Place OP and update MRT                                     *
      *****************************************************************/
      mrt.Reserve_Resources (ops(candidate));

      if (trace_mrt) {
	mrt.Print (TFile);
      }

      if (trace_scheduler) {
	Print (TFile);
      }
      /****************************************************************/

      /*****************************************************************
      * 5. Update Estart and Lstart for all unplaced OPs               *
      *****************************************************************/
      if (conflict.first || conflict.second) {
	slack.Update_Slack (candidate);
	slack.Update_Slack ();
      }
      else {
	slack.Update_Slack (candidate);
      }

      if (trace_slack) {
	slack.Print (TFile);
      }

      slack.Verify ();
      /****************************************************************/
    } // while (TRUE)

  }
  MEM_POOL_Pop (_m);

  return;
} /* SWP_SCHEDULER::SWP_SCHEDULER */


void 
SWP_SCHEDULER::Print (FILE *f) const {
  fprintf (f, "%sCurrent Kernel Schedule:\n%s" , DBar, DBar);
  fprintf (f, "Cycle | Scheduled OP(s)\n");

  for (INT i = 0; i < _ii; ++i) {
    fprintf (f, "%4d  | ", i);

    for (INT op = 1; op < _ops.size () - 1; ++op) {
      if (   _ops(op).Is_Scheduled ()
          && (_ops(op).Get_Cycle () % _ii == i)) 
      {
        fprintf (f, "%4d ", op);
      }
    }

    fprintf (f, "\n");
  }
  
  fprintf (f, "%s", DBar);

  return;
}


void 
SWP_SCHEDULER::Verify (void) const {

  return;
}

