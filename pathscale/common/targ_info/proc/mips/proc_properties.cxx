/*
 *  Copyright (C) 2008 PathScale, LLC.  All Rights Reserved.
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


//  
//  Generate PROCESSOR properties information
///////////////////////////////////////
//  $Revision: 1.4 $
//  $Date: 05/03/02 15:28:01-08:00 $
//  $Author: bos@eng-24.internal.keyresearch.com $
//  $Source: common/targ_info/proc/mips/SCCS/s.proc_properties.cxx $

#include <stddef.h>
#include "targ_proc.h"
#include "proc_properties_gen.h"

main()
{
  PROC_PROPERTY 
    branch_delay_slot,		/* branch delay slot */
    same_cycle_branch_shadow,   /* execute branch shadow parallel with branch */
    out_of_order, 		/* out of order execution */
    superscalar,		/* multiple insts per cycle */
    bundles,			/* executes insts as sequence of bundles */
    delayed_exception,		/* has delayed exception support */
    fast_recip;			/* recip inst is fast */

  PROC_Properties_Begin ("MIPS");

/* ====================================================================
 *              Operator attributes descriptors
 * ====================================================================
 */

  /* Does the target have branch delay slots?
   */
  branch_delay_slot = PROC_Property_Create ("has_branch_delay_slot");
  Processor_Group (branch_delay_slot, 
	  		PROCESSOR_mips5kf,
	  		PROCESSOR_twc9a,
	  		PROCESSOR_r10000,
			PROCESSOR_UNDEFINED);

  /* Can the branch shadow be executed in the same cycle as the branch on
   * the target?
   */
  same_cycle_branch_shadow = PROC_Property_Create ("has_same_cycle_branch_shadow");
  Processor_Group (same_cycle_branch_shadow, 
	  		PROCESSOR_mips5kf,
	  		PROCESSOR_twc9a,
			PROCESSOR_UNDEFINED);

  /* Is the target an out-of-order machine?
   */
  out_of_order = PROC_Property_Create ("is_out_of_order");
  Processor_Group (out_of_order, 
	  		PROCESSOR_r10000,
			PROCESSOR_UNDEFINED);

  /* Can the current target issue multiple instructions per cycle?
   */
  superscalar = PROC_Property_Create ("is_superscalar");
  Processor_Group (superscalar,
	  		PROCESSOR_mips5kf,
	  		PROCESSOR_twc9a,
	  		PROCESSOR_r10000,
			PROCESSOR_UNDEFINED);

  /* Does the target execute insts as sequence of bundles, or require 
   * bundle alignment? The info is used to align instructions to bundles, 
   * resolve any bundle packing requirements, etc...
   */
  bundles = PROC_Property_Create ("has_bundles");
  Processor_Group (bundles,
			PROCESSOR_UNDEFINED);

  /* Does the target support delayed_exception mechanism, i.e ability to
   * suppress possible exceptions for speculative instructions with
   * delayed recovery mechanism.
   */
  delayed_exception = PROC_Property_Create ("has_delayed_exception");
  Processor_Group (delayed_exception,
			PROCESSOR_UNDEFINED);

  /* Does the target have a fast recip instruction? 
   * i.e. is it profitable to convert a/b -> a*recip(b)
   */
  fast_recip = PROC_Property_Create ("has_fast_recip");
  Processor_Group (fast_recip,
	  		PROCESSOR_mips5kf,
	  		PROCESSOR_twc9a,
	  		PROCESSOR_r10000,
			PROCESSOR_UNDEFINED);

  PROC_Properties_End();
  return 0;
}
