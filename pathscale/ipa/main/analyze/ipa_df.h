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


// ====================================================================
// ====================================================================
//
// Module: ipa_df.h
//
// Revision history:
//  19-Sep-95 - Housekeeping
//
// Description:
//
// This package provides a generic interface for solving dataflow
// problems over the call graph.
//
// ====================================================================
// ====================================================================

#ifndef cxx_ipa_df_INCLUDED
#define cxx_ipa_df_INCLUDED

/* specify the direction of the dataflow problem */
enum DF_DIRECTION {
  FORWARD, 
  BACKWARD
};


/* every dataflow problem must first initialize this structure          */
/* note, the dataflow solver will initialize the entry and exit nodes   */
/* and the depth first ordering of vertices is initialized on a demand  */
/* driven fashion. That way it is created only once if several dataflow */
/* problems are solved on the same graph                                */
 
struct DFS {
  DF_DIRECTION di;    /* data flow direction                         */
  NODE_INDEX entry, exit; /* entry and exit nodes that connect the entire graph */
  DFN *d;             /* depth first ordering of vertices            */
  INT change;         /* change field, determine if dataflow problem */
                      /* has settled                                 */

/* array of in annotations, one for each node in the graph, it is    */
/* used to hold the result of the meet operation                     */
  void* *in;

/* array of out annotations, one for each node in the graph, it is   */
/* used to hold the result of the trans operation                    */
  void* *out;

};


/* access macros */

#define DFS_di(df)       ((df)->di)
#define DFS_d(df)        ((df)->d)
#define DFS_entry(df)    ((df)->entry)
#define DFS_exit(df)     ((df)->exit)
#define DFS_in(df)       ((df)->in)
#define DFS_out(df)      ((df)->out)
#define DFS_in_i(df,i)   ((df)->in[i])
#define DFS_out_i(df,i)  ((df)->out[i])
#define DFS_change(df)   ((df)->change)


class IPA_NODE;
class IPA_EDGE;

class IPA_DATA_FLOW
{
 private:

  DF_DIRECTION d;
  DFS *df;

  void iterative_solver(DFS*);
  void dataflow(DFS*);

 protected:

  MEM_POOL *m;

 public:

  IPA_DATA_FLOW (DF_DIRECTION df, MEM_POOL *m);

  IPA_NODE* Get_caller(IPA_EDGE *callee);
  IPA_NODE* Get_callee(IPA_EDGE *callee);
  IPA_NODE* Clone(IPA_NODE *n);

  void Init();
  void Print(FILE* fp);

  virtual void Solve();
  virtual void* Meet(void* in,void* vertex, INT *change);
  virtual void* Trans(void* in, void* out, void* vertex, INT *change);
  virtual void InitializeNode(void*) {};
  virtual void Print_entry(FILE *fp, void* out, void* n);
  virtual void PostProcessIO(void *){};
};

#endif /* cxx_ipa_df_INCLUDED */
