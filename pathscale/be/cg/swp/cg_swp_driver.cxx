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
* @file cg_swp_driver.cxx
* @brief Implementation of the Software Pipelining (SWP) Driver.
***********************************************************************/

#define __STDC_LIMIT_MACROS
#include <stdint.h>

/* Basic compiler include files */
#include "defs.h"
#include "glob.h"
#include "mempool.h"
#include "cxx_memory.h"
#include "tracing.h"
#include "timing.h"

/* Code Generator (CG) related include files */
#include "cg.h"
#include "cg_loop.h"
#include "cg_loop_mii.h"
#include "cgexp.h"
#include "hb_sched.h"

/* Software Pipelining (SWP) related include files */
#include "cg_swp.h"
#include "cg_swp_driver.h"
#include "cg_swp_loop.h"
#include "cg_swp_op_vector.h"
#include "cg_swp_lifeness.h"
#include "cg_swp_lifetime.h"
#include "cg_swp_options.h"
#include "cg_swp_tracing.h"
#include "cg_swp_scheduler.h"
#include "cg_swp_mve.h"
#include "cg_swp_allocator.h"
#include "cg_swp_code_gen.h"
#include "cg_swp_note.h"
#include "cg_swp_target.h"

/* Standard Template Library (STL) related include files */
#include <algorithm>
#include <cmath>

using namespace std;



/* static variables */
INT SWP_DRIVER::_loop_index = 0;


/*******************************************************************//**
* @brief SWP_DRIVER default constructor
*
* The default constructor creates a "not zeroed" memory pool.
***********************************************************************/
SWP_DRIVER::SWP_DRIVER (void) :
  CXX_MEM_POOL        ("Software Pipelining", SWP_Not_Zeroed),
  _m                  ((*this)()),
  _prologue_bp_backup (SWP_BP_ALLOC ((*this)())),
  _epilogue_bp_backup (SWP_BP_ALLOC ((*this)()))
{
  _loop_index += 1;
  
  return;
}


/*******************************************************************//**
* @brief SWP_DRIVER default destructor
*
* Nothing to do here. The default destructor of CXX_MEM_POOL will
* destroy the memory pool for us.
***********************************************************************/
SWP_DRIVER::~SWP_DRIVER (void) {
    
  return;
}


/*******************************************************************//**
* @brief Performs the actual Software Pipelining (SWP)
*
* These is the control logic for the Software Pipeliner:
* -# Create cyclic Data Dependence Graph (DDG)
* -# Calculate resource- and recurence-MII
* -# Schedule until a valid schedule was found in certain boundaries
* -# Modulo Variable Expansion (MVE)
* -# Perform Register Allocation
* -# Code Generation
*
* @param[in] cg_loop   The loop we should Software Pipeline
* @param[in] loop_type Defines the loop type (WHILE- or DO-LOOP)\n
*                      Right now only DO-LOOPS are supported.
* @return    TRUE if succesful\n
*            FALSE otherwise
***********************************************************************/
BOOL
SWP_DRIVER::Perform_SWP (const CG_LOOP& cg_loop,
                         SWP_LOOP_TYPE loop_type)
{
  /* Save the current error phase, so we can restore it at the end of 
   * this method.
   */
  const char *error_phase = Get_Error_Phase ();
  Set_Error_Phase ("Software Pipelining");
  
  /* Check if we should skip the loop. This is normally used only for
   * debugging.
   */
  if (CG_skip_local_swp && Is_Loop_Skipped ()) {
    /* Restore old error phase */
    Set_Error_Phase (error_phase);

    return FALSE;
  }
  
  /* Start SWP phase timer */
  Start_Timer (T_SWpipe_CU);

  /* Obtain basic loop information */
  LOOP_DESCR *loop_descr = cg_loop.Loop ();
  _body                  = LOOP_DESCR_loophead (loop_descr);
  ANNOTATION *annot      = ANNOT_Get (BB_annotations (_body), 
                                      ANNOT_LOOPINFO);
  LOOPINFO   *info       = ANNOT_loopinfo (annot);
  INT         mii        = 0;

  TN         *trip_count = LOOPINFO_trip_count_tn (info);

  Is_True (BB_SET_Size (LOOP_DESCR_bbset (loop_descr)) == 1, 
           ("SWP: Can't Software Pipeline multi-BB loops"));

  /* Tracing */
  BOOL trace_ir        = FALSE;
  BOOL trace_detailed  = FALSE;
  BOOL trace_summary   = FALSE;
  BOOL trace_ddg       = FALSE;
  BOOL trace_mii       = FALSE;
  BOOL trace_scheduler = FALSE;
  BOOL trace_lifetime  = FALSE;
  BOOL trace_lifeness  = FALSE;
  BOOL trace_mve       = FALSE;
  BOOL trace_register  = FALSE;
  BOOL trace_code_gen  = FALSE;
  
  trace_ir = Get_Trace (TKIND_IR,  TP_SWPIPE);
  if (Get_Trace (TP_SWPIPE, 0xffffffff)) {
    trace_detailed  = Get_Trace (TP_SWPIPE, DETAILED_FLAG);
    trace_summary   =    trace_detailed
                      || Get_Trace (TP_SWPIPE, SUMMARY_FLAG);
    trace_ddg       =    trace_detailed
                      || Get_Trace (TP_SWPIPE, DDG_FLAG);
    trace_mii       =    trace_detailed
                      || Get_Trace (TP_SWPIPE, MII_FLAG);
    trace_scheduler =    trace_detailed
                      || Get_Trace (TP_SWPIPE, SCHEDULER_FLAG);
    trace_lifetime  =    trace_detailed
                      || Get_Trace (TP_SWPIPE, LIFETIME_FLAG);
    trace_lifeness  =    trace_detailed
                      || Get_Trace (TP_SWPIPE, LIFENESS_FLAG);
    trace_mve       =    trace_detailed
                      || Get_Trace (TP_SWPIPE, MVE_FLAG);
    trace_register  =    trace_detailed
                      || Get_Trace (TP_SWPIPE, REGISTER_FLAG);
    trace_code_gen  =    trace_detailed
                      || Get_Trace (TP_SWPIPE, CODE_GEN_FLAG);
  }

  /* Timing */
  double time_ddg       = 0.0;
  double time_mii       = 0.0;
  double time_scheduler = 0.0;
  double time_mve       = 0.0;
  double time_register  = 0.0;
  double time_code_gen  = 0.0;
  double time_total     = 0.0;

  time_total -= Get_User_Time (T_SWpipe_CU);
  
  SWP_LOOP swp_loop;
  swp_loop.loop_descr      = loop_descr;
  swp_loop.loop_unrollings = BB_unrollings (_body);
  swp_loop.srcpos          = LOOPINFO_srcpos (info);

  
  /* Check for loops with small trip counts. If the trip count is too
   * small, we skip the loop.
   */
  if (TN_is_constant (trip_count)) {
    INT trip_count_val = TN_value (trip_count);

    if (trip_count_val < SWP_Options.Min_Trip_Count) {
      if (trace_detailed) {
        fprintf (TFile, 
                 "Skip loop: Trip count (%d) too small!\n",
                 trip_count_val);
      }

      swp_loop.status_code = SWP_Small_Trip_Count;
      SWP_Add_Loop_Note (_body, swp_loop);
      Print_Status_Message (loop_descr, SWP_Small_Trip_Count);
  
      /* Restore old error phase */
      Set_Error_Phase (error_phase);

      return FALSE;
    }
  }

  /* Trace loop before SWP */
  if (trace_detailed) {
    CG_LOOP_Trace_Loop (loop_descr, "before Software Pipelining");
  }

  Backup_Loop_Body_BB ();
  Backup_Backpatches ();
  
  if (trace_detailed) {
   Trace_BB (_body, "before copy insertion"); 
   CG_LOOP_Backpatch_Trace (0, 0);
  }

  /* Preprocess all same_res (cond_def) OPs */
  Insert_Copy_For_Every_Same_Res_OP ();
  CG_LOOP_Recompute_Liveness (loop_descr);

  if (trace_detailed) {
   Trace_BB (_body, "after copy insertion"); 
   CG_LOOP_Backpatch_Trace (0, 0);
  }
    
  double time0 = Get_User_Time (T_SWpipe_CU);
  
  /*********************************************************************
  * 1. Create cyclic Data Dependence Graph (DDG)
  *********************************************************************/
  CYCLIC_DEP_GRAPH cyclic_graph (_body);
  
  if (trace_ddg) {
    CG_DEP_Trace_Graph (_body);
  } 
  /********************************************************************/

  double time1 = Get_User_Time (T_SWpipe_CU);
  time_ddg = time1 - time0;

  /*********************************************************************
  * 2. Compute resource- and recurence-MII (Minimum Initiation
  * Interval) to get MII
  *********************************************************************/
  CG_LOOP_rec_min_ii = CG_LOOP_res_min_ii = CG_LOOP_min_ii = 0;

  MEM_POOL_Push (_m);
  {
    CG_LOOP_Make_Strongly_Connected_Components (_body, _m, FALSE);

    /* Info: CG_LOOP_Calculate_Min_Recurrence_II doesn't save the 
     * correct recurrence MII information in CG_LOOP_rec_min_ii. The 
     * correct value is in CG_LOOP_min_ii. We need to save the result
     * right away, because CG_LOOP_Calculate_Min_Resources_II will
     * overwrite CG_LOPP_min_ii. 
     * Furthermore is the order of CG_LOOP_Calculate_Min_Recurrence_II
     * and CG_LOOP_Calculate_Min_Resource_II important too. Don't change
     * it.
     * TODO: Why is not the correct value saved in CG_LOOP_rec_min_ii?
     */
    CG_LOOP_Calculate_Min_Recurrence_II (_body, FALSE);
    swp_loop.rec_mii = CG_LOOP_min_ii;
  
    /* TODO : Handle loop overhead instructions */
    CG_LOOP_Calculate_Min_Resource_II (_body, 0, FALSE, TRUE);
    swp_loop.res_mii = CG_LOOP_min_ii;
  
    mii = max (swp_loop.res_mii, swp_loop.rec_mii);
    swp_loop.mii = mii;

    if (trace_mii) {
      swp_loop.Print_MII (TFile);
    }
  
    CG_LOOP_Clear_SCCs (loop_descr);
  }
  MEM_POOL_Pop (_m);
  /********************************************************************/
  
  double time2 = Get_User_Time (T_SWpipe_CU);
  time_mii = time2 - time1;

  /* Skip loops with too large resource MII. SWP would take too long. */
  if (swp_loop.res_mii > SWP_Options.Max_II) {
    if (trace_detailed) {
      fprintf (TFile, 
               "Skip loop: Resource MII (%d) exceeds Max_II (%d)!\n",
               swp_loop.res_mii,
               SWP_Options.Max_II);
    }
    swp_loop.status_code = SWP_Too_Large_MII;
    SWP_Add_Loop_Note (_body, swp_loop);
    Print_Status_Message (loop_descr, SWP_Too_Large_MII);
  
    /* Restore old error phase */
    Set_Error_Phase (error_phase);

    return FALSE;
  }

  /* Start from mii or SWP_Options.Starting_MII. Starting_MII is 
   * normally used for debugging purposes only.
   */
  INT min_ii = max (mii, SWP_Options.Starting_II);
  
  /* define upper bound for II search space */
  INT max_ii;
  max_ii = static_cast<INT> (ceil ((float)(mii * SWP_Options.MaxII_Factor) / 100));
  max_ii = min (max_ii, SWP_Options.Max_II);
 

  INT ii_incr = 0;
  INT cur_ii  = min_ii;
  INT lower_bound;
  INT upper_bound;
  SWP_SEARCH_PHASE search_phase = SWP_Linear_Search;
  
  while (cur_ii <= max_ii) {
    MEM_POOL_Push (_m);
    {
      /* Create a list of all OPs and annoate additonal information to
       * each OP
       */
      SWP_OP_VECTOR ops (_body, _m);
      if (trace_detailed) {
        ops.Print ("after initialization with loop body OPs", TFile);
      }
      ops.Verify ();
  
      double time3 = Get_User_Time (T_SWpipe_CU);
    
      /*****************************************************************
      * 3. Modulo Scheduling
      *****************************************************************/
      SWP_SCHEDULER scheduler (ops, 
                               cur_ii,
                               SWP_Options.Budget, 
                               _m);

      if (trace_summary || trace_scheduler) {
        if (scheduler.Get_Status ()) {
          fprintf (TFile,
                   "SWP: Scheduling with II=%d succeeded.\n",
                   cur_ii);
        }
        else {
          fprintf (TFile,
                   "SWP: Scheduling with II=%d failed.\n",
                   cur_ii);
        }
      }
      
      /* Obtain II of the current schedule */
      swp_loop.ii = cur_ii;

      switch (search_phase) {
        case SWP_Linear_Search:
          if (!scheduler.Get_Status ()) {
            swp_loop.status_code = SWP_Scheduling_Failed;
      
            ii_incr += 1;

            if (ii_incr >= SWP_Options.Linear_Search_Limit) {
              search_phase = SWP_Exponential_Search;
            }
       
            cur_ii = min_ii + ii_incr;

            goto CLEANUP;
          }
          else {
            // we found a schedule
          }
          break;

        case SWP_Exponential_Search:
          if (!scheduler.Get_Status ()) {
            lower_bound = cur_ii;
          
            if (ii_incr == 0) {
              ii_incr = 1;
            }
            else {
              ii_incr = ii_incr * 2;
            }

            if (   cur_ii != max_ii
                && (min_ii + ii_incr) > max_ii)
            {
              ii_incr = max_ii - min_ii;
            }

            cur_ii = min_ii + ii_incr;
            
            goto CLEANUP;
          }
          else {
            if (ii_incr <= 2) {
              // we found a schedule
            }
            else {
              upper_bound  = cur_ii + 1;
              ii_incr      = (upper_bound - lower_bound) / 2 + 
                             lower_bound - min_ii;
              search_phase = SWP_Binary_Search;
      
              cur_ii = min_ii + ii_incr;
            
              goto CLEANUP;
            }
          }
          break;

        case SWP_Binary_Search:
          if (!scheduler.Get_Status ()) {
            lower_bound = cur_ii;
            ii_incr     = (upper_bound - lower_bound) / 2 + 
                          lower_bound - min_ii;
            
            cur_ii = min_ii + ii_incr;
            
            goto CLEANUP;
          }
          else {
            upper_bound = cur_ii + 1;
            ii_incr     = (upper_bound - lower_bound) / 2 + 
                          lower_bound - min_ii;
            
            if ((upper_bound - lower_bound) == 2) {
              // we found a schedule
            }
            else {
              cur_ii = min_ii + ii_incr;
              
              goto CLEANUP;
            }
          }
          break;

        default:
          Fail_FmtAssertion ("SWP: Unexpected search phase.");
          break;
      };


      if (trace_detailed) {
        ops.Print ("after scheduling", TFile);
      }
      ops.Verify ();
    
      /* Sort OPs */
      ops.Sort_OPs_By_Cycles ();
      if (trace_detailed) {
        ops.Print ("after 1st sorting", TFile);
      }
    
      /* We need to update the omega value of every TN, because after 
       * scheduling we might have moved some OPs around the backedge
       */
      ops.Update_Omega ();
      if (trace_detailed) {
        ops.Print ("after omega update", TFile);
      }
      
      ops.Sort_OPs_Within_Cycles ();
      if (trace_detailed) {
        ops.Print ("after 2nd sorting", TFile);
      }
      
      if (SWP_Options.Enable_Offset_Adjustment) {
        SWP_TARGET_Adjust_LoadStore_Offset (ops, trace_detailed);
        if (trace_detailed) {
          ops.Print ("after Load/Store offset update", TFile);
        }
      }

      SWP_LIFENESS lifeness1 (ops, _m);
      if (trace_lifeness) {
        lifeness1.Print (TFile);
      }
      lifeness1.Verify ();

      SWP_LIFETIME lifetime1 (ops, NULL, lifeness1, cur_ii, _m);
      if (trace_lifetime) {
        lifetime1.Print (TFile);
      }
      lifetime1.Verify ();

      Calculate_Prolog_Epilog_Stages (swp_loop, ops, lifetime1);
      /****************************************************************/
    
      double time4 = Get_User_Time (T_SWpipe_CU);
      time_scheduler += time4 - time3;

      /*****************************************************************
      * 4. Modulo Variable Expansion
      *****************************************************************/
      SWP_OP_VECTOR mve_ops (_m);

      SWP_MVE mve (ops, 
                   mve_ops, 
                   lifeness1, 
                   lifetime1, 
                   cur_ii, 
                   _m);
      if (trace_mve) {
        mve.Print (TFile);
      }
      mve.Verify ();
      swp_loop.kernel_unrollings = mve.Get_Kernel_Unrollings ();
      /****************************************************************/
    
      double time5 = Get_User_Time (T_SWpipe_CU);
      time_mve += time5 - time4;

      /*****************************************************************
      * 5. Register Allocation
      *****************************************************************/
      SWP_LIFENESS lifeness2 (mve_ops, _m);
      if (trace_lifeness) {
        lifeness2.Print (TFile);
      }
      lifeness2.Verify ();

      SWP_ALLOCATOR reg_alloc (mve_ops,
			       mve,
                               lifeness2,
                               swp_loop,
                               Order_By_Start_Time, 
                               First_Fit_Algo,
                               _m);
      if (trace_register) {
        reg_alloc.Print (TFile);
      }
      reg_alloc.Verify ();
    
      if (!reg_alloc.Get_Status ()) {
        swp_loop.status_code = SWP_Register_Failed;
      
        if (trace_detailed) {
          fprintf (TFile, "SWP: Register allocation failed!\n");
        }
      
        goto CLEANUP;
      }
      else {
        swp_loop.status_code = SWP_Successful;
      }

      ISA_REGISTER_CLASS reg_class;
      FOR_ALL_ISA_REGISTER_CLASS (reg_class) { 
        swp_loop.register_count[reg_class] = 
          reg_alloc.Get_Register_Count (reg_class);
      }
      
      if (trace_register) {
        mve_ops.Print ("after register allocation", TFile);
      }
      /****************************************************************/
    
      double time6 = Get_User_Time (T_SWpipe_CU);
      time_register += time6 - time5;

      /*****************************************************************
      * 6. Code Emission
      *****************************************************************/
      SWP_CODE_GEN code_gen (swp_loop, 
                             mve_ops,
                             mve,
                             lifeness2,
                             _m);
      if (trace_code_gen) {
        code_gen.Print (TFile);
      }
      code_gen.Verify ();
      /****************************************************************/
    
      double time7 = Get_User_Time (T_SWpipe_CU);
      time_code_gen += time7 - time6;
    
    }
    
    CLEANUP:
    MEM_POOL_Pop (_m);
    
    if (swp_loop.status_code == SWP_Scheduling_Failed) {
      continue;
    }
    else {
      break;
    }
  } /* while (cur_ii <= max_ii) */

  /* If we were not successful, add an annotation to the loop with the
   * error code.
   */
  if (swp_loop.status_code != SWP_Successful) {
    Restore_Loop_Body_BB ();
    Restore_Backpatches ();
    CG_LOOP_Recompute_Liveness (loop_descr);
    SWP_Add_Loop_Note (_body, swp_loop);
  }

  /* Run BB scheduler in the forward direction to synchronize the scycle
   * numbers with the way the processor sees the instructions.
   */
  if (swp_loop.status_code == SWP_Successful &&
      LOCS_Renumber_Cycles) {
    if (CG_DEP_Has_Graph(_body))
      CG_DEP_Delete_Graph(_body);
    HB_Schedule *Sched = CXX_NEW(HB_Schedule(), &MEM_local_pool);
    Sched->Init(_body, HBS_NO_REORDER, INT32_MAX, NULL, NULL);
    Sched->Schedule_BB(_body, NULL, 1);
  }

  /* Trace IR after SWP */
  if (trace_ir) {
    Trace_IR (TP_SWPIPE, "Software Pipelining", 0);
  }    

  /* Trace loop after SWP */
  if (trace_detailed) {
    CG_LOOP_Trace_Loop (loop_descr, "after Software Pipelining");
  }

  /* Trace memory allocation */
  Trace_Memory_Allocation (TP_SWPIPE, Get_Error_Phase ());

  /* Stop SWP phase timer */
  time_total += Get_User_Time (T_SWpipe_CU);
  Stop_Timer (T_SWpipe_CU);

  swp_loop.time_total     = time_total;
  swp_loop.time_ddg       = time_ddg;
  swp_loop.time_mii       = time_mii;
  swp_loop.time_scheduler = time_scheduler;
  swp_loop.time_mve       = time_mve;
  swp_loop.time_register  = time_register;
  swp_loop.time_code_gen  = time_code_gen;

  /* Show summary */
  if (trace_detailed || trace_summary) {
    swp_loop.Print (TFile);
  }

  /* Restore old error phase */
  Set_Error_Phase (error_phase);

  
  Print_Status_Message (loop_descr, swp_loop.status_code);
  
  return (swp_loop.status_code == SWP_Successful);
} /* SWP_DRIVER::Perform_SWP */


void
SWP_DRIVER::Rename_Dedicated_Registers (SWP_OP_VECTOR &ops) {
  for (INT i = 0; i < ops.size (); ++i) {
    if (i == ops.Get_Start_Idx ()) continue;
    if (i == ops.Get_Stop_Idx ()) continue;

    OP *op = ops[i].Get_OP ();
    for (INT res = 0; res < OP_results (op); ++res) {
      TN *dedicated_tn = OP_result (op, res);
      if (!TN_is_dedicated (dedicated_tn)) continue;

      TN *new_tn = Dup_TN_Even_If_Dedicated (dedicated_tn);
      TN_Allocate_Register (new_tn, TN_register (dedicated_tn));
      Set_TN_is_dedicated (new_tn);

      Set_OP_result (op, res, new_tn);
      BOOL alive = TRUE;

      for (INT j = i+1; j < ops.size () + i; ++j) {
        OP *op2 = ops[j % ops.size ()].Get_OP ();

        for (INT opr = 0; opr < OP_opnds (op2); ++opr) {
          TN *opr_tn = OP_opnd (op2, opr);
          if (opr_tn == dedicated_tn) {
            Set_OP_opnd (op2, opr, new_tn);
          }
        }

        for (INT res = 0; res < OP_results (op2); ++res) {
          TN *res_tn = OP_result (op2, res);
          if (res_tn == dedicated_tn) {
            alive = FALSE;
            break;
          }
        }

        if (!alive) break;
      }

      if (!alive) break;
    } /* for (INT res = 0; res < OP_results (op); ++res) */
  } /* for (INT i = 0; i < ops.size (); ++i) */

  return;
}


/*******************************************************************//**
* @brief Calculate the number of neccessary prolog and epilog stages.
*
* This function caluclates LPS, LES, PS, ES and SC according to
* B. Ra, M. Lee, P. Tirumalai, M. Schlansker, "Register Allocation for
* Software Pipelined Loops", PLDI '92
*
* @param[inout] swp_loop   General data structure to store loop related
*                          information
* @param[in]    ops        A list of all ops in the loop (+ fake START
*                          and STOP)
* @param[in]    lt         Lifetime information about the loop
* @return       void
***********************************************************************/
void
SWP_DRIVER::Calculate_Prolog_Epilog_Stages (SWP_LOOP& swp_loop, 
                                            const SWP_OP_VECTOR& ops,
                                            const SWP_LIFETIME& lt)
{
  INT ii      = swp_loop.ii;
  
  /* Get the maximum stage number. START/STOP will be ignored. */
  INT max_stage = 0;
  for (SWP_OP_VECTOR::size_type i = 0; i < ops.size (); ++i) {
    if (i == ops.Get_Start_Idx ()) continue;
    if (i == ops.Get_Stop_Idx ()) continue;
    
    if (ops(i).Get_Stage () > max_stage) {
      max_stage = ops(i).Get_Stage ();
    }
  }
  swp_loop.sc = max_stage + 1;
  

  swp_loop.lps = 0;
  for (SWP_TN_LIFETIME_VECTOR::size_type i = 0; i < lt.size (); ++i) {
    if (lt(i).Get_Omega () > 0) {
      float start = lt(i).Get_Start_Cycle ();
      swp_loop.lps = max (swp_loop.lps,
                          static_cast<INT> (ceil (start / ii)) - 1);
    }
  }
  
  swp_loop.les = 0;
  for (SWP_TN_LIFETIME_VECTOR::size_type i = 0; i < lt.size (); ++i) {
    if (lt(i).Get_Alpha () > 0) {
      float end = lt(i).Get_End_Cycle ();
      INT les =   swp_loop.sc 
                - static_cast<INT> (floor (end / ii)) 
                + lt(i).Get_Alpha ()
                - 1;
      swp_loop.les = max (swp_loop.les, les);
    }
  }

  swp_loop.ps = max (swp_loop.sc-1, swp_loop.lps);
  swp_loop.es = max (swp_loop.sc-1, swp_loop.les);

  return;
} /* SWP_DRIVER::Calculate_Prolog_Epilog_Stages */


/*******************************************************************//**
* @brief Tells us if this loop should be skipped.
*
* This is used for debugging and tells us if this loop should not be
* Software Pipelined. The following CG parameters must be set:\n
* -CG:skip_local_swp=1:local_skip_[before|after|equal]=0..n
*
* @return TRUE  if loop should be skipped\n
*         FALSE otherwise
***********************************************************************/
BOOL 
SWP_DRIVER::Is_Loop_Skipped (void) const {
  BOOL skip_loop = (   _loop_index < CG_local_skip_before
                    || _loop_index > CG_local_skip_after
                    || _loop_index == CG_local_skip_equal);

  if (CG_skip_local_swp) {
    printf ("%s Software Pipelining for loop: index=%d\n",
            skip_loop ? "Skipping" : "Attempting",
            _loop_index);
  }

  return skip_loop;
} /* SWP_DRIVER::Is_Loop_Skipped */


/*******************************************************************//**
* @brief Print loop related status messages
*
* In verbose mode we print additional status messages about loops to
* give direct feedback to the user.

* @param[in]    loop   The current loop descriptor
* @param[in]    status SWP status code of the current loop
* @return void
***********************************************************************/
void
SWP_DRIVER::Print_Status_Message (LOOP_DESCR *loop, 
                                  SWP_STATUS_CODE status)
{
  if (!SWP_Options.Enable_Verbose_Mode) return;

  LOOPINFO   *loop_info  = LOOP_DESCR_loopinfo (loop); 

  switch (status) {
    case SWP_Small_Trip_Count:
      fprintf (stdout, 
               "(%s:%i) LOOP IS NOT SOFTWARE-PIPELINED, "\
               "BECAUSE TRIP COUNT IS TOO SMALL\n",
               Src_File_Name,
               LOOPINFO_line (loop_info)
              );
      break;

    case SWP_Multi_BB_Loop:
      fprintf (stdout, 
               "(%s:%i) LOOP IS NOT SOFTWARE-PIPELINED, "\
               "BECAUSE IT IS A MULTI-BB LOOP\n",
               Src_File_Name,
               LOOPINFO_line (loop_info)
              );
      break;

    case SWP_Too_Large_MII:
      fprintf (stdout, 
               "(%s:%i) LOOP IS NOT SOFTWARE-PIPELINED, "\
               "BECAUSE THE LOOP IS TOO LARGE\n",
               Src_File_Name,
               LOOPINFO_line (loop_info)
              );
      break;

    case SWP_Scheduling_Failed:
      fprintf (stdout, 
               "(%s:%i) LOOP IS NOT SOFTWARE-PIPELINED, "\
               "BECAUSE SCHEDULING FAILED\n",
               Src_File_Name,
               LOOPINFO_line (loop_info)
              );
      break;

    case SWP_Register_Failed:
      fprintf (stdout, 
               "(%s:%i) LOOP IS NOT SOFTWARE-PIPELINED, "\
               "BECAUSE REGISTER ALLOCATION FAILED\n",
               Src_File_Name,
               LOOPINFO_line (loop_info)
              );
      break;

    case SWP_Successful:
      fprintf (stdout, 
               "(%s:%i) LOOP IS SOFTWARE-PIPELINED\n",
               Src_File_Name,
               LOOPINFO_line (loop_info)
              );
      break;

    default:
      Fail_FmtAssertion ("Unknown SWP status code");
      break;
  }

  return;
}


/*******************************************************************//**
* @brief Insert a copy before every same_res or cond_def OP
*
* If we don't have rotating registers, we have to insert copies before
* all cond_def and same_res OPs to do the register rotation manually.
* Furthermore, the prologue and epilogue backpatches need to be updated
* to reflect the change in the non_body_tn.
*
* i.e.
* TN100 <- ... TN100[2] cond_def
*
* will be converted to
*
* TN200 <- TN100[2]
* TN200 <- ... TN200 cond_def
*
* A later optimization phase has to remove redundant copies.
*
* Note: These copies are only needed when there is a dependence cycle:
*   TN100 <- ... TN100[1] cond_def
*   TN100 <- ... TN100    cond_def
* In particular, the copies are not needed for lwr,lwl / ldr,ldl pairs.
* In MIPS, they are primarily needed for movt / movf / movn / movz.
*
* @return void
***********************************************************************/
void
SWP_DRIVER::Insert_Copy_For_Every_Same_Res_OP (void) const
{
  // 15008, 15030, 15201: Reimplement algorithm to fix various bugs.

  // Iterate through all same_res/cond_def OPs in _body.
  OP *op;
  FOR_ALL_BB_OPs (_body, op) {
    if (! OP_same_res (op) && ! OP_cond_def (op)) continue;
    // 15030: The fix for 15008 is not needed for lwr/lwl/ldr/ldl.
    if (OP_load(op)) continue;

    UINT8 same_res_idx = OP_find_opnd_use (op, OU_same_res);
    TN *res_tn = OP_result (op, 0);
    TN *opr_tn = OP_opnd (op, same_res_idx);

    // Insert a copy before the same res OP.
    OPS ops;
    OPS_Init (&ops);
    Exp_COPY (res_tn, opr_tn, &ops);
    FmtAssert (OPS_length (&ops) == 1,
	       ("Insert_Copy_For_Every_Same_Res_OP: Unexpected OPS length"));
    OP *copy_op = OPS_first (&ops);
    CG_LOOP_Init_Op (copy_op);
    Set_OP_omega (copy_op, 0, OP_omega (op, same_res_idx));
    OP_srcpos (copy_op) = OP_srcpos (op);
    Set_OP_unrolling (copy_op, OP_unrolling (op));
    BB_Insert_Op_Before (_body, op, copy_op);

    // Update the same res OP.
    Set_OP_omega (op, same_res_idx, 0);

    // Find any unconditional definition of opr_tn earlier in this BB.
    OP *op2;
    UINT8 idx2;
    for (op2 = OP_prev (copy_op); op2; op2 = OP_prev (op2)) {
      if (OP_same_res (op2) || OP_cond_def (op2)) continue;
      FOR_ALL_OP_RESULTs (op2, idx2) {
	if (opr_tn == OP_result (op2, idx2)) {
	  goto found;
	}
      }
    }
  found:

    // If opr_tn was defined earlier, replace all instances of opt_tn
    // within that live range with a new TN.
    if (op2) {
      TN *new_tn = Dup_TN (opr_tn);
      do {
	FOR_ALL_OP_RESULTs (op2, idx2) {
	  if (opr_tn == OP_result (op2, idx2)) {
	    Set_OP_result (op2, idx2, new_tn);
	  }
	}
	op2 = OP_next(op2);
	FOR_ALL_OP_OPNDs (op2, idx2) {
	  if (OP_opnd (op2, idx2) == opr_tn &&
	      OP_omega (op2, idx2) == 0) {
	    Set_OP_opnd (op2, idx2, new_tn);
	  }
	}
      } while (op2 != copy_op);
    }
  }
}


void
SWP_DRIVER::Backup_Loop_Body_BB (void) { 
  OPS_Init (&_body_ops_backup);
  OP *op;
  FOR_ALL_BB_OPs (_body, op) {
    OP *new_op = Dup_OP (op);
    CG_LOOP_Copy_Loop_Info (new_op, op);
    Copy_WN_For_Memory_OP (new_op, op);
    OPS_Append_Op (&_body_ops_backup, new_op);
  }

  return;
}


void
SWP_DRIVER::Restore_Loop_Body_BB (void) { 
  BB_Remove_All (_body);
  BB_Append_Ops (_body, &_body_ops_backup);
  OPS_Init (&_body_ops_backup);

  return;
}


void 
SWP_DRIVER::Backup_Backpatches (void) {
  _prologue_bp_backup.clear ();
  _epilogue_bp_backup.clear ();

  CG_LOOP_BACKPATCH *bp;
  for (bp = CG_LOOP_Backpatch_First (CG_LOOP_prolog, 0);
       bp != 0;
       bp = CG_LOOP_Backpatch_Next (bp))
  {
    CG_LOOP_BACKPATCH *actual_bp = _CG_LOOP_BP_actual_ptr (bp);
    _prologue_bp_backup.push_back (actual_bp);
  }
  
  for (bp = CG_LOOP_Backpatch_First (CG_LOOP_epilog, 0);
       bp != 0;
       bp = CG_LOOP_Backpatch_Next (bp))
  {
    CG_LOOP_BACKPATCH *actual_bp = _CG_LOOP_BP_actual_ptr (bp);
    _epilogue_bp_backup.push_back (actual_bp);
  }
  
  return;
}


void
SWP_DRIVER::Restore_Backpatches (void) {
  CG_LOOP_Backpatch_Clear (CG_LOOP_prolog);
  CG_LOOP_Backpatch_Clear (CG_LOOP_epilog);

  SWP_BP_VECTOR::size_type i;
  for (i = 0; i < _prologue_bp_backup.size (); ++i) {
    CG_LOOP_Backpatch_Add (CG_LOOP_prolog, _prologue_bp_backup[i]);
  }

  for (i = 0; i < _epilogue_bp_backup.size (); ++i) {
    CG_LOOP_Backpatch_Add (CG_LOOP_epilog, _epilogue_bp_backup[i]);
  }

  return;
}


