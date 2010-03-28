/*
 * Copyright (C) 2007. QLogic Corporation. All Rights Reserved.
 */

/*
 * Copyright 2005 PathScale, Inc.  All Rights Reserved.
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

#include "comm.h"
#include "swp.h"
#include "scc.h"


/* This type holds the state we need to track for each OPR during  Tarjan's algorithm. */
class SCC_OPR {
private:
  int   dfs_number;
  int   low_link;
  bool  stacked;

  SCC*  scc;       /* belongs to which scc. */
  int   indx;      /* indx number for that scc */

public:
/* Accessors for SCC_OP structure. */

  const SCC* OPR_scc() const           { return scc; }
  void Set_OPR_scc( SCC* scc )         { this->scc = scc; }
  const int OPR_scc_indx() const       { return indx; }
  void Set_OPR_scc_indx( int indx )    { this->indx = indx; }
  const int OPR_dfs_number() const     { return dfs_number; }
  void  Set_OPR_dfs_number( int num )  { dfs_number = num; }
  const int OPR_low_link() const       { return low_link; }
  void Set_OPR_low_link( int link )    { low_link = link; }
  bool OPR_stacked() const             { return stacked; }
  void Set_OPR_stacked()               { stacked = true; }
  void Reset_OPR_stacked()             { stacked = false; }
  bool OPR_visited() const             { return OPR_dfs_number() >= 0; }
  void Reset_OPR_visited()             { Set_OPR_dfs_number( -1 ); }
  
};

static SCC_OPR* scc_oprs = NULL;
#define Get_SCC_OPR(o)   (&scc_oprs[OPR_indx(o)])

/* Storage for all the _members vectors are preallocated, since each OP is a member
   of exactly one SCC.  */
static const OPR** members_vec = NULL;

/* Counter used to assign DFS numbers to OPs during Tarjan's algorithm. */
static int dfs_count = 0;

static COST_VEC** cost_table = NULL;


void SCC::Print( FILE* f )
{
  fprintf( f, "\n===================== S C C (%d) ================\n", SCC_id() );
  fprintf( f, "RecMII:%d\n", SCC_RecMII() );

  for( int indx = 0; indx < SCC_member_count(); indx++ ){
    const OPR* op = SCC_member( indx );
    SCC_OPR* scc_op = Get_SCC_OPR( op );

    fprintf( f, "\top%d (scc_indx %d)\n", OPR_indx(op), scc_op->OPR_scc_indx() );
  }

  fprintf( f, "================================================\n" );
}


/* Everything on the stack above and including "op" are members of
   the same stongly connected component.  Pop them off the stack and
   put them into a new SCC.  Initialize its fields. */

void Compute_SCCs::Make_SCC( const OPR* op )
{
  int n, i;
  const OPR** mp = NULL;
  const OPR** pop = NULL;
  SCC* scc = SCCs[SCC_Count] = (SCC*)MEM_POOL_Alloc( &sas_pool, sizeof(SCC) );

  scc->Set_SCC_id( SCC_Count );
  SCC_Count++;

  /* Count elements in component.
   */
  for( pop = stack - 1; *pop != op; pop-- )
    ;

  n = stack - pop;
  scc->Set_SCC_member_count( n );

  max_scc_member_count = MAX( max_scc_member_count, n );

  /* Allocate memory for members:  */
  scc->Set_SCC_members( (mp = members_vec) );
  members_vec += n;

  /* Loop up the stack from op, adding elements to scc, and
     setting scc related fields.  Can use do..while because we know
     there will always be at least 1 (op) element on stack.  */

  i = 0;
  do {
    const OPR* sop = pop[i];
    SCC_OPR* scc_sop = Get_SCC_OPR( sop );

    mp[i] = sop;

    scc_sop->Reset_OPR_stacked();
    scc_sop->Set_OPR_scc( scc );
    scc_sop->Set_OPR_scc_indx( i );

  } while( ++i < n );


  /* Pop OP and everything above off the stack. */
  stack = pop;
}

/* Recursive DFS routine for finding strongly connected components.
   This is the amazing part. */

void Compute_SCCs::Visit( const OPR* op )
{
  /* The op becomes 'visited' by creating the op-map entry for the op.  */
  SCC_OPR* scc_op = Get_SCC_OPR( op );

  scc_op->Set_OPR_dfs_number( dfs_count );
  scc_op->Set_OPR_low_link( dfs_count );
  dfs_count++;

  *stack++ = op;
  scc_op->Set_OPR_stacked();

  for( int arc = 0; arc < OPR_num_succs(op); arc++ ){
    DEP* dep = OPR_succ( op, arc );
    OPR* succ = DEP_succ( dep );
    SCC_OPR* scc_succ = Get_SCC_OPR( succ );

    if( !scc_succ->OPR_visited() ){
      Visit( succ );
      scc_op->Set_OPR_low_link( MIN( scc_op->OPR_low_link(), scc_succ->OPR_low_link() ) );

    } else if( ( scc_succ->OPR_dfs_number() < scc_op->OPR_dfs_number() ) &&
	       scc_succ->OPR_stacked() ){
      scc_op->Set_OPR_low_link( MIN( scc_op->OPR_low_link(), scc_succ->OPR_dfs_number() ) );
    }
  }

  /* Everything on the stack above and including op is a strongly
    connected component. */

  if( scc_op->OPR_low_link() == scc_op->OPR_dfs_number() ){
    Make_SCC( op );
  }
}


/* Build Strongly_Connected_Components from bb. */

void Compute_SCCs::Build_SCCs()
{
  int op_count = bb->Oprs();

  max_scc_member_count = 0;

  scc_oprs = (SCC_OPR *) MEM_POOL_Alloc( &sas_pool, sizeof(SCC_OPR) * op_count );

  SCC_mark_count = 0;

  SCCs = (SCC **) MEM_POOL_Alloc( &sas_pool, sizeof(SCC *) * op_count );
  members_vec = (const OPR **)
    MEM_POOL_Alloc( &sas_pool, sizeof(OPR *) * op_count );

  SCC_Count = 0;

  stack = (const OPR **) MEM_POOL_Alloc( &sas_pool, sizeof(OPR *) * op_count );
  dfs_count = 0;

  for( int indx = 0; indx < op_count; indx++ ){
    const OPR* op = bb->Opr( indx );
    Get_SCC_OPR( op )->Reset_OPR_visited();
  }

  for( int indx = 0; indx < op_count; indx++ ){
    const OPR* op = bb->Opr( indx );

    if( !(Get_SCC_OPR( op )->OPR_visited()) ){
      Visit( op );
    }
  }
}


void COST_VEC::Add_Cost( int omega, int latency )
{
  int num_elements = COST_VEC_num_elements();

  if( num_elements == COST_VEC_length() ){
    int new_num_elements = num_elements * 2;
    COST* new_vec = (COST*)MEM_POOL_Alloc( &sas_pool, sizeof(new_vec[0]) * new_num_elements );
    const COST* old_vec = COST_VEC_costs();

    for( int i = 0; i < num_elements; i++ ){
      new_vec[i] = old_vec[i];
    }

    Set_COST_VEC_length( new_num_elements );
    Set_COST_VEC_costs( new_vec );
    ASSERT( false );
  }

  Set_COST_VEC_omega( num_elements, omega );
  Set_COST_VEC_latency( num_elements, latency );
  Set_COST_VEC_num_elements( num_elements + 1 );
}


/* Is the cost <'omega','latency'> maximal relative to the elements in 'cv' given 'mii'?
   'Offset' gives the index of the first element in 'cv' to check.
 */
bool COST_VEC::Is_Max_Cost( int omega, int latency, int min_ii, int offset )
{
  for( int i = offset; i < COST_VEC_num_elements(); i++ ){
    const COST *cost = COST_VEC_cost( i );
    int cvomega   = cost->COST_omega();
    int cvlatency = cost->COST_latency();

    if( ( omega == cvomega && latency <= cvlatency ) ||
	( omega >= cvomega &&
	  (latency - cvlatency) <= ((omega - cvomega) * min_ii) ) ){
      return false;
    }
  }

  return true;
}


/* 'cvij', 'cvik', 'cvkj' are existing entries in the cost table.
   Make 'cvij' be the maximal costs (given the current MII) in the
   set cvij U (cvik + cvkj).  In other words, we want to consider all the
   paths from i to j via k as well as all the paths from i to j we
   have already considered.  Only those that we can be maximal (given
   the MII) are included in the resulting 'cvij'.
*/
void SCC::Add_Maximal_Costs( COST_VEC* cvij, COST_VEC* cvik, COST_VEC* cvkj )
{
  bool bAdded = false;

  /* First consider costs in the cross product of cjik and cvkj.
     We'll add a cost if it is maximal relative to the current costs
     in cvij. */

  for( int i = 0; i < cvik->COST_VEC_num_elements(); i++ ){
    const COST* c_ik = cvik->COST_VEC_cost(i);
    int   ik_omega   = c_ik->COST_omega();
    int   ik_latency = c_ik->COST_latency();

    for( int j = 0; j < cvkj->COST_VEC_num_elements(); j++ ){
      const COST* c_kj  = cvkj->COST_VEC_cost( j );
      int   kj_omega    = c_kj->COST_omega();
      int   kj_latency  = c_kj->COST_latency();
      int   ikj_omega   = ik_omega + kj_omega;
      int   ikj_latency = ik_latency + kj_latency;

      if( cvij->Is_Max_Cost( ikj_omega,ikj_latency, mii,0 ) ){
        cvij->Add_Cost( ikj_omega, ikj_latency );
	bAdded = true;
      }
    }
  }

  //if( !bAdded )    return;

  /* Now each cost in cvij is maximal relative to the preceeding
     costs, but not necessarily relative to the succeeding costs.
     We'll compare each cost to the succeeding costs, and delete it if
     it is not (possibly) maximal.  Deletion is accompilshed by
     copying the last element of cvij into the element to be deleted
     and decrementing the length of cvij.  We consider the elements in
     reverse order so that will not disturb the part of the vector not
     yet processed.   */

  int cvij_num_elements = cvij->COST_VEC_num_elements();

  for( int i = cvij_num_elements - 2; i >= 0; i-- ){
    COST *cpij = (COST*)cvij->COST_VEC_cost( i );
    int omega   = cpij->COST_omega();
    int latency = cpij->COST_latency();

    if( !cvij->Is_Max_Cost( omega, latency, mii, i+1 ) ){
      ASSERT( bAdded );

      /* Delete by replacing with last element... */
      if( i != cvij_num_elements - 1 ){
        const COST* ij_last = cvij->COST_VEC_cost( cvij_num_elements - 1 );

        *cpij = *ij_last;           /* Class copy. */
      }

      cvij_num_elements--;
    }
  }

  cvij->Set_COST_VEC_num_elements( cvij_num_elements );
}


void SCC::Compute_SCC_RecMII( int bb_mii )
{
  int count = SCC_member_count();

  mii = bb_mii;

  // Clear up cost_table

  for( int i = 0; i < count; i++ ){
    for( int j = 0; j < count; j++ ){
      COST_VEC* cv = &cost_table[i][j];
      cv->Set_COST_VEC_num_elements( 0 );
    }
  }

  // Initialize cost_table.

  for( int i = 0; i < count; i++ ){
    const OPR* op = SCC_member( i );
    SCC_OPR* scc_op = Get_SCC_OPR( op );

    for( int arc = 0; arc < OPR_num_succs( op ); arc++ ){
      DEP* dep = OPR_succ( op, arc );
      OPR* succ_op = DEP_succ(dep);
      SCC_OPR* scc_succ = Get_SCC_OPR( succ_op );

      if( scc_succ->OPR_scc() != this )
	continue;

      COST_VEC* cv = &cost_table[scc_op->OPR_scc_indx()][scc_succ->OPR_scc_indx()];

      cv->Add_Cost( DEP_omega(dep), DEP_latency(dep) );

      if( succ_op == op ){
	Set_SCC_RecMII( MAX( SCC_RecMII(),
			     Ceiling_Divide( DEP_latency(dep), DEP_omega(dep)) ) );
	mii = MAX( mii, SCC_RecMII() );
      }
    }
  }

  /* Run Floyd's All-Pares-Shortest-Path algorithm. */

  for( int k = 0 ; k < count; k++ ){
    for( int i = 0; i < count; i++ ){
      for ( int j = 0; j < count; j++ ){

	/* Don't bother checking self paths through self.
	   These are guaranteed to not be maximal. */

	if( i == j && j == k )
	  continue;

        /* Consider the existing costs of paths from i to j and the
	   costs of paths from i to j via k.  Filter out those that
	   are cannot be maximal (given a min II) and replace the
	   costs of paths from i to j with the result.	 */

        Add_Maximal_Costs( &cost_table[i][j], &cost_table[i][k], &cost_table[k][j] );

        /* Check to see whether we now have a self path with better cost. */

        Add_Maximal_Costs( &cost_table[i][i], &cost_table[i][j], &cost_table[j][i] );

	/* Update MII and SCC_RecMII here. */

	COST_VEC* cv = &cost_table[i][i];
	for( int ele = 0; ele < cv->COST_VEC_num_elements(); ele++ ){
	  const COST* cost = cv->COST_VEC_cost( ele );
	  int path_mii = Ceiling_Divide( cost->COST_latency(), cost->COST_omega() );

	  Set_SCC_RecMII( MAX( SCC_RecMII(), path_mii ) );
	  mii = MAX( mii, SCC_RecMII() );
	}
      }
    }
  }
}


int Compute_SCCs::Compute_RecMII_through_SCCs()
{
  int rec_mii = 0;

  Build_SCCs();

  // Allocate space for cost table.
  cost_table = (COST_VEC **)
    MEM_POOL_Alloc( &sas_pool, sizeof(COST_VEC *) * max_scc_member_count );

  for( int i = 0; i < max_scc_member_count; i++ ){
    const int length = 4;
    COST* costs = (COST*)MEM_POOL_Alloc( &sas_pool, sizeof(costs[0]) * max_scc_member_count * length );
    cost_table[i] = (COST_VEC *) MEM_POOL_Alloc( &sas_pool, sizeof(COST_VEC) * max_scc_member_count );

    for( int j = 0; j < max_scc_member_count; j++ ){
      COST_VEC* cv = &cost_table[i][j];

      cv->Set_COST_VEC_length( length );
      cv->Set_COST_VEC_num_elements( 0 );
      cv->Set_COST_VEC_costs( costs );
      costs += 4;
    }
  }

  for( int i = 0; i < SCC_Count; i++ ){
    SCC* scc = SCCs[i];

    if( scc->SCC_member_count() > 1 ){
      scc->Compute_SCC_RecMII( mii );
      rec_mii = MAX( rec_mii, scc->SCC_RecMII() );
      mii = MAX( mii, rec_mii );

      /* Use cost_table to update the DDG. */

      for( int i = 0; i < scc->SCC_member_count(); i++ ){
	const OPR* iop = scc->SCC_member( i );

	for( int j = 0; j < scc->SCC_member_count(); j++ ){
	  const OPR* jop = scc->SCC_member( j );
	  COST_VEC* cv  = &cost_table[i][j];
	  int       num = cv->COST_VEC_num_elements();

	  for( int k = 0; k < num; k++ ){
	    bb->Add_Arc( (OPR*)iop, (OPR*)jop, DEP_SCC, MAX_OPNDS,
			 cv->COST_VEC_latency(k), cv->COST_VEC_omega(k), mii );
	  }
	}
      }

    } else {
      const OPR* op = scc->SCC_member( 0 );

      for( int arc = 0; arc < OPR_num_succs(op); arc++ ){
	DEP* dep = OPR_succ( op, arc );

	if( DEP_succ( dep ) != DEP_pred( dep ) )
	  continue;

	scc->Set_SCC_RecMII( MAX( scc->SCC_RecMII(),
				  Ceiling_Divide( DEP_latency(dep), DEP_omega(dep) ) ) );

	rec_mii = MAX( rec_mii, scc->SCC_RecMII() );
	mii = MAX( mii, rec_mii );
      }
    }

    if( talk >= LACONIC )
      scc->Print();
  }

  return rec_mii;
}
