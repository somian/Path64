/*
 * Copyright (C) 2007. PathScale, LLC. All Rights Reserved.
 */
/*
 *  Copyright (C) 2006. QLogic Corporation. All Rights Reserved.
 */

// ====================================================================
// ====================================================================
//
// Module: wodriver.cxx
// $Revision$
// $Date$
// $Author$
// $Source$
//
// Revision history:
//  08-Sep-94 - Original Version
//
// ====================================================================
//
// Copyright (C) 2000, 2001 Silicon Graphics, Inc.  All Rights Reserved.
//
/*
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
//
// ====================================================================
//
// Description:
//
// Main driver -- command line processing and file name manipulation --
// for the Whirl Optimizer
//
// ====================================================================
// ====================================================================


#ifdef USE_PCH
#include "opt_pch.h"
#endif // USE_PCH
#pragma hdrstop


#include <sys/elf_whirl.h>	    /* for WHIRL_REVISION */

#include "defs.h"
#include "config.h"		/* for Run_wopt, etc.		*/
#include "config_debug.h"	/* for DEBUG_Ir_Version_Check	*/
#include "glob.h"		/* for Irb_File_Name, Ipa_File_Name */
#include "timing.h"		/* for T_Optimize_CU		*/
#include "tracing.h"		/* for Get_Trace, etc.		*/

#include "wn.h"			/* WHIRL Node descriptor	*/
#include "stab.h"		/* WHIRL symtab global variables*/
#include "optimizer.h"		/* External interface for optimizer phase */

#include "opt_defs.h"
#include "opt_config.h"
#include "region_util.h"
#include "region_main.h"	/* for RBI and RAIL prototypes	*/
#include "be_version.h"         /* for Get_BE_Version */

/* The global -WOPT option group flag variables are now defined in: */
#include "config_wopt.h"

#include "config_opt.h"

#include "wodriver.h"		// So we pick up 'extern "C"'
#include "ir_reader.h"		// for fdump_tree()

#ifdef SHARED_BUILD
#if defined(BUILD_OS_DARWIN)
extern char * Ipa_File_Name;
#endif /* defined(BUILD_OS_DARWIN) */
#endif

#ifdef KEY
extern void WN_reassoc(WN *);
extern BOOL WN_reassoc_deepest_blocks(WN *);
#endif

/* ====================================================================
 *
 * main
 *
 * Main entry point and driver for WOPT.  We no longer need to do
 * processing of -WOPT here -- it is handled in config.c.
 *
 * ====================================================================
 */


/*ARGSUSED*/
void
wopt_main (INT wopt_argc, char **wopt_argv, INT be_argc, char **be_argv)
{
  extern char *Whirl_Revision;

  if (strcmp (Whirl_Revision, WHIRL_REVISION) != 0)
    FmtAssert (!DEBUG_Ir_Version_Check,
	       ("WHIRL revision mismatch between be.so (%s) and wopt.so (%s)", 
		Whirl_Revision, WHIRL_REVISION));
  
  if (&Get_BE_Version == NULL ||
      strcmp (Get_BE_Version(), BE_VERSION) != 0)
    FmtAssert (FALSE,
	       ("BE version (in be/com/be_version.h) mismatch between be.so (%s) and wopt.so (%s)", 
		&Get_BE_Version != NULL ? Get_BE_Version() : "undef", BE_VERSION));

  /* Construct a skip list from the -WOPT:skip_* options: */
  WOPT_Skip_List = Build_Skiplist ( WOPT_Skip );

} /* wopt_main */


/* Initialization that needs to be done after the global symtab is read */
void
Wopt_Init (void)
{
  Set_Error_Phase ( "Wopt Initialization" );

  return;
} /* Wopt_Init */



/* Per program unit global optimization entry point	*/
/* if region_wn != pu_wn, optimize the region		*/
WN *
Perform_Global_Optimization (WN *pu_wn, WN *region_wn, 
			     struct ALIAS_MANAGER *alias_mgr)
{
    WN *opt_pu;
    struct DU_MANAGER *du_mgr;

    Is_True(region_wn != NULL,
	    ("Perform_Global_Optimization, NULL region found"));
    OPT_POOL_Push ( &MEM_local_pool, MEM_DUMP_FLAG+17 );

    Start_Timer ( T_Wopt_CU );
    Set_Error_Phase ( "Global Optimizer" );

#ifdef KEY
    if (OPT_Reassoc_For_Cse) {
      du_mgr = Create_Du_Manager(MEM_pu_nz_pool_ptr);

      WOPT_Enable_LNO_Copy_Propagate = FALSE;

      opt_pu = Pre_Optimizer(PREOPT_PHASE, region_wn, du_mgr, alias_mgr);

      Delete_Du_Manager(du_mgr,MEM_pu_nz_pool_ptr);

      WN_reassoc_deepest_blocks(opt_pu);
      region_wn = opt_pu;
      // fdump_tree(TFile, region_wn);
    }
#endif

    du_mgr = Create_Du_Manager(MEM_pu_nz_pool_ptr);

    opt_pu = Pre_Optimizer(MAINOPT_PHASE, region_wn, du_mgr, alias_mgr);

    Delete_Du_Manager(du_mgr,MEM_pu_nz_pool_ptr);

    Stop_Timer ( T_Wopt_CU );

    OPT_POOL_Pop ( &MEM_local_pool, MEM_DUMP_FLAG+17 );

    return opt_pu;
} /* Perform_Global_Optimization */


/* Per program unit preopt optimization entry point */
/* called in place of LNO if -PHASE:p is set */
WN *
Perform_Preopt_Optimization(WN *pu_wn, WN *region_wn)
{
    WN *opt_pu = NULL;
    DU_MANAGER *du_mgr;
    ALIAS_MANAGER *alias_mgr;
    
    OPT_POOL_Push ( &MEM_local_pool, MEM_DUMP_FLAG+19 );

    Start_Timer ( T_Preopt_CU );
    Set_Error_Phase ( "Global Optimizer" );

    du_mgr = Create_Du_Manager(MEM_pu_nz_pool_ptr);

    alias_mgr = Create_Alias_Manager(MEM_pu_nz_pool_ptr);
    opt_pu = Pre_Optimizer(PREOPT_PHASE, region_wn, du_mgr, alias_mgr);

    Delete_Du_Manager(du_mgr,MEM_pu_nz_pool_ptr);
    Delete_Alias_Manager(alias_mgr,MEM_pu_nz_pool_ptr);

    Stop_Timer ( T_Preopt_CU );

    OPT_POOL_Pop ( &MEM_local_pool, MEM_DUMP_FLAG+19 );

    return opt_pu;
}


void
Wopt_Fini (void)
{
    return;
} /* Wopt_Fini */
