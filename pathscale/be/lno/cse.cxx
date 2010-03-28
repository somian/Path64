/*
 * Copyright 2008 PathScale, LLC.  All Rights Reserved.
 */

/*
 *  Copyright (C) 2006. QLogic Corporation. All Rights Reserved.
 */

/*
 * Copyright 2003, 2004, 2005, 2006 PathScale, Inc.  All Rights Reserved.
 */

/*

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


//                     Inter-iteration cse's of associative operations
//                     -----------------------------------------------
//
// Description:
//
//	Given something like
//
//	   do i = 1,n
//	      a(i) + b(i)
//	      a(i+1) + b(i+1)
//  
//	generate
//
//	   tp1 = a(1) + b(1)
//	   do i
//	     t = tp1
//	     tp1 = a(i+1) + b(i+1)
//	     t
//	     tp1
//
//   This is called right at the end of lno.  So we have must dependences
//   on inner loop references and loops should be guarded.
//   This should only be called when roundoff is sufficiently high to allow
//   reassociation of pluses
//
//  Algorithm applied to every inner loop:
//
//	We group together loads that are being added/multiplied/mined/maxed
//  into equivalence classes.  For example, given (a[i]+b[k])*(c[i]+d[j]),
//  'a' and 'b' are put into one equivalence class; 'c' and 'd' are put
//  into another.  We only look at loads with no incomming dependences.
//  Also, we only look at loads that execute on each iteration (loads not
//  under ifs and other cf nodes).
//	We walk all the loads.  For each load, we search for must 
//  dependences of distance 1 whose parents have the same opcodes
//  (the last condition insures that we don't cse a sum with a product)
//  For every such pair, we enter into
//  a hash table the equivalence class numbers of the two load.  Each
//  entry, (eq1,eq2) in the table implies that we're loading something
//  from eq1 and then loading the same think one iteration later in eq2.
//  Given a pair of such entries, we've found a cse.
//     Each cse gives us four loads, a1,a2,b1,b2 such that
// we know that were computing a1+b1 and that the
// same value is being used one iteration later in a2+b2.
// We now try to grow the set by checking if the same value is used
// in earlier (one earlier) or later (one later) iterations.
//    Once we have a set of pairs, substitution is straight forward.
//
// 12/96: Extend algorithm to deal with (a[i]+b) vrs (a[i+1]+b), etc
//	  the approach is to temporarily add these ldids to the dependence
//	  graph and then use the regular algorithm
//
//
/* ====================================================================
 * ====================================================================
 *
 * Module: cse.cxx
 * $Revision: 1.13 $
 * $Date: 05/11/28 15:53:02-08:00 $
 * $Author: fchow@fluorspar.internal.keyresearch.com $
 * $Source: be/lno/SCCS/s.cse.cxx $
 *
 * Revision history:
 *  dd-mmm-94 - Original Version
 *
 * Description: Inter-iteration cse of additions
 * ====================================================================
 * ====================================================================
 */

#define __STDC_LIMIT_MACROS
#include <stdint.h>
#ifdef USE_PCH
#include "lno_pch.h"
#endif // USE_PCH
#pragma hdrstop

static const char *source_file = __FILE__;
static const char *rcs_id = "$Source: be/lno/SCCS/s.cse.cxx $ $Revision: 1.13 $";

#include <sys/types.h>
#include "lnopt_main.h"
#include "dep_graph.h"
#include "lnoutils.h"
#include "lwn_util.h"
#include "opt_du.h"
#include "cse.h"
#include "reduc.h"
#include "config_opt.h"

enum EQUIVALENCE_TYPE { EQ_NONE=0, EQ_ADD, EQ_MPY, EQ_MIN, EQ_MAX, EQ_RECIP, EQ_DIV,
			EQ_RSQRT,EQ_SQRT,EQ_LOAD };

static void Inter_Iteration_Cses_R(WN *wn);
static void Inter_Iteration_Cses_Loop(WN *loop);
static EQUIVALENCE_TYPE Set_Up_Equivalence_Classes(WN *wn, WN *loop);
static void Set_Up_Equivalence_Class(WN *wn, OPERATOR etype, WN *loop,TYPE_ID type);
static void Process_Pair(WN *a1, WN *b1, WN *a2, WN *b2, WN *loop);
static void Transform_Code(STACK<WN_PAIR_EC> *cse_stack, WN *loop, BOOL all_invariant);
static void  Add_Invariant_Deps();
static UINT16 equivalence_class_number;
static WN_MAP Equivalence_Class_Map;

static INT name = 0;  // used in preg name
static INT debug;

extern void Inter_Iteration_Cses(WN *func_nd)
{
  debug = Get_Trace(TP_LNOPT, TT_LNO_DEBUG_CSE);
  if (debug) fprintf(TFile,"Begin Inter_Iteration_Cses");
  Equivalence_Class_Map = WN_MAP32_Create(&LNO_default_pool);
  FmtAssert(Equivalence_Class_Map != -1, 
	("Ran out off mappings in Inter_Iterations_Cses"));
  Inter_Iteration_Cses_R(func_nd);
  WN_MAP_Delete(Equivalence_Class_Map);
}

static void Inter_Iteration_Cses_R(WN *wn)
{
  OPCODE opcode = WN_opcode(wn);
  if (opcode == OPC_BLOCK) {
    WN *kid = WN_first(wn);
    while (kid) {
      WN *next = WN_next(kid);
      Inter_Iteration_Cses_R(kid);
      kid = next;
    }
  } else if ((opcode == OPC_DO_LOOP) && Do_Loop_Is_Inner(wn) &&
	     !Do_Loop_Is_Mp(wn)) {
    if (Do_Loop_Is_Good(wn) && !Do_Loop_Has_Gotos(wn) && 
		(WN_kid_count(wn) == 6)) {
      WN *loop_info = WN_do_loop_info(wn);
      if (WN_Loop_Nz_Trip(loop_info)) {
	Inter_Iteration_Cses_Loop(wn);
      }
    }
  } else {
    for (INT kidno=0; kidno<WN_kid_count(wn); kidno++) {
      Inter_Iteration_Cses_R(WN_kid(wn,kidno));
    }
  }
}

// is child1 the same child number of parent1 as child2 is of parent2
// this is used for divides to make sure we don't cse a[i]/b[i] vrs b[i-1]/a[i-1]
BOOL Same_Side(WN *child1, WN *parent1, WN *child2, WN *parent2) 
{
  if (child1 == WN_kid0(parent1)) {
    return (child2 == WN_kid0(parent2));
  } else {
    return (child2 == WN_kid1(parent2));
  }
}


typedef STACK<WN *> STACK_OF_WN;
STACK_OF_WN *delete_stack; // wn's to be deleted
STACK_OF_WN *load_stack; // all the good loads
STACK_OF_WN *invariant_ldid_stack; 
WN *insertion_point;

// Process a good inner loop
static void Inter_Iteration_Cses_Loop(WN *loop)
{
  if (debug) fprintf(TFile,"Processing an inner loop.");
  MEM_POOL_Push(&LNO_local_pool);

  // Find Last prefetch, we insert after code after prefetches 
  // to make WOPT happy
  WN *tmp = WN_first(WN_do_body(loop));
  insertion_point = NULL;
  while (tmp != NULL && OPCODE_is_prefetch(WN_opcode(tmp))) {
    insertion_point = tmp;
    tmp = WN_next(tmp);
  }

  load_stack = CXX_NEW(STACK_OF_WN(&LNO_local_pool),&LNO_local_pool);
  invariant_ldid_stack = CXX_NEW(STACK_OF_WN(&LNO_local_pool),&LNO_local_pool);
  delete_stack = CXX_NEW(STACK_OF_WN(&LNO_local_pool),&LNO_local_pool);
  name=-1;

  // step 1, mark each potential load with an equivalence class number
  // also, set up the stack of good loads
  equivalence_class_number = 0;
  Set_Up_Equivalence_Classes(WN_do_body(loop),loop);

#ifdef KEY
  // Bug 4211 - Too many loop invariants may overflow the dependence graph. 
  // CICSE on a large loop can also adversely affect quality of compilation for 
  // 32-bit ABI (probably register allocation related). Here 1000 is a large 
  // number small enough to peak 173.applu (under -m32).
  if (invariant_ldid_stack->Elements() > 1000) {
    CXX_DELETE(load_stack,&LNO_local_pool);
    CXX_DELETE(invariant_ldid_stack,&LNO_local_pool);
    CXX_DELETE(delete_stack,&LNO_local_pool);
    MEM_POOL_Pop(&LNO_local_pool);
    return;
  }
#endif
  // step 1.5 add dependences between invariant ldids
  Add_Invariant_Deps();

  // step 2, walk all the loads, enter into the hash table every pair
  // of loads with a distance 1 dependence

  EQUIV_WN_HASH *hash_table = CXX_NEW(EQUIV_WN_HASH(200,&LNO_local_pool),
					&LNO_local_pool);
  INT i;
  for (i=0; i<load_stack->Elements(); i++) {   
    WN *load = load_stack->Bottom_nth(i);
    UINT16 eq1 = (UINT16) WN_MAP32_Get(Equivalence_Class_Map,load);
    if (eq1) { // might be zero if we've dealt with this load already
      VINDEX16 v = Current_Dep_Graph->Get_Vertex(load);
      EINDEX16 e = Current_Dep_Graph->Get_In_Edge(v);
      WN *parent = LWN_Get_Parent(load);
      WN *child = load;
      if (WN_operator(parent) == OPR_CVT) {
	child = parent;
	parent = LWN_Get_Parent(parent);
      }
      OPCODE parent_opc = WN_opcode(parent);
      OPERATOR oper = OPCODE_operator(parent_opc);
      while (e && eq1) {
        DEP dep = Current_Dep_Graph->Dep(e);
        if (Current_Dep_Graph->Is_Must(e) &&
	    (!DEP_IsDistance(dep) || (DEP_Distance(dep) == 1))) {
          VINDEX16 v2 = Current_Dep_Graph->Get_Source(e);
	  if (v2 != v) {
	    WN *load2 = Current_Dep_Graph->Get_Wn(v2);
            UINT16 eq2 = (UINT16) WN_MAP32_Get(Equivalence_Class_Map,load2);
	    WN *parent2 = LWN_Get_Parent(load2);
	    WN *child2 = load2;
            if (WN_operator(parent2) == OPR_CVT) {
	      child2 = parent2;
	      parent2 = LWN_Get_Parent(parent2);
            }
	    if (eq2 && (parent_opc == WN_opcode(parent2)) &&
		((oper != OPR_DIV) || Same_Side(child,parent,child2,parent2))) {
	      if ((oper == OPR_RECIP) || (oper == OPR_SQRT) ||
			      (oper == OPR_RSQRT)
#ifdef TARG_X8664
			      || (oper == OPR_ATOMIC_RSQRT)
#endif
	         ) {
	        Process_Pair(load,NULL,load2,NULL,loop);
	        eq1 = 0;
	      } else {
	        WN_PAIR *wn_pair = hash_table->Find(EQUIV_PAIR(eq1,eq2));
	        if (wn_pair) {
	          if (!WN_MAP32_Get(Equivalence_Class_Map,wn_pair->Wn1) ||
	              !WN_MAP32_Get(Equivalence_Class_Map,wn_pair->Wn2)) {
		    hash_table->Remove(EQUIV_PAIR(eq1,eq2));
	            WN_PAIR *wn_pair=CXX_NEW(WN_PAIR(load,load2),&LNO_local_pool);
	            hash_table->Enter(EQUIV_PAIR(eq1,eq2),wn_pair);
                  } else if ((load != wn_pair->Wn1) && (load != wn_pair->Wn2) &&
		      (load2 != wn_pair->Wn1) && (load2 != wn_pair->Wn2)) {
	            Process_Pair(load,wn_pair->Wn1,load2,wn_pair->Wn2,
				    loop);
		    eq1 = 0;
                  }
	        } else {
	          WN_PAIR *wn_pair = CXX_NEW(WN_PAIR(load,load2),&LNO_local_pool);
	          hash_table->Enter(EQUIV_PAIR(eq1,eq2),wn_pair);
                }
              }
	    }
          }
        }
        e = Current_Dep_Graph->Get_Next_In_Edge(e);
      }
    }
  }

  // remove ldids from dependence graph
  for (i=0; i<load_stack->Elements(); i++) {
    WN *load = load_stack->Bottom_nth(i);
    if (WN_operator(load) == OPR_LDID) {
      LWN_Delete_CG_dep_graph(load);
    }
  }

  // remove deleted loads
  for (i=0; i<delete_stack->Elements(); i++) {
    WN *to_delete = delete_stack->Bottom_nth(i);
    LWN_Delete_Tree(to_delete);
  }
  CXX_DELETE(hash_table,&LNO_local_pool);
  CXX_DELETE(load_stack,&LNO_local_pool);
  CXX_DELETE(delete_stack,&LNO_local_pool);
  MEM_POOL_Pop(&LNO_local_pool);
}

static OPERATOR Norm_Opr(OPERATOR oper)
{
  if (oper == OPR_SUB) return OPR_ADD;
  return oper;
}

// mark loads with equivalence class numbers
// two loads are in the same equivalence class if they are being op'd together
// only add indirect loads that are guranteed to execute and that have no
// dependences to any stores
// now also add invariant ldids that are guranteed to execute
// everything else has an equivalence class of zero
//
// Return TRUE iff the subtree routed at wn is a sum of leaves
static EQUIVALENCE_TYPE Set_Up_Equivalence_Classes(WN *wn, WN *loop)
{
  OPCODE opcode = WN_opcode(wn);
  if (opcode == OPC_BLOCK) {
    WN *kid = WN_first(wn);
    while (kid) {
      WN *next = WN_next(kid);
      Set_Up_Equivalence_Classes(kid,loop);
      kid = next;
    }
    return EQ_NONE;
  }
  TYPE_ID type = OPCODE_rtype(opcode);

  if (OPCODE_is_store(opcode)) {
    if (!OPCODE_is_load(WN_opcode(WN_kid0(wn)))) { // shortcut avoid a[i]=b
#ifdef TARG_X8664
      if (WN_desc(wn) == MTYPE_V16I1 || WN_desc(wn) == MTYPE_V16I2 || 
	  WN_desc(wn) == MTYPE_V16I4 || WN_desc(wn) == MTYPE_V16I8 || 
	  WN_desc(wn) == MTYPE_V16F4 || WN_desc(wn) == MTYPE_V16F8)
	return EQ_NONE;      
#endif /* TARG_X8664 */
      if (Set_Up_Equivalence_Classes(WN_kid0(wn),loop) != EQ_NONE) {
	equivalence_class_number++;
	Set_Up_Equivalence_Class(WN_kid0(wn),
		Norm_Opr(WN_operator(WN_kid0(wn))),loop,WN_rtype(WN_kid0(wn)));
      }
    }
    return EQ_NONE;
  } else if (OPCODE_is_leaf(opcode) || OPCODE_is_load(opcode)) {
    return EQ_LOAD;
  } else if ((OPCODE_operator(opcode) == OPR_CVT) &&
	     (OPCODE_is_load(WN_opcode(WN_kid0(wn)))) &&
	     (OPCODE_desc(opcode) == WN_rtype(WN_kid0(wn)))) {
    return EQ_LOAD;
  } else if (OPCODE_is_expression(opcode)) {
    OPERATOR oper = OPCODE_operator(opcode);
    if ((oper == OPR_ADD) || (oper == OPR_MPY) || (oper == OPR_MAX) ||
	(oper == OPR_MIN) || (oper == OPR_SUB) || (oper == OPR_DIV)) {
      EQUIVALENCE_TYPE result0 = Set_Up_Equivalence_Classes(WN_kid0(wn),loop);
      EQUIVALENCE_TYPE result1 = Set_Up_Equivalence_Classes(WN_kid1(wn),loop);
      EQUIVALENCE_TYPE return_result = EQ_NONE;
      TYPE_ID type0 = WN_rtype(WN_kid0(wn));
      TYPE_ID type1 = WN_rtype(WN_kid1(wn));

      BOOL finished_kid0 = FALSE; // can't go any higher, set up subtree as a class
      BOOL finished_kid1 = FALSE;
      if ((result0 != EQ_NONE) && (result0 != EQ_LOAD) && (type0 != type)) {
	finished_kid0 = TRUE;
      }
      if ((result1 != EQ_NONE) && (result1 != EQ_LOAD) && (type1 != type)) {
	finished_kid1 = TRUE;
      }

      if (oper == OPR_ADD) {
        if ((result0 != EQ_NONE)&&(result0 != EQ_LOAD)&&(result0 != EQ_ADD)){
	  finished_kid0 = TRUE;
        }
        if ((result1 != EQ_NONE)&&(result1 != EQ_LOAD)&&(result1 != EQ_ADD)){
	  finished_kid1 = TRUE;
        }
	if (type0 == type && ((result0 == EQ_ADD) || (result0 == EQ_LOAD))) {
          return_result = EQ_ADD;
        }
	if (type1 == type && ((result1 == EQ_ADD) || (result1 == EQ_LOAD))) {
          return_result = EQ_ADD;
        }
      } else if (oper == OPR_SUB) { // kid0 is like an add
        if ((result1 != EQ_NONE) && (result1 != EQ_LOAD)) {
	  finished_kid1 = TRUE;
        }
	if (type0 == type && ((result0 == EQ_ADD) || (result0 == EQ_LOAD))) {
          return_result = EQ_ADD;
        }
      } else if (oper == OPR_MPY) {
        if ((result0 != EQ_NONE)&&(result0 != EQ_LOAD)&&(result0 != EQ_MPY)){
	  finished_kid0 = TRUE;
        }
        if ((result1 != EQ_NONE)&&(result1 != EQ_LOAD)&&(result1 != EQ_MPY)){
	  finished_kid1 = TRUE;
        }
	if (type0 == type && ((result0 == EQ_MPY) || (result0 == EQ_LOAD))) {
          return_result = EQ_MPY;
        }
	if (type1 == type && ((result1 == EQ_MPY) || (result1 == EQ_LOAD))) {
          return_result = EQ_MPY;
        }
      } else if (oper == OPR_DIV) { 
	if ((result0 == EQ_LOAD) && (result1 == EQ_LOAD) && (type0 == type) &&
	    (type1 == type)) {
         return EQ_DIV; 
        }
        if ((result1 != EQ_NONE) && (result1 != EQ_LOAD)) {
	  finished_kid1 = TRUE;
        }
        if ((result0 != EQ_NONE) && (result0 != EQ_LOAD)) {
	  finished_kid0 = TRUE;
        }
      } else if (oper == OPR_MAX) {
        if ((result0 != EQ_NONE)&&(result0 != EQ_LOAD)&&(result0 != EQ_MAX)){
	  finished_kid0 = TRUE;
        }
        if ((result1 != EQ_NONE)&&(result1 != EQ_LOAD)&&(result1 != EQ_MAX)) {
	  finished_kid1 = TRUE;
        }
	if (type0 == type && ((result0 == EQ_MAX) || (result0 == EQ_LOAD))) {
          return_result = EQ_MAX;
        }
	if (type1 == type && ((result1 == EQ_MAX) || (result1 == EQ_LOAD))) {
          return_result = EQ_MAX;
        }
      } else if (oper == OPR_MIN) {
        if ((result0 != EQ_NONE)&&(result0 != EQ_LOAD)&&(result0 != EQ_MIN)){
	  finished_kid0 = TRUE;
        }
        if ((result1 != EQ_NONE)&&(result1 != EQ_LOAD)&&(result1 != EQ_MIN)) {
	  finished_kid1 = TRUE;
        }
	if (type0 == type && ((result0 == EQ_MIN) || (result0 == EQ_LOAD))) {
          return_result = EQ_MIN;
        }
	if (type1 == type && ((result1 == EQ_MIN) || (result1 == EQ_LOAD))) {
          return_result = EQ_MIN;
        }
      }
      if ((result0 != EQ_NONE) && (result0 != EQ_LOAD)) {
	finished_kid0 = TRUE;
      }
      if ((result1 != EQ_NONE) && (result1 != EQ_LOAD)) {
	finished_kid1 = TRUE;
      }

      if (finished_kid0) {
	if (equivalence_class_number==UINT16_MAX) return EQ_NONE; // overflow
	equivalence_class_number++;
	Set_Up_Equivalence_Class(WN_kid0(wn), 
		Norm_Opr(WN_operator(WN_kid0(wn))),loop,type);
      }
      if (finished_kid1)  {
	if (equivalence_class_number==UINT16_MAX) return EQ_NONE; // overflow
	equivalence_class_number++;
	Set_Up_Equivalence_Class(WN_kid1(wn), 
		Norm_Opr(WN_operator(WN_kid1(wn))),loop,type);
      }

      return return_result;
    } else if ((oper == OPR_RECIP) || (oper == OPR_SQRT) || 
		(oper == OPR_RSQRT)
#ifdef TARG_X8664
		|| (oper == OPR_ATOMIC_RSQRT)
#endif
	      ) {
      EQUIVALENCE_TYPE result = Set_Up_Equivalence_Classes(WN_kid0(wn),loop);
      if ((result == EQ_LOAD) && 
	  (WN_rtype(WN_kid0(wn)) == type)) {
	if (equivalence_class_number==UINT16_MAX) {
	  return EQ_NONE; // overflow
        }
	equivalence_class_number++;
	Set_Up_Equivalence_Class(wn,oper,loop,type);
      }
    } else {
      for (INT kidno=0; kidno<WN_kid_count(wn); kidno++) {
        EQUIVALENCE_TYPE result = 
		Set_Up_Equivalence_Classes(WN_kid(wn,kidno),loop);
        if ((result != EQ_NONE) && (result != EQ_LOAD)) {
	  if (equivalence_class_number==UINT16_MAX) {
	    return EQ_NONE; // overflow
          }
	  equivalence_class_number++;
	  Set_Up_Equivalence_Class(WN_kid(wn,kidno),
		Norm_Opr(WN_operator(WN_kid(wn,kidno))),loop,type);
        }
      }
      return EQ_NONE;
    }
  }
  return EQ_NONE;
}

// WN is a subtree of things being added
// mark every indirect load or invariant ldid with no incomming write dependences 
// as a member of the class (outgoing, or anti-dependence, are allowed)
static void Set_Up_Equivalence_Class(WN *wn, OPERATOR etype, WN *loop,TYPE_ID type)
{
  OPCODE opcode = WN_opcode(wn);
  if ((OPCODE_operator(opcode) == OPR_CVT) &&
      (OPCODE_rtype(opcode) == type)) {
    WN *kid = WN_kid0(wn);
    OPCODE kido = WN_opcode(kid);
    if (OPCODE_is_load(kido)) {
      Set_Up_Equivalence_Class(kid,etype,loop,type);
      return;
    }
  }
  if (OPCODE_is_load(opcode)) {
    WN *parent = LWN_Get_Parent(wn);
    OPCODE parent_opcode = WN_opcode(parent);
    if ((OPCODE_operator(parent_opcode) == OPR_CVT)) {
      if (OPCODE_desc(parent_opcode) != OPCODE_rtype(opcode)) {
	return;  // mismatched type
      }
    } else {
      if (OPCODE_rtype(opcode) != type) return;
    }
    if (OPCODE_operator(opcode) == OPR_LDID) {
      DEF_LIST *defs = Du_Mgr->Ud_Get_Def(wn);
      BOOL invar = TRUE;
      if (defs) {
        DEF_LIST_ITER iter(defs);
        for(DU_NODE *node=iter.First(); !iter.Is_Empty() && invar; node=iter.Next()) {
          WN *def = node->Wn();
          WN *parent = def;
          while (parent && WN_opcode(parent) != OPC_DO_LOOP) {
            parent = LWN_Get_Parent(parent);
          }
          if (parent == loop) invar = FALSE;
        }
      }
      if (invar) {
        WN_MAP32_Set(Equivalence_Class_Map,wn,(INT32) equivalence_class_number);
        load_stack->Push(wn);
        invariant_ldid_stack->Push(wn);
      }
    } else {
      VINDEX16 v = Current_Dep_Graph->Get_Vertex(wn);
      if (v) {
	EINDEX16 e = Current_Dep_Graph->Get_In_Edge(v);
	while (e) {
	  VINDEX16 v2 = Current_Dep_Graph->Get_Source(e);
	  if (OPCODE_is_store(WN_opcode(Current_Dep_Graph->Get_Wn(v2)))) {
	    return;
          }
	  e = Current_Dep_Graph->Get_Next_In_Edge(e);
	}
      }
      WN_MAP32_Set(Equivalence_Class_Map,wn,(INT32) equivalence_class_number);
      load_stack->Push(wn);
    }
  } else if ((WN_operator(wn) == etype) &&
	     (WN_rtype(wn) == type)) {
    for (INT kidno=0; kidno<WN_kid_count(wn); kidno++) {
      Set_Up_Equivalence_Class(WN_kid(wn,kidno),etype,loop,type);
    }
  } else if ((etype == OPR_ADD) && 
	     (WN_operator(wn) == OPR_SUB)) {
    Set_Up_Equivalence_Class(WN_kid0(wn),etype,loop,type);
  } 
}

// append a pair of wns to a stack of wns
static void Append_Wn_Pair(STACK<WN_PAIR_EC> *cse_stack, WN *a3, WN *b3,
			   INT32 eclass)
{
  cse_stack->Push(WN_PAIR_EC(a3,b3,eclass));
}

// prepend a pair of wns to a stack of wns
// we accomplish this by shifting the entire stack up
// this is ok since this will almost never be called with stacks bigger
// than 2 or 3 elements
static void Prepend_Wn_Pair(STACK<WN_PAIR_EC> *cse_stack, WN *a0, WN *b0,
				INT32 eclass)
{
  WN_PAIR_EC *top = & cse_stack->Top_nth(0);
  cse_stack->Push(WN_PAIR_EC(top->Wn1,top->Wn2,top->EClass)); 
  for (INT i=cse_stack->Elements()-2; i>=1; i--) {
    WN_PAIR_EC *in = &cse_stack->Bottom_nth(i);
    WN_PAIR_EC *out = &cse_stack->Bottom_nth(i-1);
    in->Wn1 = out->Wn1;
    in->Wn2 = out->Wn2;
    in->EClass = out->EClass;
  }
  WN_PAIR_EC *bot = &cse_stack->Bottom_nth(0);
  bot->Wn1 = a0;
  bot->Wn2 = b0;
  bot->EClass = eclass;
}

// are all the pairs in the cse stack invariant
// actually, they are if the first pair is
// and the first pair is invariant if it's dependence from
// the second pair isn't a distance
static BOOL Both_Invariant(STACK<WN_PAIR_EC> *cse_stack)
{
  WN_PAIR_EC wn_pair0 = cse_stack->Bottom_nth(0);
  WN_PAIR_EC wn_pair1 = cse_stack->Bottom_nth(1);
  VINDEX16 v0 = Current_Dep_Graph->Get_Vertex(wn_pair0.Wn1);
  VINDEX16 v1 = Current_Dep_Graph->Get_Vertex(wn_pair1.Wn1);
  EINDEX16 e = Current_Dep_Graph->Get_Edge(v1,v0);
  DEP dep = Current_Dep_Graph->Dep(e);
  if (DEP_IsDistance(dep)) return FALSE;

  if (!wn_pair0.Wn2) return TRUE;
  v0 = Current_Dep_Graph->Get_Vertex(wn_pair0.Wn2);
  v1 = Current_Dep_Graph->Get_Vertex(wn_pair1.Wn2);
  e = Current_Dep_Graph->Get_Edge(v1,v0);
  dep = Current_Dep_Graph->Dep(e);
  if (DEP_IsDistance(dep)) return FALSE;

  return TRUE;
}

// at this point we know that were computing a1 op b1, and that that
// same value is being used one iteration later in a2 op b2
//
// in the case of unary ops (recip,sqrt,rsqrt) b1 and b2 are null
static void Process_Pair(WN *a1, WN *b1, WN *a2, WN *b2, WN *loop)
{
  // remove these from future consideration
  INT32 ec1 = WN_MAP32_Get(Equivalence_Class_Map,a1); 
  INT32 ec2 = WN_MAP32_Get(Equivalence_Class_Map,a2); 
  WN_MAP32_Set(Equivalence_Class_Map,a1,(INT32) 0);
  if (b1) WN_MAP32_Set(Equivalence_Class_Map,b1,(INT32) 0);
  WN_MAP32_Set(Equivalence_Class_Map,a2,(INT32) 0);
  if (b2) WN_MAP32_Set(Equivalence_Class_Map,b2,(INT32) 0);

  WN *parent = LWN_Get_Parent(a1);
  WN *child = a1;
  if (WN_operator(parent) == OPR_CVT) {
    child = parent;
    parent = LWN_Get_Parent(parent);
  }
  if (WN_operator(parent) == OPR_DIV) {
    if (child != WN_kid0(parent)) { // swap since divide isn't communative
      WN *tmp = a1;
      a1 = b1;
      b1 = tmp;

      tmp = a2;
      a2 = b2;
      b2 = tmp;
    }
  }


  STACK<WN_PAIR_EC> cse_stack(&LNO_local_pool);
  cse_stack.Push(WN_PAIR_EC(a1,b1,ec1));
  cse_stack.Push(WN_PAIR_EC(a2,b2,ec2));

  // look for the same value in earlier iterations

  BOOL found_earlier;
  do {
    found_earlier = FALSE;
    VINDEX16 v = Current_Dep_Graph->Get_Vertex(a2);
    WN *par_a2 = LWN_Get_Parent(a2);
    WN *child_a2 = a2;
    if (WN_operator(par_a2) == OPR_CVT) {
      child_a2 = par_a2;
      par_a2 = LWN_Get_Parent(par_a2);
    }
    OPCODE parop_a2 = WN_opcode(par_a2);
    EINDEX16 e = Current_Dep_Graph->Get_In_Edge(v);
    while (e && !found_earlier) {
      DEP dep = Current_Dep_Graph->Dep(e);
      if (Current_Dep_Graph->Is_Must(e) &&
	  (!DEP_IsDistance(dep) || (DEP_Distance(dep) == 1))) {
	VINDEX16 sourcev = Current_Dep_Graph->Get_Source(e);
	WN *a3 = Current_Dep_Graph->Get_Wn(sourcev);
	UINT16 eq0 = (UINT16) WN_MAP32_Get(Equivalence_Class_Map,a3);
        WN *par_a3 = LWN_Get_Parent(a3);
	WN *child_a3 = a3;
        if (WN_operator(par_a3) == OPR_CVT) {
	  child_a3 = par_a3;
          par_a3 = LWN_Get_Parent(par_a3);
        }
	if (eq0 && (WN_opcode(par_a3) == parop_a2) &&
	    ((OPCODE_operator(parop_a2) != OPR_DIV) || 
	      Same_Side(child_a2,par_a2,child_a3,par_a3))) {
	  if (!b1) {
            INT32 ec = WN_MAP32_Get(Equivalence_Class_Map,a3); 
            WN_MAP32_Set(Equivalence_Class_Map,a3,(INT32) 0);
	    Append_Wn_Pair(&cse_stack,a3,NULL,ec);
	    found_earlier = TRUE;
	  } else {
            VINDEX16 vb = Current_Dep_Graph->Get_Vertex(b2);
            EINDEX16 eb = Current_Dep_Graph->Get_In_Edge(vb);
            while (eb && !found_earlier) {
              DEP dep = Current_Dep_Graph->Dep(eb);
              if (Current_Dep_Graph->Is_Must(eb) &&
	          (!DEP_IsDistance(dep) || (DEP_Distance(dep) == 1))) {
	        VINDEX16 sourcev = Current_Dep_Graph->Get_Source(eb);
	        WN *b3 = Current_Dep_Graph->Get_Wn(sourcev);
	        if (b3 != a3) {
	          UINT16 eqb3 = (UINT16) WN_MAP32_Get(Equivalence_Class_Map,b3);
	          if (eqb3 == eq0) {
                    INT32 ec = WN_MAP32_Get(Equivalence_Class_Map,a3); 
                    WN_MAP32_Set(Equivalence_Class_Map,a3,(INT32) 0);
                    WN_MAP32_Set(Equivalence_Class_Map,b3,(INT32) 0);
		    Append_Wn_Pair(&cse_stack,a3,b3,ec);
		    found_earlier = TRUE;
                  }
	        }
              }
              eb = Current_Dep_Graph->Get_Next_In_Edge(eb);
            }
	  }
	}
      }
      e = Current_Dep_Graph->Get_Next_In_Edge(e);
    }
    if (found_earlier) {
      a2 = cse_stack.Top_nth(0).Wn1;
      b2 = cse_stack.Top_nth(0).Wn2;
    }
  } while (found_earlier);

  // look for the same value in later iterations

  BOOL found_later;
  do {
    found_later = FALSE;
    VINDEX16 v = Current_Dep_Graph->Get_Vertex(a1);
    WN *par_a1 = LWN_Get_Parent(a1);
    WN *child_a1 = a1;
    if (WN_operator(par_a1) == OPR_CVT) {
      child_a1 = par_a1;
      par_a1 = LWN_Get_Parent(par_a1);
    }
    OPCODE parop_a1 = WN_opcode(par_a1);
    EINDEX16 e = Current_Dep_Graph->Get_Out_Edge(v);
    while (e && !found_later) {
      DEP dep = Current_Dep_Graph->Dep(e);
      if (Current_Dep_Graph->Is_Must(e) &&
	 (!DEP_IsDistance(dep) || (DEP_Distance(dep) == 1))) {
	VINDEX16 sinkv = Current_Dep_Graph->Get_Sink(e);
	WN *a0 = Current_Dep_Graph->Get_Wn(sinkv);
	UINT16 eq3 = (UINT16) WN_MAP32_Get(Equivalence_Class_Map,a0);
        WN *par_a0 = LWN_Get_Parent(a0);
	WN *child_a0 = a0;
        if (WN_operator(par_a0) == OPR_CVT) {
	  child_a0 = par_a0;
          par_a0 = LWN_Get_Parent(par_a0);
        }
	if (eq3 && (WN_opcode(par_a0) == parop_a1) &&
	    ((OPCODE_operator(parop_a1) != OPR_DIV) || 
	      Same_Side(child_a1,par_a1,child_a0,par_a0))) {
	  if (!b1) {
            INT32 ec = WN_MAP32_Get(Equivalence_Class_Map,a0); 
            WN_MAP32_Set(Equivalence_Class_Map,a0,(INT32) 0);
	    Prepend_Wn_Pair(&cse_stack,a0,NULL,ec);
	    found_later = TRUE;
	  } else {
            VINDEX16 vb = Current_Dep_Graph->Get_Vertex(b1);
            EINDEX16 eb = Current_Dep_Graph->Get_Out_Edge(vb);
            while (eb && !found_later) {
              DEP dep = Current_Dep_Graph->Dep(eb);
              if (Current_Dep_Graph->Is_Must(eb) &&
	          (!DEP_IsDistance(dep) || (DEP_Distance(dep) == 1))) {
	        VINDEX16 sinkv = Current_Dep_Graph->Get_Sink(eb);
	        WN *b0 = Current_Dep_Graph->Get_Wn(sinkv);
	        if (a0 != b0) {
	          UINT16 eqb0 = (UINT16) WN_MAP32_Get(Equivalence_Class_Map,b0);
	          if (eqb0 == eq3) {
                    INT32 ec = WN_MAP32_Get(Equivalence_Class_Map,a0); 
                    WN_MAP32_Set(Equivalence_Class_Map,a0,(INT32) 0);
                    WN_MAP32_Set(Equivalence_Class_Map,b0,(INT32) 0);
		    Prepend_Wn_Pair(&cse_stack,a0,b0,ec);
		    found_later = TRUE;
	          }
	        }
              }
              eb = Current_Dep_Graph->Get_Next_Out_Edge(eb);
            }
	  }
        }
      }
      e = Current_Dep_Graph->Get_Next_Out_Edge(e);
    }
    if (found_later) {
      a1 = cse_stack.Bottom_nth(0).Wn1;
      b1 = cse_stack.Bottom_nth(0).Wn2;
    }
  } while (found_later);
  if (Both_Invariant(&cse_stack)) {
    Transform_Code(&cse_stack,loop,1);
  } else {
    Transform_Code(&cse_stack,loop,0);
  }
}




// cse_stack->Bottom_nth(i) contains an ai+bi where ai+bi was calculated
// one iteration earlier by a(i+1)+b(i+1)
//
// Transform the code to get rid of the cse
//
// if all_invariant then 
// cse_stack->Bottom_nth(i) contains the invariant expression a+b 
// basically a normal cse, we do this case because we can find
// things such as a+b+c vrs a+c+b, whereas the optimizer doesn't do this
// We special case all_invariant to generate more efficient and simpler code
//
// for unary ops (recip, sqrt, rsqrt), all the Wn2's are null
//
static void Transform_Code(STACK<WN_PAIR_EC> *cse_stack, WN *loop, BOOL all_invariant)
{
#ifdef KEY
  static UINT cse_num = 0;
  cse_num ++;
  if (cse_num < LNO_Cse_Loop_Skip_Before ||
      cse_num > LNO_Cse_Loop_Skip_After ||
      cse_num == LNO_Cse_Loop_Skip_Equal)
    return;
#endif /* KEY */
  DO_LOOP_INFO *dli = Get_Do_Loop_Info(loop);
  WN *guard = dli->Guard;
  if (guard) {
    WN_Reset_If_Guard(guard);
  }



  name++;
  // create element pregs
  INT number_sums = cse_stack->Elements() ;
  WN_OFFSET *preg_array = CXX_NEW_ARRAY(WN_OFFSET,number_sums,
		&LNO_local_pool);

  if (debug) {
    fprintf(TFile,"Cse'ing a sum used across %d iterations.\n",number_sums);
  }

  // what's the type type for the preg
  // they all have to be the same type or they wouldn't have must deps 
  TYPE_ID type;
  if (WN_operator(LWN_Get_Parent(
		cse_stack->Bottom_nth(0).Wn1)) == OPR_CVT) {
    type = 
      WN_rtype(LWN_Get_Parent(cse_stack->Bottom_nth(0).Wn1));
  } else {
    type = WN_rtype(cse_stack->Bottom_nth(0).Wn1);
  }
  ST *preg_st = MTYPE_To_PREG(type);
  char preg_name[20];
  SYMBOL index_symbol(WN_start(loop));
  char *index_name = ST_name(index_symbol.St());
  INT length = strlen(index_name);
  if (length < 10) {
    sprintf(preg_name,"tmp%d_%s",name,index_name);
  } else {
    sprintf(preg_name,"tmp%d_i",name);
  }
  length = strlen(preg_name);
  sprintf(&preg_name[length],"_%d",0);
#ifdef _NEW_SYMTAB
  preg_array[0] = Create_Preg(type,preg_name);
#else
  preg_array[0] = Create_Preg(type,preg_name,NULL);
#endif
  INT i;
  for (i=1; i<number_sums; i++) {
    if (all_invariant) {
      preg_array[i] = preg_array[0]; // only need one , since they're the same
    } else {
      sprintf(&preg_name[length],"_%d",i);
#ifdef _NEW_SYMTAB
      preg_array[i] = Create_Preg(type,preg_name);
#else
      preg_array[i] = Create_Preg(type,preg_name,NULL);
#endif
    }
  }

  BOOL unary = (cse_stack->Bottom_nth(0).Wn2 == NULL);
  WN *wn1p = LWN_Get_Parent(cse_stack->Bottom_nth(0).Wn1);
  if (WN_operator(wn1p) == OPR_CVT) wn1p = LWN_Get_Parent(wn1p);

  OPERATOR oper = Norm_Opr(WN_operator(wn1p));
  OPCODE op = OPCODE_make_op(oper,type,MTYPE_V);
  OPCODE stid_op = OPCODE_make_op(OPR_STID, MTYPE_V, type);
  OPCODE ldid_op = OPCODE_make_op(OPR_LDID, type, type);

  // store the references to the pregs so that we can fix up DU chains
  WN **init_stids = CXX_NEW_ARRAY(WN *,number_sums,&LNO_local_pool);
  WN **loop_stids = CXX_NEW_ARRAY(WN *,number_sums,&LNO_local_pool);
  WN **loop_ldids = CXX_NEW_ARRAY(WN *,number_sums,&LNO_local_pool);

  DOLOOP_STACK *loop_stack=CXX_NEW(DOLOOP_STACK(&LNO_local_pool),
                             &LNO_local_pool);
  Build_Doloop_Stack(LWN_Get_Parent(loop), loop_stack);

  // preload pregs  1 .. number_sums-1 with the initial
  // values of sums 0 .. number_sums-2
  if (!all_invariant) {
    for (i=0; i<number_sums-1; i++) {
      WN_PAIR_EC *wn_pair = &cse_stack->Bottom_nth(i);
      WN *sum1,*sum2=NULL;
      if (WN_operator(wn_pair->Wn1) == OPR_LDID) {
        // we're replacing one of the earlier tmp variable
        // the initial value is the next tmp variable
        WN_PAIR_EC *wn_pair_next = &cse_stack->Bottom_nth(i+1);
	if (WN_operator(LWN_Get_Parent(wn_pair_next->Wn1)) == 
								OPR_CVT) {
          sum1 = LWN_Copy_Tree(LWN_Get_Parent(wn_pair_next->Wn1));
          LWN_Copy_Def_Use(wn_pair_next->Wn1,WN_kid0(sum1),Du_Mgr);
        } else {
          sum1 = LWN_Copy_Tree(wn_pair_next->Wn1);
          LWN_Copy_Def_Use(wn_pair_next->Wn1,sum1,Du_Mgr);
	}
      } else {
	if (WN_operator(LWN_Get_Parent(wn_pair->Wn1)) == 
								OPR_CVT) {
          sum1 = LWN_Copy_Tree(LWN_Get_Parent(wn_pair->Wn1));
          LWN_Copy_Def_Use(wn_pair->Wn1,WN_kid0(sum1),Du_Mgr);
        } else {
          sum1 = LWN_Copy_Tree(wn_pair->Wn1);
          LWN_Copy_Def_Use(wn_pair->Wn1,sum1,Du_Mgr);
        }
        Replace_Ldid_With_Exp_Copy(index_symbol,sum1,
		  WN_kid0(WN_start(loop)),Du_Mgr);
      }
      if (wn_pair->Wn2) {
       if (WN_operator(wn_pair->Wn2) == OPR_LDID) {
        WN_PAIR_EC *wn_pair_next = &cse_stack->Bottom_nth(i+1);
	if (WN_operator(LWN_Get_Parent(wn_pair_next->Wn2)) == 
								OPR_CVT) {
          sum2 = LWN_Copy_Tree(LWN_Get_Parent(wn_pair_next->Wn2));
          LWN_Copy_Def_Use(wn_pair_next->Wn2,WN_kid0(sum2),Du_Mgr);
        } else {
          sum2 = LWN_Copy_Tree(wn_pair_next->Wn2);
          LWN_Copy_Def_Use(wn_pair_next->Wn2,sum2,Du_Mgr);
	}
       } else {
	if (WN_operator(LWN_Get_Parent(wn_pair->Wn2)) == 
							OPR_CVT) {
          sum2 = LWN_Copy_Tree(LWN_Get_Parent(wn_pair->Wn2));
          LWN_Copy_Def_Use(wn_pair->Wn2,WN_kid0(sum2),Du_Mgr);
        } else {
          sum2 = LWN_Copy_Tree(wn_pair->Wn2);
          LWN_Copy_Def_Use(wn_pair->Wn2,sum2,Du_Mgr);
	}
        Replace_Ldid_With_Exp_Copy(index_symbol,sum2,
		  WN_kid0(WN_start(loop)),Du_Mgr);
       }
      }
      WN *add;
      if (sum2) {
        add = LWN_CreateExp2(op,sum1,sum2);
      } else {
	add = LWN_CreateExp1(op,sum1);
      }
      WN *stid = LWN_CreateStid(stid_op,preg_array[i+1],
                                preg_st,Be_Type_Tbl(type),add);
      LWN_Copy_Frequency(add,sum1);
      LWN_Copy_Frequency(stid,add);
      init_stids[i+1] = stid;
      Create_alias(Alias_Mgr,stid);
      LWN_Insert_Block_Before(LWN_Get_Parent(loop),loop,stid);
      LWN_Copy_Linenumber(LWN_Get_Statement(wn_pair->Wn1),stid);
      LNO_Build_Access(stid, loop_stack, &LNO_default_pool);
    }
  }

  // inside the loop


  // replace loads with pregs
  WN_PAIR_EC *wn_pair;
  WN *wn1, *wn2;
  WN *statement;
  statement = LWN_Get_Statement(cse_stack->Top_nth(0).Wn1);
  for (i=0; i<number_sums; i++) {
    WN *ldid = WN_CreateLdid(ldid_op,preg_array[i],
	preg_st,Be_Type_Tbl(type));
    loop_ldids[i] = ldid;
    Create_alias(Alias_Mgr,ldid);
    wn_pair = &cse_stack->Bottom_nth(i);
    wn1 = wn_pair->Wn1;
    if (WN_operator(LWN_Get_Parent(wn1)) == OPR_CVT) {
      wn1 = LWN_Get_Parent(wn1);
    }
    WN *plus1 = LWN_Get_Parent(wn1);
    LWN_Copy_Frequency_Tree(ldid,wn1);
    if (unary) {
      if (i != number_sums-1) {
	delete_stack->Push(WN_kid0(plus1));
      }
      WN *parent = LWN_Get_Parent(plus1);
      INT j=0;
      while (WN_kid(parent,j) != plus1) j++;
      WN_kid(parent,j) = ldid;
      LWN_Set_Parent(ldid,parent);
#ifndef KEY
      // Bug 3170: Do not delete this node. Node is used for copying line number
      // and frequency information at the end of this module.
      WN_Delete(plus1);
#endif
    } else {
      if (wn1 == WN_kid0(plus1)) {
        // don't delete for last preg because we will use it below
        if (i != number_sums-1) {
	  delete_stack->Push(WN_kid0(plus1));
        }
        WN_kid0(plus1) = ldid;
        LWN_Set_Parent(ldid,plus1);
      } else {
        if (i != number_sums-1) {
	  delete_stack->Push(WN_kid1(plus1));
        }
        WN_kid1(plus1) = ldid;
        LWN_Set_Parent(ldid,plus1);
      }
    }

    wn2 = wn_pair->Wn2;
    if (wn2) {
      if (WN_operator(LWN_Get_Parent(wn2)) == OPR_CVT) {
        wn2 = LWN_Get_Parent(wn2);
      }
      WN *plus2 = LWN_Get_Parent(wn2);
      WN *other_kid=NULL;
      if (wn2 == WN_kid0(plus2)) {
        // don't delete for last preg because we will use it below
        if (i != number_sums-1) {
           delete_stack->Push(WN_kid0(plus2));
        }
        if (WN_kid_count(plus2) > 1) {
          other_kid = WN_kid1(plus2);
        }
      } else {
        if (i != number_sums-1) {
          delete_stack->Push(WN_kid1(plus2));
        }
        other_kid = WN_kid0(plus2);
      }
      if (WN_operator(plus2) == OPR_SUB) {
	other_kid = LWN_CreateExp1(OPCODE_make_op(OPR_NEG,type,MTYPE_V),
								other_kid);
      }


      WN *parent = LWN_Get_Parent(plus2);
      INT j=0;
      while (WN_kid(parent,j) != plus2) j++;
      WN_kid(parent,j) = other_kid;
      LWN_Set_Parent(other_kid,parent);
#ifndef KEY
      // Bug 3170: Do not delete this node. Node is used for copying line number
      // and frequency information at the end of this module.
      WN_Delete(plus2);
#endif

    }
  }


  // load preg number_sums-1 with the value of sums number_sums-1
  WN *add;
  if (wn2) {
    add = LWN_CreateExp2(op,wn1,wn2);
  } else {
    add = LWN_CreateExp1(op,wn1);
  }
  WN *stid = LWN_CreateStid(stid_op,preg_array[number_sums-1],
                            preg_st,Be_Type_Tbl(type),add);
  WN *prev_stid = stid;
  loop_stids[number_sums-1] = stid;
  Create_alias(Alias_Mgr,stid);
  WN* new_insertion_point;
  if (all_invariant) {
    LWN_Insert_Block_Before(LWN_Get_Parent(loop),loop,stid);
#ifdef KEY // bug 8624
   // inserton_point = NULL means inserting at the very begining
   // of the loop and thus bug 8624 causes wrong ordering of 
   // instructions in loop and dependencies are broken.
    new_insertion_point = insertion_point;
#else
    new_insertion_point = NULL;
#endif
  } else {
    LWN_Insert_Block_After(WN_do_body(loop),insertion_point,stid);
    new_insertion_point = stid;
  }
  LWN_Copy_Linenumber(statement,stid);
  LWN_Copy_Frequency_Tree(stid,wn1);

  // set pregs0 .. number_sums-2 to pregs1 .. number_sums-1
  if (!all_invariant) {
    for (i=number_sums-2; i>=0; i--) {
      WN_PAIR_EC *wn_pair = &cse_stack->Bottom_nth(i);
      WN *ldid = WN_CreateLdid(ldid_op,preg_array[i+1],
	  preg_st,Be_Type_Tbl(type));
      Create_alias(Alias_Mgr,ldid);
      stid = LWN_CreateStid(stid_op,preg_array[i],
                            preg_st,Be_Type_Tbl(type),ldid);
      loop_stids[i] = stid;
      Create_alias(Alias_Mgr,stid);
      LWN_Insert_Block_After(WN_do_body(loop),insertion_point,stid);
      LWN_Copy_Linenumber(LWN_Get_Statement(wn_pair->Wn1),stid);
      LWN_Copy_Frequency_Tree(stid,wn_pair->Wn1);
      Du_Mgr->Add_Def_Use(prev_stid,ldid);
      Du_Mgr->Add_Def_Use(init_stids[i+1],ldid);
      Du_Mgr->Ud_Get_Def(ldid)->Set_loop_stmt(loop);
      prev_stid = stid;
    }
  }
  insertion_point = new_insertion_point;


  if (!all_invariant) {
    for (i=0; i<number_sums; i++) {
      Du_Mgr->Add_Def_Use(loop_stids[i],loop_ldids[i]);
    }
  } else {
    for (i=0; i<number_sums; i++) {
      Du_Mgr->Add_Def_Use(loop_stids[number_sums-1],loop_ldids[i]);
    }
  }

  // temporaryily put tmp ldids into dependence graph so that we can later
  // cse them (i.e. catch a+b+c or a+b+c+d)
  // don't do this for unary ops since there can't be multiple element unary ops
  if (!unary) {
    for (i=0; i<number_sums; i++) {
      Current_Dep_Graph->Add_Vertex(loop_ldids[i]);
      load_stack->Push(loop_ldids[i]);
      WN_MAP32_Set(Equivalence_Class_Map,
	  loop_ldids[i],cse_stack->Bottom_nth(i).EClass);
    }
  }

  if (!all_invariant) {
    for (INT from=1; from<number_sums; from++) {
      VINDEX16 vfrom = Current_Dep_Graph->Get_Vertex(loop_ldids[from]);
      if (vfrom) {
        INT to = from - 1;
        VINDEX16 vto = Current_Dep_Graph->Get_Vertex(loop_ldids[to]);
        if (vto) {
          Current_Dep_Graph->Add_Edge(vfrom,vto,DEP_SetDistance(1),1);
        }
      }
    }
  } else {
    for (INT from=0; from<number_sums; from++) {
      VINDEX16 vfrom = Current_Dep_Graph->Get_Vertex(loop_ldids[from]);
      if (vfrom) {
        for (INT to=from+1; to<number_sums; to++) {
          VINDEX16 vto = Current_Dep_Graph->Get_Vertex(loop_ldids[to]);
          if (vto) {
	    Current_Dep_Graph->Add_Edge(vfrom,vto,DEP_SetDirection(DIR_POSEQ),1);
	    Current_Dep_Graph->Add_Edge(vto,vfrom,DEP_SetDirection(DIR_POS),1);
	  }
        }
      }
    }
  }

  CXX_DELETE_ARRAY(preg_array,&LNO_local_pool);
  CXX_DELETE_ARRAY(init_stids,&LNO_local_pool);
  CXX_DELETE_ARRAY(loop_stids,&LNO_local_pool);
  CXX_DELETE_ARRAY(loop_ldids,&LNO_local_pool);

}

// Add dependences between ldids of the same location
static void  Add_Invariant_Deps()
{
  MEM_POOL_Push(&LNO_local_pool);
  VINDEX16 *vertices = 
    CXX_NEW_ARRAY(VINDEX16,invariant_ldid_stack->Elements(),&LNO_local_pool);
  STACK_OF_WN *tmp_stack; 
  tmp_stack = CXX_NEW(STACK_OF_WN(&LNO_local_pool),&LNO_local_pool);
  INT i2=0;
  INT i;
  for (i=0; i<invariant_ldid_stack->Elements(); i++) {
    WN *tmp = invariant_ldid_stack->Bottom_nth(i);
    if (!Current_Dep_Graph->Get_Vertex(tmp)) {
      vertices[i2++] = Current_Dep_Graph->Add_Vertex(tmp);
      tmp_stack->Push(tmp);
    }
  }
  for (i=0; i<tmp_stack->Elements(); i++) {
    ST *st1 = WN_st(tmp_stack->Bottom_nth(i));
    WN_OFFSET offset1 = WN_offset(tmp_stack->Bottom_nth(i));
    for (INT j=i+1; j<tmp_stack->Elements(); j++) {
      ST *st2 = WN_st(tmp_stack->Bottom_nth(j));
      WN_OFFSET offset2 = WN_offset(tmp_stack->Bottom_nth(j));
      if ((st1 == st2) && (offset1 == offset2)) {
	Current_Dep_Graph->Add_Edge(vertices[i],vertices[j],DEP_SetDirection(DIR_POSEQ),1);
	Current_Dep_Graph->Add_Edge(vertices[j],vertices[i],DEP_SetDirection(DIR_POS),1);
      }
    }
  }
  MEM_POOL_Pop(&LNO_local_pool);
}

#ifdef KEY
#include "const.h"
#include "wn_simp.h"
#include "glob.h"

static REDUCTION_MANAGER *reduc_mgr=NULL;
static MEM_POOL FACT_default_pool; 
static INT current_level=0;
static int local_num=0;

extern WN* Find_Stmt_Under(WN* stmt,WN* body);
extern void Fully_Unroll_Short_Loops(WN *);
//don't do invariant factorization if current level >=3
#define STOP_LEVEL 3
#define GOOD_FACTORS 4

static INT In_Invar_Stack(WN *wn, STACK_OF_WN *invar_stack)
{
  for(INT i=0; i<invar_stack->Elements(); i++){
    WN *tmp = invar_stack->Top_nth(i);
    if(Tree_Equiv(wn,tmp)){
        if(wn==tmp) return 2;
        else return 1;
    }
   }
  return 0;
}

//build invariant and variant tree---- how
static void Split_Var_Tree(WN *mult, WN *loop, STACK_OF_WN *invar_stack)
{
   TYPE_ID type = WN_rtype(mult);
   OPERATOR opr = WN_operator(mult);
   switch(opr){
    case OPR_ADD: case OPR_MPY:
         Split_Var_Tree(WN_kid0(mult),loop,invar_stack);
         Split_Var_Tree(WN_kid1(mult),loop,invar_stack);
         break;
    case OPR_LDID:
       {
         INT ret_num = In_Invar_Stack(mult, invar_stack);
         if(ret_num){
           TCON tcon; ST* st;
           tcon = Host_To_Targ_Float(type, 1.0);//may be integer here
           st = New_Const_Sym (Enter_tcon(tcon), Be_Type_Tbl(type));
           WN *const_wn = WN_CreateConst(OPR_CONST, type, MTYPE_V, st);
           WN *parent = LWN_Get_Parent(mult);
           INT which_kid=1;
           if(mult == WN_kid0(parent))
              which_kid = 0;
           WN_kid(parent, which_kid) = const_wn;
           LWN_Set_Parent(const_wn, parent);
           LWN_Parentize(parent);
         //------- how to deal with this, it will cause segfault
         //if you delete. Otherwise, dangling point here
         //  if(ret_num==1)
         //   LWN_Delete_Tree(mult);
          }
           break;
         }
      case OPR_ILOAD:
           break;
      default:
          FmtAssert(FALSE,("Not an vaild term!"));
          break;
     }
}

static WN *Build_Invar_Tree(STACK_OF_WN *invar_stack)
{
  WN *invar_tree = NULL;
  OPCODE mpy_opc;
  for(INT i=0; i< invar_stack->Elements(); i++){
    WN *wn = invar_stack->Top_nth(i);
    if(invar_tree==NULL)
      invar_tree = wn;
    else{
      mpy_opc = OPCODE_make_op(OPR_MPY, WN_rtype(wn), MTYPE_V);
      invar_tree = LWN_CreateExp2(mpy_opc, invar_tree, wn);
    }
  }
 return invar_tree;  
}

static BOOL Well_Formed_Mult(WN *mpy)
{
  if(WN_rtype(mpy) != MTYPE_F8 && WN_rtype(mpy) != MTYPE_F4)
    return FALSE; //handle only fp for now

  if(WN_operator(mpy)==OPR_MPY){
    if(!Well_Formed_Mult(WN_kid0(mpy)) || !Well_Formed_Mult(WN_kid1(mpy)))
     return FALSE;
    else return TRUE;
  }else if(WN_operator(mpy)==OPR_ILOAD || WN_operator(mpy)==OPR_LDID)
    return TRUE;
  else return FALSE; //CVT should also be good?
}

static BOOL Well_Formed_Terms(WN *reduction_term)
{
  if(WN_operator(reduction_term)==OPR_ADD){
     if(!Well_Formed_Terms(WN_kid0(reduction_term)) ||
        !Well_Formed_Terms(WN_kid1(reduction_term)))
        return FALSE;
     else 
        return TRUE;
  }else if(WN_operator(reduction_term)==OPR_MPY)
      return Well_Formed_Mult(reduction_term);
     else 
      return FALSE;
}

static STACK_OF_WN *Invar_Stack_Intersection(STACK_OF_WN *one, STACK_OF_WN *two)
{
 STACK_OF_WN *tmp_stack=CXX_NEW(STACK_OF_WN(&FACT_default_pool),
                                &FACT_default_pool);
 for(INT i=0; i<one->Elements(); i++){
   WN *ele1 = one->Top_nth(i);
   for(INT j=0; j<two->Elements();j++){
     WN *ele2 = two->Top_nth(j);
     if(Tree_Equiv(ele1,ele2))
      tmp_stack->Push(ele1);
    }
 }
 //CXX_DELETE(one, &FACT_default_pool);
 //CXX_DELETE(two, &FACT_default_pool);
 return tmp_stack;
}

//duplicate invars in one terms
static BOOL Gather_Term_Invars(WN *term, STACK_OF_WN *invar_stack, WN *loop)
{
  if((WN_operator(term)==OPR_LDID 
     && Is_Loop_Invariant_Exp(term, loop))){
     for(INT ii=0; ii<invar_stack->Elements(); ii++)
        if(Tree_Equiv(invar_stack->Bottom_nth(ii), term))
          return FALSE;
     invar_stack->Push(term);
  }
  else if(WN_operator(term)==OPR_MPY){
    if(!Gather_Term_Invars(WN_kid0(term), invar_stack, loop))
      return FALSE;
    if(!Gather_Term_Invars(WN_kid1(term), invar_stack, loop))
      return FALSE;
  }
 return TRUE;
}

static BOOL Build_Term_Stack(WN *term, STACK_OF_WN *term_stack)
{
  if(WN_operator(term)==OPR_MPY){
    term_stack->Push(term);
  }else if(WN_operator(term)==OPR_ADD ||
           WN_operator(term)==OPR_SUB){
  if(!Build_Term_Stack(WN_kid0(term), term_stack))
    return FALSE;
  if(!Build_Term_Stack(WN_kid1(term), term_stack))
    return FALSE;
 }else return FALSE;
 return TRUE;
}


static BOOL Gather_Stmt_Common_Invar(WN *stmt, STACK_OF_WN *common_invar, WN *loop)
{
 
 FmtAssert(common_invar && common_invar->Elements()==0, 
           ("Expect an initialized but empty stack!"));
 STACK_OF_WN *term_stack = CXX_NEW(STACK_OF_WN(&FACT_default_pool),
                                 &FACT_default_pool);
 if(!Build_Term_Stack(WN_kid1(WN_kid0(stmt)), term_stack))
   return FALSE;
 if(term_stack->Elements()==0) //no terms? 
    return FALSE;
 STACK_OF_WN *ret_invar; 
 for(INT ii=0; ii< term_stack->Elements(); ii++){
   WN *term = term_stack->Bottom_nth(ii);
   STACK_OF_WN *tmp_invar = CXX_NEW(STACK_OF_WN(&FACT_default_pool),
                                     &FACT_default_pool);
   if(!Gather_Term_Invars(term, tmp_invar, loop))
      return FALSE;
   if(tmp_invar->Elements()==0) 
      return FALSE;
   if(ii==0)
      ret_invar = tmp_invar;
   else
      ret_invar = Invar_Stack_Intersection(ret_invar, tmp_invar);
   if(ret_invar->Elements()==0)
      return FALSE;
 }

 while(ret_invar->Elements()>0)
   common_invar->Push(ret_invar->Pop());
 return TRUE;
}


static WN *Find_Reduction_Variable(WN *add)
{
  FmtAssert(WN_operator(add)==OPR_ADD,("Not an addition!"));
 if(WN_operator(WN_kid0(add))==OPR_LDID &&
   reduc_mgr->Which_Reduction(WN_kid0(add))==RED_ADD)
   return WN_kid0(add);
 else if(WN_operator(WN_kid1(add))==OPR_LDID &&
   reduc_mgr->Which_Reduction(WN_kid1(add))==RED_ADD)
   return WN_kid1(add);

 if(WN_operator(WN_kid0(add))==OPR_ADD){
   WN *reduc = Find_Reduction_Variable(WN_kid0(add));
   if(reduc) return reduc;
 }

 if(WN_operator(WN_kid1(add))==OPR_ADD){
   WN *reduc = Find_Reduction_Variable(WN_kid1(add));
   if(reduc) return reduc;
 }
 return NULL;
}


static BOOL Well_Formed_Reduction_Stmt(WN *stmt, WN *loop)
{
  //requie reduction on stid and an add.sub reduction
  if(WN_operator(stmt) != OPR_STID ||
     reduc_mgr->Which_Reduction(stmt) != RED_ADD)
  return FALSE;

  WN *kid0 = WN_kid0(stmt);
  WN *terms=NULL;
  INT which_kid;
  //exchange kid?
  if(WN_operator(kid0)==OPR_ADD){
    WN *reduc = Find_Reduction_Variable(kid0);
    if(!reduc) 
       return FALSE;
    INT which_path=0;
    WN *parent = LWN_Get_Parent(reduc);
    if(WN_kid1(parent)==reduc)
      which_path=1;
    WN *tmp = reduc;
    while(LWN_Get_Parent(tmp) != kid0)
      tmp = LWN_Get_Parent(tmp);
     FmtAssert(tmp, ("Error!"));
  if(tmp==WN_kid0(kid0)){//exchange kid 1 with reduc
    if(tmp!=reduc){
    WN *temp = WN_kid1(kid0);
    WN_kid(parent, which_path) = temp;
    LWN_Set_Parent(temp, parent);
    WN_kid1(kid0)=tmp;
    LWN_Set_Parent(tmp, kid0);
    WN_kid0(kid0)=reduc;
    LWN_Set_Parent(reduc, kid0);
    LWN_Parentize(kid0);
    LWN_Parentize(parent);
    }
   }else{//tmp = WN_kid1(kid0)
   WN *temp = WN_kid0(kid0);
    WN_kid0(kid0)=reduc;
    LWN_Set_Parent(reduc, kid0);
    WN_kid(parent, which_path) = temp;
    LWN_Set_Parent(temp, parent);
    LWN_Parentize(kid0);
    LWN_Parentize(parent);
  }
  terms = WN_kid1(kid0);
  which_kid = 1;
 }else if(WN_operator(kid0)==OPR_SUB){
  //TODO: need to consider SUB
  return FALSE;
  FmtAssert(WN_operator(WN_kid0(kid0))==OPR_LDID &&
            reduc_mgr->Which_Reduction(WN_kid0(kid0))==RED_ADD,
            ("Wrong reduction!"));
  terms = WN_kid1(kid0);
  which_kid = 1;
 }else//TODO: ---:) 
    return FALSE;
 //terms are formed as expected
 if(!Well_Formed_Terms(terms))
   return FALSE;

 //now need to consider def use relation here
 if (!Du_Mgr)
      return FALSE;
 USE_LIST* use_list=Du_Mgr->Du_Get_Use(stmt);
 if (!use_list) return FALSE;
 WN *body = WN_do_body(loop);
 USE_LIST_ITER uiter(use_list);
 for (DU_NODE* u = uiter.First(); !uiter.Is_Empty(); u=uiter.Next()){
    WN* use=u->Wn();
    if (Wn_Is_Inside(use, loop)) {
      WN* stmt1 = Find_Stmt_Under(use, body);
        if (reduc_mgr->Which_Reduction(stmt1) == RED_NONE ||
            (WN_operator(stmt1) == OPR_STID &&
             (WN_st(stmt1) != WN_st(stmt) ||
              WN_store_offset(stmt1) != WN_store_offset(stmt)))){
          return FALSE;
         }
      }
   }
 DEF_LIST *def_list = Du_Mgr->Ud_Get_Def(WN_kid(kid0, 1-which_kid));
 if (!def_list) return FALSE;
 DEF_LIST_ITER diter(def_list);
 for (DU_NODE* d = diter.First(); !diter.Is_Empty(); d=diter.Next()){
 WN* def=d->Wn();
 if (Wn_Is_Inside(def, loop)){
  WN* stmt1 = Find_Stmt_Under(def, body);
  if (reduc_mgr->Which_Reduction(stmt1) == RED_NONE ||
            (WN_operator(stmt1) == OPR_STID &&
             (WN_st(stmt1) != WN_st(stmt) ||
              WN_store_offset(stmt1) != WN_store_offset(stmt)))){
          return FALSE;
         }
      }
  }
 return TRUE;
}


//-----------------------------------------------------------------------------
// the cool part, we actually do thing here
//-----------------------------------------------------------------------------
static void  Handle_Stmt(STACK_OF_WN *group, STACK_OF_WN *common_invar, WN *loop)
{
 WN *first = group->Bottom_nth(0);
 TYPE_ID type = WN_rtype(WN_kid1(WN_kid0(first)));
 WN_OFFSET preg_num=0;
 ST *preg_st=0;
 char preg_name[20];
 preg_st = MTYPE_To_PREG(type);
 sprintf(preg_name,"invar_fact%d",local_num++);
 preg_num = Create_Preg(type,preg_name);
 OPCODE preg_s_opcode = OPCODE_make_op(OPR_STID,MTYPE_V,type);
 OPCODE preg_l_opcode = OPCODE_make_op(OPR_LDID,type,type);
 OPCODE add_opc= OPCODE_make_op(OPR_ADD,type, MTYPE_V);
 OPCODE sub_opc= OPCODE_make_op(OPR_SUB,type, MTYPE_V);
 WN *preg_store;
 WN *preg_load;
 for(INT i=0; i<group->Elements(); i++){
  WN *stmt = group->Bottom_nth(i);
  if(i==0){//first stmt, we need to generate t = 0, and insert before the loop
  TCON tcon;
  ST* st;
  tcon = Host_To_Targ_Float(type, 0.0);
  st = New_Const_Sym (Enter_tcon(tcon), Be_Type_Tbl(type));
  WN *const_wn = WN_CreateConst(OPR_CONST, type, MTYPE_V, st);
  preg_store = LWN_CreateStid(preg_s_opcode,preg_num,
                     preg_st, Be_Type_Tbl(type),const_wn);
  LWN_Insert_Block_Before(LWN_Get_Parent(loop),loop,preg_store); //t = 0
  }
 
  WN *terms = WN_kid1(WN_kid0(stmt));
  
  Split_Var_Tree(terms, loop, common_invar);
  WN *var_tree = WN_Simplify_Tree(terms);
  //only the last one need to build an invar tree
  WN *preg_load = WN_CreateLdid(preg_l_opcode,preg_num, preg_st, Be_Type_Tbl(type));
  Du_Mgr->Add_Def_Use(preg_store,preg_load);
  WN *add = LWN_CreateExp2(WN_operator(WN_kid0(stmt))==OPR_ADD?add_opc:sub_opc,preg_load,var_tree);
  preg_store = LWN_CreateStid(preg_s_opcode,preg_num, //t = t + var
                     preg_st, Be_Type_Tbl(type),add);
  LWN_Insert_Block_Before(LWN_Get_Parent(stmt),stmt,preg_store);
  if(i==group->Elements()-1){
   WN *invar_tree = Build_Invar_Tree(common_invar);
   WN *preg_load = WN_CreateLdid(preg_l_opcode,preg_num, preg_st, Be_Type_Tbl(type));
   Du_Mgr->Add_Def_Use(preg_store,preg_load);
   OPCODE mpy_opc = OPCODE_make_op(OPR_MPY,WN_rtype(invar_tree),MTYPE_V);
   invar_tree = LWN_CreateExp2(mpy_opc, invar_tree, preg_load);  
   WN_kid1(WN_kid0(stmt))=invar_tree;
   LWN_Set_Parent(invar_tree, WN_kid0(stmt));
   LWN_Parentize(WN_kid0(stmt));
   LWN_Extract_From_Block(LWN_Get_Parent(stmt), stmt);
   LWN_Insert_Block_After(LWN_Get_Parent(loop),loop,stmt);// x = x + t *invar
  }else{
    WN_kid1(WN_kid0(stmt))=NULL;
    LWN_Parentize(WN_kid0(stmt));
    LWN_Extract_From_Block(LWN_Get_Parent(stmt), stmt);
    LWN_Delete_Tree(stmt);
   }
  }
}

//let's assume it can
static BOOL Factorize_Statement(STACK_OF_WN *current_stmt_group, WN *loop)
{
 //get_reduction_invariant of the first statement
 STACK_OF_WN *common_invar; 
 for(INT ii=0; ii<current_stmt_group->Elements(); ii++){
    WN * stmt=current_stmt_group->Bottom_nth(ii);
    STACK_OF_WN *tmp_invar = CXX_NEW(STACK_OF_WN(&FACT_default_pool),
                                     &FACT_default_pool);
    if(!Gather_Stmt_Common_Invar(stmt, tmp_invar, loop))
      return FALSE;
    if(tmp_invar->Elements()==0) 
        return FALSE; //no invar in this statement
    if(ii == 0) //the first
        common_invar = tmp_invar;
    else
        common_invar = Invar_Stack_Intersection(common_invar, tmp_invar);
    if(common_invar->Elements()==0) 
        return FALSE; //no common invar
 }
if (LNO_Invar_Factor_Verbose || LNO_Lno_Verbose){
     printf("  %d statements with %d common_invariants have been factorized.\n", 
           current_stmt_group->Elements(),
           common_invar->Elements());
}
 Handle_Stmt(current_stmt_group, common_invar, loop);
 return TRUE;
}

static BOOL Under_Same_Level(WN_OFFSET offset, ST *st, 
                             WN *loop, WN* body)
{
  for (LWN_ITER* itr = LWN_WALK_TreeIter(loop);
       itr;
       itr = LWN_WALK_TreeNext(itr)){
      WN* wn = itr->wn;
     if (WN_operator(wn) == OPR_STID &&
         WN_st(wn) == st             &&
         WN_store_offset(wn)==offset){
         if(LWN_Get_Parent(wn) != body)
          return FALSE;
     }
   }
  return TRUE;
}

static BOOL Already_Passed_Stid(WN *stid, STACK_OF_WN *stmt_stack)
{ 
  for(INT ii=0; ii<stmt_stack->Elements(); ii++){
     WN *wn = stmt_stack->Top_nth(ii);
     if(WN_st(stid) == WN_st(wn) &&
        WN_store_offset(stid) == WN_store_offset(wn))
        return TRUE;
  }
 return FALSE;
}

//need to determine which loops are good candidates
static BOOL Factorize_Loop(WN *loop)
{
 if(WN_operator(loop) != OPR_DO_LOOP  ||
           Do_Loop_Has_Calls(loop)    ||
           Do_Loop_Has_Exits(loop)    ||
           Do_Loop_Has_Gotos(loop)    ||
           Do_Loop_Is_Mp(loop)        ||
           Is_Nested_Doacross(loop)   ||
           !Do_Loop_Is_Good(loop))
   return FALSE;
  //what about a statement is good, or bad
 
 BOOL has_factorization = FALSE;
 WN *body = WN_do_body(loop);
 WN *next=WN_first(body);
 WN *stmt;
 if(current_level >= STOP_LEVEL){  
  //  DO_LOOP_INFO *dli = Get_Do_Loop_Info(loop);
  //  if(dli && dli->Delay_Full_Unroll && LNO_Full_Unrolling_Limit != 0)
  //     Fully_Unroll_Short_Loops(loop);
    return FALSE;
 }
 //for a stmt
 STACK_OF_WN *stid_stmt_list = CXX_NEW
                 (STACK_OF_WN(&FACT_default_pool),&FACT_default_pool);
 //first pass -- collecting stids
 for(stmt=WN_first(body); stmt; stmt=WN_next(stmt)){
  if(WN_operator(stmt) == OPR_STID){
   if(!Already_Passed_Stid(stmt, stid_stmt_list))
     stid_stmt_list->Push(stmt);
  }
 }

 //second pass, whether this stid is good, if yes, then factorize it 
 for(INT ii=0; ii<stid_stmt_list->Elements(); ii++){
      WN *orig_stmt = stid_stmt_list->Bottom_nth(ii);    
      WN_OFFSET offset = WN_store_offset(orig_stmt);
      ST *st = WN_st(orig_stmt);
      if(Under_Same_Level(offset, st, loop, body)){
      STACK_OF_WN *current_stmt_group=CXX_NEW(STACK_OF_WN(&FACT_default_pool),&FACT_default_pool);
      BOOL everything_good = TRUE;
     for(stmt=WN_first(body); stmt; stmt=WN_next(stmt)){
      if(WN_operator(stmt)==OPR_STID &&
         WN_store_offset(stmt)==offset &&
         WN_st(stmt)==st) {
         if(Well_Formed_Reduction_Stmt(stmt, loop))
           current_stmt_group->Push(stmt);
         else{
            everything_good = FALSE;
            break;
         }
        }
      }
      if(everything_good){
         BOOL factorized = Factorize_Statement(current_stmt_group,loop);
         if(factorized)
           has_factorization = TRUE;
      }
    }
  }
     
 if(has_factorization){//try next level of loop
   current_level++;
   WN *parent = LWN_Get_Parent(loop);
   WN *outer_loop = Enclosing_Do_Loop(parent);
   if(outer_loop)
     Factorize_Loop(outer_loop);
 }
 return has_factorization;
}

static INT Factors_In_Mult_Tree(WN *mpy)
{
 if(WN_operator(mpy) != OPR_MPY)
  return 0;
 return (Factors_In_Mult_Tree(WN_kid0(mpy)) + 
         Factors_In_Mult_Tree(WN_kid1(mpy)) + 1);  
}

static INT Factors_In_Terms(WN *terms)
{
  if(WN_operator(terms)==OPR_ADD){
   INT left = Factors_In_Terms(WN_kid0(terms));
   INT right = Factors_In_Terms(WN_kid1(terms));
   return (left > right ? right : left);
  }else if(WN_operator(terms)==OPR_MPY)
    return Factors_In_Mult_Tree(terms);
 return 0;
}

//remember that we do not know any reduction now, so only an extimation now
static INT Statement_Invariant_Factors(WN *stmt, WN *innerloop)
{
  if(WN_operator(stmt)!=OPR_STID && WN_operator(stmt)!=OPR_ISTORE)
    return 0;  
  WN *terms;
  WN *kid0 = WN_kid0(stmt);
  if(WN_operator(kid0) != OPR_ADD)
    return 0;
  if(WN_operator(stmt)==OPR_STID){ 
    if(WN_operator(WN_kid0(kid0))==OPR_LDID                   && 
    //   SYMBOL(stmt)==SYMBOL(WN_kid0(kid0))                    &&
       WN_st(stmt)==WN_st(WN_kid0(kid0))                      &&
       WN_store_offset(stmt) == WN_load_offset(WN_kid0(kid0)) &&
       WN_field_id(stmt) == WN_field_id(WN_kid0(kid0)))
      terms = WN_kid1(kid0);
    else if(WN_operator(WN_kid1(kid0))==OPR_LDID              &&
    //   SYMBOL(stmt)==SYMBOL(WN_kid1(kid0))                    &&
       WN_st(stmt)==WN_st(WN_kid1(kid0))                      &&
       WN_store_offset(stmt) == WN_load_offset(WN_kid1(kid0)) &&
       WN_field_id(stmt) == WN_field_id(WN_kid1(kid0)))
      terms = WN_kid0(kid0);
    else return 0;
  }else if(WN_operator(stmt)==OPR_ISTORE){
   WN *array_to = WN_kid1(stmt);
   if(WN_operator(array_to) != OPR_ARRAY)
     return 0;
    WN *array_from;
    if(WN_operator(WN_kid0(kid0))==OPR_ILOAD &&
       WN_operator(WN_kid0(WN_kid0(kid0))) == OPR_ARRAY){
       array_from = WN_kid0(WN_kid0(kid0));
       terms = WN_kid1(kid0);
     }else if(WN_operator(WN_kid1(kid0))==OPR_ILOAD &&
       WN_operator(WN_kid0(WN_kid1(kid0))) == OPR_ARRAY){
       array_from = WN_kid0(WN_kid1(kid0));
       terms = WN_kid0(kid0);
     }else return 0;
     if(!Tree_Equiv(array_from, array_to))
      return 0;
  }//end istore
 if(!Well_Formed_Terms(terms))
  return 0;
 return Factors_In_Terms(terms);
}

//whether the loop is possible to be factorized
static BOOL Invar_Factor_Candidate(WN *innerloop)
{   
  INT invar_factors = 0;
  INT stmt_invar_factors;
  WN *body = WN_do_body(innerloop);
  for(WN *stmt=WN_first(body); stmt; stmt=WN_next(stmt)){
      stmt_invar_factors = Statement_Invariant_Factors(stmt, innerloop);
      if(stmt_invar_factors > invar_factors)
       invar_factors = stmt_invar_factors;
  }
 return (invar_factors >= GOOD_FACTORS);
}

extern BOOL Is_Invariant_Factorization_Beneficial(WN *loop){

  if(!LNO_Invariant_Factorization     || //do not do factorization
     !Enable_Cfold_Reassociate ||  //reassociation is not allowed
     Get_Trace(TP_LNOPT, TT_LNO_GUARD))  //loop is not guarded
    return FALSE;

  //collecting inner do loops (inner does are important)
  STACK_OF_WN *innerdo_stack = CXX_NEW
          (STACK_OF_WN(&LNO_default_pool),&LNO_default_pool);
 for (LWN_ITER* itr = LWN_WALK_TreeIter(WN_do_body(loop));
       itr;
       itr = LWN_WALK_TreeNext(itr)){
     WN* wn = itr->wn;
     if (WN_operator(wn) == OPR_DO_LOOP){
       DO_LOOP_INFO *dli = Get_Do_Loop_Info(wn);
     if(dli && dli->Is_Inner)
       innerdo_stack->Push(wn);
     }
  }
  for(INT ii=0; ii<innerdo_stack->Elements(); ii++){
     WN *innerloop = innerdo_stack->Bottom_nth(ii);
     if(Invar_Factor_Candidate(innerloop))
      return TRUE;
  }
 return FALSE;
}

static BOOL pool_initialized = FALSE;

extern void Invariant_Factorization(WN *func_nd)
{ 
  if(!pool_initialized){ 
   MEM_POOL_Initialize(&FACT_default_pool,"FACT_default_pool",FALSE);
   MEM_POOL_Push(&FACT_default_pool);
   pool_initialized = TRUE;
  }
  reduc_mgr = CXX_NEW
          (REDUCTION_MANAGER(&FACT_default_pool), &FACT_default_pool);
  reduc_mgr->Build(func_nd,TRUE,FALSE); //build scalar reductions

  INT factorized_loops = 0;
  //collecting inner do loops
  STACK_OF_WN *inner_do_stack = CXX_NEW
          (STACK_OF_WN(&FACT_default_pool),&FACT_default_pool);
  STACK_OF_WN *delayed_unroll_stack = CXX_NEW
          (STACK_OF_WN(&FACT_default_pool),&FACT_default_pool);

 for (LWN_ITER* itr = LWN_WALK_TreeIter(func_nd);
       itr;
       itr = LWN_WALK_TreeNext(itr)){
     WN* wn = itr->wn;
     if (WN_operator(wn) == OPR_DO_LOOP){
       DO_LOOP_INFO *dli = Get_Do_Loop_Info(wn);
     if(dli && dli->Is_Inner)
       inner_do_stack->Push(wn);
     if(dli && dli->Delay_Full_Unroll && LNO_Full_Unrolling_Limit != 0)
       delayed_unroll_stack->Push(wn);
     }
  }

 for(INT ii=0; ii<inner_do_stack->Elements(); ii++){
   WN *loop = inner_do_stack->Bottom_nth(ii);
   current_level=0;
   if(Factorize_Loop(loop)){    
      if (LNO_Invar_Factor_Verbose || LNO_Lno_Verbose){
        printf("(%s:%d) Loop invariants were factorized.\n", Src_File_Name,
          Srcpos_To_Line(WN_Get_Linenum(loop)));
      }
     factorized_loops++;
    }
  }

   unsigned saved_limit = LNO_Full_Unrolling_Loop_Size_Limit;
   if(LNO_Full_Unroll_Aggressive)
     LNO_Full_Unrolling_Loop_Size_Limit = 14000; //bug 10644
  for(int ii=0; ii<delayed_unroll_stack->Elements(); ii++){
    WN *loop = delayed_unroll_stack->Bottom_nth(ii);
    Fully_Unroll_Short_Loops(func_nd);
   }
 LNO_Full_Unrolling_Loop_Size_Limit = saved_limit;

  //MEM_POOL_Pop(&FACT_default_pool);
  //MEM_POOL_Delete(&FACT_default_pool);  //memory to be cleaned
}
#endif


#ifdef TARG_MIPS
static BOOL Innermost_Array(WN *array)
{
for (LWN_ITER* itr = LWN_WALK_TreeIter(array);
       itr;
       itr = LWN_WALK_TreeNext(itr)){
       WN* wn = itr->wn;
       if(WN_operator(wn)==OPR_ARRAY && wn != array)
        return FALSE;
    }
  return TRUE;
}

//for the array, all ldids should be loop invariants except index
static INT Well_Formed_Array_Coefficient(WN *array, WN *loop)
{
 if(!Innermost_Array(array))
    return 0;

 if(WN_element_size(array)<0)
    return 0; //not sure whether contiguous

 if(WN_num_dim(array) != 1)
    return 0; //consider only one dimention array now

 ACCESS_ARRAY* aa = (ACCESS_ARRAY*)WN_MAP_Get(LNO_Info_Map,array);
 if(!aa || aa->Too_Messy)
    return 0;
 WN *index = WN_index(loop); //index
 INT loopno = Do_Loop_Depth(loop); //depth of the innermost
 INT index_stride;
 ACCESS_VECTOR* av = aa->Dim(aa->Num_Vec()-1);
 if(!av || av->Too_Messy || av->Loop_Coeff(loopno)==0)
  return 0;
 else index_stride = av->Loop_Coeff(loopno);
 for (LWN_ITER* itr = LWN_WALK_TreeIter(array);
       itr;
       itr = LWN_WALK_TreeNext(itr)){
     WN* wn = itr->wn;
     if(WN_operator(wn)==OPR_LDID &&
        SYMBOL(index) !=SYMBOL(wn) &&
        !Is_Loop_Invariant_Exp(wn, loop))
      return 0;
 }
 return index_stride;
}


void Gather_Array_References(WN * wn, DYN_ARRAY<WN *>& array_list,
                             DYN_ARRAY<INT>& coef_tracker)
{
  if(WN_operator(wn)==OPR_ILOAD &&
     (WN_desc(wn)==MTYPE_F4 || WN_desc(wn)==MTYPE_F8)){
    WN *array = WN_kid0(wn);
    INT coef = Well_Formed_Array_Coefficient(array, Enclosing_Do_Loop(array));
    if(coef !=0){
         array_list.AddElement(array);
         coef_tracker.AddElement(coef);
   }
  }else if(WN_operator(wn)==OPR_ISTORE &&
     (WN_desc(wn)==MTYPE_F4 || WN_desc(wn)==MTYPE_F8)){
    WN *array = WN_kid1(wn);
    INT coef = Well_Formed_Array_Coefficient(array, Enclosing_Do_Loop(array));
    if(coef != 0){
       array_list.AddElement(array);
       coef_tracker.AddElement(coef);
    }
  }
 else if(WN_operator(wn)==OPR_BLOCK){
   for(WN *stmt = WN_first(wn); stmt; stmt=WN_next(stmt))
       Gather_Array_References(stmt, array_list, coef_tracker);
     return;
  }

 for(INT kid =0; kid <WN_kid_count(wn); kid++)
     Gather_Array_References(WN_kid(wn, kid), array_list, coef_tracker);
}

WN *Most_Frequent_Array_Base( DYN_ARRAY<WN *>& array_list)
{
 DYN_ARRAY<WN *> array_base(&LNO_local_pool);
 DYN_ARRAY<INT>  base_count(&LNO_local_pool);
 for(INT i = 0; i < array_list.Elements(); i++){
  WN *base = WN_array_base(array_list[i]);
  BOOL exist = FALSE;
  for(INT j=0; j <array_base.Elements(); j++){
   if(Tree_Equiv(base, array_base[j])){
     base_count[j]++;
     exist = TRUE;
     break;
    }
   }
   if(!exist){
     array_base.AddElement(base);
     base_count.AddElement(1);
   }
 }
 INT index = 0;
 INT max = base_count[0];
 for(INT i=1; i<base_count.Elements(); i++){
   if(base_count[i] > max){
     max = base_count[i];
     index = i;
   }
 }
 return array_base[index];
}


static void Transform_Array_List( DYN_ARRAY<WN *>& array_list)
{
 WN *base = Most_Frequent_Array_Base(array_list);
  for (INT i = 0; i < array_list.Elements(); i++){
   WN * array = array_list[i];
   if(!Tree_Equiv(base, WN_array_base(array))){
     WN *base_copy = LWN_Copy_Tree(base,TRUE, LNO_Info_Map);
     WN *second_copy = LWN_Copy_Tree(base,TRUE, LNO_Info_Map);
     LWN_Copy_Def_Use(base, base_copy, Du_Mgr);
     LWN_Copy_Def_Use(base, second_copy, Du_Mgr);
     Create_alias(Alias_Mgr,base_copy);
     Create_alias(Alias_Mgr,second_copy);
     WN *my_base = WN_array_base(array);
     WN_array_base(array) = base_copy;
     LWN_Set_Parent(base_copy, array);
     TYPE_ID right_type = WN_rtype(my_base);
     if (MTYPE_is_unsigned(right_type))
      right_type = MTYPE_complement(right_type);

     OPCODE sub_opc = OPCODE_make_op(OPR_SUB, right_type, MTYPE_V);
     WN *right = LWN_CreateExp2(sub_opc, my_base, second_copy);
     WN *left = array;
     WN *parent = LWN_Get_Parent(array);
     WN_OFFSET offset=WN_offset(parent);
     if(offset != 0){
     OPCODE intconst_opc= OPCODE_make_op(OPR_INTCONST, right_type, MTYPE_V);
     OPCODE add_opc = OPCODE_make_op(OPR_ADD, right_type, MTYPE_V);
     right = LWN_CreateExp2(add_opc, right, WN_CreateIntconst(intconst_opc, offset));
     }

     WN *iloadx_istorex=NULL;

     if(WN_operator(parent)==OPR_ILOAD)
        iloadx_istorex= WN_CreateIloadx(OPR_ILOADX, WN_rtype(parent), WN_desc(parent),
                                         WN_ty(parent),WN_load_addr_ty(parent),left, right);
     else if(WN_operator(parent)==OPR_ISTORE)
        iloadx_istorex= WN_CreateIstorex(OPR_ISTOREX, MTYPE_V, WN_desc(parent), WN_ty(parent),
                                         WN_kid0(parent), left, right);

      WN *g_parent = LWN_Get_Parent(parent);
      if(WN_operator(g_parent)==OPR_BLOCK){
       LWN_Insert_Block_After(g_parent, parent, iloadx_istorex);
       LWN_Extract_From_Block(g_parent, parent);
      }else{
      INT who;
      for(who=0; who < WN_kid_count(g_parent); who++)
       if(WN_kid(g_parent, who)==parent) break;
      WN_kid(g_parent, who) = iloadx_istorex;
      LWN_Set_Parent(iloadx_istorex, g_parent);
     }
      LWN_Parentize(g_parent);
      for(INT i=0; i<WN_kid_count(parent); i++)
          WN_kid(parent, i) = NULL;
      WN_Delete(parent);
    
   }//end if
  }//end for
}

static void Transform_Loop(WN *loop)
{
  DYN_ARRAY<WN *> array_list(&LNO_local_pool);
  DYN_ARRAY<INT > coef_tracker(&LNO_local_pool);
  Gather_Array_References(WN_do_body(loop), array_list, coef_tracker);

  for(INT i=0; i<coef_tracker.Elements(); i++){
   if(coef_tracker[i] != 0){
     DYN_ARRAY<WN *> current_array(&LNO_local_pool);
     INT coef = coef_tracker[i];
     for(INT j=i; j< coef_tracker.Elements(); j++){
         if(coef_tracker[j]==coef){
            coef_tracker[j] = 0;
            current_array.AddElement(array_list[j]);
         }
       }
     Transform_Array_List(current_array);
   }
  }
}

static void Iloadx_Walk(WN* wn){
  OPCODE opc=WN_opcode(wn);

  if (!OPCODE_is_scf(opc))
    return;
  else if (opc==OPC_DO_LOOP) {
    if (Do_Loop_Is_Inner(wn))
      Transform_Loop(wn);
    else
     Iloadx_Walk(WN_do_body(wn));
  } else if (opc==OPC_BLOCK)
    for (WN* stmt=WN_first(wn); stmt; stmt=WN_next(stmt)){
      Iloadx_Walk(stmt);
    }
  else
    for (UINT kidno=0; kidno<WN_kid_count(wn); kidno++) {
      Iloadx_Walk(WN_kid(wn,kidno));
    }
}

extern void Iload2Iloadx(WN *func_nd)
{
  Iloadx_Walk(func_nd);
}

#endif


