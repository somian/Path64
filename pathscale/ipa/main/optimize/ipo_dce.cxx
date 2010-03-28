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


//* -*-Mode: c++;-*- (Tell emacs to use c++ mode) */

#define __STDC_LIMIT_MACROS
#include <stdint.h>
#include "defs.h"
#include "symtab.h"			// symbol table
#include "targ_sim.h"			// Get_Return_Mtypes ()
#include "lwn_util.h"			// WN_EXTRACE_FromBlock ()

#include "ipa_option.h"			// Trace_IPA
#include "ipa_cg.h"			// call graphs
#include "ipo_defs.h"			// IPA_NODE_CONTEXT
#include "ipo_parent.h"			// WN_Get_Parent()
#include "ipo_dce.h"

//--------------------------------------------------------------
// figure out the stid preg's that we need to be looking for
//--------------------------------------------------------------
static UINT32 
Compute_Return_Pregs(TY_IDX callee_ty)
{
    PREG_NUM Reg1, Reg2;
    TYPE_ID ty1, ty2;
    TY_IDX typ;

#ifdef KEY
    BOOL ret_via_first_arg = FALSE;
#endif

    // get the return type from the callee's opcode
    if (typ = Tylist_Table[TY_tylist(Ty_Table[callee_ty])])
	if (TY_kind(typ) != KIND_VOID) {

	    if (WHIRL_Return_Info_On) {

		RETURN_INFO return_info = Get_Return_Info (typ, Use_Simulated);

		if (RETURN_INFO_count(return_info) <= 2) {

		    ty1 = RETURN_INFO_mtype (return_info, 0);
		    ty2 = RETURN_INFO_mtype (return_info, 1);
		    Reg1 = RETURN_INFO_preg (return_info, 0);
		    Reg2 = RETURN_INFO_preg (return_info, 1);
#ifdef KEY
		    ret_via_first_arg = RETURN_INFO_return_via_first_arg (return_info);
#endif
		}

		else
		    Fail_FmtAssertion ("Compute_Return_Pregs: more than 2 return registers");
	    }

	    else {
		Get_Return_Mtypes(typ, Use_Simulated, &ty1, &ty2);
		Get_Return_Pregs(ty1, ty2, &Reg1, &Reg2);
	    }
	    
	    if (Reg1 && Reg2)
		return 2;
	    else if (Reg1 || Reg2
#ifdef KEY
	             || ret_via_first_arg
#endif
	            )
		return 1;
	}

    return 0;
}

//--------------------------------------------------------------
// delete the call whirl node
//--------------------------------------------------------------
BOOL
Delete_Call (IPA_NODE *caller, const IPA_NODE *callee, IPA_EDGE *edge,
	     IPA_CALL_GRAPH* cg)
{
    IPA_NODE_CONTEXT context (caller);
    cg->Map_Callsites (caller);           // map callsites to WN nodes
    WN* call = edge->Whirl_Node();
    WN* b = WN_Get_Parent(call, Parent_Map, Current_Map_Tab);

    if ((WN_operator(b) != OPR_BLOCK))
	return FALSE;

    UINT32 Reg_count = (WN_operator (call) == OPR_ICALL) ?
	Compute_Return_Pregs (WN_ty (call)) :
	Compute_Return_Pregs (ST_pu_type (WN_st (call)));

    if (Reg_count) {
	for (INT i = 0; i < Reg_count; ++i) {
	    WN* stid = WN_next(call);
	    if (stid && (WN_operator(stid) == OPR_STID)) {
		if ((WN_operator(WN_kid0(stid)) == OPR_LDID) && 
		    (ST_sclass(WN_st(WN_kid0(stid))) == SCLASS_REG) &&
		    Preg_Is_Dedicated(WN_offset(WN_kid0(stid)))) {
		    WN_EXTRACT_FromBlock(b, stid);
		}
	    }
	}
    }
    WN_EXTRACT_FromBlock(b, call);
    if (Trace_IPA || Trace_Perf) {
	fprintf (TFile, "%s called from ", DEMANGLE(ST_name(callee->Func_ST())));
	fprintf (TFile, "%s deleted\n", DEMANGLE(ST_name(caller->Func_ST())));
    }
    return TRUE;
} // Delete_Call
