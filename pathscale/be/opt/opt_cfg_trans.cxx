/*
 * Copyright (C) 2008-2009 Advanced Micro Devices, Inc.  All Rights Reserved.
 */

//-*-c++-*-

/*
 *  Copyright (C) 2007. QLogic Corporation. All Rights Reserved.
 */

/*
 * Copyright 2003, 2004, 2005, 2006 PathScale, Inc.  All Rights Reserved.
 */

// ====================================================================
// ====================================================================
//
// Module: opt_cfg_trans.cxx
// $Revision: 1.7 $
// $Date: 04/12/21 14:57:17-08:00 $
// $Author: bos@eng-25.internal.keyresearch.com $
// $Source: /home/bos/bk/kpro64-pending/be/opt/SCCS/s.opt_cfg_trans.cxx $
//
// ====================================================================
//
// Copyright (C) 2000, 2001 Silicon Graphics, Inc.  All Rights Reserved.
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of version 2 of the GNU General Public License as
// published by the Free Software Foundation.
//
// This program is distributed in the hope that it would be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//
// Further, this software is distributed without any warranty that it
// is free of the rightful claim of any third person regarding
// infringement  or the like.  Any license provided herein, whether
// implied or otherwise, applies only to this software file.  Patent
// licenses, if any, provided herein do not apply to combinations of
// this program with other software, or any other product whatsoever.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write the Free Software Foundation,
// Inc., 59 Temple Place - Suite 330, Boston MA 02111-1307, USA.
//
// Contact information:  Silicon Graphics, Inc., 1600 Amphitheatre Pky,
// Mountain View, CA 94043, or:
//
// http://www.sgi.com
//
// For further information regarding this notice, see:
//
// http://oss.sgi.com/projects/GenInfo/NoticeExplan
//
// ====================================================================
//
// Description:
//
//  Module for CFG transformation:
//
//    STEP 1:  build a generic graph representation from WOPT CFG
//    STEP 2:  generate zone form the generic graph and SSA.
//    STEP 3:  select profitable zones to clone
//    STEP 4:  rebuild WOPT CFG
//         4a: rebuild basic block layout (i.e. BB_next)
//         4b: rebuild succ/pred list 
//         4c: add/remove dead goto/label.
//         4d: update structured control flow
//    STEP 5:  invalidate some phi functions
//             remove phi function with single opnd,
//             remove phi function whose #opnd didn't match #pred.
//    STEP 6:  rerun ssa rename in the caller.  
//
//
// Transformation example 1:  loop butterfly
// 
//   CFG before transformation:
//   edges: (1,2) (2,3) (2,9) (3,4) (4,6) (4,5) (5,7) (6,7) (7,4) (7,8) (8,9)
//
//   zone representing a loop butterfly transformation:
//   entry edges: (3,4)
//   clone edges: (4,6) (6,7) (7,4)
//   exit  edges: (4,5) (7,8)
//
//   CFG after transformation:
//   edges: (1,2) (2,3) (2,9) (3,13) (4,6) (4,5) (5,7) (6,7) (7,13)
//          (7,8) (8,9) (10,11) (10,5) (11,12) (12,10) (12,8) (13,10)
//
//   Translation of old to new blocks:
//   translation: 10<-4 11<-6 12<-7
//
//   introduced vertex 13 
//     -- the new preheader for inner-loop, and as the 
//     -- the header of the outer loop.
//
// ====================================================================
// ====================================================================


#define USE_STANDARD_TYPE
#include "opt_defs.h"
#include "opt_bb.h"
#include "bb_node_set.h"
#include "opt_cfg.h"
#include "opt_htable.h"
#include "opt_fb.h"
#include "opt_cfg_trans.h"
#include "config_wopt.h"
#include "config_lno.h"

#include "opt_main.h"
#include "wn_simp.h"
#include "w2op.h"

using std::insert_iterator;
using std::map;
using std::set;

// for debugging
#ifdef Is_True_On
extern void show_graph(successor_graph& g);
extern void show_zone(zone& z);
extern void show_all_zones(successor_graph&, zone_iterator, zone_iterator);
extern void mark_attr_begin();
extern void mark_attr_end();
extern void mark_translated_vertex(vertex_id, vertex_id);
#endif


// Build successor graph and collect fall-thru requirement.
//    - BB_ENTRY must be empty.
//
//  CFG containing REGIONs is not handled:
//      - BB_REGIONSTART must be empty.
//      - Region entry/Region exit must have specific ordering
//      - exception region must be contagious.
//
template <class Insert_iterator>
static bool
build_successor_graph(CFG *cfg, successor_graph& g, Insert_iterator entry)
{
  for (BB_NODE *bb = cfg->First_bb();
       bb != NULL;
       bb = bb->Next()) {

    if (bb->Kind() == BB_REGIONSTART)
      return false; 

    if (bb->Branch_stmtrep() && bb->Branch_stmtrep()->Op() == OPC_REGION)
      return false;

    if (bb->Kind() == BB_ENTRY)
      *entry++ = bb->Id();

    BB_LIST_ITER bb_succ_iter;
    BB_NODE *succ;
    BB_NODE *fall_thru = NULL;
    int count = 0;
    FOR_ALL_ELEM(succ, bb_succ_iter, Init(bb->Succ())) {
      ++count;
      if (succ == bb->Next())
       fall_thru = succ;
      else
	add_edge(g, edge(bb->Id(), succ->Id()));
    }
    if (fall_thru)
      add_edge(g, edge(bb->Id(), fall_thru->Id()));
      
    if (fall_thru)
      if (count == 2 || 
	  bb->Kind() == BB_REGIONSTART ||
	  bb->Kind() == BB_ENTRY ||
	  (bb->Next() != NULL &&
	   bb->Next()->Kind() == BB_REGIONSTART) ||   
	  (count == 1 &&
	   bb->Branch_stmtrep() != NULL &&
	   bb->Branch_stmtrep()->Op() != OPC_GOTO)) {
	edge *e = find_edge(g, bb->Id(), fall_thru->Id());
	e->must_fall_thru = true;
      }
  }

  // Add fake_exit_bb (it is not reachable from any BB)
  if (cfg->Fake_exit_bb()) {
    BB_NODE *bb = cfg->Fake_exit_bb();
    BB_LIST_ITER bb_pred_iter;
    BB_NODE *pred;
    edge *fall_thru = NULL;
    FOR_ALL_ELEM(pred, bb_pred_iter, Init(bb->Pred())) {
      if (pred->Kind() == BB_EXIT)
	add_edge(g, edge(pred->Id(), bb->Id()));
    }
  }
  
#ifdef __STRESS_TESTING
  // stress testing -- random shuffle the succ edges in 'g'
  for (int i = 0; i < g.size(); ++i) {
    if (g[i].size() > 0) {
      random_shuffle(g[i].begin(), g[i].end());
    }
  }
#endif

  return true;
}


// reconstruct_CFG will build the WOPT CFG from the generic graph.
// It assumes basic block cloning has been done, i.e. does not need
// to replicate statements or expressions, with the exception of
// generating new basic block containing single goto statements.
void
reconstruct_CFG(successor_graph& g, CFG *cfg, bool trace)
{
  if (trace) {
    fprintf(TFile, "edges: \n");
    for (successor_graph::iterator e = g.begin(); e != g.end(); ++e)
      fprintf(TFile, "(%d,%d)%c ",
	      first(*e), second(*e), (*e).must_fall_thru?'y':'n');
    fprintf(TFile,"\n");
  }

  // No basic block can be the fall-through of two source block!
  {
    vector<bool> was_fall_thru_target(g.size(), false);
    vector<pair<edge,edge> > out_buffer;
    successor_graph::cluster_id next_cluster_id = g.size();
    for (successor_graph::iterator ep = g.begin(); 
	 ep != g.end(); 
	 ++ep) {
      if ((*ep).must_fall_thru) {
	if (was_fall_thru_target[second(*ep)]) {
	  successor_graph::cluster_id v = next_cluster_id++;
	  out_buffer.push_back(pair<edge,edge>(*ep,edge(v, second(*ep))));
	  (*ep).second = v; 
	  if (trace)
	    fprintf(TFile, "CFG trans: added fall-thru basic block %d\n", v);
	  BB_NODE *bb = cfg->Create_and_allocate_bb(BB_GOTO);
	  Is_True(bb->Id() == v, ("vertex id not match"));
	  bb->Clear();
	  bb->Set_id(v);
	  bb->Set_labnam(0);
	  bb->Set_kind(BB_GOTO);
	  bb->Set_phi_list(NULL);
	} else
	  was_fall_thru_target[second(*ep)] = true;
      }
    }

    for (vector<pair<edge,edge> >::iterator p = out_buffer.begin();
	 p != out_buffer.end();
	 ++p) {
      add_edge(g, (*p).second);

      if (cfg->Feedback()) {
	IDTYPE nx_src = (*p).first.first;
	IDTYPE nx_mid = (*p).second.first;
	IDTYPE nx_dst = (*p).second.second;
	cfg->Feedback()->Split_edge(nx_src, nx_mid, nx_dst);
      }
    }
  }

  // Produce layout, i.e. setup bb->Next().
  vector<int> layout_order;
  {
    vector<int> rpo;
    generate_reverse_post_order(g, cfg->First_bb()->Id(), rpo);

    if (trace) {
      fprintf(TFile, "rpo order: ");
      for (int i = 0; i < rpo.size(); ++i)
	fprintf(TFile, "%d ", rpo[i]);
      fprintf(TFile, "\n");
      
      fprintf(TFile, "edges: \n");
      for (successor_graph::iterator e = g.begin(); e != g.end(); ++e)
	fprintf(TFile, "(%d,%d)%c ",
		first(*e), second(*e), (*e).must_fall_thru ? 'y' : 'n');
      fprintf(TFile,"\n");
    }

    vector<bool> visited(g.size(), false);

    // mark BB that must come from fall-thru visited
    int i;
    for (i = 0; i < rpo.size(); ++i) {
      int cur_bb_id = rpo[i];
      for (successor_graph::fast_iterator e = g[cur_bb_id].begin();
	   e != g[cur_bb_id].end();
	   ++e) {
	if ((*e).must_fall_thru) {
	  visited[second(*e)] = true;
	  break;
	}
      }
    }

    insert_iterator<vector<int> > ii(layout_order,layout_order.begin());

    for (i = 0; i < rpo.size(); ++i) {
      int cur_bb_id = rpo[i];
      if (visited[cur_bb_id]) 
	continue;

      bool cont;
      do {
	Is_True(! visited[cur_bb_id],
		("restructure_CFG: conflicting layout requirement BB%d"
		 " cannot be the fall-thru of BB%d.",
		 cur_bb_id, *(rpo.end() - 1)));

	*ii++ = cur_bb_id;
	visited[cur_bb_id] = true;
	cont = false;
	for (successor_graph::fast_iterator e = g[cur_bb_id].begin();
	     e != g[cur_bb_id].end();
	     ++e) {
	  if ((*e).must_fall_thru) {
	    cont = true;
	    cur_bb_id = second(*e);
	    visited[cur_bb_id] = false;
	    break;
	  }
	}
      } while (cont);
    }

    if (trace) {
      fprintf(TFile, "layout order: ");
      for (int i = 0; i < layout_order.size(); ++i)
	fprintf(TFile, "%d ", layout_order[i]);
      fprintf(TFile, "\nold bb order: ");
      for (BB_NODE *bb = cfg->First_bb(); bb != NULL; bb = bb->Next())
	fprintf(TFile, "%" PRIdPTR " ", bb->Id());
      fprintf(TFile, "\n");
    }

    Is_True(rpo.size() == layout_order.size(),
	    ("some BB are lost during layout."));


    // Remove unreachable bbs from the cfg
    {
      // Identify all bbs unreachable from the root of the rebuilt graph
      vector<bool> reachable( g.size(), false );
      for (int i = layout_order.size() - 1; i >= 0; --i) {
	int bb_id = layout_order[i];
	reachable[bb_id] = true;
      }

      // Remove all unreachable bbs
      BB_NODE *bb_next = NULL;
      for (BB_NODE *bb = cfg->First_bb(); bb != NULL; bb = bb_next) {
	bb_next = bb->Next();
	if ( ! reachable[bb->Id()] && cfg->Removable_bb( bb ) ) {
	  cfg->Remove_bb(bb); // this also updates OPT_FEEDBACK
	}
      }
    }

    // Reset next, prev, succ, and pred of bb
    for (i = layout_order.size() - 1; i >= 0; --i) {
      int bb_id = layout_order[i];
      BB_NODE *bb = cfg->Get_bb(bb_id);
      bb->Set_succ(NULL);
      bb->Set_pred(NULL);
    }

    for (i = 0; i < layout_order.size() - 1; ++i) {
      int bb_id = layout_order[i];
      int bb_id2 = layout_order[i+1];
      BB_NODE *bb = cfg->Get_bb(bb_id);
      BB_NODE *bb2 = cfg->Get_bb(bb_id2);
      bb->Set_next(bb2);
      bb2->Set_prev(bb);
    }
    {
      BB_NODE *bb = cfg->Get_bb(layout_order[0]);
      bb->Set_prev(NULL);
      bb = cfg->Get_bb(layout_order[layout_order.size() - 1]);
      bb->Set_next(NULL);
    }

    // Update cfg->Last_bb() !!!
    cfg->Set_last_bb( cfg->Get_bb(*(layout_order.end()-1)) );
  }


  // Create bb->Succ() and bb->Pred().
  for (successor_graph::iterator e = g.begin(); 
       e != g.end(); 
       ++e) {
    BB_NODE *bb1 = cfg->Get_bb(first(*e));
    BB_NODE *bb2 = cfg->Get_bb(second(*e));

    if (bb2 != cfg->Fake_exit_bb())
      bb1->Append_succ(bb2, cfg->Mem_pool());
   
    if (bb1 != cfg->Fake_entry_bb())
      bb2->Append_pred(bb1, cfg->Mem_pool());
  }

  // Add/Remove goto/labels
  {
    vector<bool> need_label(g.size(), false);

    // update gotos
    int i;
    for (i = 0; i < layout_order.size(); ++i) {
      vertex_id bb_id = layout_order[i];
      vertex_id fall_thru_bb_id =
	(i + 1 < layout_order.size()) ? layout_order[i + 1] : -1;
      BB_NODE *bb = cfg->Get_bb(bb_id);

      // no goto from a fake-bb
      if (bb == cfg->Fake_entry_bb()) continue;

      if (1 == g[bb_id].size()) {
	vertex_id goto_bb_id = second(*g[bb_id].begin());
	BB_NODE *goto_bb = cfg->Get_bb(goto_bb_id);
	if (goto_bb_id == fall_thru_bb_id) {
	  // remove redundant gotos
	  STMTREP *bb_branch = bb->Branch_stmtrep();
	  if (bb_branch != NULL &&
	      OPC_GOTO == bb_branch->Op())  // might break BB at CALLs
	    bb->Remove_stmtrep(bb_branch);
	} else { 
	  if (goto_bb != cfg->Fake_exit_bb()) {
	    // target BB needs a label
	    need_label[goto_bb_id] = true;
	    if (goto_bb->Labnam() == 0) goto_bb->Add_label(cfg);

	    STMTREP *branch_sr = bb->Branch_stmtrep();
	    if (branch_sr == NULL) {
	      branch_sr = CXX_NEW( STMTREP(OPC_GOTO), cfg->Mem_pool() );
	      branch_sr->Init_Goto( NULL, goto_bb->Labnam(), 0);
	      bb->Append_stmtrep( branch_sr);
	    } else {
#ifdef KEY // bug 12839
	      if (branch_sr->Op() == OPC_AGOTO) {
		branch_sr->Set_op(OPC_GOTO);
		branch_sr->Rhs()->DecUsecnt();
		branch_sr->Set_rhs(NULL);
		bb->Set_kind(BB_GOTO);
	      }
#endif
	      Is_True(branch_sr->Op() == OPC_GOTO, ("expected OPC_GOTO"));
	      branch_sr->Set_label_number(goto_bb->Labnam());
	    }
	  }
	}
      } else if (1 < g[bb_id].size()) {
	for (successor_graph::fast_iterator e = g[bb_id].begin();
	     e != g[bb_id].end();
	     ++e) {
	  vertex_id goto_bb_id = second(*e);
	  if (goto_bb_id != fall_thru_bb_id) {
	    BB_NODE *goto_bb = cfg->Get_bb(goto_bb_id);

	    if (goto_bb != cfg->Fake_exit_bb()) {
	      // target BB needs a label
	      need_label[goto_bb_id] = true;
	      if (goto_bb->Labnam() == 0) goto_bb->Add_label(cfg);

	      STMTREP *branch_sr = bb->Branch_stmtrep();
	      Is_True(branch_sr != NULL, 
		      ("missing branch stmt in BB%d", bb->Id()));

	      // the following test is to screen out COMPGOTO and XGOTO, ...
	      if ((branch_sr->Op() == OPC_TRUEBR ||
		   branch_sr->Op() == OPC_FALSEBR)) {
		if (branch_sr->Label_number() != goto_bb->Labnam()) {
		  branch_sr->Set_st(NULL);
		  branch_sr->Set_label_number(goto_bb->Labnam());
		}
	      } else {
		Is_True(branch_sr->Op() == OPC_COMPGOTO ||
			branch_sr->Op() == OPC_XGOTO ||
			branch_sr->Op() == OPC_AGOTO ||
			branch_sr->Op() == OPC_IO ||
			branch_sr->Op() == OPC_REGION,
			("branch not expected in BB%d.", bb_id));
	      }
	    }
	  }
	}
      }
    }

    // update labels and phi list
    for (i = 0; i < layout_order.size(); ++i) {
      vertex_id bb_id = layout_order[i];
      BB_NODE *bb = cfg->Get_bb(bb_id);
      if (need_label[bb_id]) {
	if (bb->Label_stmtrep() == NULL) {
	  Is_True(bb->Labnam() != 0, ("missing label name."));
	  bb->Add_label_stmtrep(cfg->Mem_pool());
	}
      } else
	// must keep labels whose address is stored
	if ( bb->Labnam() > 0 && ! LABEL_addr_saved( bb->Labnam() ) )
	  bb->Remove_label_stmtrep();
      
      // DCE requires non-null PHI-list to transfer dead phi functions
      // from one block to another.
      if (bb->Phi_list() == NULL) { 
	bb->Set_phi_list(CXX_NEW(PHI_LIST(bb), cfg->Mem_pool()));
      }
    }
  }

#ifdef Is_True_On
  // Verify that the number of successors matches with the branch operator.
  {
    for (int i = 0; i < layout_order.size(); ++i) {
      vertex_id bb_id = layout_order[i];
      BB_NODE *bb = cfg->Get_bb(bb_id);
      if (bb->Branch_stmtrep() != NULL) {
	OPERATOR br_op = OPCODE_operator(bb->Branch_stmtrep()->Op());
	if (br_op == OPR_TRUEBR || br_op == OPR_FALSEBR)
	  FmtAssert(bb->Succ()->Len() == 2,
		    ("TRUEBR/FALSEBR must have 2 succs (BB%d).", bb_id));
	if (br_op == OPR_GOTO)
	  FmtAssert(bb->Succ()->Len() == 1,
		    ("GOTO must have 1 succ (BB%d).", bb_id));
      }
    }
  }
#endif

  if (trace) {
    fprintf(TFile, "====\n After CFG transformation\n====\n");
    cfg->Print(TFile, false, (unsigned) -1);
    if (cfg->Feedback())
      cfg->Feedback()->Print(TFile);
  }

  // Invalidate CFG aux info
  cfg->Invalidate_and_update_aux_info(TRUE);
}


void
zone::print(FILE *fp)
{
  fprintf(fp, "zone-id %d priority %f", id, priority());
  if (skip) fprintf(fp, " skipped:\n");
  else if (id != merged_into)
    fprintf(fp, " merged into %d:\n", merged_into);
  else
    fprintf(fp, ":\n");

  if (id == merged_into && !skip) {
    fprintf(fp, "entry "); print_edges(entry, fp);
    fprintf(fp, "clone "); print_edges(clone, fp);
    fprintf(fp, "exit  "); print_edges(exit, fp);
    fprintf(fp, "side_entry "); print_edges(side_entry, fp);
  }
}


void print_zone(FILE *fp, zone& zone)
{
  zone.print(fp);
}


struct comp_zones {
  zone_container *zones;
  bool operator()(int x, int y) {
#ifdef KEY
    /* If this function is compiled with x87 stack register,
       the result is undeterministic depending on the value
       of (*zones)[x/y].priority(). One simply solution is
       to add the following comparision to avoid any surprise
       introduce by x87 operations.       (bug#3532)
       BTW, I did not use TARG_X8664, since the following stmt
       can speed up the running time.
     */
    if( x == y ){
      return false;
    }
#endif

#if 0
    bool t = (*zones)[x].priority() > (*zones)[y].priority();

    if (t) {
      Is_True( !((*zones)[y].priority() > (*zones)[x].priority()),
	       ("vx > vy && vy > vx."));
    }
#else

    // Work around g++ bug!
    //   g++ can't compare results of two double functions.

    double vx = (*zones)[x].priority();
    double vy = (*zones)[y].priority();

    bool t = vx > vy;

    if (t) {
      Is_True( !(vy > vx),
	       ("vx > vy && vy > vx."));
    }
#endif
    return t;
  }
  comp_zones(zone_container& z):zones(&z) {}
};


void print_zone(FILE *fp, zone_container& zones)
{
  vector<int> sorted;
  int i;
  for (i = 0; i < zones.size(); ++i)
    sorted.push_back(i);
  sort(sorted.begin(), sorted.end(), comp_zones(zones));
  
  for (i = 0; i < sorted.size(); ++i) 
    zones[ sorted[i] ].print(fp);
}


// Returns true if z1 and z2's entries are mutually exclusive.
// Although some basic blocks are involved in both, allowing
// both z1 and z2 to be cloned are OK.
static bool no_bad_interference(zone& z1, zone& z2)
{
  if (z1.loop_butterfly || z2.loop_butterfly) return false;

  vector<edge> t;
  insert_iterator<vector<edge> > ins_t(t, t.begin());
  
  set_intersection(z1.entry.begin(), z1.entry.end(),
		   z2.clone.begin(), z2.clone.end(), ins_t);
  if (t.begin() != t.end()) return false;

  set_intersection(z1.entry.begin(), z1.entry.end(),
		   z2.exit.begin(),  z2.exit.end(), ins_t);
  if (t.begin() != t.end()) return false;
    
  set_intersection(z1.entry.begin(), z1.entry.end(),
		   z2.entry.begin(), z2.entry.end(), ins_t);
  if (t.begin() != t.end()) return false;
    
  set_intersection(z2.entry.begin(), z2.entry.end(),
		   z1.clone.begin(), z1.clone.end(), ins_t);
  if (t.begin() != t.end()) return false;
    
  set_intersection(z2.entry.begin(), z2.entry.end(),
		   z1.exit.begin(),  z1.exit.end(), ins_t);
  if (t.begin() != t.end()) return false;
    
  return true;
}

// Returns true if after merging of z1 and z2, there is no extra
// paths going into either z1 or z2, i.e., the optimization enabled
// by restructuring is intact.
static bool can_be_merged(zone& z1, zone& z2)
{
  if (z1.loop_butterfly || z2.loop_butterfly) return false;

  vector<edge> t;
  insert_iterator<vector<edge> > ins_t(t, t.begin());

  set_intersection(z1.side_entry.begin(), z1.side_entry.end(),
		   z2.clone.begin(), z2.clone.end(), ins_t);
  if (t.begin() != t.end()) return false;
  
  set_intersection(z1.side_entry.begin(), z1.side_entry.end(),
		   z2.entry.begin(), z2.entry.end(), ins_t); 
  if (t.begin() != t.end()) return false;

  set_intersection(z2.side_entry.begin(), z2.side_entry.end(),
		   z1.clone.begin(), z1.clone.end(), ins_t);
  if (t.begin() != t.end()) return false;

  set_intersection(z2.side_entry.begin(), z2.side_entry.end(),
		   z1.entry.begin(), z1.entry.end(), ins_t);
  if (t.begin() != t.end()) return false;
      
  return true;
}


//  Merge z2 into z1
static void merge_zone(zone& z1, zone& z2)
{
  vector<edge> entry, clone, exit, side_entry;
  insert_iterator<vector<edge> > ins_entry(entry, entry.begin());
  insert_iterator<vector<edge> > ins_clone(clone, clone.begin());
  insert_iterator<vector<edge> > ins_exit(exit, exit.begin());
  insert_iterator<vector<edge> > ins_side_entry(side_entry,
						side_entry.begin());

  set_union(z1.entry.begin(), z1.entry.end(),
	    z2.entry.begin(), z2.entry.end(), ins_entry);
  set_union(z1.clone.begin(), z1.clone.end(),
	    z2.clone.begin(), z2.clone.end(), ins_clone);
  set_union(z1.exit.begin(),  z1.exit.end(),
	    z2.exit.begin(),  z2.exit.end(),  ins_exit);
  set_union(z1.side_entry.begin(), z1.side_entry.end(),
	    z2.side_entry.begin(), z2.side_entry.end(), ins_side_entry);
  
  z1.entry.erase(z1.entry.begin(), z1.entry.end());
  z1.clone.erase(z1.clone.begin(), z1.clone.end());
  z1.exit.erase(z1.exit.begin(), z1.exit.end());
  z1.side_entry.erase(z1.side_entry.begin(), z1.side_entry.end());

  z1.clone.insert(z1.clone.begin(), clone.begin(), clone.end());

  insert_iterator<zone::edge_container> ins_z1_entry(z1.entry,
						     z1.entry.begin());
  insert_iterator<zone::edge_container> ins_z1_exit(z1.exit, z1.exit.begin());
  
  set_difference(entry.begin(), entry.end(),
		 clone.begin(), clone.end(), ins_z1_entry);
  set_difference( exit.begin(),  exit.end(),
		 clone.begin(), clone.end(), ins_z1_exit);

  z1.profit += z2.profit;
  z1.code_expansion_saved = unique_bb_count(z1.clone,z1.exit);
  z2.merged_into = z1.id;
}


// A data structure to "quickly" find out the set of zones that
// the cur_zone interfere with, i.e., Compile-time enhancement.
//
class interference_cache {
  vector<int> zones;
  map<vertex_id, set<int> > belongs_to;

  void find_interference_from_edge(edge e, set<int>& interfered_zones) {
    map<vertex_id, set<int> >::iterator t1 = belongs_to.find(first(e));
    map<vertex_id, set<int> >::iterator t2 = belongs_to.find(second(e));
    if (t1 != belongs_to.end()) 
      interfered_zones.insert((*t1).second.begin(), (*t1).second.end());
    if (t2 != belongs_to.end()) 
      interfered_zones.insert((*t2).second.begin(), (*t2).second.end());
  }

  void add_edge(edge e, int cur_zone_id) {
    if (belongs_to.find(first(e)) == belongs_to.end())
      belongs_to[first(e)] = set<int>();
    if (belongs_to.find(second(e)) == belongs_to.end())
      belongs_to[second(e)] = set<int>();
    belongs_to[first(e)].insert(cur_zone_id);
    belongs_to[second(e)].insert(cur_zone_id);
  }
  
public:
  void find_interference_zones(zone& cur_zone, set<int>& interfered_zones) {
    int j;
    for (j = 0; j < cur_zone.clone.size(); ++j) 
      find_interference_from_edge(cur_zone.clone[j], interfered_zones);
    for (j = 0; j < cur_zone.exit.size(); ++j) 
      find_interference_from_edge(cur_zone.exit[j], interfered_zones);
  }
  
  void add_zone(zone& cur_zone) {
    int cur_zone_id = cur_zone.id;
    int j;
    for (j = 0; j < cur_zone.clone.size(); ++j) 
      add_edge(cur_zone.clone[j], cur_zone_id);
    for (j = 0; j < cur_zone.exit.size(); ++j) 
      add_edge(cur_zone.exit[j], cur_zone_id);
  }
};


// Returns true if a zone is clonable
static bool zone_is_clonable(zone& z, CFG *cfg, const BVECTOR &vol)
{
  // check code expansion limit
  if (z.code_expansion() > WOPT_Enable_CFG_Opt_Limit)
    return false;
  
  bool clone_loop = (z.loop_butterfly != 0);
  zone::iterator e;
  for (e = z.clone.begin(); e != z.clone.end(); ++e) {      
    vertex_id from = (*e).first;
    vertex_id to = (*e).second;
    if (!cfg->Get_bb(from)->Clonable(clone_loop, &vol)) return false;
    if (!cfg->Get_bb(to)->Clonable(clone_loop, &vol)) return false;
  }
  for (e = z.entry.begin(); e != z.entry.end(); ++e) {      
    vertex_id to = (*e).second;
    if (!cfg->Get_bb(to)->Clonable(clone_loop, &vol)) return false;
  }
  for (e = z.exit.begin(); e != z.exit.end(); ++e) {      
    vertex_id from = (*e).first;
    if (!cfg->Get_bb(from)->Clonable(clone_loop, &vol)) return false;
  }
  return true;
}


// Sort zone based on their priority.
// Merge interfered but compatible zones.
// Remove interfered but incompatible zones.
//
void sort_merge_and_delete_zones(zone_container& zones, CFG *cfg, bool trace)
{
  OPT_POOL_Push(cfg->Loc_pool(), -1);
  {
    // "vol" maps each coderep id to a boolean value, which is true whenever
    // the coderep contains any reference to a volatile value.
    //
    BVECTOR vol(cfg->Htable()->Coderep_id_cnt()+1,
		bool(FALSE), 
		BVECTOR_ALLOCATOR(cfg->Loc_pool()));

    Set_volatile_map(cfg, vol);

    vector<int> sorted;
    int i;
    for (i = 0; i < zones.size(); ++i) {
      sorted.push_back(i);
      zones[i].canonicalize();
    }
    sort(sorted.begin(), sorted.end(), comp_zones(zones));

    interference_cache zones_will_be_cloned;
    
    for (i = 0; i < sorted.size(); ++i) {

      int cur_zone_id = sorted[i];
      zone *cur_zone = &zones[cur_zone_id];
      if (trace)
	fprintf(TFile, "priority %f\n", zones[cur_zone_id].priority());

      if (!zone_is_clonable(*cur_zone, cfg, vol)) {
	if (trace)
	  fprintf(TFile, "zone %d is not clonable.\n", cur_zone_id);
	(*cur_zone).skip = true;
	continue;
      }
    
      set<int> interfered_zones;  // set of zone id
      zones_will_be_cloned.find_interference_zones(*cur_zone, 
						   interfered_zones);

      bool skip = false;
      vector<int> need_merge;  // set of zone id
      for (set<int>::iterator k = interfered_zones.begin(); 
	   k != interfered_zones.end();
	   ++k) {
	int zone_id = *k;
	if (zones[zone_id].merged_into == zone_id) {
	  if (!can_be_merged(*cur_zone, zones[zone_id])) {
	    if (no_bad_interference(*cur_zone, zones[zone_id])) {
	      // continue checking
	    } else {
	      if (trace)
		fprintf(TFile, 
			"zone %d skipped due to overlapping with zone %d\n",
			(*cur_zone).id, zone_id);
	      skip = true;
	      break;
	    }
	  } else {
	    need_merge.push_back(zone_id);
	  }
	}
      }
      if (need_merge.size() > 1) 
	if (!WOPT_Enable_CFG_Merge_Multi_Zone ||
	    (Cur_PU_Feedback && !WOPT_Enable_CFG_Merge_Multi_Zone_Set)) {
	  if (trace)
	    fprintf(TFile, 
		    "zone %d skipped due to overlapping with multiples zones\n",
		    (*cur_zone).id);
	  skip = true;
	}
      if (!skip) {
	for (vector<int>::iterator k = need_merge.begin(); 
	     k != need_merge.end();
	     ++k) {
	  int zone_id = *k;
	  if (trace)
	    fprintf(TFile, "merging zone %d and zone %d\n",
		    (*cur_zone).id, zone_id);
	  merge_zone(*cur_zone, zones[zone_id]);
	}
	zones_will_be_cloned.add_zone(*cur_zone);
      }
      (*cur_zone).skip = skip;
    }
  }
  OPT_POOL_Pop(cfg->Loc_pool(), -1);
}

static void remove_SCF(BB_NODE *header)
{
  // erase high level information
  BB_LOOP *loop = header->Loop();
  BB_NODE *bb;
  BB_NODE_SET_ITER bb_iter;
  FOR_ALL_ELEM(bb, bb_iter, Init(loop->True_body_set())) {
    switch (bb->Kind()) {
    case BB_DOSTART:
    case BB_DOEND:
    case BB_DOHEAD:
    case BB_DOSTEP:
    case BB_DOTAIL:
    case BB_REPEATBODY: 
    case BB_REPEATEND: 
    case BB_WHILEEND:
      bb->Set_kind(BB_GOTO);
    }
  }
}


// Invoke various zone generation routines to gather a list of zone
// to operate on.
void
generate_zones(COMP_UNIT *cu, successor_graph &g, zone_container& zones,
	       bool do_butterfly, bool trace, bool display)
{
  if (WOPT_Enable_CFG_Opt1)
    generate_conditional_const_zones(cu, g, zones, trace);
  
  if (do_butterfly)
    generate_loop_butterfly_zones(cu, g, zones,
				  WOPT_Enable_CFG_Opt2_Limit, trace);

  if (trace) {
    fprintf(TFile, ("set of clone zones before merging:\n"));
    print_zone(TFile, zones);
  }
  sort_merge_and_delete_zones(zones, cu->Cfg(), trace);
  if (trace) {
    fprintf(TFile, ("set of clone zones after merging:\n"));
    print_zone(TFile, zones);
  }

  for (zone_iterator ri = zones.begin(); ri != zones.end(); ++ri) {
    if ((*ri).loop_butterfly) {
      edge e = *((*ri).entry.begin());
      remove_SCF(cu->Cfg()->Get_bb(e.second));
    }
  }

#ifdef Is_True_On  
  if (display) show_all_zones(g, zones.begin(), zones.end());
#endif
}


// If a loop have multiple backedge, convert the loop into a 
// double nested loop.
//  Before:   old_preheader -> old_header 
//
//  After:    old_preheader -> new_preheader 
//            new_preheader -> new_header
//            (old_header is disconnected)
//
//   -- change edge (*,old_header) to (*,new_preheader) 
//      where edge is in zone.{entry,clone,exit,side_entry}.
//   -- add edge (new_preheader, new_header)
//
//
static void connect_butterfly_zone(successor_graph& g, zone& z, 
				   vertex_id old_header, vertex_id new_header,
				   vertex_id new_preheader,
				   OPT_FEEDBACK *feedback)
{
  vector<edge> edge_incident_to_header;
  insert_iterator<vector<edge> > ins(edge_incident_to_header,
				     edge_incident_to_header.begin());
  zone::iterator e;
  for (e = z.entry.begin(); e != z.entry.end(); ++e) {
    if ((*e).second == old_header) 
      *ins++ = (*e);
  }
  for (e = z.clone.begin(); e != z.clone.end(); ++e) {
    if ((*e).second == old_header)
      *ins++ = (*e);
  }
  for (e = z.exit.begin(); e != z.exit.end(); ++e) {
    if ((*e).second == old_header)
      *ins++ = (*e);
  }
  for (e = z.side_entry.begin(); e != z.side_entry.end(); ++e) {
    if ((*e).second == old_header)
      *ins++ = (*e);
  }

  {
    for (vector<edge>::iterator e =  edge_incident_to_header.begin();
	 e !=  edge_incident_to_header.end();
	 ++e) {
      vertex_id from = (*e).first;
      vertex_id to = (*e).second;
      edge *fix = find_edge(g, from, to);
      (*fix).second = new_preheader;
    }
  }
  add_edge(g, edge(new_preheader, new_header, true));

  if (feedback) {
    feedback->Split_node(old_header, new_preheader);
    feedback->Move_edge_dest(new_preheader, old_header, new_header);
  }
}


// Connect original graph to a cloned acyclic zone.
//
static void connect_acyclic_zone(successor_graph& g, zone& z, 
				 map<vertex_id,vertex_id>& old_to_new,
				 OPT_FEEDBACK *feedback) 
{
  for (zone::iterator e = z.entry.begin();
       e != z.entry.end();
       ++e) {
    vertex_id from = (*e).first;
    vertex_id to = (*e).second;
    edge *fix = find_edge(g, from, to);
    Is_True(fix, ("cannot update entry edge (%d,%d).", from, to));
    (*fix).second = old_to_new[to];

    if (feedback) {
      feedback->Move_edge_dest(from, to, old_to_new[to]);
      // vertex freq of "to" and "old_to_new[to]" are already updated in
      // adjust_feedback_for_cloned_zone
    }
  }
}

// Generate a new basic block id for each bb in the head(clone)+head(exit)
// and maintain a map from old to new.
// Update must_fall_thru from g.
// Modify tail(edge) in g that are the entries
// translate cloned edges using old to new mapping 
// translate the head of exit edges using old to new mapping
//
static void
clone_zones(successor_graph& g, vector<vertex_id>& entry, 
	    zone_iterator first, zone_iterator last, CFG *cfg,
	    bool trace, bool display)
{
  vertex_id new_id = cfg->Total_bb_count();
  map<vertex_id, vertex_id> new_to_old;

  if (trace) {
    fprintf(TFile, "before clone_zone:\n");
    print_nodes(g, TFile);
    print_edges(g, TFile);
  }

  for (zone_iterator ri = first; ri != last; ++ri) {

    if ((*ri).skip) continue;
    if ((*ri).id != (*ri).merged_into) continue;
    
    map<vertex_id, vertex_id> old_to_new; 

    zone::iterator e;
    for (e = (*ri).clone.begin(); e != (*ri).clone.end(); ++e) {
      vertex_id from = (*e).first;
      vertex_id to   = (*e).second;
      if (old_to_new.find(from) == old_to_new.end()) {
	old_to_new[from] = new_id++;
      }
      if (old_to_new.find(to) == old_to_new.end()) {
	old_to_new[to] = new_id++;
      }
      edge* old_edge = find_edge(g, from, to);
      add_edge(g, edge(old_to_new[from], old_to_new[to],
		       old_edge->must_fall_thru));
    }

    for (e = (*ri).exit.begin(); e != (*ri).exit.end(); ++e) {
      vertex_id from = (*e).first;
      vertex_id to   = (*e).second;
      if (old_to_new.find(from) == old_to_new.end()) {
	old_to_new[from] = new_id++;
      }
      edge* old_edge = find_edge(g, from, to);
      add_edge(g, edge(old_to_new[from], to, old_edge->must_fall_thru));
    }
    Is_True(new_id == g.size(), ("new_id != g.size()"));

    // Feedback adjustment
    if ( cfg->Feedback() ) {
      cfg->Feedback()->Clone_zone( *ri, old_to_new );
    }

    vertex_id new_preheader = new_id++;
    if ((*ri).loop_butterfly) {
      vertex_id header = (*ri).loop_butterfly;
      connect_butterfly_zone(g, *ri, header, old_to_new[header],
			     new_preheader, cfg->Feedback());
    } else 
      connect_acyclic_zone(g, *ri, old_to_new, cfg->Feedback());

    for (map<vertex_id,vertex_id>::iterator mi = old_to_new.begin();
	 mi != old_to_new.end();
	 ++mi) {
      vertex_id o = (*mi).first;
      vertex_id n = (*mi).second;
      new_to_old[n] = o;
    }
  }

  vector<bool> reachable(g.size(), false);
  for (vector<vertex_id>::iterator p = entry.begin();
       p != entry.end();
       ++p) 
    find_reachable_vertex_set(g, *p, reachable);

  vector<edge> g_tmp;
  subgraph(g, g_tmp, reachable);
  erase(g);
  copy(g_tmp, g);

  if (trace) {
    fprintf(TFile, "after clone_zone:\n");
    print_nodes(g, TFile);
    print_edges(g, TFile);
    fprintf(TFile, "translation: ");
    for (vertex_id i = 0; i < g.size(); i++) 
      if (reachable[i] && new_to_old[i] != 0) 
	fprintf(TFile, "%d<-%d ", i, new_to_old[i]);
    fprintf(TFile, "\n");
    if (cfg->Feedback())
      cfg->Feedback()->Print(TFile);
  }

  while (cfg->Total_bb_count() < g.size()) 
    cfg->Create_and_allocate_bb(BB_GOTO);

#ifdef Is_True_On
  if (display) show_graph(g);
#endif

  vertex_id i;
  for (i = 0; i < g.size(); i++) 
    if (reachable[i] &&	new_to_old[i] != 0) 
      cfg->Clone_bb(new_to_old[i] /*src*/, i /*dest*/, FALSE);

#ifdef Is_True_On
  if (display) {
    mark_attr_begin();
    for (i = 0; i < g.size(); i++) 
      if (reachable[i] && new_to_old[i] != 0) 
	mark_translated_vertex(new_to_old[i], i);
    mark_attr_end();
  }
#endif
}

void
CFG_transformation(COMP_UNIT *cu, bool do_butterfly, bool trace, bool display)
{
  CFG *cfg = cu->Cfg();
  cfg->Analyze_loops();

  successor_graph g;
  vector<vertex_id> entry;
  bool ok = build_successor_graph(cfg, g, 
				  insert_iterator<vector<vertex_id> >
				  (entry, entry.begin()));
  if (!ok) {
    if (trace)
      fprintf(TFile, ("skip CFG transformation because of REGION."));
    return;
  }
  if (trace) {
    fprintf(TFile, "Successor graph:\n");
    print_nodes(g, TFile);
    fprintf(TFile, "edges: \n");
    for (successor_graph::iterator e = g.begin(); e != g.end(); ++e)
      fprintf(TFile, "(%d,%d)%c ",
	      first(*e), second(*e), (*e).must_fall_thru?'y':'n');
    fprintf(TFile,"\n");
  }

  vector<zone> zones;
  generate_zones(cu, g, zones, do_butterfly, trace, display);
  clone_zones(g, entry, zones.begin(), zones.end(), cfg, trace, display);

  reconstruct_CFG(g, cfg, trace);

  cfg->Invalidate_loops();
  cfg->Analyze_loops();
}

// Reset/clear fields.
void
IF_MERGE_TRANS::Clear(void)
{
  _cu = NULL;
  _trace = FALSE;
  _dump = FALSE;
  _count = 0;
  _val_map = NULL;
  _true_val = NULL;
  _pool = NULL;
  _action = DO_NONE;
  _pass = PASS_NONE;
  
}

// Query whether given aux_id represents a scalar non-address-taken non-virtual variable.

BOOL IF_MERGE_TRANS::Is_trackable_var(AUX_ID aux_id)
{
  OPT_STAB * op_stab = _cu->Opt_stab();

  if (aux_id && (aux_id <= op_stab->Lastidx())) {
    AUX_STAB_ENTRY * aux_entry = op_stab->Aux_stab_entry(aux_id);
    if (aux_entry && (aux_entry->Stype() == VT_NO_LDA_SCALAR)) {
      ST * st = aux_entry->St();

      if (st && (ST_class(st) == CLASS_VAR) 
	  && !ST_addr_passed(st)
	  && !ST_addr_saved(st))
	return TRUE;
    }
  }
  return FALSE;
}

// For every load in the expression tree rooted at the given wn, check whether
// it loads a scalar trackable variable.  See IF_MERGE_TRANS::Is_trackable_var.

BOOL IF_MERGE_TRANS::Is_trackable_expr(WN * wn)
{
  int i;
  OPCODE opc = WN_opcode(wn);
  if (!OPCODE_is_expression(opc))
    return FALSE;

  if ((opc == OPC_IO) || OPCODE_is_call(opc)) 
    return FALSE;

  if (OPCODE_is_load(opc)) {
    if (OPERATOR_is_scalar_load(WN_operator(wn))) {
      AUX_ID aux_id = WN_aux(wn);
      if (!Is_trackable_var(aux_id)) 
	return FALSE;
    }
    else
      return FALSE;
  }

  for ( i = 0; i < WN_kid_count(wn); i++) 
    if (!Is_trackable_expr(WN_kid(wn,i)))
      return FALSE;

  return TRUE;
}


// Query whether the values of loads in the given expression tree 
// are modified by the given SC_NODE.  We perform a quick and simple
// value-number hashing to the given SC_NODE. 
// eval_true indicates whether wn is evaluated to TRUE at the entry
// of sc.
BOOL IF_MERGE_TRANS::Val_mod(SC_NODE * sc, WN * wn, BOOL eval_true)
{
  BOOL ret_val = TRUE;

  OPT_POOL_Push(_pool, MEM_DUMP_FLAG + 1);
  Init_val_map(wn, eval_true);

  if (_val_map != NULL) {
    Track_val(sc, sc->First_bb(), wn);
    ret_val = !Val_match(wn);
  }

  Delete_val_map();
  OPT_POOL_Pop(_pool, MEM_DUMP_FLAG + 1);
  return ret_val;
}

// Query whether the values of loads in the given expression tree match
// values hashed in _val_map.
BOOL IF_MERGE_TRANS::Val_match(WN * wn)
{
  FmtAssert((_val_map != NULL), ("Expect non-NULL _val_map"));
  
  if (OPERATOR_is_scalar_load(WN_operator(wn))) {
    AUX_ID aux_id = WN_aux(wn);
    AUX_ID val = (AUX_ID) Get_val(aux_id);
    if (val != aux_id)
      return FALSE;
  }

  for (int i = 0; i < WN_kid_count(wn); i++)
    if (!Val_match(WN_kid(wn,i)))
      return FALSE;

  return TRUE;
}

// Free storage of _val_map.
void
IF_MERGE_TRANS::Delete_val_map()
{
  if (_val_map) {
    CXX_DELETE(_val_map, _pool);
    _val_map = NULL;
  }

  if (_true_val)
    _true_val = NULL;
}

// Initialize _val_map by hashing all loads in the given wn.
// eval_true indicates whether wn is evaluated to TRUE.
void
IF_MERGE_TRANS::Init_val_map(WN * wn, BOOL eval_true)
{
  if (OPERATOR_is_scalar_load(WN_operator(wn))) {
    AUX_ID aux_id = WN_aux(wn);
    if (aux_id) {
      if (_val_map == NULL)
	_val_map = CXX_NEW(MAP(CFG_BB_TAB_SIZE, _pool), _pool);
      _val_map->Add_map((POINTER) aux_id, (POINTER)aux_id);
    }
  }

  OPERATOR op = WN_operator(wn);

  // Infer non-zero values
  if (eval_true
      && ((op == OPR_NE) || (op == OPR_EQ))
      && (WN_operator(WN_kid1(wn)) == OPR_INTCONST)
      && OPERATOR_is_scalar_load(WN_operator(WN_kid0(wn)))) {
    AUX_ID aux_id = WN_aux(WN_kid0(wn));
    INT64 const_val = WN_const_val(WN_kid1(wn));

    if (((op == OPR_NE) && (const_val == 0))
	|| ((op == OPR_EQ) && (const_val != 0))) {
      if (_true_val == NULL) 
	_true_val = BS_Create_Empty(_cu->Opt_stab()->Lastidx() + 1, _pool);

      _true_val = BS_Union1D(_true_val, aux_id, _pool);
    }
  }

  for ( int i = 0; i < WN_kid_count(wn); i++) 
    Init_val_map(WN_kid(wn, i), FALSE);
}

// Obtain the hashed value number for the given AUX_ID.
AUX_ID
IF_MERGE_TRANS::Get_val(AUX_ID aux_id)
{
  AUX_ID val = 0;
  if (aux_id)
    val = (AUX_ID) (unsigned long) _val_map->Get_val((POINTER) aux_id);
  return val;
}

// Hash aux_id to val.
void IF_MERGE_TRANS::Set_val(AUX_ID aux_id, AUX_ID val)
{
  if (aux_id) {
    MAP_LIST * map_lst = _val_map->Find_map_list((POINTER) aux_id);
    if (map_lst)
      map_lst->Set_val((POINTER) val);
  }
}

// Interface to invoke alias info queries.
BOOL IF_MERGE_TRANS::Is_aliased(WN * wn1, WN * wn2)
{
  ALIAS_MANAGER * alias_mgr = _cu->Alias_mgr();
  WN * call_wn = NULL;
  WN * load_wn = NULL;

  if (WN_operator(wn1) == OPR_CALL)
    call_wn = wn1;
  else if (WN_operator(wn2) == OPR_CALL)
    call_wn = wn2;

  if (OPERATOR_is_scalar_load(WN_operator(wn1)))
    load_wn = wn1;
  else if (OPERATOR_is_scalar_load(WN_operator(wn2)))
    load_wn = wn2;

  if (Aliased(alias_mgr, wn1, wn2) != NOT_ALIASED) {
    if (call_wn && load_wn) {
      AUX_ID load_aux = WN_aux(load_wn);
      if (Is_trackable_var(load_aux)) {
	OPT_STAB * opt_stab = _cu->Opt_stab();
	ST * load_st = opt_stab->Aux_stab_entry(load_aux)->St();
	ST * call_st = WN_sym(call_wn);
	INT mod = 0;
	INT ref = 0;
      
	opt_stab->check_ipa_mod_ref_info(call_st, load_st, &mod, &ref);
	if (mod == 0)
	  return FALSE;

	if (Is_Global_Symbol(load_st)
	    && _true_val
	    && BS_MemberP(_true_val, load_aux)) {
	  INT same_entry_exit_value_or_1 = 0;	  
	  opt_stab->check_ipa_same_entry_exit_value_or_1_info(call_st, load_st,
							      &same_entry_exit_value_or_1);
	  if (same_entry_exit_value_or_1)
	    return FALSE;
	}
      }
    }

    return TRUE;
  }

  return FALSE;
}

// Given a WN, query whether all of its kids and itself can be speculative.

BOOL IF_MERGE_TRANS::Can_be_speculative(WN * wn)
{
  INT i;
  struct ALIAS_MANAGER * alias = _cu->Alias_mgr();
  
  for (i=0; i<WN_kid_count(wn); i++) {
    if (!Can_be_speculative(WN_kid(wn,i)))
      return FALSE;
  }

  OPERATOR op = WN_operator(wn);

  if (OPERATOR_is_store(op) || OPERATOR_is_load(op)) {
    if (WN_Is_Volatile_Mem(wn))
      return FALSE;
  }
  if ((op == OPR_ALLOCA) || (op == OPR_DEALLOCA)
      || (op == OPR_ASM_STMT)
      || (op == OPR_FORWARD_BARRIER) || (op == OPR_BACKWARD_BARRIER))
    return FALSE;
  
  if (OPCODE_is_call(WN_opcode(wn)))
    return FALSE;

  return TRUE;
}

// Given a BB_NODE, query whether all of its real statements can be speculative.
BOOL
IF_MERGE_TRANS::Can_be_speculative(BB_NODE * bb)
{
  WN * tmp;

  for (tmp = bb->Firststmt(); tmp != NULL; tmp = WN_next(tmp)) {
    if ((WN_operator(tmp) == OPR_LABEL)
	|| (WN_operator(tmp) == OPR_PRAGMA))
      continue;
    
    if (!Can_be_speculative(tmp))
      return FALSE;
  }

  return TRUE;
}

// Given a SC_NODE, query whether all of its BB_NODEs can be speculative.
BOOL
IF_MERGE_TRANS::Can_be_speculative(SC_NODE * sc)
{
  BB_LIST_ITER bb_list_iter;
  BB_NODE * tmp;

  tmp = sc->Get_bb_rep();

  if ((tmp != NULL) && !Can_be_speculative(tmp))
    return FALSE;

  FOR_ALL_ELEM(tmp, bb_list_iter,Init(sc->Get_bbs())) {
    if (!Can_be_speculative(tmp))
      return FALSE;
  }

  SC_LIST_ITER sc_list_iter(sc->Kids());
  SC_NODE * sc_tmp;

  FOR_ALL_ELEM(sc_tmp, sc_list_iter, Init()) {
    if (!Can_be_speculative(sc_tmp))
      return FALSE;
  }
  
  return TRUE;
}

// For every load in wn, if it aliases with wn_iter,
// remove its value from _val_map

void IF_MERGE_TRANS::Remove_val(WN * wn_iter, WN * wn)
{
  FmtAssert((_val_map != NULL), ("Expect non-NULL _val_map"));
  OPCODE opc = WN_opcode(wn);
  
  if (OPCODE_is_load(opc)) {
    if (Is_aliased(wn_iter, wn))
      Set_val(WN_aux(wn), 0);
  }
  
  for (int i = 0; i < WN_kid_count(wn); i++) 
    Remove_val(wn_iter, WN_kid(wn,i));
}

// Track the flow of values for loads in the given WN tree by 
// evaluating all blocks in the given sc in source order. 
// bb_entry gives the initial Gen block of tracked values.
//
// Current implementation is quick and light-weight.
// In the future, consider using existing value-numbering
// mechanism.
 
void IF_MERGE_TRANS::Track_val(SC_NODE * sc, BB_NODE * bb_entry, WN * wn)
{

  FmtAssert(_val_map, ("Expect non-NULL _val_map"));
  BB_NODE * bb = sc->Get_bb_rep();

  if (bb != NULL)
    Track_val(bb, bb_entry, wn);
  
  BB_LIST * bb_list = sc->Get_bbs();

  if (bb_list != NULL) {
    BB_LIST_ITER bb_list_iter(bb_list);
    BB_NODE * tmp;
    
    FOR_ALL_ELEM(tmp, bb_list_iter, Init()) {
      Track_val(tmp, bb_entry, wn);
    }
  }

  SC_LIST * kids = sc->Kids();
  
  if (kids != NULL) {
    SC_LIST_ITER sc_list_iter(kids);
    SC_NODE *tmp;
    FOR_ALL_ELEM(tmp, sc_list_iter, Init()) {
      Track_val(tmp, bb_entry, wn);
    }
  }
}

// Track the flow of values for loads in the given WN tree by
// evaluating all statements in the given bb_cur. bb_entry gives
// the initial Gen block of tracked values.

void IF_MERGE_TRANS::Track_val(BB_NODE * bb_cur, BB_NODE * bb_entry, WN * wn)
{
  STMT_ITER stmt_iter;
  WN * wn_iter;
  ALIAS_MANAGER * alias_mgr = _cu->Alias_mgr();

  FOR_ALL_ELEM (wn_iter, stmt_iter, Init(bb_cur->Firststmt(), bb_cur->Laststmt())) {
    OPCODE opc = WN_opcode(wn_iter);

    // Check kills.
    if (OPCODE_is_call(opc) || (opc == OPC_IO))
      Remove_val(wn_iter, wn);

    if (OPERATOR_is_scalar_store(WN_operator(wn_iter))) {
      WN * wn_data = WN_kid0(wn_iter);
      AUX_ID rval = WN_has_sym(wn_data) && 
      		    OPERATOR_is_load(WN_operator(wn_data)) ?
					    Get_val(WN_aux(wn_data)) : 0;
      AUX_ID lval = Get_val(WN_aux(wn_iter));
      
      if (Is_trackable_var(WN_aux(wn_iter)) && rval
	  && bb_entry->Dominates(bb_cur)
	  && bb_cur->Postdominates(bb_entry)) {
	// For a store to a trackable variable in a block that is control-equivalent
	// to the bb_entry, transfer the RHS value (if exists) to the LHS.
	Set_val(WN_aux(wn_iter), rval);
      }
      else if (lval)
	Set_val(WN_aux(wn_iter), 0);
    }
  }
}

// Do top-down if-merging for the SC tree rooted at the given SC_NODE.
// CFG, LOOPs and SC tree are updated upon exit of this routine.

void
IF_MERGE_TRANS::Top_down_trans(SC_NODE * sc)
{
  SC_LIST * kids = sc->Kids();

  if (kids == NULL)
    return;

  CFG * cfg = _cu->Cfg();
  BOOL do_analyze_loops = cfg->Loops_valid(); 

  SC_LIST_ITER kids_iter;
  SC_NODE * tmp;
  SC_NODE * sc1 = NULL;
  SC_NODE * sc2 = NULL;
  SC_NODE * sc_new;

  // Do if-collapsing first. See IF_MERGE_TRANS::Is_uncond_cand.
  _action = DO_IFCOLLAPSE;
  sc1 = sc->First_kid_of_type(SC_IF);

  if (sc1 != NULL) {
    sc2 = sc1->Next_sibling_of_type(SC_IF);
    
    while (sc2 != NULL) {
      sc_new = Do_merge(sc1, sc2);

      if (sc_new) {
	sc1 = sc_new;
	sc2 = sc1->Next_sibling_of_type(SC_IF);
      }
      else {
	sc1 = sc2;
	sc2 = sc1->Next_sibling_of_type(SC_IF);
      }
    }
  }

  // Do if-merging
  _action = DO_IFMERGE;
  sc1 = sc->First_kid_of_type(SC_IF);
  while (sc1 != NULL) {
    sc2 = sc1->Next_sibling_of_type(SC_IF);
    
    while (sc2 != NULL) {
      sc_new = Do_merge(sc1, sc2);
      
      if (sc_new) {
	sc1 = sc_new;
	sc2 = sc1->Next_sibling_of_type(SC_IF);
      }
      else
	sc2 = sc2->Next_sibling_of_type(SC_IF);
    }

    sc1 = sc1->Next_sibling_of_type(SC_IF);
  }
  
  FOR_ALL_ELEM(tmp, kids_iter, Init(sc->Kids())) {
    this->Top_down_trans(tmp);
  }

  if (do_analyze_loops && !cfg->Loops_valid()) 
    cfg->Analyze_loops();
}

// Lower level if-merging of two if-regions.
// Upon exit of this routine, CFG are updated.
// Loops are invalidated but not updated.
void
IF_MERGE_TRANS::Merge_CFG(SC_NODE * sc1, SC_NODE * sc2)
{
  FmtAssert(((sc1->Type() == SC_IF) && (sc2->Type() == SC_IF)), ("Expect SC_Ifs"));
  CFG * cfg = _cu->Cfg();
  MEM_POOL * pool = cfg->Mem_pool();

  // find boundary BB_NODEs

  BB_NODE * sc1_then_end = sc1->Then_end();
  BB_NODE * sc1_else_end = sc1->Else_end();
  BB_NODE * sc1_else = sc1->Else();
  BB_NODE * sc1_merge = sc1->Merge();
  BB_NODE * sc2_then = sc2->Then();
  BB_NODE * sc2_else = sc2->Else();
  BB_NODE * sc2_then_end = sc2->Then_end();
  BB_NODE * sc2_else_end = sc2->Else_end();
  BB_NODE * sc2_merge = sc2->Merge();
  BB_NODE * sc2_head = sc2->Get_bb_rep();

  if (_trace) {
    if (_count == 0) {
      printf("\n*********** If-merge for %s(%d) ***********\n",
	     Current_PU_Name(), Current_PU_Count());
    }
    
    printf("\n\t If-merge (SC%" PRIdPTR ",SC%" PRIdPTR ")\n", 
	   sc1->Id(), sc2->Id());
  }

  if (_dump) {
    fprintf(TFile, "\n Before if-merge\n");
    cfg->Print(TFile, false, (unsigned) -1) ;
  }

  sc1_then_end->Replace_succ(sc1_merge, sc2_then);

  if (cfg->Feedback())
    cfg->Feedback()->Move_edge_dest(sc1_then_end->Id(), sc1_merge->Id(), sc2_then->Id());

  sc1_else_end->Replace_succ(sc1_merge, sc2_else);

  if (cfg->Feedback())
    cfg->Feedback()->Move_edge_dest(sc1_else_end->Id(), sc1_merge->Id(), sc2_else->Id());
  
  sc2_then->Replace_pred(sc2_head, sc1_then_end);
  
  if (cfg->Feedback())
    cfg->Feedback()->Delete_edge(sc2_head->Id(), sc2_then->Id());

  sc2_else->Replace_pred(sc2_head, sc1_else_end);

  if (cfg->Feedback())
    cfg->Feedback()->Delete_edge(sc2_head->Id(), sc2_else->Id());

  sc2_then_end->Replace_succ(sc2_merge, sc1_merge);
  
  if (cfg->Feedback())
    cfg->Feedback()->Move_edge_dest(sc2_then_end->Id(), sc2_merge->Id(), sc1_merge->Id());

  sc2_else_end->Replace_succ(sc2_merge, sc1_merge);
  
  if (cfg->Feedback())
    cfg->Feedback()->Move_edge_dest(sc2_else_end->Id(), sc2_merge->Id(), sc1_merge->Id());

  sc1_merge->Replace_pred(sc1_then_end, sc2_then_end);
  sc1_merge->Replace_pred(sc1_else_end, sc2_else_end);

  sc2_head->Remove_succ(sc2_then, pool);
  sc2_head->Replace_succ(sc2_else, sc2_merge);

  sc2_merge->Remove_pred(sc2_then_end, pool);
  sc2_merge->Replace_pred(sc2_else_end, sc2_head);

  if (cfg->Feedback()) {
    cfg->Feedback()->Add_edge(sc2_head->Id(), sc2_merge->Id(), FB_EDGE_OUTGOING, 
			      cfg->Feedback()->Get_edge_freq(sc2_then_end->Id(), sc1_merge->Id())
			      + cfg->Feedback()->Get_edge_freq(sc2_else_end->Id(), sc1_merge->Id()));

  }
  sc2_head->Set_ifinfo(NULL);

  // remove conditional branch

  WN * last_stmt = sc2_head->Laststmt();
  WN * prev_stmt = WN_prev(last_stmt);

  if (prev_stmt)
    WN_next(prev_stmt) = NULL;
  WN_prev(last_stmt) = NULL;

  sc2_head->Set_laststmt(prev_stmt);

  if (prev_stmt == NULL)
    sc2_head->Set_firststmt(NULL);

  sc2_head->Set_kind(BB_GOTO);

  // Fix Prev() and Next()
  sc1_then_end->Set_next(sc2_then);
  sc2_then->Set_prev(sc1_then_end);

  sc2_then_end->Set_next(sc1_else);
  sc1_else->Set_prev(sc2_then_end);

  sc1_else_end->Set_next(sc2_else);
  sc2_else->Set_prev(sc1_else_end);
  
  sc2_else_end->Set_next(sc1_merge);
  sc1_merge->Set_prev(sc2_else_end);
  
  sc2_head->Set_next(sc2_merge);
  sc2_merge->Set_prev(sc2_head);

  cfg->Invalidate_and_update_aux_info(FALSE);
  cfg->Invalidate_loops();

}

// Merge sc2 into sc1:
// sc1 takes over sc2's kids, unlink sc2 from the SC tree.

void
IF_MERGE_TRANS::Merge_SC(SC_NODE * sc1, SC_NODE * sc2)
{
  FmtAssert((sc1->Type() == sc2->Type()), ("Expect same SC type"));
  SC_LIST * kids2 = sc2->Kids();

  // If sc2 has no kids, simply unlink it from the SC tree.
  if (kids2 == NULL) {
    sc2->Unlink();
    return;
  }
  
  // If Both sc1's last kid and sc2's first kid are SC_BLOCKs, merge BB_NODEs
  SC_NODE * last1 = sc1->Last_kid();
  SC_NODE * first2 = sc2->First_kid();

  if ((last1 != NULL) && (last1->Type() == first2->Type())
      && (last1->Type() == SC_BLOCK)) {
    BB_LIST * bbs = first2->Get_bbs();
    MEM_POOL * pool = first2->Get_pool();

    while (bbs) {
      BB_NODE * bb = bbs->Node();
      last1->Append_bbs(bb);
      bbs = bbs->Remove(bb, pool);
    }
    
    first2->Set_bbs(NULL);
    first2->Unlink();
  }

  // Merge remaining kids from sc2 into sc1.
  first2 = sc2->First_kid();

  while (first2) {
    sc1->Append_kid(first2);
    first2->Set_parent(sc1);
    sc2->Remove_kid(first2);
    first2 = sc2->First_kid();
  }
  
  sc2->Unlink();
  sc2->Delete();
}

// Attempt if-merging for the given pair of SC_NODEs.  If successful, return
// the merged SC_NODE.
SC_NODE *
IF_MERGE_TRANS::Do_merge(SC_NODE * sc1, SC_NODE * sc2)
{
  if ((WOPT_Enable_If_Merge_Limit >= 0)
      && (Count() >= WOPT_Enable_If_Merge_Limit))
    return NULL;

  if (!Is_cand(sc1, sc2, FALSE))
    return NULL;

  // Merge CFG BBs.
  Merge_CFG(sc1, sc2);

  CFG * cfg = _cu->Cfg();

  if (_dump) {
    fprintf(TFile, "\n====SC tree before if-merge====\n");
    cfg->SC_root()->Print(TFile,1);
  }
  
  // Merge SC tree.
  SC_NODE * then1 = sc1->Find_kid_of_type(SC_THEN);
  SC_NODE * else1 = sc1->Find_kid_of_type(SC_ELSE);
  SC_NODE * then2 = sc2->Find_kid_of_type(SC_THEN);
  SC_NODE * else2 = sc2->Find_kid_of_type(SC_ELSE);

  Merge_SC(then1, then2);
  Merge_SC(else1, else2);

  sc2->Convert(SC_BLOCK);
  
  // Converting sc2 into a SC_BLOCK could expose opportunities to 
  // merge SC_BLOCKs among sc2's immediate siblings.

  SC_NODE * sc_blk = sc2->Prev_sibling();
  
  if ((sc_blk == NULL) || (sc_blk->Type() != SC_BLOCK))
    sc_blk = sc2;
  
  FmtAssert((sc_blk->Kids() == NULL), ("SC_BLOCK should have no kid"));
  
  SC_NODE * next_sibling = sc_blk->Next_sibling();
  
  while ((next_sibling) && (next_sibling->Type() == SC_BLOCK)) {
    // merge next_sibling's BB_NODES into sc_blk.
    BB_LIST * bb_list = next_sibling->Get_bbs();
    SC_NODE * next = next_sibling->Next_sibling();
    MEM_POOL * pool = next_sibling->Get_pool();
    
    FmtAssert((next_sibling->Kids() == NULL), ("SC_BLOCK should have no kid"));
    
    while (bb_list) {
      BB_NODE * bb = bb_list->Node();
      bb_list = bb_list->Remove(bb, pool);
      sc_blk->Append_bbs(bb);
    }
    
    next_sibling->Unlink();
    next_sibling = next;
  }

  if (_dump) {
    fprintf(TFile, "\n====SC tree After if-merge====\n");
    cfg->SC_root()->Print(TFile,1);
  }

  FmtAssert(sc1->Is_well_behaved(), ("Not well-behaved after if-merge"));
  Inc_count();
  return sc1;
}

// Query whether loads/stores in the WN tree rooted at wn_root could alias with wn1,
// where wn1 is a store/call statement.

BOOL
IF_MERGE_TRANS::Maybe_assigned_expr(WN * wn1, WN * wn_root)
{
  OPCODE opc = WN_opcode(wn_root);

  if (OPCODE_is_load(opc) || OPCODE_is_store(opc) || (opc == OPC_IO)) {
    OPCODE opc = WN_opcode(wn1);

    // OPC_IO has side effect and hidden control flow.
    if (opc == OPC_IO)
      return TRUE;

    ALIAS_MANAGER * alias_mgr = _cu->Alias_mgr();

    if ((OPCODE_is_store(opc) || OPCODE_is_call(opc))
	&& Is_aliased(wn1, wn_root))
      return TRUE;
  }

  for ( int i = 0; i < WN_kid_count(wn_root); i++) {
    if (Maybe_assigned_expr(wn1, WN_kid(wn_root,i)))
      return TRUE;
  }

  return FALSE;
}

// Query whether loads/stores in the WN tree rooted at wn_root could alias with
// a store/call statement in bb.

BOOL
IF_MERGE_TRANS::Maybe_assigned_expr(BB_NODE * bb, WN * wn_root)
{
  STMT_ITER stmt_iter;
  WN * wn_iter;

  FOR_ALL_ELEM (wn_iter, stmt_iter, Init(bb->Firststmt(), bb->Laststmt())) {
    if (Maybe_assigned_expr(wn_iter, wn_root))
      return TRUE;
  }

  return FALSE;
}

// Query whether loads/stores in the WHILE tree rooted at wn_root could
// alias with a store/call statement  in the SC tree rooted at sc. 
// eval_true indicates whether wn_root is evaluated to TRUE at the entry
// of the sc.

BOOL
IF_MERGE_TRANS::Maybe_assigned_expr(SC_NODE * sc, WN * wn_root, BOOL eval_true)
{
  if (Is_trackable_expr(wn_root)) {
    // Track whether values of loads in the WHILR tree rooted at wn_root
    // are modified by sc
    if (Val_mod(sc, wn_root, eval_true))
      return TRUE;
    else
      return FALSE;
  }

  BB_NODE * bb = sc->Get_bb_rep();

  if ((bb != NULL) && Maybe_assigned_expr(bb, wn_root))
    return TRUE;
  
  BB_LIST * bb_list = sc->Get_bbs();

  if (bb_list != NULL) {
    BB_LIST_ITER bb_list_iter(bb_list);
    BB_NODE * tmp;
    
    FOR_ALL_ELEM(tmp, bb_list_iter, Init()) {
      if (Maybe_assigned_expr(tmp, wn_root))
	return TRUE;
    }
  }

  SC_LIST * kids = sc->Kids();
  
  if (kids != NULL) {
    SC_LIST_ITER sc_list_iter(kids);
    SC_NODE *tmp;
    FOR_ALL_ELEM(tmp, sc_list_iter, Init()) {
      if (Maybe_assigned_expr(tmp, wn_root, FALSE))
	return TRUE;
    }
  }

  return FALSE;
}

// Query whether loads/stores of WN trees in bb could alias with a store/call
// statement in the SC tree rooted at sc.

BOOL
IF_MERGE_TRANS::Maybe_assigned_expr(SC_NODE *sc1, BB_NODE *bb)
{
  STMT_ITER stmt_iter;
  WN * wn_iter;

  FOR_ALL_ELEM (wn_iter, stmt_iter, Init(bb->Firststmt(), bb->Laststmt())) {
    if ((WN_operator(wn_iter) == OPR_FALSEBR)
	|| (WN_operator(wn_iter) == OPR_TRUEBR))
      wn_iter = WN_kid0(wn_iter);

    if (Maybe_assigned_expr(sc1, wn_iter, FALSE))
      return TRUE;
  }

  return FALSE;
}

// Query whether loads/stores of WN trees in bb2 could alias with a store/call
// statement in bb1.

BOOL
IF_MERGE_TRANS::Maybe_assigned_expr(BB_NODE *bb1, BB_NODE *bb2)
{
  STMT_ITER stmt_iter;
  WN * wn_iter;

  FOR_ALL_ELEM (wn_iter, stmt_iter, Init(bb2->Firststmt(), bb2->Laststmt())) {
    if (Maybe_assigned_expr(bb1, wn_iter))
      return TRUE;
  }

  return FALSE;
}

// Query whether loads/stores of WN tree in SC tree rooted at sc could alias
// with a store/call statement in bb.

BOOL
IF_MERGE_TRANS::Maybe_assigned_expr(BB_NODE * bb, SC_NODE * sc)
{
  BB_NODE * tmp = sc->Get_bb_rep();
  
  if ((tmp != NULL) && Maybe_assigned_expr(bb, tmp))
    return TRUE;

  BB_LIST * bb_list = sc->Get_bbs();

  if (bb_list != NULL) {
    BB_LIST_ITER bb_list_iter(bb_list);
    BB_NODE * tmp;
    
    FOR_ALL_ELEM(tmp, bb_list_iter, Init()) {
      if (Maybe_assigned_expr(bb, tmp))
	return TRUE;
    }
  }
  
  SC_LIST * kids = sc->Kids();

  if (kids != NULL) {
    SC_LIST_ITER sc_list_iter(kids);
    SC_NODE *tmp;
    FOR_ALL_ELEM(tmp, sc_list_iter, Init()) {
      if (Maybe_assigned_expr(bb, tmp))
	return TRUE;
    }
  }
  
  return FALSE;
}

// Query whether loads/stores of WN trees in the SC tree rooted at sc2 could alias
// with a store/call statement in the SC tree rooted at sc1.

BOOL
IF_MERGE_TRANS::Maybe_assigned_expr(SC_NODE * sc1, SC_NODE * sc2)
{
  BB_NODE * bb = sc2->Get_bb_rep();

  if ((bb != NULL) && Maybe_assigned_expr(sc1, bb))
    return TRUE;

  BB_LIST * bb_list = sc2->Get_bbs();
  
  if (bb_list != NULL) {
    BB_LIST_ITER bb_list_iter(bb_list);
    BB_NODE * tmp;

    FOR_ALL_ELEM(tmp, bb_list_iter, Init()) {
      if (Maybe_assigned_expr(sc1, tmp))
	return TRUE;
    }
  }

  SC_LIST * kids = sc2->Kids();

  if (kids != NULL) {
    SC_LIST_ITER sc_list_iter(kids);
    SC_NODE *tmp;
    FOR_ALL_ELEM(tmp, sc_list_iter, Init()) {
      if (Maybe_assigned_expr(sc1, tmp))
	return TRUE;
    }
  }
  
  return FALSE;
}

// Check whether sc2 can be collapsed into sc1
// Look for the pattern like below. This can be considered as a special
// case of if-merging.
// if () {
//   ......
//   x = const1;
// }
// else {
//   ......
//   x = const2;
// }
// if (x == const1) {
//   ......
// }
// else {
//   ......
// }
// where the value of const1 is not equal to the value of const2.
BOOL
IF_MERGE_TRANS::Is_if_collapse_cand(SC_NODE * sc1, SC_NODE * sc2)
{
  if (sc1->Next_sibling() != sc2)
    return FALSE;

  WN * wn;
  BB_NODE * head2 = sc2->Head();

  // Count non-label statements in head2
  STMT_ITER stmt_iter;
  int count = 0;

  FOR_ALL_ELEM (wn, stmt_iter, Init(head2->Firststmt(), head2->Laststmt())) {
    if ((WN_opcode(wn) != OPC_LABEL) && (WN_opcode(wn) != OPC_PRAGMA))
      count++;
  }
  
  // head2 should only contain a comparison on EQ between a I4 constant and a scalar load.
  if (count != 1)
    return FALSE;

  wn = head2->Laststmt();
  wn = WN_kid0(wn);
  if (WN_operator(wn) != OPR_EQ)
    return FALSE;

  WN * wn_const = NULL;
  WN * wn_load = NULL;

  for ( int i = 0; i < WN_kid_count(wn); i ++) {
    WN * kid = WN_kid(wn,i);
    if (WN_operator(kid) == OPR_INTCONST)
      wn_const = kid;
    else if (OPERATOR_is_scalar_load(WN_operator(kid)))
      wn_load = kid;
  }

  if (!wn_const || !wn_load)
    return FALSE;
  
  // Check whether both then-path and else-path of sc1 end with a scalar store of a
  // constant value, the store has the same symbol as wn_load, the constant value
  // on the then-path is the same as wn_const, and the constant value on the else-path
  // is different from wn_const.
  
  BB_NODE * then_end = sc1->Then_end();
  BB_NODE * else_end = sc1->Else_end();

  wn = then_end->Laststmt();
  if (!wn 
      || !OPERATOR_is_scalar_store(WN_operator(wn))
      || (WN_aux(wn) != WN_aux(wn_load))
      || (WN_operator(WN_kid0(wn)) != OPR_INTCONST)
      || (WN_const_val(WN_kid0(wn)) != WN_const_val(wn_const)))
    return FALSE;

  wn = else_end->Laststmt();
  if (!wn
      || !OPERATOR_is_scalar_store(WN_operator(wn))
      || (WN_aux(wn) != WN_aux(wn_load))
      || (WN_operator(WN_kid0(wn)) != OPR_INTCONST)
      || (WN_const_val(WN_kid0(wn)) == WN_const_val(wn_const)))
    return FALSE;

  return TRUE;
}

// Check whether the given pair of SC_NODEs are if-merging candidates.
// where, sc1 precedes sc2 in source order.  Do not invoke tail duplication
// transformation if do_query is TRUE.

BOOL
IF_MERGE_TRANS::Is_cand(SC_NODE * sc1, SC_NODE * sc2, BOOL do_query)
{
  if ((sc1->Type() != SC_IF) || (sc2->Type() != SC_IF))
    return FALSE;

  // should have the same parent in the SC tree.
  if (sc1->Parent() != sc2->Parent())
    return FALSE;

  BB_NODE * rep1 = sc1->Get_bb_rep();
  BB_NODE * rep2 = sc2->Get_bb_rep();

  BB_IFINFO * info1 = rep1->Ifinfo();
  BB_IFINFO * info2 = rep2->Ifinfo();

  BB_NODE * tail1 = info1->Merge();
  BB_NODE * tail2 = info2->Merge();
  BB_NODE * head1 = rep1;
  BB_NODE * head2 = rep2;

  if (tail1->Pred()->Len() != 2)
    return FALSE;

  if (tail2->Pred()->Len() != 2)
    return FALSE;
  
  // tail1 should be control-equivalent to head2.
  if ((tail1 != head2)
      && (!tail1->Dominates(head2) || !head2->Postdominates(tail1)))
    return FALSE;

  if (!sc1->Is_well_behaved())
    return FALSE;

  if (!sc2->Is_well_behaved())
    return FALSE;

  // If merge SC is a SC_BLOCK, it should be a SESE.

  SC_NODE * next_sibling = sc1->Next_sibling();

  if (next_sibling
      && (next_sibling->Type() == SC_BLOCK)
      && !next_sibling->Is_sese())
    return FALSE;

  next_sibling = sc2->Next_sibling();
  
  if (next_sibling
      && (next_sibling->Type() == SC_BLOCK)
      && !next_sibling->Is_sese())
    return FALSE;
  
  // Check whether sc2 can be if-collapsed with sc1.
  // See IF_MERGE_TRANS:Is_if_collapse_cand.
  if (_action == DO_IFCOLLAPSE) {
    if (Is_if_collapse_cand(sc1, sc2)) 
      return TRUE;
    else
      return FALSE;
  }

  // Should have the same condition expression value
  WN * cond1 = head1->Laststmt();
  WN * cond2 = head2->Laststmt();  
  OPCODE op1 = WN_opcode(cond1);
  OPCODE op2 = WN_opcode(cond2);

  FmtAssert(((op1 == OPC_FALSEBR) || (op1 == OPC_TRUEBR)), ("Unexpect cond"));
  FmtAssert(((op2 == OPC_FALSEBR) || (op2 == OPC_TRUEBR)), ("Unexpect cond"));
  
  if (op1 != op2)
    return FALSE;

  WN * expr1 = WN_kid0(cond1);
  WN * expr2 = WN_kid0(cond2);

  if (WN_Simp_Compare_Trees(expr1, expr2) != 0)
    return FALSE;

  SC_LIST_ITER sc_list_iter;
  SC_NODE* tmp;

  // sc1' then-path and else-path should not modify condition expression.
  FOR_ALL_ELEM(tmp, sc_list_iter, Init(sc1->Kids())) {
    if (Maybe_assigned_expr(tmp, expr1, (tmp->Type() == SC_THEN) ? TRUE : FALSE))
      return FALSE;
  }

  // All siblings between sc1 and sc2 (exclusive) should not
  // modify condition expression.
  next_sibling = sc1->Next_sibling();
  
  while (next_sibling && (next_sibling != sc2)) {
    if (Maybe_assigned_expr(next_sibling, expr2, FALSE))
      return FALSE;
    next_sibling = next_sibling->Next_sibling();
  }

  // head2 should not modifiy condition expression.
  if (Maybe_assigned_expr(head2, expr2))
    return FALSE;

  // sc2's then-path and else-path should have no dependency on head2
  // excluding the conditional branch.

  if (head2->Real_stmt_count() > 1) {
    BOOL has_non_sp = !Can_be_speculative(head2);

    FOR_ALL_ELEM(tmp, sc_list_iter, Init(sc2->Kids())) {
      if (Has_dependency(tmp, head2))
	return FALSE;
      // Do not reorder operations that can not be speculative.
      if (has_non_sp && !Can_be_speculative(tmp))
	return FALSE;
    }
  }

  // For every sibling between sc1 and sc2 (exclusive), if it has 
  // dependency on sc2, and all siblings are SC_BLOCKs, do tail
  // duplication.

  BOOL has_dep = FALSE;
  BOOL all_blk = TRUE;
  BOOL has_non_sp = FALSE;
  BOOL all_sese = TRUE;

  next_sibling = sc1->Next_sibling();
  int count = 0;

  while (next_sibling && (next_sibling != sc2)) {
    count++;

    FOR_ALL_ELEM(tmp, sc_list_iter, Init(sc2->Kids())) {
      if (Has_dependency(next_sibling, tmp))
	has_dep = TRUE;
    }

    if (!Can_be_speculative(next_sibling))
      has_non_sp = TRUE;

    if (next_sibling->Type() != SC_BLOCK)
      all_blk = FALSE;
    else if (!next_sibling->Is_sese())
      all_sese = FALSE;

    next_sibling = next_sibling->Next_sibling();
  }

  if (!has_dep) {
    // Do not reorder operations that can not be speculative.
    if (has_non_sp && !Can_be_speculative(sc2))
      return FALSE;
    else
      return TRUE;
  }

  if (!all_blk || !all_sese)
    return FALSE;

  if ((WOPT_Enable_Tail_Dup_Limit >= 0)
      && ((_tail_dup->Transform_count() + count) >= WOPT_Enable_Tail_Dup_Limit))
    return FALSE;

  // Heuristic: in the global pass, avoid tail duplication unless both
  // SC_NODEs contain loop.

  if ((_pass == PASS_GLOBAL)
      && (!sc1->Has_loop() || !sc2->Has_loop()))
    return FALSE;

  if (!do_query) {
    next_sibling = sc1->Next_sibling();
  
    while (next_sibling && (next_sibling != sc2)) {
      _tail_dup->Do_tail_duplication(next_sibling, sc1);
      next_sibling = sc1->Next_sibling();
    }
  }
  
  return TRUE;
}

// Query whether sc1 and sc2 have dependency on each other.
BOOL
IF_MERGE_TRANS::Has_dependency(SC_NODE * sc1, SC_NODE * sc2)
{
  if (sc1->Type() == SC_IF) {
    // Recursively query TRUE path and FALSE path
    // to detect value equality on two paths separately.
    if (Has_dependency(sc1->First_kid(), sc2)
	|| Has_dependency(sc2->Last_kid(), sc2))
      return TRUE;

    // Check head of SC_IF.
    BB_NODE * head = sc1->Get_bb_rep();

    if (Maybe_assigned_expr(head, sc2)
	|| Maybe_assigned_expr(sc2, head))
      return TRUE;
  }

  else  if (Maybe_assigned_expr(sc1, sc2)
	    || Maybe_assigned_expr(sc2, sc1))
    return TRUE;

  return FALSE;
}

// Query whether SC_NODE and BB_NODE have dependency on each other.
BOOL
IF_MERGE_TRANS::Has_dependency(SC_NODE * sc, BB_NODE * bb)
{
  if (sc->Type() == SC_IF) {
    if (Has_dependency(sc->First_kid(), bb)
	|| Has_dependency(sc->Last_kid(), bb))
      return TRUE;

    // Check head of SC_IF.
    BB_NODE * head = sc->Get_bb_rep();

    if (Maybe_assigned_expr(head, bb)
	|| Maybe_assigned_expr(bb, head))
      return TRUE;
  }
  else if (Maybe_assigned_expr(sc, bb)
	   || Maybe_assigned_expr(bb, sc))
    return TRUE;

  return FALSE;

}

// Collect classified loops for the SC tree rooted at sc.

void
PRO_LOOP_FUSION_TRANS::Collect_classified_loops(SC_NODE * sc)
{
  if ((sc->Type() == SC_LOOP) && (sc->Class_id() != 0)) {
    if (_loop_list == NULL)
      _loop_list = (SC_LIST *) CXX_NEW(SC_LIST(sc), _pool);
    else
      _loop_list->Append(sc, _pool);
  }

  SC_LIST_ITER sc_list_iter(sc->Kids());
  SC_NODE * tmp;
  FOR_ALL_ELEM(tmp, sc_list_iter, Init()) {
    Collect_classified_loops(tmp);
  }
}

// Check whether it is worthy to do proactive loop fusion transformation
// for the given SC_NODE. Currently we avoid loops of small trip counts.

#define TRIP_COUNT_THRESHOLD 1000

BOOL
PRO_LOOP_FUSION_TRANS::Is_worthy(SC_NODE * sc)
{
  FmtAssert(sc->Type() == SC_LOOP, ("Expect a SC_LOOP."));

  SC_NODE * sc_start = sc->Find_kid_of_type(SC_LP_START);
  SC_NODE * sc_end = sc->Find_kid_of_type(SC_LP_COND);
  SC_NODE * sc_step = sc->Find_kid_of_type(SC_LP_STEP);

  if (!sc_start || !sc_end || !sc_step)
    return FALSE;

  BB_NODE * bb_start = sc_start->First_bb();
  BB_NODE * bb_end = sc_end->First_bb();
  BB_NODE * bb_step = sc_step->First_bb();

  if (!bb_start || !bb_end || !bb_step)
    return FALSE;

  WN * wn_start = bb_start->Laststmt();  
  WN * wn_end = bb_end->Laststmt();
  WN * wn_step = bb_step->Laststmt();

  if (wn_step && (WN_operator(wn_step) == OPR_GOTO))
    wn_step = WN_prev(wn_step);

  if (!wn_start || !wn_end || !wn_step)
    return FALSE;

  WN * wn_lower = NULL;
  WN * wn_upper = NULL;
  WN * wn_incr = NULL;
  WN * wn_kid;
  WN * wn_kid0;
  WN * wn_kid1;
  OPERATOR opr;
  OPCODE op;

  // Find trip count lower bound.
  if (WN_operator(wn_start) == OPR_STID) 
    wn_lower = WN_kid0(wn_start);

  // Find trip count upper bound.
  if ((WN_operator(wn_end) == OPR_FALSEBR)
      || (WN_operator(wn_end) == OPR_TRUEBR)) {
    wn_kid = WN_kid0(wn_end);
    opr = WN_operator(wn_kid);
    op = WN_opcode(wn_kid);

    if (((opr == OPR_EQ) || (opr == OPR_NE) 
	 || (opr == OPR_LT) || (opr == OPR_LE)
	 || (opr == OPR_GT) || (opr == OPR_GE))
	&& MTYPE_is_integral(OPCODE_desc(op))) {
      wn_kid0 = WN_kid0(wn_kid);
      wn_kid1 = WN_kid1(wn_kid);
      wn_upper = (WN_operator(wn_kid0) == OPR_LDID) ? wn_kid1 :
	((WN_operator(wn_kid1) == OPR_LDID) ? wn_kid0 : NULL);
    }
  }
  
  // Find step.
  if (wn_step && (WN_operator(wn_step) == OPR_STID)) {
    wn_kid = WN_kid0(wn_step);
    opr = WN_operator(wn_kid);

    if ((opr == OPR_ADD) || (opr == OPR_SUB)) {
      wn_kid0 = WN_kid0(wn_kid);
      wn_kid1 = WN_kid1(wn_kid);
      wn_incr = (WN_operator(wn_kid0) == OPR_LDID) ? wn_kid1 :
	((WN_operator(wn_kid1) == OPR_LDID) ? wn_kid0 : NULL);
    }
  }
  
  if (wn_lower && wn_upper && wn_incr) {
    if ((WN_operator(wn_lower) == OPR_INTCONST)
	&& (WN_operator(wn_step) == OPR_INTCONST)
	&& (WN_operator(wn_upper) == OPR_INTCONST)) {
      int trip_count = abs(WN_const_val(wn_upper) - WN_const_val(wn_lower))
	/ abs(WN_const_val(wn_incr));
      
      if (trip_count < TRIP_COUNT_THRESHOLD)
	return FALSE;
    }
  }

  return TRUE;
}

// Find a pair of tail duplication candidates for the SC_tree rooted at sc_root.
// a non-NULL sc_begin gives the initial search point in _loop_list.
// This routine can only be called from PRO_LOOP_FUSION_TRANS::Top_down_trans.
void
PRO_LOOP_FUSION_TRANS::Find_cand
(
 SC_NODE *  sc_root, 
 SC_NODE ** cand1_ptr, 
 SC_NODE ** cand2_ptr, 
 SC_NODE *  sc_begin
)
{
  SC_NODE * cand1 = NULL;
  SC_NODE * cand2 = NULL;
  SC_NODE * tmp1;
  SC_NODE * tmp2;
  SC_LIST * list1;
  SC_LIST * list2;

  // Find a pair of loops such that the pair (loop1 and loop2)
  // 1.1 Has the same loop class id.
  // 1.2 Is a single entry single exit
  // 1.3 LCP is sc_root.
  // 1.4 Not control equivalent or not adjacent to each 
  // 1.5 No SC_LOOP on the path from loop1 to LCP, and on the path from
  //    loop2 to LCP excluding loop1 and loop2.  (Future implementation
  //    can relax this rule if loop transformation is invoked in this
  //    transformation).
  // 
  // When such a pair is found, walk up SC tree to find a pair of
  // transformation candidates such that the pair (cand1 and cand2)
  // 2.1 Is kid of LCP.
  // 2.2 SC_TYPE is a SC_IF or a SC_LOOP.
  // 2.3 For all the sibling nodes between cand1 and cand2 EXCLUSIVE,
  //    - It must be a SC_BLOCK or a SC_IF.
  //    - In the case of a SC_BLOCK, it should have no dependency on loop1.
  //      It can be speculative.  It should have a single-entry and single-exit.
  //      It should also have a single successor that has a single predecessor.
  //      If cand1 is a loop, cand1 should be control equivalent to the SC_BLOCK
  //      for safe code motion.  The requirement of single successor is to make
  //      sure the successor can become a merge block for a loop after the code
  //      motion or a merge block for a SC_IF after tail duplication.
  //    - In the case of a SC_IF, there must exist at least one path that does not
  //      have dependency on both loop1 and loop2. 
  //
  // Limit is used to control code size bloat due to head/tail duplication.
  // 
  // 2.4 For all sibling nodes between cand1 and cand2 INCLUSIVE, 
  //     - If it is a SC_IF, it must be well-behaved; its head has no dependency
  //       on preceding siblings unless the preceding sibling is a SC_IF that can 
  //       be if-merged with this SC_IF and its merge has no dependency on succeeding
  //       siblings unless the succeeding SC_IF is a SC_IF that can be if-merged with
  //       this SC_IF.   This is a legality check for head/tail duplication.  There 
  //       should exist at most one non single-entry-singe-exit (SESE) SC_IFs since we
  //       don't duplicate non-SESE SC_IFs to avoid complexity.  If cand2 is a loop, 
  //       its merge block must have a single predecessor so that it can be tail-duplicated
  //       into the SC_IF.

  for (list1 = _loop_list; list1; list1 = list1->Next()) {
    tmp1 = list1->Node();

    SC_NODE * prev_sibling = tmp1->Prev_sibling();
    if (prev_sibling && (prev_sibling->Class_id() == tmp1->Class_id()))
      continue;

    if ((sc_begin != NULL) && (tmp1 != sc_begin))
      continue;

    if (!sc_root->Is_pred_in_tree(tmp1))
      continue;

    // Condition 1.2
    if (!tmp1->Is_sese())
      continue;

    // Condition 1.5
    if (tmp1->Num_of_loops(sc_root, TRUE, TRUE) != 0)
      continue;

    // heuristic to avoid unprofitable loops.
    if (!Is_worthy(tmp1))
      continue;

    // Condition 1.2
    if (tmp1->Is_sese()) {
      for (list2 = list1->Next(); list2; list2 = list2->Next()) {
	tmp2 = list2->Node();

	if (!sc_root->Is_pred_in_tree(tmp2))
	  continue;

	// Condition 1.2
	if (!tmp2->Is_sese())
	  continue;

	// Condition 1.5
	if (tmp2->Num_of_loops(sc_root, TRUE, TRUE) != 0)
	  continue;

	// Condition 1.1
	if (tmp1->Class_id() == tmp2->Class_id()) {
	  SC_NODE * lcp = tmp1->Find_lcp(tmp2);
	  
	  // Condition 1.3
	  if (lcp == sc_root) {
	    BB_NODE * bb1 = tmp1->First_bb();
	    BB_NODE * bb2 = tmp2->First_bb();

	    // Condition 1.4
	    if (!bb1->Dominates(bb2)
		|| !bb2->Postdominates(bb1)
		|| tmp1->Next_sibling() != tmp2) {
	      cand1 = tmp1;
	      cand2 = tmp2;
	      
	      // Condition 2.1
	      while (cand1->Parent() != lcp)
		cand1 = cand1->Parent();

	      while (cand2->Parent() != lcp)
		cand2 = cand2->Parent();

	      // Condition 2.2
	      if (!Is_cand_type(cand1->Type())
		  || !Is_cand_type(cand2->Type())) {
		cand1 = NULL;
		cand2 = NULL;
		continue;
	      }

	      // Condition 2.4, inclusive check.
	      SC_NODE * sc_tmp1 = cand1;
	      SC_NODE * sc_tmp2;
	      int non_sese_count = 0;

	      while (sc_tmp1) {
		if (sc_tmp1->Type() == SC_IF) {
		  if (!sc_tmp1->Is_well_behaved()) {
		    cand1 = NULL;
		    cand2 = NULL;
		    break;
		  }

		  if (!sc_tmp1->Is_sese()) {
		    non_sese_count++;
		    if (non_sese_count > 1) {
		      cand1 = NULL;
		      cand2 = NULL;
		      break;
		    }
		  }

		  // Check preceding siblings		  
		  BB_NODE * bb_head = sc_tmp1->Head();
		  sc_tmp2 = cand1;

		  while (sc_tmp2) {
		    if (sc_tmp2 == sc_tmp1)
		      break;
		    
		    if ((sc_tmp2->Type() == SC_IF)
			&& _if_merge->Is_cand(sc_tmp2, sc_tmp1, TRUE)) {
		    }
		    else if (_if_merge->Has_dependency(sc_tmp2, bb_head)) {
		      cand1 = NULL;
		      cand2 = NULL;
		      break;
		    }
		    sc_tmp2 = sc_tmp2->Next_sibling();
		  }
		  
		  if ((cand1 == NULL) || (cand2 == NULL))
		    break;

		  // Check succeeding siblings
		  sc_tmp2 = sc_tmp1->Next_sibling();
		  BB_NODE * bb_merge = sc_tmp1->Merge();

		  while (sc_tmp2) {
		    if ((sc_tmp2->Type() == SC_IF)
			&& _if_merge->Is_cand(sc_tmp1, sc_tmp2, TRUE)) {

		    }
		    else if (_if_merge->Has_dependency(sc_tmp2, bb_merge)) {
		      cand1 = NULL;
		      cand2 = NULL;
		      break;
		    }

		    if (sc_tmp2 == cand2)
		      break;
		    sc_tmp2 = sc_tmp2->Next_sibling();
		  }
		  
		  if ((cand1 == NULL) || (cand2 == NULL))
		    break;

		  if ((cand2->Type() == SC_LOOP)
		      && (cand2->Merge()->Pred()->Multiple_bbs())) {
		    cand1 = NULL;
		    cand2 = NULL;
		    break;
		  }
		}

		if (sc_tmp1 == cand2)
		  break;

		sc_tmp1 = sc_tmp1->Next_sibling();
	      }

	      if ((cand1 == NULL) || (cand2 == NULL))
		continue;

	      // Condition 2.3, exclusive check.
	      SC_NODE * next = cand1->Next_sibling();
	      int stmt_count = 0;  
	      int orig_stmt_count = 0;
	      
	      while (next != cand2) {
		SC_TYPE next_type = next->Type();
		
		if (next_type == SC_BLOCK) {
		  BB_NODE * next_last_bb = next->Last_bb();
		  if (!next_last_bb->Succ()
		      || next_last_bb->Succ()->Multiple_bbs()
		      || next_last_bb->Succ()->Node()->Pred()->Multiple_bbs()
		      ) {
		    cand1 = NULL;
		    cand2 = NULL;
		    break;
		  }

		  if (!next->Is_sese()) {
		    cand1 = NULL;
		    cand2 = NULL;
		    break;
		  }

		  BB_LIST_ITER bb_list_iter;
		  BB_NODE * tmp;

		  if (cand1->Type() == SC_LOOP) {
		    BB_NODE * cand1_first_bb = cand1->First_bb();

		    FOR_ALL_ELEM(tmp, bb_list_iter,Init(next->Get_bbs())) {
		      if (!cand1_first_bb->Dominates(tmp)
			  || !tmp->Postdominates(cand1_first_bb)) {
			cand1 = NULL;
			cand2 = NULL;
			break;
		      }
		    }
		    if (cand1 == NULL)
		      break;
		  }
		  
		  // No dependency on loop1.
		  if (_if_merge->Has_dependency(tmp1, next)) {
		    cand1 = NULL;
		    cand2 = NULL;
		    break;
		  }

		  // Can be speculative
		  if (!_if_merge->Can_be_speculative(next)) {
		    cand1 = NULL;
		    cand2 = NULL;
		    break;
		  }
		}
		else if (next_type == SC_IF) {
		  int i;
		  BOOL find_path = FALSE;
		  for (i = 0; i <= 1; i++) {
		    SC_NODE * sc_tmp = (i == 0) ? next->First_kid() : next->Last_kid();
		    if (!_if_merge->Has_dependency(sc_tmp, tmp1)
			&& !_if_merge->Has_dependency(sc_tmp, tmp2)) {
		      find_path = TRUE;
		      break;
		    }
		  }

		  if (!find_path) {
		    cand1 = NULL;
		    cand2 = NULL;
		    break;
		  }
		  
		  // Control code size bloat due to head/tail duplication.
		  if ((WOPT_Tail_Dup_Max_Clone > 0)
		      && ((_code_bloat_count + stmt_count - orig_stmt_count)
			  >= WOPT_Tail_Dup_Max_Clone)) {
		    cand1 = NULL;
		    cand2 = NULL;
		    break;
		  }

		  // Head/tail duplication doubles statement count.
		  stmt_count *= 2;
		}
		else {
		  // Must be a SC_BLOCK or a SC_IF
		  cand1 = NULL;
		  cand2 = NULL;
		  break;
		}
		 
		int cur_count = next->Real_stmt_count();
		orig_stmt_count += cur_count;
		stmt_count += cur_count;
		next = next->Next_sibling();
	      }
		
	      if (cand1 && cand2) 
		break;
	    }
	  }
	}
      }

      if (cand1 && cand2)
	break;
    }
  }

  *cand1_ptr = cand1;
  *cand2_ptr = cand2;
}

// Move BB_NODEs in sc2 before the first BB_NODE in sc1.
// The caller of this routine should be responsible for the legality check.
void
PRO_LOOP_FUSION_TRANS::Do_code_motion(SC_NODE * sc1, SC_NODE * sc2)
{
  FmtAssert((sc1->Parent() == sc2->Parent()), ("Expect sibling SC_NODEs"));
  BB_NODE * first_bb1 = sc1->First_bb();
  BB_NODE * first_bb2 = sc2->First_bb();
  BB_NODE * last_bb2 = sc2->Last_bb();
  CFG * cfg = _cu->Cfg();

  // Other scenario not tested yet.
  FmtAssert((sc1->Type() == SC_LOOP) && (sc2->Type() == SC_BLOCK), 
	    ("TODO: test code motion"));

  // Other kinds of loops not tested yet.
  if (sc1->Type() == SC_LOOP)
    FmtAssert((sc1->Loopinfo()->Is_flag_set(LOOP_PRE_DO)), ("TODO: other loops"));    

  if (_trace) {
    printf("\n\t\t Code-motion (SC%" PRIdPTR ",SC%" PRIdPTR ")\n", 
	   sc1->Id(), sc2->Id());
  }

  BB_LIST_ITER bb_list_iter;
  BB_NODE * tmp;

  // last_bb2_succ will become new loop merge
  FmtAssert((last_bb2->Succ()->Len() == 1), ("Expect single successor"));
  BB_NODE * last_bb2_succ = last_bb2->Succ()->Node();

  FmtAssert((first_bb2->Pred()->Len() == 1), ("Expect single predecessor"));
  BB_NODE * first_bb2_pred = first_bb2->Pred()->Node();

  // Fix pred/succ
  FOR_ALL_ELEM(tmp, bb_list_iter, Init(first_bb1->Pred())) {
    if (sc1->Type() == SC_LOOP)
      FmtAssert(!sc1->Contains(tmp), ("TODO: back edge"));
    tmp->Replace_succ(first_bb1, first_bb2);

    if (cfg->Feedback()) 
      cfg->Feedback()->Move_edge_dest(tmp->Id(), first_bb1->Id(), first_bb2->Id());
  }
  
  last_bb2_succ->Replace_pred(last_bb2, first_bb2_pred);
  first_bb2_pred->Replace_succ(first_bb2, last_bb2_succ);

  if (cfg->Feedback())
    cfg->Feedback()->Move_edge_dest(first_bb2_pred->Id(), first_bb2->Id(), last_bb2_succ->Id());

  first_bb2->Set_pred(first_bb1->Pred());
  last_bb2->Replace_succ(last_bb2_succ, first_bb1);

  if (cfg->Feedback())
    cfg->Feedback()->Move_edge_dest(last_bb2->Id(), last_bb2_succ->Id(), first_bb1->Id());

  BB_LIST * new_pred = CXX_NEW(BB_LIST(last_bb2), cfg->Mem_pool());
  first_bb1->Set_pred(new_pred);

  // Fix up prev/next 
  BB_NODE * first_bb1_prev = first_bb1->Prev();
  BB_NODE * first_bb2_prev = first_bb2->Prev();
  BB_NODE * last_bb2_next = last_bb2->Next();

  first_bb1_prev->Set_next(first_bb2);
  first_bb2->Set_prev(first_bb1_prev);
  last_bb2->Set_next(first_bb1);
  first_bb1->Set_prev(last_bb2);
  
  first_bb2_prev->Set_next(last_bb2_next);
  last_bb2_next->Set_prev(first_bb2_prev);

  if (last_bb2_succ->Prev() == last_bb2)
    last_bb2_succ->Set_prev(first_bb1_prev);

  SC_NODE * parent = sc1->Parent();
  
  if (sc1->Type() == SC_LOOP) {
    // Update loop info.
    BB_NODE * merge = sc1->Merge();
    FmtAssert(merge == first_bb2, ("Unexpected merge block"));
    sc1->Loopinfo()->Set_merge(last_bb2_succ);

    // fix label on loop exit.
    cfg->Add_label_with_wn(last_bb2_succ);
    BB_NODE * bb_exit = sc1->Exit();
    WN * branch_wn = bb_exit->Branch_wn();
    FmtAssert(WN_label_number(branch_wn), ("Null label"));
    WN_label_number(branch_wn) = last_bb2_succ->Labnam();

    // Fix label on branch to sc1.
    if (first_bb1->Labnam() != 0) {
      cfg->Add_label_with_wn(first_bb2);
      
      FOR_ALL_ELEM(tmp, bb_list_iter, Init(first_bb2->Pred())) {
	FmtAssert(!sc1->Contains(tmp), ("TODO: back edge"));

	if (tmp->Is_branch_to(first_bb1)) {
	  WN * branch_wn = tmp->Branch_wn();	  
	  WN_label_number(branch_wn) = first_bb2->Labnam();
	  
	  if (parent->Type() != SC_IF) {
	    // If first_bb1 has a label WN and first_bb2 does not have
	    // a label WN, create one for first_bb2.	    
	    WN * wn_label = first_bb1->Firststmt();
	    if (wn_label && (WN_operator(wn_label) == OPR_LABEL)) {
	      wn_label = first_bb2->Firststmt();
	      if (!wn_label || (WN_operator(wn_label) != OPR_LABEL)) {
		wn_label = WN_CreateLabel(0, first_bb2->Labnam(), 0, NULL);
		cfg->Prepend_wn_in(first_bb2, wn_label);
	      }
	    }
	  }
	}
      }
    }
  }

  // swap sc1 and sc2 in their parent's kids.
  SC_LIST * cur_list;

  for (cur_list = parent->Kids(); cur_list; cur_list = cur_list->Next()) {
    SC_NODE * cur_node = cur_list->Node();
    if (cur_node == sc1)
      cur_list->Set_node(sc2);
    else if (cur_node == sc2)
      cur_list->Set_node(sc1);
  }

  // Fix previous sibling's merge info 
  SC_NODE * prev_sibling = sc2->Prev_sibling();

  if (prev_sibling) {
    BB_NODE * merge = prev_sibling->Merge();
    if (merge) {
      FmtAssert((merge == first_bb1), ("Unexpected merge block"));
      prev_sibling->Set_merge(first_bb2);
    }
  }

  // Fix parent info.
  Fix_parent_info(sc1, sc2);

  cfg->Invalidate_and_update_aux_info(FALSE);
  cfg->Invalidate_loops();
  Inc_transform_count();
}

// Insert a single-entry-single-exit region defined by (src_entry, src_exit) between
// dst_begin and dst_end.
// Before insertion, src_entry should have no predecessor, src_exit should have at most
// one successor. Also fix Prev/Next links.
void
PRO_LOOP_FUSION_TRANS::Insert_region
(
 BB_NODE * src_entry, 
 BB_NODE * src_exit, 
 BB_NODE * dst_begin, 
 BB_NODE * dst_end,
 MEM_POOL * pool
)
{
  FmtAssert(!src_entry->Pred(), ("Expect no predecessor"));
  FmtAssert((!src_exit->Succ() || (src_exit->Succ()->Len() == 1)), ("Expect unique successor"));

  dst_begin->Replace_succ(dst_end, src_entry);
  dst_end->Replace_pred(dst_begin, src_exit);
  src_entry->Append_pred(dst_begin, pool);

  // Next() is normally the last successor. If src_exit already has a successor, 
  // the successor must be in the same region and is the Next(). Therefore
  // we use prepend here.
  src_exit->Prepend_succ(dst_end, pool);

  BB_NODE * src_last = NULL;
  BB_NODE * bb_tmp = src_entry;
  while (bb_tmp) {
    src_last = bb_tmp;
    bb_tmp = bb_tmp->Next();
  }

  FmtAssert(src_last, ("Last BB_NODE not found"));

  // Insert into prev/next links.
  if (dst_end->Pred()->Len() == 1) {
    BB_NODE * old_prev = dst_end->Prev();
    old_prev->Set_next(src_entry);
    src_entry->Set_prev(old_prev);
    dst_end->Set_prev(src_last);
    src_last->Set_next(dst_end);
  }
  else {
    BB_NODE * old_next = dst_begin->Next();
    dst_begin->Set_next(src_entry);
    src_entry->Set_prev(dst_begin);
    src_last->Set_next(old_next);
    old_next->Set_prev(src_last);
  }
}

// Fix parent ifinfo and loop info after sc1 is head-duplicated into sc2
// or after sc2 is moved above sc1.  
void
PRO_LOOP_FUSION_TRANS::Fix_parent_info(SC_NODE * sc1, SC_NODE * sc2)
{
  SC_NODE * parent = sc2->Parent();
  SC_TYPE parent_type = parent->Type();

  if ((parent_type == SC_THEN) || (parent_type == SC_ELSE)) {
    BB_NODE * bb_cond = parent->Parent()->Head();
    BB_IFINFO * ifinfo = bb_cond->Ifinfo();
    
    if (parent_type == SC_THEN)
      ifinfo->Set_then(parent->First_bb());
    else
      ifinfo->Set_else(parent->First_bb());
  }
  else if (parent_type == SC_LP_BODY) {
    BB_LOOP * loop = parent->Parent()->Loopinfo();
    if (loop->Body() == sc1->First_bb()) {
      loop->Set_body(sc2->First_bb());
    }
  }
}

// Do head duplication of sc_src into sc_dst.
// Caller of this routine should take the responsiblity of legality check.
void
PRO_LOOP_FUSION_TRANS::Do_head_duplication(SC_NODE * sc_src, SC_NODE * sc_dst)
{
  FmtAssert((sc_dst->Type() == SC_IF), ("Expect a SC_IF"));
  FmtAssert(sc_src->Is_sese(), ("Expect a single entry single exit"));

  // Other kinds of loop not tested yet.
  if (sc_src->Type() == SC_LOOP)
    FmtAssert((sc_src->Loopinfo()->Is_flag_set(LOOP_PRE_DO)), ("TODO: Test code motion"));  

  if (_trace) {
    printf("\n\t\t Head-duplication (SC%" PRIdPTR ",SC%" PRIdPTR ")\n", 
	   sc_src->Id(), sc_dst->Id());
  }

  BB_NODE * dst_merge = sc_dst->Merge();
  BB_NODE * dst_head = sc_dst->Head();
  BB_NODE * dst_else = sc_dst->Else();
  BB_NODE * dst_then = sc_dst->Then();
  CFG * cfg = _cu->Cfg();
  float scale = 0.0;

  if (cfg->Feedback()) {
    FB_FREQ freq = cfg->Feedback()->Get_edge_freq(dst_head->Id(), dst_else->Id()) 
      / cfg->Feedback()->Get_node_freq_out(dst_head->Id()) * 1.0;
    if (freq.Known())
      scale = freq.Value();
  }

  SC_NODE * prev_sibling = sc_src->Prev_sibling();

  SC_NODE * sc_new = cfg->Clone_sc(sc_src, TRUE, scale);
  FmtAssert(sc_new, ("NULL clone"));

  // Fix CFG.

  BB_NODE * new_entry;
  BB_NODE * new_exit;
  BB_NODE * old_entry;
  BB_NODE * old_exit;

  MEM_POOL * pool = cfg->Mem_pool();
  BB_LOOP * loopinfo;
  BB_IFINFO * ifinfo;
  SC_NODE * sc_insert_before;
  BB_LIST_ITER bb_list_iter;
  BB_NODE * tmp;
  BB_NODE * src_merge;
  WN * branch_wn;
  
  switch (sc_new->Type()) {
  case SC_LOOP:
    old_entry = sc_src->Head();
    src_merge = sc_src->Merge();
    old_exit = sc_src->Exit();
    new_entry = cfg->Get_cloned_bb(old_entry);
    new_exit = cfg->Get_cloned_bb(old_exit);

    // Fix label on if-branch.
    if (dst_head->Is_branch_to(dst_else)) {
      branch_wn = dst_head->Branch_wn();
      cfg->Add_label_with_wn(new_entry);
      WN_label_number(branch_wn) = new_entry->Labnam();
    }

    // Insert BB_NODEs into else-path.
    Insert_region(new_entry, new_exit, dst_head, dst_else, pool);

    if (cfg->Feedback()) {
      FB_EDGE_TYPE edge_type = cfg->Feedback()->Get_edge_type(old_exit->Id(), src_merge->Id());
      cfg->Feedback()->Move_edge_dest(dst_head->Id(), dst_else->Id(), new_entry->Id());
      cfg->Feedback()->Add_edge(new_exit->Id(), dst_else->Id(), 
				edge_type,
				cfg->Feedback()->Get_edge_freq(dst_head->Id(), new_entry->Id()));
    }
    
    // Prepend sc_new to SC_ELSE's kids.
    sc_insert_before = sc_dst->Find_kid_of_type(SC_ELSE);
    sc_insert_before->Prepend_kid(sc_new);
    sc_new->Set_parent(sc_insert_before);

    // Add merge to loopinfo.
    loopinfo = sc_new->Loopinfo();
    loopinfo->Set_merge(dst_else);

    // Fix label on loop exit
    cfg->Add_label_with_wn(dst_else);
    branch_wn = new_exit->Branch_wn();
    FmtAssert(WN_label_number(branch_wn), ("NULL label"));
    WN_label_number(branch_wn) = dst_else->Labnam();

    // Update sc_dst's ifinfo
    ifinfo = dst_head->Ifinfo();
    ifinfo->Set_else(new_entry);

    // Disconnect BB_NODEs in src_src from CFG and then insert it into then-path.
    FOR_ALL_ELEM(tmp, bb_list_iter, Init(old_entry->Pred())) {
      tmp->Replace_succ(old_entry, src_merge);

      if (tmp->Is_branch_to(old_entry)) {
	branch_wn = tmp->Branch_wn();
	FmtAssert(src_merge->Labnam(), ("Expect a non-NULL label"));
	WN_label_number(branch_wn) = src_merge->Labnam();
      }

      if (cfg->Feedback()) 
	cfg->Feedback()->Move_edge_dest(tmp->Id(), old_entry->Id(), src_merge->Id());
    }
    src_merge->Remove_pred(old_exit, pool);
    src_merge->Set_pred(old_entry->Pred());
    old_entry->Set_pred(NULL);
    old_exit->Remove_succ(src_merge, pool);

    if (cfg->Feedback()) {
       cfg->Feedback()->Move_edge_dest(dst_head->Id(), dst_then->Id(), old_entry->Id());
       cfg->Feedback()->Move_edge_dest(old_exit->Id(), src_merge->Id(), dst_then->Id());
     }

    tmp = src_merge->Prev();
    tmp->Set_next(NULL);
    tmp = old_entry->Prev();
    tmp->Set_next(src_merge);
    src_merge->Set_prev(tmp);
    old_entry->Set_prev(NULL);

    // Fix label on if-branch.
    if (dst_head->Is_branch_to(dst_then)) {
      branch_wn = dst_head->Branch_wn();
      cfg->Add_label_with_wn(old_entry);
      WN_label_number(branch_wn) = old_entry->Labnam();
    }

    Insert_region(old_entry, old_exit, dst_head, dst_then, pool);

    // Unlink sc_src from SC tree and prepend it to SC_THEN's kids.
    sc_src->Unlink();
    sc_insert_before = sc_dst->Find_kid_of_type(SC_THEN);
    sc_insert_before->Prepend_kid(sc_src);
    sc_src->Set_parent(sc_insert_before);

    // Update loopinfo merge
    loopinfo = sc_src->Loopinfo();
    loopinfo->Set_merge(dst_then);

    // Fix label on loop exit.
    cfg->Add_label_with_wn(dst_then);
    branch_wn = old_exit->Branch_wn();
    FmtAssert(WN_label_number(branch_wn), ("NULL label"));
    WN_label_number(branch_wn) = dst_then->Labnam();
    
    // Update sc_dst's ifinfo
    ifinfo = dst_head->Ifinfo();
    ifinfo->Set_then(old_entry);

    break;
  case SC_BLOCK:
    FmtAssert(FALSE,("TODO"));
    break;
  case SC_IF:
    FmtAssert(FALSE,("TODO"));
    break;
  default:
    FmtAssert(FALSE, ("Unexpected SC type"));
  }

  

  if (prev_sibling) {
    BB_NODE * merge = prev_sibling->Merge();
    if (merge) {
      FmtAssert((merge == sc_src->First_bb()), ("Unexpected merge block"));
      prev_sibling->Set_merge(dst_head);
    }
  }

  // Fix parent info.
  Fix_parent_info(sc_src, sc_dst);

  cfg->Invalidate_and_update_aux_info(FALSE);
  cfg->Invalidate_loops();

  _code_bloat_count += sc_src->Real_stmt_count();
  Inc_transform_count();
}

// Do tail duplication of sc_src into sc_dst.
// Caller of this routine should take the responsiblity of legality check.

void
PRO_LOOP_FUSION_TRANS::Do_tail_duplication(SC_NODE * sc_src, SC_NODE * sc_dst)
{
  FmtAssert((sc_dst->Type() == SC_IF), ("Expect a SC_IF"));
  FmtAssert(sc_src->Is_sese(), ("Expect a single entry single exit"));

  // Other kinds of loops not tested yet.
  if (sc_src->Type() == SC_LOOP)
    FmtAssert((sc_src->Loopinfo()->Is_flag_set(LOOP_PRE_DO)), ("TODO: test other loops"));

  if (_trace) {
    printf("\n\t\t Tail-duplication (SC%" PRIdPTR ",SC%" PRIdPTR ")\n", 
	   sc_src->Id(), sc_dst->Id());
  }
  
  BB_NODE * dst_head = sc_dst->Head();
  BB_NODE * dst_merge = sc_dst->Merge();
  BB_NODE * dst_else = sc_dst->Else();
  BB_NODE * dst_else_end = sc_dst->Else_end();
  BB_NODE * dst_then_end = sc_dst->Then_end();
  CFG * cfg = _cu->Cfg();
  float scale = 0.0;
  FB_FREQ      then_edge_freq;
  FB_FREQ      else_edge_freq;
  FB_EDGE_TYPE exit_edge_type;

  if (cfg->Feedback()) {
    FB_FREQ freq = cfg->Feedback()->Get_edge_freq(dst_head->Id(), dst_else->Id()) 
      / cfg->Feedback()->Get_node_freq_out(dst_head->Id()) * 1.0;
    if (freq.Known())
      scale = freq.Value();
    then_edge_freq = cfg->Feedback()->Get_edge_freq(dst_then_end->Id(), dst_merge->Id());
    else_edge_freq = cfg->Feedback()->Get_edge_freq(dst_else_end->Id(), dst_merge->Id());

  }

  SC_NODE * sc_new = cfg->Clone_sc(sc_src, TRUE, scale);
  FmtAssert(sc_new, ("NULL clone"));

  // Fix CFG
  
  BB_NODE * new_entry;
  BB_NODE * new_exit;
  BB_NODE * old_entry;
  BB_NODE * old_exit;

  MEM_POOL * pool = cfg->Mem_pool();
  BB_LOOP * loopinfo;
  BB_IFINFO * ifinfo;
  SC_NODE * sc_insert_after;
  BB_LIST_ITER bb_list_iter;
  BB_NODE * tmp;
  BB_NODE * tmp2;
  BB_NODE * src_merge;
  BB_NODE * new_merge;
  BB_LIST * bb_list;
  MAP_LIST * map_lst;
  SC_NODE * sc_blk;
  WN * branch_wn;

  switch (sc_new->Type()) {
  case SC_LOOP:
    old_entry = sc_src->Head();
    src_merge = sc_src->Merge();
    old_exit = sc_src->Exit();
    new_entry = cfg->Get_cloned_bb(old_entry);
    new_exit = cfg->Get_cloned_bb(old_exit);

    if (old_entry->Labnam() != 0) 
      FmtAssert(FALSE, ("TODO: fix label"));

    FmtAssert(!src_merge->Pred()->Multiple_bbs(), ("Expect single predecessor"));

    new_merge = cfg->Create_and_allocate_bb(BB_GOTO);

    if (cfg->Feedback()) {
      cfg->Feedback()->Add_node(new_merge->Id());
      exit_edge_type = cfg->Feedback()->Get_edge_type(old_exit->Id(), src_merge->Id());
    }

    // Disconnect BB_NODEs in src_src from CFG
    FOR_ALL_ELEM(tmp, bb_list_iter, Init(old_entry->Pred())) {
      tmp->Replace_succ(old_entry, src_merge);
      if (cfg->Feedback() && (old_entry != dst_merge))
	cfg->Feedback()->Move_edge_dest(tmp->Id(), old_entry->Id(), src_merge->Id());
    }
    
    if (cfg->Feedback()) {
      if (old_entry == dst_merge)
	cfg->Feedback()->Move_edge_dest(dst_else_end->Id(), dst_merge->Id(), new_entry->Id());
      cfg->Feedback()->Move_edge_dest(old_exit->Id(), src_merge->Id(), new_merge->Id());
      cfg->Feedback()->Add_edge(new_merge->Id(), src_merge->Id(), FB_EDGE_OUTGOING, then_edge_freq);

      if (!cfg->Feedback()->Edge_has_freq(dst_then_end->Id(), old_entry->Id()))
	cfg->Feedback()->Add_edge(dst_then_end->Id(), old_entry->Id(), 
				  FB_EDGE_OUTGOING, then_edge_freq);
    }

    old_exit->Replace_succ(src_merge, new_merge);
    new_merge->Append_pred(old_exit, pool);
    src_merge->Remove_pred(old_exit, pool);
    src_merge->Set_pred(old_entry->Pred());
    old_entry->Set_pred(NULL);

    // Fix ifinfo
    ifinfo = dst_head->Ifinfo();
    ifinfo->Set_merge(src_merge);

    // Fix label on loop exit
    cfg->Add_label_with_wn(new_merge);
    branch_wn = old_exit->Branch_wn();
    FmtAssert(WN_label_number(branch_wn), ("NULL label"));
    WN_label_number(branch_wn) = new_merge->Labnam();

    // Fix loop info
    loopinfo = sc_src->Loopinfo();
    loopinfo->Set_merge(new_merge);

    tmp = src_merge->Prev();
    tmp->Set_next(new_merge);
    new_merge->Set_prev(tmp);
    tmp = old_entry->Prev();
    tmp->Set_next(src_merge);
    src_merge->Set_prev(tmp);
    old_entry->Set_prev(NULL);
    
    dst_merge = src_merge;

    Insert_region(old_entry, new_merge, dst_then_end, dst_merge, pool);
      
    // UNlink sc_src from SC tree and append it to SC_THEN's kids.
    sc_src->Unlink();
    sc_insert_after = sc_dst->Find_kid_of_type(SC_THEN);
    sc_insert_after->Append_kid(sc_src);
    sc_src->Set_parent(sc_insert_after);

    sc_blk = cfg->Create_sc(SC_BLOCK);
    sc_blk->Append_bbs(new_merge);
    sc_insert_after->Append_kid(sc_blk);
    sc_blk->Set_parent(sc_insert_after);
    
    // Insert new BB_NODEs into else-path
    new_merge = cfg->Create_and_allocate_bb(BB_GOTO);
    
    if (cfg->Feedback()) {
      cfg->Feedback()->Add_node(new_merge->Id());
      cfg->Feedback()->Add_edge(new_exit->Id(), new_merge->Id(), exit_edge_type, else_edge_freq);
      cfg->Feedback()->Add_edge(new_merge->Id(), dst_merge->Id(), FB_EDGE_OUTGOING, else_edge_freq);
      if (!cfg->Feedback()->Edge_has_freq(dst_else_end->Id(), new_entry->Id()))
	cfg->Feedback()->Add_edge(dst_else_end->Id(), new_entry->Id(),
				  FB_EDGE_OUTGOING, else_edge_freq);
    }

    new_exit->Prepend_succ(new_merge, pool);
    new_merge->Append_pred(new_exit, pool);
    
    tmp = new_entry;
    while (tmp) {
      tmp2 = tmp;
      tmp = tmp->Next();
    }

    tmp2->Set_next(new_merge);
    new_merge->Set_prev(tmp2);

    Insert_region(new_entry, new_merge, dst_else_end, dst_merge, pool);

    // Append src_new to SC_ELSE's kids
    sc_insert_after = sc_dst->Find_kid_of_type(SC_ELSE);
    sc_insert_after->Append_kid(sc_new);
    sc_new->Set_parent(sc_insert_after);

    sc_blk = cfg->Create_sc(SC_BLOCK);
    sc_blk->Append_bbs(new_merge);
    sc_insert_after->Append_kid(sc_blk);
    sc_blk->Set_parent(sc_insert_after);
    
    // Add merge to loopinfo
    loopinfo = sc_new->Loopinfo();
    loopinfo->Set_merge(new_merge);

    // Fix label on loop exit
    cfg->Add_label_with_wn(new_merge);    
    branch_wn = new_exit->Branch_wn();
    FmtAssert(WN_label_number(branch_wn), ("NULL label"));
    WN_label_number(branch_wn) = new_merge->Labnam();

    break;
  case SC_BLOCK:
    old_entry = sc_src->First_bb();
    old_exit = sc_src->Last_bb();
    new_entry = sc_new->First_bb();
    new_exit = sc_new->Last_bb();

    // Disconnect BB_NODEs in sc_src from CFG.
    FmtAssert(!old_exit->Succ()->Multiple_bbs(), ("Expect singe successor"));
    tmp2 = old_exit->Succ()->Node();

    FOR_ALL_ELEM(tmp, bb_list_iter, Init(old_entry->Pred())) {
      tmp->Replace_succ(old_entry, tmp2);
      if (cfg->Feedback() && (old_entry != dst_merge)) 
	cfg->Feedback()->Move_edge_dest(tmp->Id(), old_entry->Id(), tmp2->Id());
    }

    if (cfg->Feedback())
      cfg->Feedback()->Delete_edge(old_exit->Id(), tmp2->Id());
    
    bb_list = tmp2->Pred();
    while (bb_list) {
      bb_list = bb_list->Remove(bb_list->Node(), pool);
    }
    tmp2->Set_pred(old_entry->Pred());
    old_entry->Set_pred(NULL);

    bb_list = old_exit->Succ();
    while (bb_list) {
      bb_list = bb_list->Remove(bb_list->Node(), pool);
    }

    old_exit->Set_succ(NULL);
    
    tmp = old_entry->Prev();
    tmp->Set_next(tmp2);
    tmp2->Set_prev(tmp);
    old_entry->Set_prev(NULL);
    old_exit->Set_next(NULL);

    if (cfg->Feedback()) {
      cfg->Feedback()->Move_edge_dest(dst_else_end->Id(), dst_merge->Id(), new_entry->Id());
      cfg->Feedback()->Move_edge_dest(dst_then_end->Id(), dst_merge->Id(), old_entry->Id());
    }
    
    if (dst_merge == old_entry) {
      ifinfo = dst_head->Ifinfo();
      ifinfo->Set_merge(tmp2);
      dst_merge = tmp2;
    }

    if (cfg->Feedback()) {
      cfg->Feedback()->Add_edge(new_exit->Id(), dst_merge->Id(),
				FB_EDGE_OUTGOING,
				cfg->Feedback()->Get_edge_freq(dst_else_end->Id(), new_entry->Id()));
      cfg->Feedback()->Add_edge(old_exit->Id(), dst_merge->Id(),
				FB_EDGE_OUTGOING,
				cfg->Feedback()->Get_edge_freq(dst_then_end->Id(), old_entry->Id()));
    }
    
    // Insert BB_NODEs into else-path
    Insert_region(new_entry, new_exit, dst_else_end, dst_merge, pool);
    
    // Append sc_new to SC_ELSE's kids
    sc_insert_after = sc_dst->Find_kid_of_type(SC_ELSE);
    sc_blk = sc_insert_after->Last_kid();

    if (sc_blk->Type() == SC_BLOCK) {
      bb_list = sc_new->Get_bbs();
      FOR_ALL_ELEM(tmp, bb_list_iter, Init(bb_list)) {
	sc_blk->Append_bbs(tmp);
      }
    }
    else {
      sc_insert_after->Append_kid(sc_new);
      sc_new->Set_parent(sc_insert_after);
    }

    // insert it into then-path.
    Insert_region(old_entry, old_exit, dst_then_end, dst_merge,pool);

    // Unlink src_src from SC tree and append it to SC_THEN's kids.
    sc_src->Unlink();
    sc_insert_after = sc_dst->Find_kid_of_type(SC_THEN);
    sc_blk = sc_insert_after->Last_kid();

    if (sc_blk->Type() == SC_BLOCK) {
      bb_list = sc_src->Get_bbs();
      FOR_ALL_ELEM(tmp, bb_list_iter, Init(bb_list)) {
	sc_blk->Append_bbs(tmp);
      }
    }
    else {
      sc_insert_after->Append_kid(sc_src);
      sc_src->Set_parent(sc_insert_after);
    }
    break;

  default:
    FmtAssert(FALSE, ("Unexpected SC type"));
  }
  
  cfg->Invalidate_and_update_aux_info(FALSE);
  cfg->Invalidate_loops();
  _code_bloat_count += sc_src->Real_stmt_count();
  Inc_transform_count();
}

// Traverse siblings between sc1 and sc2, do code motion or head/tail duplication
// to bring sc1 and sc2 adjacent to each other. Return TRUE if all transformations
// during the traversal are successful.
//
// This routine can only be called from PRO_LOOP_FUSION_TRANS::Top_down_trans.
BOOL
PRO_LOOP_FUSION_TRANS::Traverse_trans(SC_NODE * sc1, SC_NODE * sc2)
{
  FmtAssert((sc1->Parent() == sc2->Parent()), ("Expect siblings"));
  SC_NODE * sc = sc1;
  BOOL ret_val = TRUE;

  if (_trace)
    printf("\n\t Traverse (SC%" PRIdPTR ",SC%" PRIdPTR ")\n", 
	   sc1->Id(), sc2->Id());

  while (sc != sc2) {
    SC_NODE * next = sc->Next_sibling();
    SC_TYPE sc_type = sc->Type();
    SC_TYPE next_type = next->Type();

    FmtAssert(((sc_type == SC_IF) || (sc_type == SC_LOOP)),
	      ("Unexpect SC type"));
    
    switch (next_type) {
    case SC_BLOCK:
      if (sc_type == SC_LOOP) {
	if (sc->Loopinfo()->Is_flag_set(LOOP_PRE_DO)) 
	  Do_code_motion(sc, next);
	else {
	  ret_val = FALSE;
	  if (_trace)
	    printf("\n\t\t  Skip non-DO-LOOP (SC%" PRIdPTR ")\n", sc->Id());
	}
      }
      else if (sc_type == SC_IF) 
	Do_tail_duplication(next, sc);
      else 
	FmtAssert(FALSE, ("Unexpect SC type"));

      break;
    case SC_IF:
      if (sc_type == SC_IF) {
	if (_if_merge && _if_merge->Is_cand(sc, next, TRUE))
	  _if_merge->Do_merge(sc, next);
	else {
	  // FmtAssert(FALSE, ("TODO"));
	  ret_val = FALSE;
	}
      }
      else if (sc_type == SC_LOOP) {
	if (sc->Loopinfo()->Is_flag_set(LOOP_PRE_DO)) {
	  // Do head duplication.
	  Do_head_duplication(sc, next);
	  sc = next;
	}
	else {
	  ret_val = FALSE;
	  if (_trace)
	    printf("\n\t\t  Skip non-DO-LOOP (SC%" PRIdPTR ")\n", sc->Id());
	}
      }

      break;
    case SC_LOOP:
      if (sc_type == SC_IF) {
	if (next->Loopinfo()->Is_flag_set(LOOP_PRE_DO)) {
	  Do_tail_duplication(next, sc);
	  // exhause transformation opportunities on current loop classifications.
	  Nonrecursive_trans(sc->Find_kid_of_type(SC_THEN), FALSE);
	  Nonrecursive_trans(sc->Find_kid_of_type(SC_ELSE), FALSE);
	}
	else {
	  ret_val = FALSE;
	  if (_trace)
	    printf("\n\t\t  Skip non-DO-LOOP (SC%" PRIdPTR ")\n", next->Id());
	}
      }
      break;
    default:
      FmtAssert(FALSE, ("Unexpect SC type"));
    }

    if ((next == sc2) || (ret_val == FALSE))
      break;
  }

  return ret_val;
}

// Query whether the traverse transformation between the given pair
// should be delayed.
BOOL
PRO_LOOP_FUSION_TRANS::Is_delayed(SC_NODE * sc1, SC_NODE * sc2)
{
  BOOL ret_val = FALSE;

  // Case 1. Both sc1 and sc2 are SC_LOOPs, and all sibling nodes between them
  // are SC_BLOCKs.  In this scenario, we should search further for same-scenario
  // candidates and do traverse transformation on those candidates before sc1
  // and sc2 are processed.  Imposing such a delay is to reduce state transitions
  // of traverse transformation.

  if ((sc1->Type() == SC_LOOP) && (sc2->Type() == SC_LOOP)
      && (sc1->Parent() == sc2->Parent())) {
    ret_val = TRUE;
    SC_NODE * next_sibling = sc1->Next_sibling();

    while (next_sibling && (next_sibling != sc2)) {
      if (next_sibling->Type() != SC_BLOCK) {
	ret_val = FALSE;
	break;
      }
      next_sibling = next_sibling->Next_sibling();
    }
  }

  return ret_val;
}

// Do non-recursive tail-duplication transformation for candidates whose lcp is sc_root.
void
PRO_LOOP_FUSION_TRANS::Nonrecursive_trans(SC_NODE * sc_root, BOOL do_find) 
{
  if (do_find) {
    _loop_list = NULL;
    Collect_classified_loops(sc_root);
  }

  while (1) {
    if ((WOPT_Enable_Tail_Dup_Limit >= 0)
	&& (Transform_count() >= WOPT_Enable_Tail_Dup_Limit))
      break;
      
    SC_NODE * cand1 = NULL;
    SC_NODE * cand2 = NULL;
    Find_cand(sc_root, &cand1, &cand2, NULL);

    if (cand1 && cand2) {
      SC_NODE * tmp1 = cand1;
      SC_NODE * tmp2 = cand2;
      SC_NODE * last1 = NULL;
      SC_NODE * last2 = NULL;

      while (tmp1 && tmp2 && Is_delayed(tmp1, tmp2)) {
	last1 = tmp1;
	last2 = tmp2;
	Find_cand(sc_root, &tmp1, &tmp2, tmp2);
      }
      
      if (last1 && last2) {
	cand1 = last1;
	cand2 = last2;
      }
    }

    if (cand1 && cand2) {
      if (!Traverse_trans(cand1, cand2)) 
	break;
    }
    else
      break;
  }

  if (do_find) {
    while (_loop_list) {
      SC_NODE * tmp = _loop_list->Node();
      _loop_list = _loop_list->Remove(tmp, _pool);
    }
  }
}

// Top down do tail-duplication transformation for the SC tree rooted at the given sc_root.
void
PRO_LOOP_FUSION_TRANS::Top_down_trans(SC_NODE * sc_root)
{
  FmtAssert(_if_merge, ("Expect a if-merge object"));

  if (sc_root->Flag() >= HAS_SYMM_LOOP) {
    int orig_transform_count = _transform_count;
    Nonrecursive_trans(sc_root, TRUE);

    if (_transform_count > orig_transform_count) {
      _if_merge->Top_down_trans(sc_root); 
      Classify_loops(sc_root);
    }
  }

  SC_LIST_ITER sc_list_iter;
  SC_NODE * kid;
  FOR_ALL_ELEM(kid, sc_list_iter, Init(sc_root->Kids())) {
    Top_down_trans(kid);
  }
}

// Reset/clear fields
void
PRO_LOOP_FUSION_TRANS::Clear()
{
  _cu = NULL;
  _trace = FALSE;
  _dump = FALSE;
  _transform_count = 0;
  _last_class_id = 0;
  _if_merge = NULL;
  _pool = NULL;
  _loop_depth_to_loop_map = NULL;
  _loop_list = NULL;
  _edit_loop_class = FALSE;
  _code_bloat_count = 0;
}

// Reset related loop classification fields for the SC tree rooted at sc.
// Rebuild map of SC tree depth to a list of SC_LOOP nodes.
// The routine can only be invoked by PRO_LOOP_FUSION_TRANS::Classify_loops.

void
PRO_LOOP_FUSION_TRANS::Reset_loop_class(SC_NODE * sc, int cur_depth)
{
  FmtAssert(_edit_loop_class, ("Not in edit mode"));

  sc->Set_class_id(0);
  sc->Set_depth(cur_depth);
  sc->Remove_flag(HAS_SYMM_LOOP);

  if (sc->Type() == SC_LOOP) {
    SC_LIST * sc_list = (SC_LIST *) _loop_depth_to_loop_map->Get_val((POINTER) cur_depth);

    if (!sc_list) {
      sc_list = (SC_LIST *) CXX_NEW(SC_LIST(sc), _pool);
      _loop_depth_to_loop_map->Add_map((POINTER) cur_depth, (POINTER) sc_list);
    }
  
    sc_list = sc_list->Append(sc, _pool);
  }

  SC_LIST_ITER sc_list_iter(sc->Kids());
  SC_NODE *tmp = NULL;

  FOR_ALL_ELEM(tmp, sc_list_iter, Init()) {
    Reset_loop_class(tmp, cur_depth+1);
  }
}

// Mark SC_LOOPs with symmetric paths the same class id.
// The routine can only be invoked by PRO_LOOP_FUSION_TRANS::Classify_loops.

void
PRO_LOOP_FUSION_TRANS::Find_loop_class(SC_NODE * sc)
{
  FmtAssert(_edit_loop_class, ("Not in edit mode"));

  if ((sc->Type() == SC_LOOP) && (sc->Class_id() == 0)) {
    SC_LIST * sc_list = (SC_LIST *) _loop_depth_to_loop_map->Get_val((POINTER) sc->Depth());
    SC_LIST_ITER sc_list_iter(sc_list);
    SC_NODE *tmp = NULL;
    int new_id = New_class_id();
    sc->Set_class_id(new_id);

    FOR_ALL_ELEM(tmp, sc_list_iter, Init()) {
      if ((tmp->Class_id() != 0) ||  (tmp == sc))
	continue;

      if (sc->Has_symmetric_path(tmp)) {
	tmp->Set_class_id(sc->Class_id());
	SC_NODE * lcp = sc->Find_lcp(tmp);
	lcp->Add_flag(HAS_SYMM_LOOP);
      }
    }
  }
  
  SC_LIST_ITER sc_list_iter(sc->Kids());
  SC_NODE * tmp;
  FOR_ALL_ELEM(tmp, sc_list_iter, Init()) {
    Find_loop_class(tmp);
  }
}

// Classify loops for the SC tree rooted at sc.
void
PRO_LOOP_FUSION_TRANS::Classify_loops(SC_NODE *sc)
{
  _edit_loop_class = TRUE;
  OPT_POOL_Push(_pool, MEM_DUMP_FLAG + 1);
  _loop_depth_to_loop_map = CXX_NEW(MAP(CFG_BB_TAB_SIZE, _pool), _pool);
  Reset_loop_class(sc, 0);
  Find_loop_class(sc);
  CXX_DELETE(_loop_depth_to_loop_map, _pool);
  _loop_depth_to_loop_map = NULL;
  OPT_POOL_Pop(_pool, MEM_DUMP_FLAG + 1);
  _edit_loop_class = FALSE;
}

#define PRO_LOOP_FUSION_THRESHOLD 131
#define MAX_OLF_UPPER_BOUND 70

// The driver to invoke if-merge and head-tail-duplication transformations
// to allow loop fusion.

void
COMP_UNIT::Pro_loop_fusion_trans()
{
  // Debug limit
  if ((WOPT_Enable_Pro_Loop_Fusion_Func_Limit >= 0)
      && (Current_PU_Count() > WOPT_Enable_Pro_Loop_Fusion_Func_Limit))
    return;

  if (_cfg->Do_pro_loop_fusion_trans()) {
    MEM_POOL * pool = _cfg->Loc_pool();
    OPT_POOL_Push(pool, MEM_DUMP_FLAG + 1);
    BOOL trace = Get_Trace(TP_WOPT2, PRO_TRANS_TRACE_FLAG);
    BOOL dump = Get_Trace(TP_WOPT2, PRO_TRANS_DUMP_FLAG);
    BOOL changed = FALSE;

    vector<vertex_id> entry;
    successor_graph _g_tmp;

    bool ok = build_successor_graph(_cfg, _g_tmp,
				    insert_iterator<vector<vertex_id> >
				    (entry, entry.begin()));

    if (!ok) {
      if (trace) {
	printf(("skip IF-MERGING transformation because of REGION."));
      }
    }
    else {
      SC_NODE * sc_root = _cfg->SC_root();      

      // Create if-merge class object.
      IF_MERGE_TRANS * if_merge_trans = CXX_NEW(IF_MERGE_TRANS(this), pool);
      if_merge_trans->Set_trace(trace);
      if_merge_trans->Set_dump(dump);
      if_merge_trans->Set_pool(pool);

      // Create proactive loop fusion transformation class object

      PRO_LOOP_FUSION_TRANS * pro_loop_fusion_trans = CXX_NEW(PRO_LOOP_FUSION_TRANS(this), pool);
      pro_loop_fusion_trans->Set_trace(trace);
      pro_loop_fusion_trans->Set_dump(dump);
      pro_loop_fusion_trans->Set_if_merge(if_merge_trans);
      pro_loop_fusion_trans->Set_pool(pool);
      
      if_merge_trans->Set_tail_dup(pro_loop_fusion_trans);

      // Start a top-down if-merging.
      if_merge_trans->Set_pass(PASS_GLOBAL);
      if_merge_trans->Top_down_trans(sc_root);
      
      // Start a top-down proactive loop fusion transformations.
      if_merge_trans->Set_pass(PASS_LOCAL);
      pro_loop_fusion_trans->Classify_loops(sc_root);
      pro_loop_fusion_trans->Top_down_trans(sc_root);

      int pro_loop_fusion_trans_count = pro_loop_fusion_trans->Transform_count();
    
      // Verify branch target labels and feed back info.
      if ((pro_loop_fusion_trans_count > 0) || (if_merge_trans->Count() > 0)) {
	changed = TRUE;
	_cfg->Verify_label();

	if (Cur_PU_Feedback)
	  _cfg->Feedback()->Verify(_cfg, "after proactive loop fusion transformation");

	// A simple heuristic to dynamically increase OLF_Upper_Bound if number of proactive
	// loop fusion transformations exceed a threshold.
	if (pro_loop_fusion_trans_count > PRO_LOOP_FUSION_THRESHOLD) {
	  int olf_ub = Current_LNO->OLF_Upper_Bound;
	  LNO_Save_Config(OLF_UPPER_BOUND);
	  Current_LNO->OLF_Upper_Bound = Max(olf_ub, MAX_OLF_UPPER_BOUND);
	}
      }
      
      if (trace) {
	if (if_merge_trans->Count() > 0)
	  printf("\n\t If-merge total:%d\n", if_merge_trans->Count());

	if (pro_loop_fusion_trans_count > 0)
	  printf("\n\t Code-motion-Head-Tail-Dup total:%d\n", 
		 pro_loop_fusion_trans->Transform_count());
      }
    }

    OPT_POOL_Pop(pool, MEM_DUMP_FLAG + 1);
    _cfg->Free_sc();

    if (changed)
      _cfg->Invalidate_and_update_aux_info(TRUE);      
  }
}

//  For debugging

void print_succ_graph(successor_graph& g)
{
  print_edges(g, stdout);
}

void print_pred_graph(predecessor_graph& g)
{
  print_edges(g, stdout);
}
 
// for debugging
void print_path_type(path_type *p, FILE *fp)
{
  fprintf(fp, "path (wt %g): ", (*p).wt);
  for (int i = 0; i < (*p).bbs.size(); ++i)
    fprintf(fp, "%d ", (*p).bbs[i]);
  fprintf(fp, "\n");
}

void print_vertex_set(set<vertex_id> *s, FILE *fp) 
{
  fprintf(fp, "vertex set: ");
  for (set<vertex_id>::iterator si = (*s).begin();
       si != (*s).end();
       ++si) {
    fprintf(fp, "%d ", *si);
  }
  fprintf(fp, "\n");
}

