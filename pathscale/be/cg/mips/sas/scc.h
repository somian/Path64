/*
 * Copyright 2005, 2007 PathScale Inc.  All Rights Reserved.
 *

   Path64 is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by
   the Free Software Foundation version 3

   Path64 is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
   or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
   License for more details.

   You should have received a copy of the GNU General Public License
   along with Path64; see the file COPYING.  If not, write to the Free
   Software Foundation, 51 Franklin Street, Fifth Floor, Boston, MA
   02110-1301, USA.

 */

/*
  Representation and creation of Strongly_Connected_Components (SCCs).
  The definition of a SCC is that to elements a,b are in the same strongly
  connected component iff there is a path from a to b and from b to a.
  The SCCs are thus a representation of recurrences in the code.  (Note that
  elements that do not belong to any graph cycle have components to themselves.)
*/


#ifndef _SCC_H
#define _SCC_H

#include "swp.h"

class COST {
private:
  int latency;
  int omega;

public:
  const int COST_latency() const { return latency; }
  void Set_COST_latency( int l ) { latency = l; }
  const int COST_omega() const   { return omega; }
  void Set_COST_omega( int o )   { omega = o; }

};

class COST_VEC {
private:
  int   length;
  int   num_elements;
  COST* costs;

public:

  const int COST_VEC_length() const         { return length; }
  void Set_COST_VEC_length( int len )       { length = len; }
  const int COST_VEC_num_elements() const   { return num_elements; }
  void Set_COST_VEC_num_elements( int n )   { num_elements = n; }
  const COST* COST_VEC_costs() const        { return costs; }
  void Set_COST_VEC_costs( COST* costs )    { this->costs = costs; }
  const COST* COST_VEC_cost( int i )  const { return &costs[i]; }
  const int COST_VEC_latency( int i ) const { return costs[i].COST_latency(); }
  void Set_COST_VEC_latency( int i, int l ) { costs[i].Set_COST_latency(l); }
  const int COST_VEC_omega( int i )  const  { return costs[i].COST_omega(); }
  void Set_COST_VEC_omega( int i, int o )   { costs[i].Set_COST_omega(o); }

  void Add_Cost( int, int );
  bool Is_Max_Cost( int, int, int, int );
};


class SCC {
 private:

  int   id;
  const OPR** members;
  int   member_count;
  int   RecMII;

  int   mii; // used to prune the cost vector

  void Add_Maximal_Costs( COST_VEC*, COST_VEC*, COST_VEC* );

 public:

  SCC() {}
  ~SCC() {}

  const int SCC_id() const              { return id; }
  void Set_SCC_id( int id )             { this->id = id; }
  const OPR** SCC_members() const       { return members; }
  const OPR* SCC_member( int m ) const  { return members[m]; }
  void Set_SCC_members( const OPR** m ) { members = m; }
  const int SCC_member_count() const    { return member_count; }
  void Set_SCC_member_count( int c )    { member_count = c; }
  const int SCC_RecMII() const          { return RecMII; }
  void Set_SCC_RecMII( int mii )        { RecMII = mii; }

  void Compute_SCC_RecMII( int );
  void Print( FILE* f = stdout );
};


class Compute_SCCs {
 private:
  SBB* bb;
  SCC** SCCs;
  int SCC_Count;
  int max_scc_member_count;
  int SCC_mark_count;  /* To be used only by 1 set macros. */
  int mii;

  /* Stack of OP*s used by Tarjan's algorithm.
     Always points at the next location to add a new element. Grows upwards. */
  const OPR** stack;

  void Build_SCCs();
  void Visit( const OPR* );
  void Make_SCC( const OPR* );

  TALK talk;

 public:
  Compute_SCCs( SBB* bb, int mii, TALK talk ) : bb( bb ), mii( mii ), talk( talk ) {}
  ~Compute_SCCs() {}

  int Compute_RecMII_through_SCCs();
};


#endif /* _SCC_H */
