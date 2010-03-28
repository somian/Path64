/*
 * Copyright (C) 2008 PathScale, LLC.  All Rights Reserved.
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


/* ====================================================================
 * ====================================================================
 *
 *  Module: cg_loop_mii.h
 *  $Revision$
 *  $Date$
 *  $Author$
 *  $Source$
 *
 *  Revision history:
 *   16-Oct-91 - Original Version
 *
 *  Synopsis:
 *
 *      Calculate CG_LOOP_min_ii for the given loop, maximal costs of
 *      within SCC paths as a side effect.  The costs are useful
 *      during scheduling for pruning the backtracking search.
 *
 *  Interface Description:
 *
 *      Exported variables:
 *
 *          INT32 CG_LOOP_min_ii
 *
 *              Set by this module to a lower bound on the II to
 *              search for a schedule.
 *
 *          INT CG_LOOP_res_min_ii
 *
 *              Set by this module to a lower bound on the II as
 *              dictated by resource constraints (i.e., not counting
 *		recurrences).
 *
 *          INT CG_LOOP_rec_min_ii
 *
 *              Set by this module to a lower bound on the II as
 *              dictated by recurring constraints (i.e., not counting
 *		resources).
 *
 *      Exported functions:
 *
 *	    void CG_LOOP_Calculate_Min_Resource_II(
 *              BB *loop_body,
 *              OP_LIST *loop_overhead,
 *		BOOL ignore_prefetches,
 *		BOOL ignore_prefetch_strides
 *          )
 *		Calculate CG_LOOP_min_ii based solely on resources (i.e.,
 *		ignoring recurrences).  This is used to recalculate
 *		the min ii as recurrences are fixed.  If <ignore_prefetches>
 *		is TRUE, prefetches will be ignored when computing the
 *		resource usage.  If <ignore_prefetch_strides> is FALSE,
 *		each prefetch will be counted as 1/stride prefetches.
 *
 *		Results in: CG_LOOP_min_ii, CG_LOOP_res_min_ii.
 *
 *	    void CG_LOOP_Calculate_Min_Recurrence_II(
 *              BB *loop_body,
 *		BOOL ignore_non_def_mem_deps
 *          )
 *		Calculate CG_LOOP_min_ii, including restrictions caused
 *		by recurrences.  This differs from CG_LOOP_Calculate_Max_
 *		Costs_And_Min_II in that it assumes the resource min
 *		II has already been calculated, and in that it doesn't
 *		try to fix recurrences.  It's used to recalculate the
 *		min II after fixing recurrences introduces new SCCs.
 *		If <ignore_non_def_mem_deps> is TRUE, non-definite memory
 *		dependences will not be considered in self-recurrences
 *		(others should have been filtered out by
 *		CG_LOOP_Make_Strongly_Connected_Components).
 *
 *              Requires: CG_LOOP_Make_Strongly_Connected_Components.
 *
 *		Results in: CG_LOOP_min_ii.
 *
 *          void CG_LOOP_Calculate_Max_Costs_And_Min_II(
 *              BB *loop_body,
 *              OP_LIST *loop_overhead,
 *		BOOL ignore_prefetches,
 *		BOOL ignore_prefetch_strides,
 *		BOOL ignore_non_def_mem_deps
 *          )
 *
 *              Calculate CG_LOOP_min_ii and the maximal costs of intra-SCC
 *              paths.  This includes the calculations of the values
 *              traditionally called "MII" and "MRII", i.e., both
 *              resources and recurrences are taken into account.  If
 *		<ignore_prefetches> is TRUE, prefetch OPs will be ignored
 *		when computing resource usage.  If <ignore_prefetch_strides>
 *		is FALSE, each prefetch will be counted as 1/stride prefetches.
 *		If <ignore_non_def_mem_deps> is TRUE, non-definite memory
 *		dependences will not be considered in self-recurrences
 *		(others should have been filtered out by
 *		CG_LOOP_Make_Strongly_Connected_Components).
 *
 *              Requires: CG_LOOP_Make_Strongly_Connected_Components.
 *
 *              Results in: CG_LOOP_min_ii, and OP_scc_{descendents,ancestors}
 *
 * ====================================================================
 * ====================================================================
 */

#ifndef CG_LOOP_MII_INCLUDED
#define CG_LOOP_MII_INCLUDED

#ifdef _KEEP_RCS_ID
static const char cg_loop_mii_rcs_id[] = "$Source$ $Revision$";
#endif /* _KEEP_RCS_ID */

#include "defs.h"
#include "op_list.h"
#include "bb.h"

extern INT32       CG_LOOP_min_ii;
extern INT32       CG_LOOP_rec_min_ii;
extern INT32       CG_LOOP_res_min_ii;

extern void
CG_LOOP_Calculate_Min_Resource_II(
  BB *loop_body,
  OP_LIST *loop_overhead,
  BOOL ignore_prefetches,
  BOOL ignore_prefetch_strides
);

extern void
CG_LOOP_Calculate_Min_Recurrence_II(
  BB *loop_body,
  BOOL ignore_non_def_mem_deps
);

extern void
CG_LOOP_Calculate_Max_Costs_And_Min_II(
  BB *loop_body,
  OP_LIST *loop_overhead,
  BOOL ignore_prefetches,
  BOOL ignore_prefetch_strides,
  BOOL ignore_non_def_mem_deps
);

#endif /* CG_LOOP_MII_INCLUDED */
