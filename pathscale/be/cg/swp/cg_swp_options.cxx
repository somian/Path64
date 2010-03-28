/*
 * Copyright 2009 SiCortex, Inc.
 */

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

#define __STDC_LIMIT_MACROS
#include <stdint.h>

#include "cgtarget.h"

#include "cg_swp_options.h"

#include <algorithm>



SWP_OPTIONS::SWP_OPTIONS (void) {
  Enable_Verbose_Mode = FALSE;

  /* Scheduling Direction
   * 0 - heuristic
   * 1 - forward
   * 2 - backward
   */
  Sched_Direction = 0; 

  /* Budget */
  Budget = 0;

  // default max unrolling == 8
  Min_Unroll_Times = 1;
  Max_Unroll_Times = 1;  
  Min_Unroll_Times_Set = FALSE;
  Max_Unroll_Times_Set = FALSE;
  Pow2_Unroll_Times = FALSE;

  // For debugging - use Starting_II as the first II for scheduling
  Starting_II = 0;

  // if a resource is 90% utilized, it is 
  // considered critical
  Critical_Threshold = 90;

  Implicit_Prefetch        = FALSE;
  Enable_While_Loop        = FALSE;
  Enable_Jump_Block        = FALSE;
  Enable_Offset_Adjustment = FALSE;
  Predicate_Promotion      = FALSE;

  Max_II              = 500;
  Min_Trip_Count      = 0;
  MaxII_Factor        = 150;
  Linear_Search_Limit = 0;

  Offset_Skip_Before  = 0;
  Offset_Skip_Equal   = -1;
  Offset_Skip_After   = INT32_MAX;

  return;
}


void 
SWP_OPTIONS::PU_Configure (void) {
  if (!Max_Unroll_Times_Set) {
    Max_Unroll_Times = OPT_unroll_times;
  }

  Min_Unroll_Times = std::max(1, Min_Unroll_Times);
  Max_Unroll_Times = std::max(1, Max_Unroll_Times);

  if (Min_Unroll_Times_Set) {
    Max_Unroll_Times = Max(Max_Unroll_Times, Min_Unroll_Times);
  }

  if (Max_Unroll_Times_Set) {
    Min_Unroll_Times = Min(Min_Unroll_Times, Max_Unroll_Times);
  }

  if (!Budget_Set) {
    Budget = CG_opt_level * 10;
  }

  if (!Min_Trip_Count_Set) {
    Min_Trip_Count = 0;
  }
}


SWP_OPTIONS SWP_Options;

