/*
  Copyright (C) 2008 PathScale, LLC. All Rights Reserved.
  Copyright (C) 2004, 2005, 2006 PathScale, Inc.  All Rights Reserved.
  Copyright (C) 2000, 2001 Silicon Graphics, Inc.  All Rights Reserved.

  This program is free software; you can redistribute it and/or modify it
  under the terms of version 2 of the GNU General Public License as
  published by the Free Software Foundation.

  This program is distributed in the hope that it would be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  

  Further, this software is distributed without any warranty that it is
  free of the rightful claim of any third person regarding infringement 
  or the like.  Any license provided herein, whether implied or 
  otherwise, applies only to this software file.  Patent licenses, if 
  any, provided herein do not apply to combinations of this program with 
  other software, or any other product whatsoever.  

  You should have received a copy of the GNU General Public License along
  with this program; if not, write the Free Software Foundation, Inc., 59
  Temple Place - Suite 330, Boston MA 02111-1307, USA.

  Contact information:  Silicon Graphics, Inc., 1600 Amphitheatre Pky,
  Mountain View, CA 94043, or:

  http://www.sgi.com

  For further information regarding this notice, see:

  http://oss.sgi.com/projects/GenInfo/NoticeExplan

*/


#define __STDC_LIMIT_MACROS
#include <stdint.h>
#define USE_STANDARD_TYPES
#include <map>
#include "defs.h"
#include "mempool.h"
#include "tn.h"
#include "tn_set.h"
#include "bb.h"
#include "op.h"
#include "op_list.h"
#include "op_map.h"
#include "cgexp.h"
#include "cgtarget.h"
#include "register.h"
#include "cg_loop.h"
#include "cg_swp_options.h"
#include "cg_swp_target.h"
#include "tracing.h"
#include "pf_cg.h"
#include "cg_loop.h"
#include "calls.h"
#include "tag.h"

#ifdef TARG_IA64
  #include "cg_swp.h"
#else
  #include "cg_swp_op_vector.h"
#endif

#include <cmath>

using namespace std;


void 
SWP_Exp_COPY (TN *result, TN *opnd, OPS *ops) {
  Fail_FmtAssertion ("Not yet implemented");

  return;
}


void 
SWP_Loop_Init_Fini (BOOL is_doloop,
                    INT stage_count,
                    OPS *prolog_ops,
                    OPS *body_ops,
                    OPS *epilog_ops) 
{

  return;
}


BOOL 
Prepare_Loop_For_SWP_1 (CG_LOOP& cl, bool trace) {

  return TRUE;
}


BOOL 
Prepare_Loop_For_SWP_2 (CG_LOOP& cl, bool trace) {

  return TRUE;
}


void 
Convert_While_Loop_to_Fully_Predicated_Form (CG_LOOP& cl) {

  return;
}


BOOL 
Remove_Non_Definite_Dependence (CG_LOOP &cl, bool cg_loop_init, bool trace) {
  
  return TRUE;
}


BASE_UPDATE 
OP_base_update_kind (OP *op) {
    
  return NO_BASE_UPDATE;
}


// Returns the address base opnd
INT 
OP_base_opnd_num (OP *op) {

  return -1;
}


// Returns the address base opnd
INT 
OP_base_res_num (OP *op) {

  return -1;
}


INT32 
OP_incr_opnd_num (OP *op)
{
  // the last operand
  return (OP_opnds(op) - 1);
}


BOOL 
Imm_Value_In_Range (OP *op, INT64 imm) {
    
  return FALSE;
}


void 
Gen_SWP_Branch (CG_LOOP &cl, bool is_doloop) {

  return;
}


void 
Gen_SWP_Branch_Predict (BB *body, BB *prolog, BB *epilog) {

  return;
}


void 
Undo_SWP_Branch (CG_LOOP &cl, bool is_doloop) {

  return;
}


void 
Gen_Implicit_Prefetches (CG_LOOP &cl, bool trace) {

  return;
}


TN* 
Base_update_tn (OP *op) { 
  
  return 0;
}


void
SWP_TARGET_Calculate_Critical_Resources (const SWP_OP_VECTOR& ops,
                                         INT ii)
{
  Fail_FmtAssertion ("Not yet implemented");

  return;
}


float
SWP_TARGET_Get_Scale_Factor (OP *op) {
  Fail_FmtAssertion ("Not yet implemented");

  return 0.0f;
}


INT
SWP_TARGET_Get_Branch_Cycle (INT ii) { 
  Fail_FmtAssertion ("Not yet implemented");

  return 0;
}


INT 
SWP_TARGET_Scheduling_Constrain_For_OP (const SWP_OP_VECTOR& ops,
                                        INT candidate,
                                        INT cycle,
                                        INT ii)
{
  Fail_FmtAssertion ("Not yet implemented");

  return 0;
}


void
SWP_TARGET_Adjust_LoadStore_Offset (SWP_OP_VECTOR& ops, BOOL trace) {
  Fail_FmtAssertion ("Not yet implemented");

  return;
}

