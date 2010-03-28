//-*-c++-*-

/*
 * Copyright 2005, 2006 PathScale, Inc.  All Rights Reserved.
 */

// ====================================================================
// ====================================================================
//
// Module: opt_dbg.cxx
// $Revision: 1.4 $
// $Date: 05/03/10 16:47:55-08:00 $
// $Author: bos@eng-25.internal.keyresearch.com $
// $Source: /home/bos/bk/kpro64-pending/be/opt/SCCS/s.opt_dbg.cxx $
//
// Revision history:
//  28-NOV-94 fchow - Original Version
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
// This file provides things that are helpful inside dbx when
// debugging WOPT.
// 
// ====================================================================
// ====================================================================


#ifdef USE_PCH
#include "opt_pch.h"
#endif // USE_PCH
#pragma hdrstop


#ifdef _KEEP_RCS_ID
#define opt_dbg_CXX	"opt_dbg.cxx"
static char *rcs_id = 	opt_dbg_CXX"$Revision: 1.4 $";
#endif /* _KEEP_RCS_ID */

#include "defs.h"
#include "errors.h"
#include "erglob.h"
#include "bb_node_set.h"
#include "opt_bb.h"
#include "opt_cfg.h"
#include "opt_main.h"
#include "opt_cfg_trans.h"
#include "opt_htable.h"

#ifdef Is_True_On

COMP_UNIT *g_comp_unit;
OPT_STAB *g_opt_stab;

extern "C" void Dump_cfg(void);
extern FILE *Init_daVinci(void);
extern FILE *New_daVinci(void);

void
Dump_cfg(void)
{
  g_comp_unit->Cfg()->Print();
}

void Dump_bb(BB_NODE *bb)
{
  bb->Print();
}

void Dump_cr(CODEREP *cr)
{
   cr->Print(2);
}

void Dump_sr(STMTREP *sr)
{
   sr->Print();
}



/*************************************************************************

SAMPLE OUTPUT of the CFG of k01.f from show_cfg:

#ifdef DAVINCI1
new_term([
#else
[
#endif
  l("BB1", n("1", [a("OBJECT","1")], [
     e("",[],r("BB2")),  ])),
  l("BB2", n("2", [a("OBJECT","2")], [
     e("",[],r("BB3")),  ])),
  l("BB3", n("3", [a("OBJECT","3")], [
     e("",[],r("BB4")),  ])),
  l("BB4", n("4", [a("OBJECT","4")], [
     e("",[],r("BB12")),
     e("",[],r("BB5")),  ])),
  l("BB5", n("5", [a("OBJECT","5")], [
     e("",[],r("BB6")),  ])),
  l("BB6", n("6", [a("OBJECT","6")], [
     e("",[],r("BB7")),  ])),
  l("BB7", n("7", [a("OBJECT","7")], [
     e("",[],r("BB10")),
     e("",[],r("BB8")),  ])),
  l("BB8", n("8", [a("OBJECT","8")], [
     e("",[],r("BB9")),  ])),
  l("BB9", n("9", [a("OBJECT","9")], [
     e("",[],r("BB7")),  ])),
  l("BB10", n("10", [a("OBJECT","10")], [
     e("",[],r("BB11")),  ])),
  l("BB11", n("11", [a("OBJECT","11")], [
     e("",[],r("BB4")),  ])),
  l("BB12", n("12", [a("OBJECT","12")], [  ])),
#ifdef DAVINCI1
])
#else
]
#endif

************************************************************************/


// Display the CFG using daVinci
//
void show_cfg(CFG *cfg)
{
  if (cfg == NULL)
    cfg = g_comp_unit->Cfg();

  CFG_ITER cfg_iter(cfg);
  BB_NODE *bb;
  BB_LIST_ITER bb_succ_iter;
  BB_NODE *succ;
  FILE *fp;

  fp = Init_daVinci();

#ifdef DAVINCI1
  fprintf(fp, "new_term([\n");
#else
  fprintf(fp, "graph(new([");
#endif
  FOR_ALL_NODE(bb, cfg_iter, Init()) {
    INT32 id = bb->Id();
    INT32 lab_id = bb->Labnam();    
    if (lab_id) {
      fprintf(fp, "  l(\"BB%d-L%d\", n(\"%d-L%d\", [a(\"OBJECT\",\"%d-L%d\")], [", id,lab_id,id,lab_id,id,lab_id);
    }
    else {
      fprintf(fp, "  l(\"BB%d\", n(\"%d\", [a(\"OBJECT\",\"%d\")], [", id,id,id);
    }
    FOR_ALL_ELEM( succ, bb_succ_iter, Init(bb->Succ())) {
      if (succ->Labnam()) {
        fprintf(fp, "e(\"\",[],r(\"BB%" PRIdPTR "-L%d\")),", succ->Id(), succ->Labnam());
      }
      else {
        fprintf(fp, "e(\"\",[],r(\"BB%" PRIdPTR "\")),", succ->Id());
      }
    }
		 
    fprintf(fp, "  ])),");
  }
#ifdef DAVINCI1
  fprintf(fp, "])\n");
#else
  fprintf(fp, "]))\n");
#endif
}



//  Zone debugging
  
static FILE *fp = stdout;

static void show_graph_core(successor_graph& g)
{
  vector<bool> visited(g.size(), false);
  int i;

  fprintf(fp,"[");
  for (i = 0; i < g.size(); ++i) {
    if (g[i].size() > 0) {
      fprintf(fp, "l(\"%d\", n(\"\",[a(\"OBJECT\",\"%d\")],[", i, i);
      for (successor_graph::fast_iterator e = g[i].begin();
	   e != g[i].end();
	   ++e) {
	vertex_id out = second(*e);
	fprintf(fp, "l(\"%d->%d\",e(\"\",[],r(\"%d\"))),",i,out,out);
	visited[out] = true;
      }
      fprintf(fp, "])),");      
    }
  }
  for (i = 0; i < g.size(); ++i) {
    if (g[i].size() == 0 && visited[i]) {
      fprintf(fp, "l(\"%d\", n(\"\",[a(\"OBJECT\",\"%d\")],[", i, i);
      fprintf(fp, "])),");      
    }
  }
  fprintf(fp,"]");
}



void show_graph(successor_graph& g) 
{
  fp = New_daVinci();
  fprintf(fp, "graph(new(");
  show_graph_core(g);
  fprintf(fp,"))\n");
}

void mark_attr_begin()
{
  fprintf(fp, "graph(change_attr([");
}

void mark_attr_end()
{
  fprintf(fp, "]))\n");
}

static void mark_vertex(vertex_id i)
{
  // graph(change_attr([node("5",[a("COLOR","red")])]))
  fprintf(fp, "node(\"%d\",[a(\"COLOR\",\"red\")]),",i);
}

void mark_translated_vertex(vertex_id o, vertex_id n)
{
  // graph(change_attr([node("5",[a("COLOR","red")])]))
  fprintf(fp, "node(\"%d\",[a(\"COLOR\",\"red\"),a(\"OBJECT\",\"%d(%d)\")]),",n,n,o);
}

static void mark_edge(edge e)
{
  fprintf(fp, "edge(\"%d->%d\",[a(\"EDGECOLOR\",\"red\")]),",
	  e.first, e.second);
}

void mark_zone(zone& z)
{
  zone::iterator ei;
  mark_attr_begin();
  for (ei = z.clone.begin(); ei != z.clone.end(); ++ei) {
    mark_edge(*ei);
    mark_vertex((*ei).second);
    mark_vertex((*ei).first);
  }
  for (ei = z.entry.begin(); ei != z.entry.end(); ++ei) {
    mark_edge(*ei);
    mark_vertex((*ei).second);
  }
  for (ei = z.exit.begin(); ei != z.exit.end(); ++ei) {
    mark_edge(*ei);
    mark_vertex((*ei).first);
  }
  mark_attr_end();
}

void show_zone(zone& z)
{
  successor_graph g;
  add_edge(g, z.clone.begin(), z.clone.end());
  add_edge(g, z.entry.begin(), z.entry.end());
  add_edge(g, z.exit.begin(), z.exit.end());
  add_edge(g, z.side_entry.begin(), z.side_entry.end());
  show_graph(g);
  mark_zone(z);
}


void show_all_zones(successor_graph& g, zone_iterator first, zone_iterator last)
{
  show_graph(g);
  while (first != last) {
    if (!(*first).skip)
      mark_zone(*first);
    ++first;
  }
}



#endif


