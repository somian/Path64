/*
 * Copyright (C) 2008-2009 Advanced Micro Devices, Inc.  All Rights Reserved.
 */

/*
 * Copyright 2003, 2004, 2005, 2006 PathScale, Inc.  All Rights Reserved.
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


/* -*-Mode: c++;-*- (Tell emacs to use c++ mode) */
#ifndef cxx_ipa_inline_INCLUDED
#define cxx_ipa_inline_INCLUDED

#ifndef mempool_INCLUDED
#include "mempool.h"			// MEM_POOL
#endif

#ifndef cxx_ipa_cg_INCLUDED
#include "ipa_cg.h"			// IPA_NODE
#endif

extern INT Total_Prog_Size;		// size of the final program

extern INT Total_Inlined;

extern INT Total_Not_Inlined;

/* Report the reason for not inlining a call: */
extern void
Report_Reason (const IPA_NODE *callee, const IPA_NODE *caller,
	       const char *reason , const IPA_EDGE *edge);
extern void
Report_Limit_Reason (const IPA_NODE *callee, const IPA_NODE *caller,
		     const IPA_EDGE *edge, const char *reason, float limit1, float limit2 = -1);

extern void
Perform_Inline_Analysis (IPA_CALL_GRAPH *cg, MEM_POOL* pool);
//INLINING_TUNING static->exportable^
extern void 
Get_Sorted_Callsite_List (IPA_NODE *n, IPA_CALL_GRAPH *cg, AUX_IPA_EDGE<INT32>& cost_vector, vector<IPA_EDGE_INDEX>& callsite_list); //pengzhao
extern float compute_hotness(IPA_EDGE *edge, IPA_NODE *callee, INT callee_size);
extern UINT32 Effective_weight (const IPA_NODE *);

//INLINING_TUNING static->exportable$
#endif // cxx_ipa_inline_INCLUDED
