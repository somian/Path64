/*
 *  Copyright (C) 2007 PathScale, LLC.  All Rights Reserved.
 */

/*
 *  Copyright (C) 2007 QLogic Corporation.  All Rights Reserved.
 */

/*
 * Copyright (C) 2008 Advanced Micro Devices, Inc.  All Rights Reserved.
 */

/*

  Copyright (C) 2000, 2001 Silicon Graphics, Inc.  All Rights Reserved.

   Path64 is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   Path64 is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
   or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
   License for more details.

   You should have received a copy of the GNU General Public License
   along with Path64; see the file COPYING.  If not, write to the Free
   Software Foundation, 51 Franklin Street, Fifth Floor, Boston, MA
   02110-1301, USA.

   Special thanks goes to SGI for their continued support to open source

*/

//  Global register allocation driver
/////////////////////////////////////
//
//  Description:
//
//      Top level of global register allocation.  Try to limit to functions
//      exported outside of GRA, per Ken Lesniak.
//
/////////////////////////////////////


//  $Revision$
//  $Date$
//  $Author$
//  $Source$

#ifdef USE_PCH
#include "cg_pch.h"
#endif // USE_PCH
#pragma hdrstop

#ifdef _KEEP_RCS_ID
static char *rcs_id = "$Source$ $Revision$";
#endif

#include "defs.h"
#include "mempool.h"
#include "timing.h"
#include "tracing.h"
#include "erglob.h"
#include "cgir.h"
#include "cg.h"
#include "gtn_universe.h"
#include "dominate.h"
#include "cg_flags.h"
#include "gra_bb.h"
#include "gra.h"
#include "gra_region.h"
#include "gra_create.h"
#include "gra_color.h"
#include "gra_spill.h"
#include "gra_grant.h"
#include "gra_cflow.h"
#include "gra_trace.h"


static MEM_POOL  GRA_pool_struct;
MEM_POOL* const GRA_pool = &GRA_pool_struct;
		 // Pointer to a non-zeroed memory pool for general use in GRA. 
		 // Pushed in GRA_Create and Popped in GRA_Delete.
INT GRA_pu_num = 0;
float GRA_call_split_freq;
float GRA_spill_count_factor;

static BOOL gra_spill_around_save_tn_copies;

#ifdef TARG_MIPS
char *GRA_avoid_reg = NULL;
ISA_REGISTER_CLASS GRA_avoid_register_class = ISA_REGISTER_CLASS_UNDEFINED;
REGISTER GRA_avoid_register;
#endif

/////////////////////////////////////
void 
GRA_Initialize(void)
/////////////////////////////////////
//
//  see interface description
//
/////////////////////////////////////
{
  gra_spill_around_save_tn_copies = TRUE;
}

/////////////////////////////////////
BOOL
GRA_Spill_Around_Save_TN_Copies(void)
/////////////////////////////////////
//
//  see interface description
//
/////////////////////////////////////
{
  return gra_spill_around_save_tn_copies;
}

/////////////////////////////////////
static void
Initialize_Memory(void)
/////////////////////////////////////
//
//  Prepare the GRA_pool for memory allocation.
//
/////////////////////////////////////
{
  static BOOL did_init = FALSE;

  if ( ! did_init ) {
    MEM_POOL_Initialize(GRA_pool,"GRA pool",FALSE);
    did_init = TRUE;
  }
  MEM_POOL_Push(GRA_pool);
}

/////////////////////////////////////
static void
Finalize_Memory(void)
/////////////////////////////////////
//
//  Delete all the memory that we used for strictly private stuff.
//
/////////////////////////////////////
{
  if (GRA_loop_splitting) {
    Free_Dominators_Memory();
  }
  MEM_POOL_Pop(GRA_pool);
}


/////////////////////////////////////
REGISTER_SET
GRA_Local_Register_Grant( BB* bb, ISA_REGISTER_CLASS rc )
/////////////////////////////////////
//  See interface description.
/////////////////////////////////////
{
  return GRA_GRANT_Get_Local_Registers(bb,rc);
}

/////////////////////////////////////
void
GRA_Finalize_Grants(void)
/////////////////////////////////////
//  See interface description.
/////////////////////////////////////
{
  GRA_GRANT_Finalize();
}

/////////////////////////////////////
static void
Clear_Spill_BB_Flags(void)
/////////////////////////////////////
//  The BB_gra_spill flag has served its purpose. Clear it now
//  from any blocks that have it set to allow cflow to remove them
//  if they are still empty.
/////////////////////////////////////
{
  BB *bb;

  for (bb = REGION_First_BB; bb; bb = BB_next(bb)) {
    Reset_BB_gra_spill(bb);
  }
}

/////////////////////////////////////
static void
Initialize_Flags()
/////////////////////////////////////
//
//  Initialize flags used by GRA.
//
/////////////////////////////////////
{
  gra_spill_around_save_tn_copies = FALSE;
  GRA_call_split_freq = atof(GRA_call_split_freq_string);
  GRA_spill_count_factor = atof(GRA_spill_count_factor_string);

#ifdef TARG_MIPS
#ifdef ICE9A_HW_WORKAROUND
  if (CG_hw_round > 0 ||
      CG_hw_stall > 0) {
    // -CG:hw_round and -CG:hw_stall require f23.  Tell GRA to avoid it.
    // Bug 13481.
    if (GRA_avoid_reg != NULL &&
	CG_hw_round > 0)
      ErrMsg(EC_Opt_Conflict, "CG:hw_round", "GRA:avoid_reg", "-CG:hw_round");
    if (GRA_avoid_reg != NULL &&
	CG_hw_stall > 0)
      ErrMsg(EC_Opt_Conflict, "CG:hw_stall", "GRA:avoid_reg", "-CG:hw_stall");
    GRA_avoid_register_class = ISA_REGISTER_CLASS_float;
    GRA_avoid_register = 24;	// 24 is f23
  }
#endif

  if (GRA_avoid_reg != NULL) {
    char *p = GRA_avoid_reg;

    if (*p == 'f') {	// floating point register
      p++;
      GRA_avoid_register_class = ISA_REGISTER_CLASS_float;
    } else {
      GRA_avoid_register_class = ISA_REGISTER_CLASS_integer;
    }
    GRA_avoid_register = atoi(p) + 1;
  }
#endif
}

/////////////////////////////////////
void
GRA_Allocate_Global_Registers( BOOL is_region )
/////////////////////////////////////
//  See interface description.
/////////////////////////////////////
{

  Set_Error_Phase ("Global Register Allocation");
  Start_Timer (T_GRA_CU);

  Initialize_Flags();
  Initialize_Memory();
  lrange_sub_mgr.Initialize();
  lrange_mgr.Initialize();
  gbb_mgr.Initialize();
  gra_region_mgr.Initialize();
  GRA_Spill_Initialize();
  GRA_Trace_Initialize();

  GRA_Split_Entry_And_Exit_BBs(is_region);

  GRA_Create();
  GRA_Color();

  // Dump out OPs after GRA
  if (Get_Trace(TKIND_IR, TP_GRA, REGION_First_BB))
    Trace_IR(TP_GRA, "GRA0", NULL);

  GRA_Spill();      // Actually add the spills.
  GRA_Delete();

  GRA_GRANT_Unused_Caller_Saved();

  lrange_sub_mgr.Finalize();
  lrange_mgr.Finalize();
  gbb_mgr.Finalize();
  gra_region_mgr.Finalize();

  GRA_Join_Entry_And_Exit_BBs();

  Clear_Spill_BB_Flags();

  Finalize_Memory();

  // Dump out OPs after GRA
  if (Get_Trace(TKIND_IR, TP_GRA, REGION_First_BB))
    Trace_IR(TP_GRA, "GRA", NULL);

  GRA_pu_num++;

  Stop_Timer ( T_GRA_CU );
}

/////////////////////////////////////
void
GRU_Fuse_Global_Spills( BOOL is_region )
/////////////////////////////////////
//  See interface description.
/////////////////////////////////////
{
#ifdef TARG_X8664
  Set_Error_Phase ("Fuse Global Spills");
  Start_Timer (T_GRU_CU);

  // GRA will free the dominator memory when loop splitting is on
  if (GRA_loop_splitting) {
    Calculate_Dominators();
  }

  // Dump out OPs after GRA
  if (Get_Trace(TKIND_IR, TP_GRA, REGION_First_BB))
    Trace_IR(TP_GRU, "GRU0", NULL);

  GRU_Fuse();      // Actually try to remove some spills.

  // Dump out OPs after GRA
  if (Get_Trace(TKIND_IR, TP_GRU, REGION_First_BB))
    Trace_IR(TP_GRU, "GRU", NULL);

  if (GRA_loop_splitting) {
    Free_Dominators_Memory();
  }

  Stop_Timer ( T_GRU_CU );
#endif
}
