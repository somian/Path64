/*
 * Copyright (C) 2009 Advanced Micro Devices, Inc.  All Rights Reserved.
 */

//-*-c++-*-

/*
 * Copyright 2003, 2004, 2005, 2006 PathScale, Inc.  All Rights Reserved.
 */

// ====================================================================
// ====================================================================
//
// Module: opt_fb.h
// $Revision: 1.6 $
// $Date: 04/12/21 14:57:18-08:00 $
// $Author: bos@eng-25.internal.keyresearch.com $
// $Source: /home/bos/bk/kpro64-pending/be/opt/SCCS/s.opt_fb.h $
//
// ====================================================================
//
// Copyright (C) 2000, 2001 Silicon Graphics, Inc.  All Rights Reserved.
//
/*
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
// ====================================================================
//
// Description:
//
// opt_fb.h and opt_fb.cxx implement a feedback data structure to
// support both node and edge frequencies within the optimizer.
//
// The class OPT_FEEDBACK maintains a copy of the optimizer's cfg,
// annotated with node and edge frequencies and supplemental
// information.  During PREOPT/WOPT, OPT_FEEDBACK should be the sole
// location of feedback data.
//
// Optimizer feedback data is accessed through cfg->Feedback(), which
// points to an instance of the OPT_FEEDBACK class.  If feedback is not
// available, then cfg->Feedback() == NULL.
//
// Methods are provided to:
// - retrieve the whirl frequency data from Cur_PU_Feedback and use it
//   to construct and annotate cfg->Feedback()
// - update the feedback whenever the optimizer's cfg is modified
// - rebalance the frequency values whenever cloning and dead branch
//   elimination results in the deletion of an edge with frequency
// - look up frequency data, especially while emitting (annotated)
//   whirl
//
// ====================================================================
// ====================================================================


#ifndef opt_fb_INCLUDED
#define opt_fb_INCLUDED "opt_fb.h"

#ifndef fb_whirl_INCLUDED
#include "fb_whirl.h"
#endif
#ifndef opt_defs_INCLUDED
#include "opt_defs.h"
#endif
#ifndef opt_bb_INCLUDED
#include "opt_bb.h"
#endif

#ifndef opt_cfg_INCLUDED
class OPT_FEEDBACK;  // Predeclared for opt_cfg.h
#include "opt_cfg.h"
#endif
#ifndef opt_cfg_trans_INCLUDED
#include "opt_cfg_trans.h" // for zone
#endif


// IDTYPE_NULL is used to signal errors; nodes and edges are assigned
// positive id numbers
#define IDTYPE_NULL 0

// ====================================================================

struct OPT_FB_EDGE {
  // If exact_freq is FALSE, freq value is a guess (produced by cloning).

  IDTYPE       source;
  IDTYPE       destination;

  FB_EDGE_TYPE edge_type;
  FB_FREQ      freq;

  OPT_FB_EDGE( IDTYPE src, IDTYPE dst,
	       FB_EDGE_TYPE typ  = FB_EDGE_UNINIT,
	       FB_FREQ      freq = FB_FREQ_UNINIT ) :
    source(src),
    destination(dst),
    edge_type(typ),
    freq(freq) {}

  void Print( IDTYPE id, FILE *fp = stderr ) const {
    char buffer[FB_EDGE_TYPE_NAME_LENGTH];
    FB_EDGE_TYPE_sprintf( buffer, edge_type );

    fprintf( fp, "Edge[%3" PRIuPTR "]:  (%3" PRIuPTR " --> %3" PRIuPTR ") : freq = ",
	     id, source, destination );
    freq.Print( fp );
    fprintf( fp, " : %s\n", buffer );
  }
};

// ====================================================================

struct OPT_FB_NODE {

  // freq_total_in and freq_total_out maintain the sum of the edge
  //   frequencies of all incoming and outgoing edges, respectively.
  // unknown_in and unknown_out count the number of incoming and
  //   outgoing edges whose frequencies are not known.
  // unexact_in and unexact_out count the number of incoming and
  //   outgoing edges whose frequencies are not exact.
  // If in_out_same is TRUE, then freq_total_in and freq_total_out should
  //   be equal.  (However, it is possible for cloning and edge deletion
  //   to produce a node with freq_total_in != freq_total_out.  This
  //   indicates that the frequencies need to be rebalanced.)

  typedef vector<IDTYPE, mempool_allocator<IDTYPE> > EDGES;
  typedef EDGES::const_iterator EDGES_ITER;

  EDGES incoming_edges;
  EDGES outgoing_edges;

#ifdef KEY
  WN* orig_wn;
#endif

  INT      update_count;
  bool     in_out_same;

  FB_FREQ  freq_total_in;
  FB_FREQ  freq_total_out;

  INT      unknown_in;
  INT      unknown_out;

  INT      unexact_in;
  INT      unexact_out;

  OPT_FB_NODE( MEM_POOL *pool )
    : incoming_edges(mempool_allocator<IDTYPE>(pool)),
      outgoing_edges(mempool_allocator<IDTYPE>(pool)),
      update_count(0), in_out_same(TRUE),
      unknown_in(0), unknown_out(0),
      unexact_in(0), unexact_out(0),
      freq_total_in(FB_FREQ_ZERO), freq_total_out(FB_FREQ_ZERO) {
    incoming_edges.reserve(2);
    outgoing_edges.reserve(2);
  }

  void Print( IDTYPE id, FILE *fp = stderr ) const {
    INT t;
    fprintf( fp, "Node[%" PRIuPTR "]:  in_out_same %c, update_count %d\n"
	     "  in:  unknown %d, unexact %d, freq_total ",
	     id, ( in_out_same ? 'Y' : 'N' ), update_count,
	     unknown_in, unexact_in );
    freq_total_in.Print( fp );
    fprintf( fp, ", edges [" );
    for ( t = 0; t < incoming_edges.size(); t++ ) {
      fprintf( fp, " %" PRIuPTR , incoming_edges[t] );
    }
    fprintf( fp, " ],\n  out: unknown %d, unexact %d, freq_total ",
	     unknown_out, unexact_out );
    freq_total_out.Print( fp );
    fprintf( fp, ", edges [" );
    for ( t = 0; t < outgoing_edges.size(); t++ ) {
      fprintf( fp, " %" PRIuPTR , outgoing_edges[t] );
    }
    fprintf( fp, " ]\n" );
  }
};

// ====================================================================

class OPT_FEEDBACK {
private:
  MEM_POOL  *_mem_pool;
  bool       _trace;
  bool       _trace_draw;
  bool       _trace_before;
  bool       _trace_prop;
  vector<OPT_FB_NODE, mempool_allocator<OPT_FB_NODE> >  _fb_opt_nodes;
  vector<OPT_FB_EDGE, mempool_allocator<OPT_FB_EDGE> >  _fb_opt_edges;

  IDTYPE  Find_edge_by_type( IDTYPE nx, FB_EDGE_TYPE edge_type ) const;
  FB_FREQ Get_edge_freq_by_type( IDTYPE nx, FB_EDGE_TYPE edge_type ) const;
  IDTYPE  Get_node_successor( IDTYPE nx ) const;

  void Remove_edge( IDTYPE ex );
  void Change_edge_freq( IDTYPE ex, FB_FREQ new_freq );
  void Set_edge_dest( IDTYPE ex, IDTYPE nx_dst_new );

  // Compute unknown frequencies from known frequencies
  void Freq_propagate_edge_in( OPT_FB_NODE& node,
			       IDTYPE edge_id, FB_FREQ freq );
  void Freq_propagate_edge_out( OPT_FB_NODE& node,
				IDTYPE edge_id, FB_FREQ freq );
  void Freq_propagate_node_in( IDTYPE nx );   // requires unknown_in  < 2 
  void Freq_propagate_node_out( IDTYPE nx );  // requires unknown_out < 2
  void Freq_propagate();

  // Display frequencies using daVinci
  char *Node_label( IDTYPE nx ) const;

public:
  OPT_FEEDBACK( CFG *cfg, MEM_POOL *pool );
  ~OPT_FEEDBACK();

  void Emit_feedback( WN *wn, BB_NODE *bb ) const;

  bool Trace() const     { return _trace; }

  FB_FREQ Get_node_freq_out( IDTYPE nx ) const {
    return _fb_opt_nodes[nx].freq_total_out;
  }
  
  bool    Edge_has_freq( IDTYPE nx_src, IDTYPE nx_dst ) const;
  FB_FREQ Get_edge_freq( IDTYPE nx_src, IDTYPE nx_dst ) const;
  FB_EDGE_TYPE Get_edge_type(IDTYPE nx_src, IDTYPE nx_dst ) const;
  float   Get_pred_prob( IDTYPE nx_src, IDTYPE nx_dst ) const;
  float   Get_succ_prob( IDTYPE nx_src, IDTYPE nx_dst ) const;

  void Delete_edge( IDTYPE nx_src, IDTYPE nx_dst );
  void Move_edge_dest( IDTYPE nx_src, IDTYPE nx_dst_old, IDTYPE nx_dst_new );
  void Move_incoming_edges_dest( IDTYPE nx_dst_old, IDTYPE nx_dst_new );
  void Merge_outgoing_edges(IDTYPE nx_src, IDTYPE nx_dst_new);
  void Delete_node( IDTYPE nx );
  void Split_edge( IDTYPE nx_src, IDTYPE nx_mid, IDTYPE nx_dst );
  void Split_node( IDTYPE nx_old, IDTYPE nx_new );

  void Clone_edge( IDTYPE nx_src_old, IDTYPE nx_dst_old,
		   IDTYPE nx_src_new, IDTYPE nx_dst_new, float scale );
  void Clone_zone( zone& z, std::map<vertex_id, vertex_id>& nx_old_to_new );

  void Add_node( IDTYPE nx_new );
  void Add_edge( IDTYPE nx_src, IDTYPE nx_dst,
		 FB_EDGE_TYPE edge_type, FB_FREQ freq );

  void Print( FILE *fp = stderr ) const;
  FB_VERIFY_STATUS Verify( CFG *cfg, const char *const phase );

  // Display frequencies using daVinci
  void Print_node( FILE *fp, IDTYPE nx ) const;
  void Print_edge( FILE *fp, IDTYPE src_nx, IDTYPE dst_nx ) const;
  void Draw() const;
  void Display( WN *wn_tree, const char *caller ) const;
};

void dV_view_fb_opt_cfg( const OPT_FEEDBACK& cfg,
			 WN *wn_tree, const char *caller );

#endif
