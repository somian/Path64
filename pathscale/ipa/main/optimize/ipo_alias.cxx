/*
 * Copyright (C) 2007. PathScale, LLC. All Rights Reserved.
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


#define __STDC_LIMIT_MACROS
#include <stdint.h>
#if defined(BUILD_OS_DARWIN)
#include <darwin_elf.h>
#else /* defined(BUILD_OS_DARWIN) */
#include <elf.h>
#endif /* defined(BUILD_OS_DARWIN) */

#include "errors.h"			// for ipa_inline.h
#include "mtypes.h"			// for const.h
#define USE_DST_INTERNALS
#include "dwarf_DST_mem.h"		// for ipc_file.h

#include "ipc_file.h"			// for IP_FILE_HDR
#include "ipa_inline.h"			// for IPA_NODE
#include "ipa_option.h"			// for Trace_IPA
#include "ipo_defs.h"			// for IPA_NODE_CONTEXT

// Mark the reference parameters readonly
void
Mark_readonly_param (IPA_NODE *caller,
		     IPA_EDGE *edge,
		     IPA_CALL_GRAPH *cg)
{
    IPA_NODE_CONTEXT context (caller);
    cg->Map_Callsites (caller);
    WN *call = edge->Whirl_Node();

    if (Trace_IPA || Trace_Perf) {
	fprintf (TFile, "Setting parameters in %s calling ",
		 DEMANGLE (ST_name(caller->Func_ST())));
	fprintf (TFile, "%s: position = ",
		 DEMANGLE (ST_name(cg->Callee(edge)->Func_ST())));
    }

    INT actual_count = WN_num_actuals (call);

    for (INT i = 0; i < actual_count && i < edge->Max_Num_Readonly_Actuals(); ++i) {
	WN *parm = WN_kid (call, i);
	Is_True (WN_operator (parm) == OPR_PARM,
		 ("Invalid kid from a call node"));
	if ((Trace_IPA || Trace_Perf) && (edge->Is_Param_Readonly(i) ||
					  edge->Is_Param_Pass_Not_Saved(i))) {
	    fprintf (TFile, "%d ", i);
	}
	if (edge->Is_Param_Readonly(i)) {
	    WN_Set_Parm_Read_Only (parm);
	    if (Trace_IPA || Trace_Perf)
		fprintf (TFile, "RO ");
	}
	if (edge->Is_Param_Pass_Not_Saved(i)) {
	    WN_Set_Parm_Passed_Not_Saved(parm);
	    if (Trace_IPA || Trace_Perf)
		fprintf (TFile, "!saved ");
	}
    }

    if (Trace_IPA || Trace_Perf)
	fputc ('\n', TFile);
} // Mark_readonly_param
