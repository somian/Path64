/*
 * Copyright (C) 2008 PathScale, LLC. All Rights Reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it would be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  
 *
 * Further, this software is distributed without any warranty that it is
 * free of the rightful claim of any third person regarding infringement 
 * or the like.  Any license provided herein, whether implied or 
 * otherwise, applies only to this software file.  Patent licenses, if 
 * any, provided herein do not apply to combinations of this program with 
 * other software, or any other product whatsoever.  
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write the Free Software Foundation, Inc., 59
 * Temple Place - Suite 330, Boston MA 02111-1307, USA.
 */

// Description:
//
// This file implements a phase that performs reassociation on an n-ary
// expression for the purpose of maximizing CSEs opportunities, by taking into 
// account all the expressions that have occurred in the scope.  I believe the
// this problem is of some exponential complexity. The current implementation 
// uses a cheap algorithm that does not solve the problem optimally.
//
// As an example, given these expressions that occur in the give scope:
//
// ((a*b)*a)*c
//
// (a*c)*b
//
// a*b
//
// a*c
//
// we want to reassociate the expressions so as to maximize CSE opportunities.
// Only * and + are supported currently.  a, b and c can each be expressions 
// themselves.
//
// To allow the recognition of identical expressions, value numbering forms
// the basis of the algorithm.  
//
// Reassociation is separated into 3 passes:
//
// Pass 1 value number all the expressions in the scope.  WNREP is a node in
// the hash table, each with a unique ID that represents the value number.
// Associative expressions with more than 2 contiguous operands are treated
// specially. One node is used to represent such n-ary associative expressions
// with n operands.  E.g. (a*b)*(c*d) and a*c*b*d are both 
// represented by a WNREP that represents * on [a b c d].  Such a WNREP node
// is called type 2, to distinguish them from the regular type 1.  The
// goal is to find the best reasssociation of each type 2 node.
//
// Pass 1 also builds _reassoc_cands[][], which is a two-dimensional array of
// all the type 2 nodes in the scope.  The high order index is the number of 
// operands (or rank).  The low order array gives all the type 2 nodes with 
// that number of operands.
//
// Pass 2 determines the best associative form for each type 2 node.
// This implementation maximizes CSE opportunities by working on the
// associative expressions starting with those with smallest number of operands.
// When the best associated form is arrived at, the expressions will be 
// value-numbered into type 1 nodes, which will then play their roles in 
// affecting the reassociation of expressions of higher order ranks.  
// E.g. in processing [a b c] in our example, it finds that there are 
// occurrences of (a*b) and (a*c), and it will put [a b c] into either 
// (a*b)*c or (a*c)*b, which are as good as each other.  Suppose it uses 
// (a*b)*c.  Later on, when it processes [a a b c], it will find that (a*b)*c 
// occurs, so it will put [a a b c] into ((a*b)*c)*a.
//
// If it finds that there is no pre-existing subexpressions, pass 2 will find
// CSE by looking at expressions of the same rank that are not yet processed.
// When CSEs are found, it will create instances of them.  E.g. in the presence 
// of (a+b)+c and (b+d)+c, but the absence of (b+c), this part will introduce
// (b+c).  The presence of (b+c) will cause the two expressions to be later
// reassociated to (b+c)+a and (b+c)+d respectively.
//
// In working on each type 2 node, it first finds all the possible largest first
// order CSEs in the expression based on what has occurred so far in the scope,
// by calling Find_1st_order_max_cses().  The final form will be made out of
// multiple such CSEs that do not overlap with each other.  This is done by
// calling Build_final_form(), which first finds the largest CSE among the 
// first-order ones. Then it iterates to try to add more first-order CSEs to
// what has been formed so far.  Note that this algorithm is not optimal.
// E.g. in [a b c d] in the presence of (a*b), (c*d) and (a*c), it may
// arbitrarily pick (a*c) first, so the final form becomes ((a*c)*b)*d.  If it
// picks (a*b), then it can also pick (c*d), thus forming (a*b)*(c*d) which
// is better.
//
// Pass 3 transforms the program so all the type 2 nodes will be put in the
// form determined by Pass 2.

#ifdef USE_PCH
#include "be_com_pch.h"
#endif /* USE_PCH */
#pragma hdrstop

#include "defs.h"
#include "config.h"
#include "errors.h"
#include "erglob.h"
#include "tracing.h"
#include "glob.h"
#include "timing.h"
#include "wn.h"
#include "wn_util.h"
#include "wn_map.h"
#include "mempool.h"
#include "cxx_memory.h"
#include "cxx_base.h"
#include "cxx_template.h"
#include "opt_config.h"
#include "wn_lower.h"
#include "ir_reader.h"
#include "opt_defs.h"
#include "idx_32_set.h"

static BOOL debug_reassoc = FALSE;

class REASSOC;

// there are two types of WNREP nodes: type 1 is for WHIRL nodes in their 
// ordinary form; type 2 is for WHIRL nodes that are reassociation candidates,
// which make up the problems for this phase.  Type 2 is distinguished by
// _assoc_opnds being non-NULL.  At the end of Pass 2, type 2 nodes will be
// additionally represented by type 1 nodes pointed to by _final_form.
class WNREP : public SLIST_NODE {
friend class WNREP_CONTAINER;
friend class REASSOC;
private:
  WN *_wn;
  DYN_ARRAY<WNREP *> *_assoc_opnds; //the operands involved in this 
  				 // reassociation, sorted in ascending value 
				 // number; NULL if not reassociative or type 1
  UINT32 _wnrep_id;	// the value number
  union {
    INT _rank;		// number of operands in a type 1 reassociated expr
    WNREP *_final_form; // for type 2 nodes, give the final re-associated form 
    			// consisting of stictly type 1 nodes (result of pass 2)
  } _u1;

  BOOL Match(REASSOC *reassoc, WN *wn);

public:
  WN *Wn(void) { return _wn; }
  void Set_wn(WN *wn) { _wn = wn; }
  UINT32 Wnrep_id(void) { return _wnrep_id; }
  void Set_wnrep_id(UINT32 i) { _wnrep_id = i; }
  INT Rank(void) { return _u1._rank; }
  WNREP *Final_form(void) { return _u1._final_form; }

  WNREP(void) { _assoc_opnds = NULL; 
		_u1._final_form = NULL; }
  ~WNREP(void) {}
};

class WNREP_CONTAINER : public SLIST {
private:
  DECLARE_SLIST_CLASS(WNREP_CONTAINER, WNREP)

public:
  ~WNREP_CONTAINER(void) {}
  WNREP *Find_wnrep(REASSOC *reassoc, WN *wn);
  WNREP *Find_assoc_wnrep(WNREP *wr);
  WNREP *Find_binary_assoc_wnrep(REASSOC *reassoc, OPCODE opc, WNREP *l, WNREP *r);
};

class WNREP_ITER : public SLIST_ITER {
  DECLARE_SLIST_ITER_CLASS(WNREP_ITER, WNREP, WNREP_CONTAINER)
};

typedef DYN_ARRAY<WNREP *> *WNREP_DYN_ARRAY;

// class REASSOC manages the entire operation
class REASSOC {
private:
  MEM_POOL _mem_pool;
  WN_MAP _vn_map;	// to map a whirl node to its value number
  WNREP **_hash_vec;
  UINT32 _wnrep_id_cnt;
  DYN_ARRAY <WNREP *> *_wnrep_id_to_wnrep; // to get from the ID to the wnrep
  DYN_ARRAY<WNREP_DYN_ARRAY> *_reassoc_cands; // the high order dyn_array has
  		// the number of operands as its index value; the low order
		// dyn_array contains all the associative wnreps with that
		// number of operands
  DYN_ARRAY<WNREP *> *_max_cses; // work area to record all the CSEs of the
  				// current assoc expr being worked on in pass 2
  IDX_32_SET *_used_opnds; // set of operands in the _assoc_opnds of the type 2
			   // WNREP that have been used as Build_final_form()
			   // progresses
  IDX_32_SET *_counted_opnds; // set of operands in the _assoc_opnds of the 
  			   // type 2 WNREP that have been counted; used only
			   // for implementing Some_opnd_in_used()
  WN_MAP _emitted_map;// in pass 3, to record that a whirl node has been emitted

  UINT32 Next_wnrep_id(void) { return _wnrep_id_cnt++; }

  UINT32 Hash_expr(WN *wn);
  WNREP *Value_number_expr(WN *tree);
  WNREP *Find_or_append_expr(UINT32 hash_idx, WN *wn);
  WNREP *Find_binary_assoc_expr(UINT32 hash_idx, OPCODE opc, WNREP *l, WNREP *r);

  UINT32 Hash_assoc_expr(WNREP *wr);
  void Add_assoc_opnd_to_wnrep(WNREP *wr, WNREP *wr0);
  void Gather_assoc_opnds(OPCODE opc, WN *tree, WNREP *wr);
  WNREP *Find_or_append_assoc_expr(UINT32 hash_idx, WNREP *wr);

  void Enter_reassoc_cand(WNREP *assoc_wr);
  void Find_larger_cses(WNREP *wr, WNREP *x);
  void Set_used_opnds_in_wnrep(WN *tree, WNREP *wr);
  BOOL Some_opnd_in_used(WN *tree, WNREP *wr);
  void Build_into_final_form(WNREP *subwr, WNREP *wr);
  WN *Retrieve_wn_via_wnrep_id(WN *tree, OPCODE opc, UINT32 wr_id);
  WN *Reassociate_to_final_form(WNREP *wr, WN *tree);
  WN *Reassociate_expr(WN *tree);

public:
  REASSOC(void);
  ~REASSOC(void);

  WN_MAP Vn_map(void) { return _vn_map; }
  DYN_ARRAY<WNREP_DYN_ARRAY> *Reassoc_cands(void) { return _reassoc_cands; }
  DYN_ARRAY<WNREP *> *Max_cses(void) { return _max_cses; }

  UINT32 Wnrep_id(WN *wn) { return WN_MAP32_Get(_vn_map, wn); }
  WN_MAP Emitted_map(void) { return _emitted_map; }

  void Value_number_stmt(WN *tree);
  void Find_1st_order_max_cses(WNREP *wr);
  void Build_final_form(WNREP *wr);
  WNREP *Find_and_create_cse(WNREP *wr1, WNREP *wr2);
  void Reassociate_stmt(WN *tree);
};

// see if wn matches this WNREP node
BOOL
WNREP::Match(REASSOC *reassoc, WN *wn)
{
  if (WN_opcode(_wn) != WN_opcode(wn))
    return FALSE;
  OPERATOR opr = WN_operator(wn);
  switch (opr) {
    // leaves
    case OPR_INTCONST: 
      return WN_const_val(_wn) == WN_const_val(wn);
    case OPR_CONST:
      return WN_st_idx(_wn) == WN_st_idx(wn);
    case OPR_LDA:
      return WN_st_idx(_wn) == WN_st_idx(wn) && WN_offset(_wn) == WN_offset(wn);
    case OPR_LDA_LABEL:
      return WN_label_number(_wn) == WN_label_number(wn);
    case OPR_LDID:
      return WN_st_idx(_wn) == WN_st_idx(wn) && 
      	     WN_offset(_wn) == WN_offset(wn) &&
      	     (WN_desc(wn) != MTYPE_BS || WN_field_id(_wn) == WN_field_id(wn));
    case OPR_LDBITS:
      return WN_st_idx(_wn) == WN_st_idx(wn) && 
      	     WN_offset(_wn) == WN_offset(wn) &&
      	     WN_bit_offset(_wn) == WN_bit_offset(wn) &&
      	     WN_bit_size(_wn) == WN_bit_size(wn);
    default: ;
  }

  if (OPCODE_commutative_op(WN_opcode(wn)) == WN_opcode(wn))
    return reassoc->Wnrep_id(WN_kid0(_wn)) == reassoc->Wnrep_id(WN_kid0(wn)) &&
           reassoc->Wnrep_id(WN_kid1(_wn)) == reassoc->Wnrep_id(WN_kid1(wn)) ||
           reassoc->Wnrep_id(WN_kid0(_wn)) == reassoc->Wnrep_id(WN_kid1(wn)) &&
           reassoc->Wnrep_id(WN_kid1(_wn)) == reassoc->Wnrep_id(WN_kid0(wn));

  for (INT i = 0; i < WN_kid_count(wn); i++)
    if (reassoc->Wnrep_id(WN_kid(_wn, i)) != reassoc->Wnrep_id(WN_kid(wn, i)))
      return FALSE;

  // check additional fields for some operators
  if (OPERATOR_has_bits(opr))
    if (WN_cvtl_bits(_wn) != WN_cvtl_bits(wn))
      return FALSE;

  if (OPERATOR_has_offset(opr))
    if (WN_offset(_wn) != WN_offset(wn))
      return FALSE;

  if (OPERATOR_has_ndim(opr))
    if (WN_num_dim(_wn) != WN_num_dim(wn))
      return FALSE;

  if (OPERATOR_has_esize(opr))
    if (WN_element_size(_wn) != WN_element_size(wn))
      return FALSE;

  if (OPERATOR_has_inumber(opr))
    if (WN_intrinsic(_wn) != WN_intrinsic(wn))
      return FALSE;

  return TRUE;
}

WNREP*
WNREP_CONTAINER::Find_wnrep(REASSOC *reassoc, WN *wn)
{
  WNREP *tmp;
  WNREP_ITER wr_iter;
  wr_iter.Init(this);
  for (tmp = wr_iter.First(); !wr_iter.Is_Empty(); tmp = wr_iter.Next()) {
    if (tmp->_assoc_opnds != NULL)
      continue;
    if (tmp->Match(reassoc, wn))
      return tmp;
  }
  return NULL;
}

WNREP*
WNREP_CONTAINER::Find_binary_assoc_wnrep(REASSOC *reassoc, OPCODE opc,
					 WNREP *l, WNREP *r)
{
  WNREP *tmp;
  WNREP_ITER wr_iter;
  wr_iter.Init(this);
  for (tmp = wr_iter.First(); !wr_iter.Is_Empty(); tmp = wr_iter.Next()) {
    if (tmp->_assoc_opnds != NULL)
      continue;
    if (WN_opcode(tmp->_wn) != opc)
      continue;
    if (reassoc->Wnrep_id(WN_kid0(tmp->_wn)) == l->Wnrep_id() &&
        reassoc->Wnrep_id(WN_kid1(tmp->_wn)) == r->Wnrep_id() ||
        reassoc->Wnrep_id(WN_kid0(tmp->_wn)) == r->Wnrep_id() &&
        reassoc->Wnrep_id(WN_kid1(tmp->_wn)) == l->Wnrep_id())
      return tmp;
  }
  return NULL;
}

WNREP*
WNREP_CONTAINER::Find_assoc_wnrep(WNREP *wr)
{
  WNREP *tmp;
  WNREP_ITER wr_iter;
  wr_iter.Init(this);
  for (tmp = wr_iter.First(); !wr_iter.Is_Empty(); tmp = wr_iter.Next()) {
    if (tmp->_assoc_opnds == NULL)
      continue;
    if (WN_operator(tmp->_wn) != WN_operator(wr->_wn))
      continue;
    if (tmp->_assoc_opnds->Elements() != wr->_assoc_opnds->Elements())
      continue;
    BOOL reassoc_opnds_match = TRUE;
    for (INT i = 0; i < tmp->_assoc_opnds->Elements(); i++)
      if ((*tmp->_assoc_opnds)[i]->Wnrep_id() !=
	  (*wr->_assoc_opnds)[i]->Wnrep_id()) {
	reassoc_opnds_match = FALSE;
	break;
      }
    if (reassoc_opnds_match)
      return tmp;
  }
  return NULL;
}

REASSOC::REASSOC(void)
{
  // Create and initialize memory pool
  MEM_POOL_Initialize(&_mem_pool, "Reassoc_Pool", FALSE);
  MEM_POOL_Push(&_mem_pool);
  _vn_map = WN_MAP32_Create(&_mem_pool);
  _emitted_map = WN_MAP32_Create(&_mem_pool);
  _wnrep_id_cnt = 0;

  _hash_vec = CXX_NEW_ARRAY(WNREP*, CODE_HTABLE_SIZE, &_mem_pool);
  _wnrep_id_to_wnrep = CXX_NEW(DYN_ARRAY<WNREP *>(&_mem_pool), &_mem_pool);
  bzero(_hash_vec, sizeof(WNREP *) * CODE_HTABLE_SIZE);
  _reassoc_cands = CXX_NEW(DYN_ARRAY<WNREP_DYN_ARRAY>(&_mem_pool), &_mem_pool);
  _max_cses = CXX_NEW(DYN_ARRAY<WNREP *>(&_mem_pool), &_mem_pool);
  _used_opnds = CXX_NEW(IDX_32_SET(256, &_mem_pool, OPTS_FALSE), &_mem_pool);
  _counted_opnds = CXX_NEW(IDX_32_SET(256, &_mem_pool, OPTS_FALSE), &_mem_pool);
}

REASSOC::~REASSOC(void)
{
  // Clear and dispose of the memory map
  WN_MAP_Delete(_vn_map);
  WN_MAP_Delete(_emitted_map);
  MEM_POOL_Pop(&_mem_pool);
  MEM_POOL_Delete(&_mem_pool);
}

// pass 1 routines

UINT32
REASSOC::Hash_expr(WN *wn)
{
  UINT32 h;
  OPERATOR opr = WN_operator(wn);
  OPCODE opc = WN_opcode(wn);
  switch (opr) {
    // leaves
    case OPR_INTCONST: 
      h = (UINT32) WN_const_val(wn);
      break;
    case OPR_CONST:
      h = WN_st_idx(wn);
      break;
    case OPR_LDA:
      h = WN_st_idx(wn) + WN_offset(wn);
      break;
    case OPR_LDA_LABEL:
      h = WN_label_number(wn) + (UINT32) opc;
      break;
    case OPR_LDID:
      if (WN_desc(wn) != MTYPE_BS)
	h = WN_st_idx(wn) + WN_offset(wn) + (UINT32) opc;
      else h = WN_st_idx(wn) + WN_offset(wn) + WN_field_id(wn) + (UINT32) opc;
      break;
    case OPR_LDBITS:
      h = WN_st_idx(wn) + WN_offset(wn) + WN_bit_offset(wn) + (UINT32) opc;
      break;
    default:
      h = (UINT32) opc;
      for (INT i = 0; i < WN_kid_count(wn); i++)
	h += Wnrep_id(WN_kid(wn, i)) << 4;
      break;
  }
  h = h % CODE_HTABLE_SIZE;
  return h;
}

WNREP *
REASSOC::Find_or_append_expr(UINT32 hash_idx, WN *wn)
{
  WNREP_CONTAINER wnrep_list;
  wnrep_list.Init_Head(_hash_vec[hash_idx]);
  WNREP *wr = wnrep_list.Find_wnrep(this, wn);
  if (wr == NULL)  {
    wr = CXX_NEW(WNREP, &_mem_pool);
    wr->Set_wn(wn);
    wr->Set_wnrep_id(Next_wnrep_id());
    wr->_u1._rank = 1;
    wnrep_list.Prepend(wr);
    _hash_vec[hash_idx] = wr;
    _wnrep_id_to_wnrep->Setidx(wr->Wnrep_id());
    (*_wnrep_id_to_wnrep)[wr->Wnrep_id()] = wr;
  }
  WN_MAP32_Set(_vn_map, wn, wr->Wnrep_id());
  return wr;
}

WNREP *
REASSOC::Find_binary_assoc_expr(UINT32 hash_idx, OPCODE opc, WNREP *l, WNREP *r)
{
  WNREP_CONTAINER wnrep_list;
  wnrep_list.Init_Head(_hash_vec[hash_idx]);
  return wnrep_list.Find_binary_assoc_wnrep(this, opc, l, r);
}

void
REASSOC::Add_assoc_opnd_to_wnrep(WNREP *wr, WNREP *wr0)
{
  INT i;
  if (wr->_assoc_opnds->Elements() == 0) {
    wr->_assoc_opnds->AddElement(wr0);
    return;
  }
  wr->_assoc_opnds->Newidx();
  // insert in ascending order of the value number in the dynamic array
  i = wr->_assoc_opnds->Lastidx(); 
  while (wr0->Wnrep_id() < (*wr->_assoc_opnds)[i-1]->Wnrep_id()) {
    (*wr->_assoc_opnds)[i] = (*wr->_assoc_opnds)[i-1];
    i--;
    if (i == 0)
      break;
  }
  (*wr->_assoc_opnds)[i] = wr0;
}

// gather all the operands in contiguous associative operations and record
// them in _assoc_opnds[] of wr
void 
REASSOC::Gather_assoc_opnds(OPCODE opc, WN *tree, WNREP *wr)
{
  WNREP *wr0;
  if (WN_opcode(tree) != opc) {
    wr0 = Value_number_expr(tree);
    Add_assoc_opnd_to_wnrep(wr, wr0);
  }
  else {
    Gather_assoc_opnds(opc, WN_kid0(tree), wr);
    Gather_assoc_opnds(opc, WN_kid1(tree), wr);
  }
}

UINT32
REASSOC::Hash_assoc_expr(WNREP *wr)
{
  UINT32 h = (UINT32) WN_opcode(wr->Wn());
  for (INT i = 0; i < wr->_assoc_opnds->Elements(); i++)
    h += ((*wr->_assoc_opnds)[i]->Wnrep_id()) << 4;
  h = h % CODE_HTABLE_SIZE;
  return h;
}

WNREP *
REASSOC::Find_or_append_assoc_expr(UINT32 hash_idx, WNREP *wr0)
{
  WNREP_CONTAINER wnrep_list;
  wnrep_list.Init_Head(_hash_vec[hash_idx]);
  WNREP *wr = wnrep_list.Find_assoc_wnrep(wr0);
  if (wr == NULL)  {
    wr = wr0;
    wr->Set_wnrep_id(Next_wnrep_id());
    wnrep_list.Prepend(wr);
    _hash_vec[hash_idx] = wr;
    _wnrep_id_to_wnrep->Setidx(wr->Wnrep_id());
    (*_wnrep_id_to_wnrep)[wr->Wnrep_id()] = wr;
    Enter_reassoc_cand(wr);
  }
  WN_MAP32_Set(_vn_map, wr0->Wn(), wr->Wnrep_id());
  return wr;
}

WNREP *
REASSOC::Value_number_expr(WN *tree)
{
  INT i;
  OPERATOR opr = WN_operator(tree);
  OPCODE opc = WN_opcode(tree);

  switch (opr) {
  // leaves
  case OPR_LDID: 
  case OPR_LDBITS:
  case OPR_INTCONST:
  case OPR_CONST:
  case OPR_LDA:
  case OPR_LDA_LABEL:
    return Find_or_append_expr(Hash_expr(tree), tree);

  case OPR_ADD:
  case OPR_MPY:
    if (WN_opcode(WN_kid0(tree)) == opc ||
        WN_opcode(WN_kid1(tree)) == opc) { // reassoc opportunity
      WNREP *tmpwr = CXX_NEW(WNREP, &_mem_pool);
      tmpwr->Set_wn(tree);
      tmpwr->_assoc_opnds = CXX_NEW(DYN_ARRAY<WNREP*>(&_mem_pool), &_mem_pool);
      Gather_assoc_opnds(opc, tree, tmpwr);
      return Find_or_append_assoc_expr(Hash_assoc_expr(tmpwr), tmpwr);
      }
    // fall thrue

  default:
    for (i = 0; i < WN_kid_count(tree); i++)
      Value_number_expr(WN_kid(tree, i));
    WNREP *wr = Find_or_append_expr(Hash_expr(tree), tree);
    wr->_u1._rank = WN_kid_count(tree);
    return wr;
  }
}

/* ====================================================================
 * Look for all expressions inside the given statement recursively and
 * value-number them
 * ==================================================================== */
void
REASSOC::Value_number_stmt(WN *tree)
{
  OPERATOR opr = WN_operator(tree);
  INT i;

  switch (opr) {

  // operators with no expression as kid
  case OPR_GOTO:
  case OPR_GOTO_OUTER_BLOCK:
  case OPR_RETURN:
  case OPR_COMMENT:
  case OPR_TRAP	:
  case OPR_FORWARD_BARRIER:
  case OPR_BACKWARD_BARRIER:
  case OPR_ALTENTRY:
  case OPR_PRAGMA:
  case OPR_LABEL:
  case OPR_REGION_EXIT:
    return;

  case OPR_DEALLOCA:	
  case OPR_PREFETCH:
  case OPR_AGOTO:
  case OPR_EVAL: 
  case OPR_TRUEBR:
  case OPR_FALSEBR:
  case OPR_ASSERT:
  case OPR_RETURN_VAL:
  case OPR_COMPGOTO:
  case OPR_XGOTO:
  case OPR_XPRAGMA:
    Value_number_expr(WN_kid0(tree));
    return;

  case OPR_ISTORE:
  case OPR_ISTBITS:
    Value_number_expr(WN_kid1(tree));
  // fall through

  case OPR_STID:
  case OPR_STBITS:
    Value_number_expr(WN_kid0(tree));
    return;

  case OPR_CALL:          
  case OPR_ICALL:
  case OPR_INTRINSIC_CALL:
    for (i = 0; i < WN_kid_count(tree); i++) 
      Value_number_expr(WN_kid(tree,i));
    return;

  case OPR_PICCALL:
    for (i = 0; i < (WN_kid_count(tree)-1); i++) 
      Value_number_expr(WN_kid(tree,i));
    return;

  case OPR_ASM_STMT:
    for (i = 2; i < WN_kid_count(tree); i++) 
      Value_number_expr(WN_kid0(WN_kid(tree,i)));
    return;

  case OPR_MSTORE:
    Value_number_expr(WN_kid0(tree));
    Value_number_expr(WN_kid1(tree));
    Value_number_expr(WN_kid2(tree));
    return;

  case OPR_LOOP_INFO:
    if (WN_kid1(tree) != NULL)
      Value_number_expr(WN_kid1(tree));
    return;

  // structured control flow statements

  case OPR_BLOCK: {
    WN *stmt;
    for (stmt = WN_first(tree); stmt; stmt = WN_next(stmt)) {
      Is_True(OPERATOR_is_stmt(WN_operator(stmt)) || OPERATOR_is_scf(WN_operator(stmt)),
	      ("statement operator expected"));
      Value_number_stmt(stmt);
    }
    return;
    }

  case OPR_DO_LOOP:
    Value_number_stmt(WN_kid(tree, 1)); // the initialization statement
    Value_number_expr(WN_kid2(tree));
    Value_number_stmt(WN_kid(tree, 3)); // the increment statement
    Value_number_stmt(WN_kid(tree, 4)); // the block
    if (WN_kid_count(tree) == 6)
      Value_number_stmt(WN_kid(tree, 5)); // LOOP_INFO
    return;

  case OPR_DO_WHILE:
  case OPR_WHILE_DO:
    Value_number_expr(WN_kid0(tree));
    Value_number_stmt(WN_kid(tree, 1));
    return;

  case OPR_IF:
    Value_number_expr(WN_kid0(tree));
    Value_number_stmt(WN_kid(tree, 1));
    Value_number_stmt(WN_kid(tree, 2));
    return;

  case OPR_REGION:
    Value_number_stmt(WN_region_body(tree));
    return;

  case OPR_IO:
    // skip
    return;

  default:	
    Is_True(FALSE,("unexpected operator"));
  }

  return;  
}

// enter assoc_wr in _reassoc_cands[][] where the high order index is the
// number of operands
void
REASSOC::Enter_reassoc_cand(WNREP *assoc_wr)
{
  INT n = assoc_wr->_assoc_opnds->Elements();
  INT i;
  if (_reassoc_cands->Lastidx() < n) {
    INT lastidx = _reassoc_cands->Lastidx();
    _reassoc_cands->Setidx(n);
    for (i = lastidx+1; i <= n; i++)
      (*_reassoc_cands)[i] = CXX_NEW(DYN_ARRAY<WNREP*>(&_mem_pool), &_mem_pool);
  }
  i = (*_reassoc_cands)[n]->Newidx();
  (*(*_reassoc_cands)[n])[i] = assoc_wr;
}

// pass 2 routines

// x is already known to be cse in wr; find larger ones that contain x
void
REASSOC::Find_larger_cses(WNREP *wr, WNREP *x)
{
  INT rank = wr->_assoc_opnds->Elements();
  INT i;
  UINT32 hash_idx;
  BOOL has_larger = FALSE;
  OPCODE opc = WN_opcode(wr->_wn);
  WNREP *r;
  WNREP *cse;
  for (i = 0; i < rank; i++) {
    if (_used_opnds->MemberP(i))
      continue;
    r = (*wr->_assoc_opnds)[i];
    hash_idx = WN_opcode(wr->_wn) + (x->Wnrep_id() << 4) + (r->Wnrep_id() << 4);
    hash_idx = hash_idx % CODE_HTABLE_SIZE;
    cse = Find_binary_assoc_expr(hash_idx, opc, x, r);
    if (cse == NULL)
      continue;
    has_larger = TRUE;
    if (cse->Rank() == rank-1) { // CSE is large enough
      INT n = _max_cses->Newidx();
      (*_max_cses)[n] = cse;
      return;
    }
    _used_opnds->Union1D(i);
    Find_larger_cses(wr, cse);
    _used_opnds->Difference1D(i);
    if ((*_max_cses)[_max_cses->Lastidx()]->Rank() == rank-1)
      return;
  }
  if (! has_larger) {
    INT n = _max_cses->Newidx();
    (*_max_cses)[n] = x;
  }
}

// Find all the possible largest CSEs in wr based on what has occurred so far 
// in the hash table, and insert into _max_cses.  There can be multiple such
// CSEs at the same time for a given associative expression, and "first order"
// refers to each of them.
void
REASSOC::Find_1st_order_max_cses(WNREP *wr)
{
  Is_True(wr->_assoc_opnds, ("REASSOC::Find_max_cse: WNREP is not type 2"));
  INT rank = wr->_assoc_opnds->Elements();
  _max_cses->Resetidx();
  INT i, j;
  UINT32 hash_idx;
  WNREP *cse;
  OPCODE opc = WN_opcode(wr->_wn);
  WNREP *l, *r;
  for (i = 0; i < rank-1; i++) {
    _used_opnds->ClearD();
    _used_opnds->Union1D(i);
    l = (*wr->_assoc_opnds)[i];
    for (j = i+1; j < rank; j++) {
      r = (*wr->_assoc_opnds)[j];
      hash_idx = WN_opcode(wr->_wn) + (l->Wnrep_id() << 4) + (r->Wnrep_id() << 4);
      hash_idx = hash_idx % CODE_HTABLE_SIZE;
      cse = Find_binary_assoc_expr(hash_idx, opc, l, r);
      if (cse == NULL)
	continue;
      if (cse->Rank() == rank-1) { // CSE is large enough
	INT n = _max_cses->Newidx();
	(*_max_cses)[n] = cse;
	return;
      }
      _used_opnds->Union1D(j);
      Find_larger_cses(wr, cse);
      _used_opnds->Difference1D(j);
      if ((*_max_cses)[_max_cses->Lastidx()]->Rank() == rank-1)
	return;
    }
  }
}

void
REASSOC::Set_used_opnds_in_wnrep(WN *tree, WNREP *wr)
{
  INT i;
  if (WN_opcode(tree) != WN_opcode(wr->_wn)) {
    for (i = 0; i < wr->_assoc_opnds->Elements(); i++) {
      if (_used_opnds->MemberP(i))
	continue;
      if ((*wr->_assoc_opnds)[i]->Wnrep_id() == Wnrep_id(tree)) {
	_used_opnds->Union1D(i);
	return;
      }
    }
    FmtAssert(FALSE, ("REASSOC::Set_used_opnds_in_wnrep: cannot find opnd in assoc_opnds"));
  }
  else {
    Set_used_opnds_in_wnrep(WN_kid0(tree), wr);
    Set_used_opnds_in_wnrep(WN_kid1(tree), wr);
  }
}

BOOL
REASSOC::Some_opnd_in_used(WN *tree, WNREP *wr)
{
  INT i;
  if (WN_opcode(tree) != WN_opcode(wr->_wn)) {
    for (i = 0; i < wr->_assoc_opnds->Elements(); i++) {
      if (_used_opnds->MemberP(i))
	continue;
      if (_counted_opnds->MemberP(i))
	continue;
      if (Wnrep_id(tree) < (*wr->_assoc_opnds)[i]->Wnrep_id())
	return TRUE; // rely on _assoc_opnds[] being sorted via wnrep_id
      if (Wnrep_id(tree) == (*wr->_assoc_opnds)[i]->Wnrep_id()) {
	_counted_opnds->Union1D(i); // the expression can have identical opnds
	return FALSE;
      }
    }
    return TRUE;
  }
  else 
    return Some_opnd_in_used(WN_kid0(tree), wr) ||
           Some_opnd_in_used(WN_kid1(tree), wr);
}

void
REASSOC::Build_into_final_form(WNREP *subwr, WNREP *wr)
{
  if (wr->Final_form() == NULL) {
    wr->_u1._final_form = subwr;
    return;
  }
  WN *wn = WN_Binary(WN_operator(wr->_wn), WN_rtype(wr->_wn), 
  		     wr->_u1._final_form->_wn, subwr->_wn); 
  WNREP *newwr = Find_or_append_expr(Hash_expr(wn), wn);
  INT lrank = (WN_opcode(wr->_u1._final_form->_wn) == WN_opcode(wr->_wn)) ?
  		wr->_u1._final_form->Rank() : 1;
  INT rrank = (WN_opcode(subwr->_wn) == WN_opcode(wr->_wn)) ? subwr->Rank() : 1;
  newwr->_u1._rank = lrank + rrank;
  wr->_u1._final_form = newwr;
}

void
REASSOC::Build_final_form(WNREP *wr)
{
  INT i;
  _used_opnds->ClearD(); // to keep track of covered operands in _assoc_opnds[]
  if (_max_cses->Elements() > 0) {
    INT largest;
    WNREP *largest_cse;
    do {
      // find the next largest from _max_cses and add to _final_form
      largest_cse = NULL;
      for (i = 0; i < _max_cses->Elements(); i++) {
	if ((*_max_cses)[i] == NULL)
	  continue;
	_counted_opnds->ClearD(); // initialize to call Some_opnd_in_used()
	if (Some_opnd_in_used((*_max_cses)[i]->_wn, wr))
	  continue;
	if (largest_cse == NULL ||
	    (*_max_cses)[i]->Rank() > largest_cse->Rank()) {
	  largest = i;
	  largest_cse = (*_max_cses)[i];
	}
      }
      if (largest_cse != NULL) {
	Set_used_opnds_in_wnrep(largest_cse->_wn, wr);
	Build_into_final_form(largest_cse, wr);
	(*_max_cses)[largest] = NULL;
      }
    } while (largest_cse != NULL);
  }
  // build the rest of the operands in assoc_opnds into the final form
  for (i = 0; i < wr->_assoc_opnds->Elements(); i++) {
    if (_used_opnds->MemberP(i))
      continue;
    Build_into_final_form((*wr->_assoc_opnds)[i], wr);
  }
}

// given two type 2 WNREPs, find the largest CSE between them and create
// type 1 node instances for the CSE; return NULL if not found
WNREP *
REASSOC::Find_and_create_cse(WNREP *wri, WNREP *wrj)
{
  WNREP *first_common_opnd = NULL;
  WNREP *second_common_opnd = NULL;
  WNREP *new_common_opnd;
  WNREP *tmpwr = NULL; 
  WNREP *res;
  INT i = 0, j = 0;
  do {
    while (i < wri->_assoc_opnds->Elements() &&
	   (*wri->_assoc_opnds)[i]->Wnrep_id() < (*wrj->_assoc_opnds)[j]->Wnrep_id())
      i++;
    if (i == wri->_assoc_opnds->Elements())
      break;
    while (j < wrj->_assoc_opnds->Elements() &&
	   (*wrj->_assoc_opnds)[j]->Wnrep_id() < (*wri->_assoc_opnds)[i]->Wnrep_id())
      j++;
    if (j == wrj->_assoc_opnds->Elements())
      break;
    if ((*wri->_assoc_opnds)[i]->Wnrep_id() == (*wrj->_assoc_opnds)[j]->Wnrep_id()) {
      new_common_opnd = (*wri->_assoc_opnds)[i];
      if (first_common_opnd == NULL)
	first_common_opnd = new_common_opnd;
      else if (second_common_opnd == NULL)
	second_common_opnd = new_common_opnd;
      else {
	if (tmpwr == NULL) { // not yet created
	  tmpwr = CXX_NEW(WNREP, &_mem_pool);
	  tmpwr->Set_wn(WN_Binary(WN_operator(wri->_wn), WN_rtype(wri->_wn),
				  first_common_opnd->_wn, second_common_opnd->_wn));
	  tmpwr->_assoc_opnds = CXX_NEW(DYN_ARRAY<WNREP*>(&_mem_pool), &_mem_pool);
	  Add_assoc_opnd_to_wnrep(tmpwr, first_common_opnd);
	  Add_assoc_opnd_to_wnrep(tmpwr, second_common_opnd);
	}
	Add_assoc_opnd_to_wnrep(tmpwr, new_common_opnd);
	tmpwr->Set_wn(WN_Binary(WN_operator(wri->_wn), WN_rtype(wri->_wn),
				tmpwr->_wn, new_common_opnd->_wn));
      }
      i++; j++;
    }
    else i++;
  } while (i < wri->_assoc_opnds->Elements() &&
  	   j < wrj->_assoc_opnds->Elements());
  if (second_common_opnd == NULL)
    return NULL;
  if (tmpwr == NULL) {
    WN *tree = WN_Binary(WN_operator(wri->_wn), WN_rtype(wri->_wn),
			 first_common_opnd->_wn, second_common_opnd->_wn);
    res = Find_or_append_expr(Hash_expr(tree), tree);
    res->_u1._rank = 2;    
    return res;
  }
  Find_1st_order_max_cses(tmpwr);
  Build_final_form(tmpwr);
  return tmpwr->Final_form();
}

// pass 3 routines

WN *
REASSOC::Retrieve_wn_via_wnrep_id(WN *tree, OPCODE opc, UINT32 wr_id)
{
  WN *res;
  if (WN_opcode(tree) != opc) {
    WNREP *treewr = (*_wnrep_id_to_wnrep)[Wnrep_id(tree)];
    if ((Wnrep_id(tree) == wr_id ||
         treewr->_assoc_opnds !=  NULL &&
         treewr->Final_form()->Wnrep_id() == wr_id) && 
	WN_MAP32_Get(_emitted_map, tree) == 0) {
      res = Reassociate_expr(tree);
      WN_MAP32_Set(_emitted_map, tree, 1); // mark emitted, so this will not be retrieved again
      return res;
    }
    else return NULL;
  }
  res = Retrieve_wn_via_wnrep_id(WN_kid0(tree), opc, wr_id);
  if (res == NULL)
    res = Retrieve_wn_via_wnrep_id(WN_kid1(tree), opc, wr_id);
  return res;
}

// reassociate tree based on the form in wr consisting of type 1 nodes;
// implemented by recursive calls in wr, while tree is always the root of
// the associative expression
WN *
REASSOC::Reassociate_to_final_form(WNREP *wr, WN *tree)
{
  OPCODE opc = WN_opcode(tree);
  if (WN_opcode(wr->_wn) != opc)
    return Retrieve_wn_via_wnrep_id(tree, opc, wr->Wnrep_id());
  // recursive call
  WN *l = WN_kid0(wr->_wn);
  WN *r = WN_kid1(wr->_wn);
  l = Reassociate_to_final_form((*_wnrep_id_to_wnrep)[Wnrep_id(l)], tree);
  r = Reassociate_to_final_form((*_wnrep_id_to_wnrep)[Wnrep_id(r)], tree);
  WN *wn = WN_Binary(OPCODE_operator(opc), OPCODE_rtype(opc), l, r);
  WN_MAP32_Set(_vn_map, wn, wr->Wnrep_id());
  return wn;
}

WN *
REASSOC::Reassociate_expr(WN *tree)
{
  INT i;
  OPERATOR opr = WN_operator(tree);

  switch (opr) {
  // leaves
  case OPR_LDID: 
  case OPR_LDBITS:
  case OPR_INTCONST:
  case OPR_CONST:
  case OPR_LDA:
  case OPR_LDA_LABEL:
    return tree;

  case OPR_ADD:
  case OPR_MPY:
    if (Wnrep_id(tree) != 0) {
      WNREP *wr = (*_wnrep_id_to_wnrep)[Wnrep_id(tree)];
      if (wr->_assoc_opnds != NULL)
	return Reassociate_to_final_form(wr->Final_form(), tree);
      // fall thrue
    }
    // fall thrue

  default:
    for (i = 0; i < WN_kid_count(tree); i++)
      WN_kid(tree, i) = Reassociate_expr(WN_kid(tree, i));
    return tree;
  }
}

/* ====================================================================
 * Look for all expressions inside the given statement recursively and
 * effect the reassociation
 * ==================================================================== */
void
REASSOC::Reassociate_stmt(WN *tree)
{
  OPERATOR opr = WN_operator(tree);
  INT i;

  switch (opr) {

  // operators with no expression as kid
  case OPR_GOTO:
  case OPR_GOTO_OUTER_BLOCK:
  case OPR_RETURN:
  case OPR_COMMENT:
  case OPR_TRAP	:
  case OPR_FORWARD_BARRIER:
  case OPR_BACKWARD_BARRIER:
  case OPR_ALTENTRY:
  case OPR_PRAGMA:
  case OPR_LABEL:
  case OPR_REGION_EXIT:
    return;

  case OPR_DEALLOCA:	
  case OPR_PREFETCH:
  case OPR_AGOTO:
  case OPR_EVAL: 
  case OPR_TRUEBR:
  case OPR_FALSEBR:
  case OPR_ASSERT:
  case OPR_RETURN_VAL:
  case OPR_COMPGOTO:
  case OPR_XGOTO:
  case OPR_XPRAGMA:
    WN_kid0(tree) = Reassociate_expr(WN_kid0(tree));
    return;

  case OPR_ISTORE:
  case OPR_ISTBITS:
    WN_kid1(tree) = Reassociate_expr(WN_kid1(tree));
  // fall through

  case OPR_STID:
  case OPR_STBITS:
    WN_kid0(tree) = Reassociate_expr(WN_kid0(tree));
    return;

  case OPR_CALL:          
  case OPR_ICALL:
  case OPR_INTRINSIC_CALL:
    for (i = 0; i < WN_kid_count(tree); i++) 
      WN_kid(tree,i) = Reassociate_expr(WN_kid(tree,i));
    return;

  case OPR_PICCALL:
    for (i = 0; i < (WN_kid_count(tree)-1); i++) 
      WN_kid(tree,i) = Reassociate_expr(WN_kid(tree,i));
    return;

  case OPR_ASM_STMT:
    for (i = 2; i < WN_kid_count(tree); i++) 
      WN_kid0(WN_kid(tree,i)) = Reassociate_expr(WN_kid0(WN_kid(tree,i)));
    return;

  case OPR_MSTORE:
    WN_kid0(tree) = Reassociate_expr(WN_kid0(tree));
    WN_kid1(tree) = Reassociate_expr(WN_kid1(tree));
    WN_kid2(tree) = Reassociate_expr(WN_kid2(tree));
    return;

  case OPR_LOOP_INFO:
    if (WN_kid1(tree) != NULL)
      WN_kid1(tree) = Reassociate_expr(WN_kid1(tree));
    return;

  // structured control flow statements

  case OPR_BLOCK: {
    WN *stmt;
    for (stmt = WN_first(tree); stmt; stmt = WN_next(stmt)) {
      Is_True(OPERATOR_is_stmt(WN_operator(stmt)) || OPERATOR_is_scf(WN_operator(stmt)),
	      ("statement operator expected"));
      Reassociate_stmt(stmt);
    }
    return;
    }

  case OPR_DO_LOOP:
    Reassociate_stmt(WN_kid(tree, 1)); // the initialization statement
    WN_kid2(tree) = Reassociate_expr(WN_kid2(tree));
    Reassociate_stmt(WN_kid(tree, 3)); // the increment statement
    Reassociate_stmt(WN_kid(tree, 4)); // the block
    if (WN_kid_count(tree) == 6)
      Reassociate_stmt(WN_kid(tree, 5)); // LOOP_INFO
    return;

  case OPR_DO_WHILE:
  case OPR_WHILE_DO:
    WN_kid0(tree) = Reassociate_expr(WN_kid0(tree));
    Reassociate_stmt(WN_kid(tree, 1));
    return;

  case OPR_IF:
    WN_kid0(tree) = Reassociate_expr(WN_kid0(tree));
    Reassociate_stmt(WN_kid(tree, 1));
    Reassociate_stmt(WN_kid(tree, 2));
    return;

  case OPR_REGION:
    Reassociate_stmt(WN_region_body(tree));
    return;

  case OPR_IO:
    // skip
    return;

  default:	
    Is_True(FALSE,("unexpected operator"));
  }

  return;  
}

/* ====================================================================
 * Driver for the passes to perform expression reassociation
 * ==================================================================== */
static void
Perform_reassoc(WN *tree)
{
  REASSOC reassoc;
  INT i, j;

  // pass 1: value numbering
  reassoc.Value_number_stmt(tree);

  if (debug_reassoc) {
    fputs(DBar, TFile);
    fprintf(TFile, "WN_reassoc pass 1: expression trees with value numbers\n");
    fputs(DBar, TFile);
    fdump_tree_with_map( TFile, tree, reassoc.Vn_map());

    fputs(DBar, TFile);
  }

  // pass 2: determine optimal association
  for (INT i = 3; i < reassoc.Reassoc_cands()->Elements(); i++) {
    if (debug_reassoc)
      fprintf(TFile, "Candidates with %d associative operands:\n", i);
    for (INT j = 0; j < (*reassoc.Reassoc_cands())[i]->Elements(); j++) {
      INT k;
      WNREP *wr = (*(*reassoc.Reassoc_cands())[i])[j];
      reassoc.Find_1st_order_max_cses(wr);

      if (debug_reassoc) {
	fdump_tree_with_map( TFile, wr->Wn(), reassoc.Vn_map());
	fprintf(TFile, "First order max CSEs:\n");
	for (k = 0; k < reassoc.Max_cses()->Elements(); k++)
	  fdump_tree_with_map( TFile, (*reassoc.Max_cses())[k]->Wn(), reassoc.Vn_map());
	fputs(DBar, TFile);
      }

      // if there is no CSE of the largest rank, identify CSEs among 
      // expressions at the same rank and create instances of them
      if (reassoc.Max_cses()->Elements() == 0 ||
	  ((*reassoc.Max_cses())[reassoc.Max_cses()->Lastidx()])->Rank() < i-1)
      {
	WNREP *wr2, *cse;
	// go thru the remaining reassoc cands of the same rank
	for (k = j+1; k < (*reassoc.Reassoc_cands())[i]->Elements(); k++) {
	  wr2 = (*(*reassoc.Reassoc_cands())[i])[k];
	  cse = reassoc.Find_and_create_cse(wr, wr2);
	  if (cse == NULL)
	    continue;
	  INT n = reassoc.Max_cses()->Newidx();
	  (*reassoc.Max_cses())[n] = cse;
	  if (cse->Rank() == i-1) // CSE is large enough
	    break;
	}
	// need to re-do following because _max_cses has been overwritten
	reassoc.Find_1st_order_max_cses(wr); 
      }

      reassoc.Build_final_form(wr);
      if (debug_reassoc) {
	fprintf(TFile, "Final form:\n");
	fdump_tree_with_map( TFile, wr->Final_form()->Wn(), reassoc.Vn_map());
	fputs(DBar, TFile);
      }
    }
  }
  
  // pass 3: generate output WHIRL
  reassoc.Reassociate_stmt(tree);
}

/* ====================================================================
 * tree represents the region (scope) over which reassociation is performed
 * ==================================================================== */
void
WN_reassoc(WN *tree)
{ 
  // disable WHIRL simplifier which may do its own reassociation
  Enable_WN_Simp = FALSE;
  			
  Start_Timer(T_Lower_CU);
  Set_Error_Phase("WN_reassoc");

  if (WN_operator(tree) == OPR_FUNC_ENTRY) 
    Perform_reassoc(WN_func_body(tree));
  else if (WN_operator(tree) == OPR_REGION) 
    Perform_reassoc(WN_region_body(tree));
  else if (OPERATOR_is_stmt(WN_operator(tree)) || OPERATOR_is_scf(WN_operator(tree)))
    Perform_reassoc(tree);
  else Is_True(FALSE, ("unexpected WHIRL operator"));

  Stop_Timer(T_Lower_CU);

  WN_Lower_Checkdump("After wn_reassoc", tree, 0);   

  WN_verifier(tree);

  return;
}


/* ====================================================================
 * return TRUE if there is no BLOCK nested inside tree; if this is true at
 * a BLOCK, call WN_reassoc() for the block
 * ==================================================================== */
BOOL
WN_reassoc_deepest_blocks(WN *tree)
{
  OPERATOR opr = WN_operator(tree);
  INT i;

  switch (opr) {

  // operators with no expression as kid
  case OPR_GOTO:
  case OPR_GOTO_OUTER_BLOCK:
  case OPR_RETURN:
  case OPR_COMMENT:
  case OPR_TRAP	:
  case OPR_FORWARD_BARRIER:
  case OPR_BACKWARD_BARRIER:
  case OPR_ALTENTRY:
  case OPR_PRAGMA:
  case OPR_LABEL:
  case OPR_REGION_EXIT:
    return TRUE;

  case OPR_DEALLOCA:	
  case OPR_PREFETCH:
  case OPR_AGOTO:
  case OPR_EVAL: 
  case OPR_TRUEBR:
  case OPR_FALSEBR:
  case OPR_ASSERT:
  case OPR_RETURN_VAL:
  case OPR_COMPGOTO:
  case OPR_XGOTO:
  case OPR_XPRAGMA:
    return TRUE;

  case OPR_ISTORE:
  case OPR_ISTBITS:
  case OPR_STID:
  case OPR_STBITS:
  case OPR_MSTORE:
    return TRUE;

  case OPR_CALL:          
  case OPR_ICALL:
  case OPR_INTRINSIC_CALL:
  case OPR_PICCALL:
  case OPR_ASM_STMT:
    return TRUE;

  case OPR_LOOP_INFO:
    return TRUE;

  // structured control flow statements
  
  case OPR_FUNC_ENTRY:
    WN_reassoc_deepest_blocks(WN_func_body(tree));
    return FALSE;

  case OPR_BLOCK: {
    BOOL deepest = TRUE;
    WN *stmt;
    for (stmt = WN_first(tree); stmt; stmt = WN_next(stmt)) {
      Is_True(OPERATOR_is_stmt(WN_operator(stmt)) || OPERATOR_is_scf(WN_operator(stmt)),
	      ("statement operator expected"));
      if (! WN_reassoc_deepest_blocks(stmt))
	deepest = FALSE;
    }
    if (deepest)
      WN_reassoc(tree);
    return FALSE;
    }

  case OPR_DO_LOOP:
    WN_reassoc_deepest_blocks(WN_kid(tree, 4)); // the block
    return FALSE;

  case OPR_DO_WHILE:
  case OPR_WHILE_DO:
    WN_reassoc_deepest_blocks(WN_kid(tree, 1));
    return FALSE;

  case OPR_IF: {
      BOOL res1 = WN_reassoc_deepest_blocks(WN_kid(tree, 1));
      BOOL res2 = WN_reassoc_deepest_blocks(WN_kid(tree, 2));
      return res1 & res2;
    }

  case OPR_REGION:
    WN_reassoc_deepest_blocks(WN_region_body(tree));
    return FALSE;

  case OPR_IO:
    // skip
    return FALSE;

  default:	
    Is_True(FALSE,("unexpected operator"));
  }
  return FALSE;
}
