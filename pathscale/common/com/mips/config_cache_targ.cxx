/*
 *  Copyright (C) 2007, 2008 PathScale, LLC.  All Rights Reserved.
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


// This may look like C code, but it is really -*- C++ -*-
//
// ====================================================================
// ====================================================================
//
// Module: config_targ_cache.cxx
// Revision history:
//  14-Nov-96 - Original Version, copied from cache_parameters.cxx.
//
// Description:
//
// Target specific logic for config_cache.cxx.
//
// ====================================================================
// ====================================================================

#ifdef _KEEP_RCS_ID
static const char source_file[] = __FILE__;
#endif /* _KEEP_RCS_ID */

#include "defs.h"
#include "errors.h"
#include "config_cache.h"
#include "config_lno.h"
#include "config_targ.h"
#include "config_platform.h"

void MHD::Initialize()
{
  if (Is_Target_Mips5kf()) {

    Non_Blocking_Loads      = TRUE;
    Loop_Overhead_Base      = 18;
    Loop_Overhead_Memref    = 1;
    TLB_Trustworthiness     = 75;
    TLB_NoBlocking_Model    = TRUE;

    L[0] = MHD_LEVEL(MHD_TYPE_CACHE, 	// Type
                     32*1024, 		// Size
                     32,		// Line Size
                     16,		// Clean Miss Penalty
                     16,		// Dirty Miss Penalty
                     4,			// Associativity
                     32, 		// TLB Entries
                     32*1024, // TODO 		// Page Size
                     50,// TODO		// TLB Clean Miss Penalty ?
                     50,// TODO		// TLB Dirty Miss Penalty ?
                     3.0,		// Typical Outstanding Loads ?
                     0.8,		// Load_OP_Overlap_1 ?
                     0.4,		// Load_OP_Overlap_2 ?
                     100);		// Pct_Excess_Writes_Nonhidable ?

    // TODO: this might be too generous: in multiple processor situations,
    // there is a cost to loading the shared bus/memory.
    L[1] = MHD_LEVEL(MHD_TYPE_CACHE, 
                     32*1024*8, 
                     64, 
                     50,
                     50,
                     2,  
                     -1, 
                     -1, 
                     -1, 
                     -1,
                     (LNO_Run_Prefetch ? 1.8 : 1.0),  // ?
                     (LNO_Run_Prefetch ? 0.7 : 0.1),  // ?
                     (LNO_Run_Prefetch ? 0.3 : 0.05), // ?
                     (LNO_Run_Prefetch ? 25  : 50));  // ?

#ifdef Is_True_On
    if (LNO_Verbose)
      printf ("Target Processor: Mips5kf. %" SCNd64 " (%d), %" SCNd64 " (%d)\n", 
              L[0].Effective_Size, L[0].Line_Size,
              L[1].Effective_Size, L[1].Line_Size);
#endif
  } else if (Is_Target_twc9a()) {

    Non_Blocking_Loads      = TRUE;
    Loop_Overhead_Base      = 18;
    Loop_Overhead_Memref    = 1;
    TLB_Trustworthiness     = 75;
    TLB_NoBlocking_Model    = TRUE;

    L[0] = MHD_LEVEL(MHD_TYPE_CACHE, 	// Type
                     32*1024, 		// Size
                     64,		// Line Size
                     15,		// Clean Miss Penalty
                     15,		// Dirty Miss Penalty
                     4,			// Associativity
                     2*512, 		// TLB Entries
                     32*1024, // TODO 		// Page Size
                     50,// TODO		// TLB Clean Miss Penalty ?
                     50,// TODO		// TLB Dirty Miss Penalty ?
                     3.0,		// Typical Outstanding Loads ?
                     0.8,		// Load_OP_Overlap_1 ?
                     0.4,		// Load_OP_Overlap_2 ?
                     100);		// Pct_Excess_Writes_Nonhidable ?

    // TODO: this might be too generous: in multiple processor situations,
    // there is a cost to loading the shared bus/memory.
    L[1] = MHD_LEVEL(MHD_TYPE_CACHE, 
                     32*1024*8, 
                     64, 
                     120,
                     120,
                     4,  
                     -1, 
                     -1, 
                     -1, 
                     -1,
                     (LNO_Run_Prefetch ? 1.8 : 1.0),  // ?
                     (LNO_Run_Prefetch ? 0.7 : 0.1),  // ?
                     (LNO_Run_Prefetch ? 0.3 : 0.05), // ?
                     (LNO_Run_Prefetch ? 25  : 50));  // ?

#ifdef Is_True_On
    if (LNO_Verbose)
      printf ("Target Processor: twc9a. %" SCNd64 " (%d), %" SCNd64 " (%d)\n", 
              L[0].Effective_Size, L[0].Line_Size,
              L[1].Effective_Size, L[1].Line_Size);
#endif
  } else if (Is_Target_R10K()) {
	  
    Non_Blocking_Loads      = TRUE;
    Loop_Overhead_Base      = 18;
    Loop_Overhead_Memref    = 1;
    TLB_Trustworthiness     = 75;
    TLB_NoBlocking_Model    = TRUE;

    L[0] = MHD_LEVEL(MHD_TYPE_CACHE, 	// Type
                     32*1024, 		// Size
                     32,		// Line Size
                     10,		// Clean Miss Penalty
                     18,		// Dirty Miss Penalty
                     2,			// Associativity
                     60, 		// TLB Entries
                     32*1024, // TODO 		// Page Size
                     50,// TODO		// TLB Clean Miss Penalty ?
                     50,// TODO		// TLB Dirty Miss Penalty ?
                     3.0,		// Typical Outstanding Loads ?
                     0.8,		// Load_OP_Overlap_1 ?
                     0.4,		// Load_OP_Overlap_2 ?
                     100);		// Pct_Excess_Writes_Nonhidable ?

    // TODO: this might be too generous: in multiple processor situations,
    // there is a cost to loading the shared bus/memory.
    L[1] = MHD_LEVEL(MHD_TYPE_CACHE, 
                     32*1024*16*8, 
                     128, 
                     40,
                     66,
                     2,  
                     -1, 
                     -1, 
                     -1, 
                     -1,
                     3,
                     0.7,
                     0.3,
                     (LNO_Run_Prefetch ? 25  : 50));  // ?

#ifdef Is_True_On
    if (LNO_Verbose)
      printf ("Target Processor: TARGET_MIPS. %" SCNd64 " (%d), %" SCNd64 " (%d)\n", 
              L[0].Effective_Size, L[0].Line_Size,
              L[1].Effective_Size, L[1].Line_Size);
#endif
  } else {
    FmtAssert(FALSE, ("Unknown target in MHD::Initialize\n"));
  }
}
