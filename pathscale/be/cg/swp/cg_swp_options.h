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

#ifndef cg_swp_options_INCLUDED
#define cg_swp_options_INCLUDED "cg_swp_options.h"

#include "defs.h"
#include "cg_flags.h"



class SWP_OPTIONS {
  public:
  BOOL  Enable_Verbose_Mode;
  INT32 Sched_Direction;
  //INT32 Heuristics;
  //INT32 Opt_Level;
  BOOL  Enable_While_Loop;
  BOOL  Enable_Jump_Block;
  BOOL  Enable_Offset_Adjustment;
  INT32 Offset_Skip_Before;
  INT32 Offset_Skip_Equal;
  INT32 Offset_Skip_After;
  INT32 Min_Unroll_Times;
  INT32 Max_Unroll_Times;
  INT32 Min_Unroll_Times_Set;
  INT32 Max_Unroll_Times_Set;
  BOOL  Pow2_Unroll_Times;
  INT32 Critical_Threshold;
  BOOL  Implicit_Prefetch;
  BOOL  Implicit_Prefetch_Set;
  BOOL  Predicate_Promotion;
  INT32 Starting_II;
  INT   Max_II;
  INT32 Budget;
  INT32 Budget_Set;
  INT32 Min_Trip_Count;
  INT32 Min_Trip_Count_Set;
  INT32 Linear_Search_Limit;
  INT32 MaxII_Factor;
  

  public:
  SWP_OPTIONS (void);
  ~SWP_OPTIONS (void) { return; }

  void 
  PU_Configure (void);
};


extern SWP_OPTIONS SWP_Options;

#endif /* cg_swp_options_INCLUDED */

