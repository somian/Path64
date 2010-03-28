/*
 * Copyright (C) 2007 PathScale, LLC.  All Rights Reserved.
 */

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
#include "mrt.h"
#include "modulo.h"

// Maintain the searching space at each enumeration node.

class SPACE {
private:
  uint16_t** Lb;
  uint16_t** Ub;
  RESOURCE_MASK** Res_Mask;
  uint16_t stack_top;
  SBB* bb;
  int _mii;

public:
  void Init( SBB* bb, int mii )
  {
    this->bb = bb;
    _mii = mii;
    stack_top = 0;

    if( !bb->is_cyclic() )
      return;

    int i = bb->Oprs() + 1;
    Lb = (uint16_t **) MEM_POOL_Alloc( &sas_pool, sizeof(uint16_t *) * i );
    Ub = (uint16_t **) MEM_POOL_Alloc( &sas_pool, sizeof(uint16_t *) * i );
    Res_Mask = (RESOURCE_MASK **) MEM_POOL_Alloc( &sas_pool,
						  sizeof(RESOURCE_MASK *) * i );

    for( i = 0; i < bb->Oprs(); i++ ){
      Lb[i] = (uint16_t *)
	MEM_POOL_Alloc( &sas_pool, sizeof(uint16_t) * bb->Oprs() );
      Ub[i] = (uint16_t *)
	MEM_POOL_Alloc( &sas_pool, sizeof(uint16_t) * bb->Oprs() );
      Res_Mask[i] = (RESOURCE_MASK *)
	MEM_POOL_Alloc( &sas_pool, sizeof(RESOURCE_MASK) * bb->Oprs() );
    }
  }

  void Set_MII( int mii )
  {
    _mii = mii;
  }

  // TightenLb attempts to increase the release_time of the successors of op.
  // Returns false if release_time ever excedes deadline.  Invoke TightenLb
  // after increasing op's release_time to propagate the change.
  //
  bool TightenLb( OPR* op )
  {
    if( OPR_release_time( op ) > OPR_deadline( op ) )
      return false;

    for( int arc = 0; arc < OPR_num_succs( op ); arc++ ){
      DEP* dep = OPR_succ( op, arc );
      OPR* succ_op = DEP_succ(dep);
      int lb = OPR_release_time(op) + DEP_latency(dep) - DEP_omega(dep) * _mii;

      if( OPR_release_time( succ_op ) < lb ){
	OPR_release_time( succ_op ) = lb;
	if( !TightenLb( succ_op ) )
	  return false;
      }
    }

    return true;
  }

  // TightenUb attempts to decrease the deadline of the predecessors of op.
  // Returns false if release_time ever excedes deadline.  Invoke TightenUb
  // after decreasing op's deadline to propagate the change.
  //
  bool TightenUb( OPR* op )
  {
    if( OPR_release_time( op ) > OPR_deadline( op ) )
      return false;

    for( int arc = 0; arc < OPR_num_preds( op ); arc++ ){
      DEP* dep = OPR_pred( op, arc );
      OPR* pred_op = DEP_pred(dep);
      int ub = OPR_deadline(op) - ( DEP_latency(dep) - DEP_omega(dep) * _mii );

      if( OPR_deadline( pred_op ) > ub ){
	OPR_deadline( pred_op ) = ub;
	if( !TightenUb( pred_op ) )
	  return false;
      }
    }

    return true;
  }

  // Pop restores the values of release_time, deadline, and res_mask for
  // all OPRs from the previous Push onto the SPACE stack.
  //
  void Pop()
  {
    stack_top--;

    for( int indx = 0; indx < bb->Oprs(); indx++ ){
      OPR* op = (OPR*)bb->Opr( indx );
      OPR_release_time( op ) = Lb[stack_top][indx];
      OPR_deadline( op )     = Ub[stack_top][indx];
      Set_OPR_res_mask( op, Res_Mask[stack_top][indx] );
    }
  }

  // Push stores the current values of release_time, deadline, and res_mask
  // for all OPRs onto the SPACE stack.
  //
  void Push()
  {
    for( int indx = 0; indx < bb->Oprs(); indx++ ){
      const OPR* op = bb->Opr( indx );
      Lb[stack_top][indx] = OPR_release_time( op );
      Ub[stack_top][indx] = OPR_deadline( op );
      Res_Mask[stack_top][indx] = OPR_res_mask( op );
    }

    stack_top++;  
  }

  void Print( ARRAY* array, FILE* f = stdout ) const
  {
    fprintf( f, "\n=================== S P A C E ======================\n" );

    fprintf( f, "MII:%d\n", _mii );

    for( int indx = 0; indx < bb->Oprs(); indx++ ){
      const OPR* op = bb->Opr( indx );
      const char c = array->Find( (void*)op ) ? '*' : ' ';
      
      fprintf( f, "%cop%d[%d,%d] ", c, indx, OPR_release_time( op ), OPR_deadline( op ) );
      fprintf( f, "%s(%s)", Opcode_Name( OPR_opcode(op) ), ((MRT*)indx)->Resource_Name( op ) );

      if( OPR_num_succs( op ) > 0 ){
	fprintf( f, " -> " );

	for( int arc = 0; arc < OPR_num_succs( op ); arc++ ){
	  DEP* dep = OPR_succ( op, arc );
	  OPR* succ_op = DEP_succ(dep);
	  fprintf( f, "op%d ", OPR_indx(succ_op) );
	}
      }

      fprintf( f, "\n" );
    }

    fprintf( f, "=====================================================\n" );
  }

  const bool Stack_Is_Empty() const
  {
    return stack_top == 0;
  }

} space;


// SCHED::Compute_Critical_Path calculates OPR_height, OPR_Dist2Root, and
// OPR_Dist2Leaf for each OPR:
//  OPR_Dist2Root = max latency required from start of BB to reach this OPR
//  OPR_Dist2Leaf = max latency required from this OPR to reach end of BB
//  OPR_height = maximum number of arcs between this OPR and end of BB
// Returns maximum over all OPR_Dist2Root and OPR_Dist2Leaf values.
// Calculations ignore resource conflicts.
// NOTE: Nobody uses OPR_height, and calculation is wrong for loops.
//
int SCHED::Compute_Critical_Path()
{
  int cp_fwd = 0, cp_rev = 0;

  for( int indx = 0; indx < bb->Oprs(); indx++ ){
    OPR* op = (OPR*)bb->Opr( indx );
    OPR_height( op ) = OPR_Dist2Root( op ) = OPR_Dist2Leaf( op ) = 0;
  }

  for( int indx = 0; indx < bb->Oprs(); indx++ ){
    const OPR* op = bb->Opr( indx );

    for( int arc = 0; arc < OPR_num_succs(op); arc++ ){
      DEP* dep = OPR_succ( op, arc );
      OPR* succ_op = DEP_succ(dep);

      int dist = OPR_Dist2Root(op) + DEP_latency(dep) - DEP_omega(dep) * _mii;
      cp_fwd = MAX( cp_fwd, dist );
      OPR_Dist2Root(succ_op) = MAX( dist, OPR_Dist2Root(succ_op) );
    }
  }

  for( int indx = bb->Oprs() - 1; indx >= 0; indx-- ){
    const OPR* op = bb->Opr( indx );

    for( int arc = 0; arc < OPR_num_preds(op); arc++ ){
      DEP* dep = OPR_pred( op, arc );
      OPR* pred_op = DEP_pred(dep);

      int dist = OPR_Dist2Leaf(op) + DEP_latency(dep) - DEP_omega(dep) * _mii;
      cp_rev = MAX( cp_rev, dist );
      OPR_Dist2Leaf(pred_op) = MAX( dist, OPR_Dist2Leaf(pred_op) );

      dist = OPR_height( op ) + 1;
      OPR_height(pred_op) = MAX( OPR_height(pred_op), dist );
    }
  }

  return MAX( cp_rev, cp_fwd );
}


// sort_by_deadline returns -1 / 1 / 1 if the first / second / neither
// argument has the earlier deadline, respectively.
//
static int sort_by_deadline( const void* a, const void* b )
{
  OPR* op1 = *(OPR**)a;
  OPR* op2 = *(OPR**)b;

  // The one with an earlier deadline goes first.
  if( OPR_deadline( op1 ) < OPR_deadline( op2 ) )
    return -1;

  else if( OPR_deadline( op2 ) < OPR_deadline( op1 ) )
    return 1;

#if 0
  uint8_t src1 = OPR_consumes_src( op1 );
  uint8_t src2 = OPR_consumes_src( op2 );

  if( ( ~src1 & src2 ) == 0 )
    return -1;

  else if( ( ~src2 & src1 ) == 0 )
    return 1;

  // The one with a smaller indx goes first.
  if( OPR_indx( op1 ) < OPR_indx( op2 ) )
    return -1;
#endif

  return 1;
}


// SCHED::Compute_Scheduling_Ranges invokes Compute_Critical_Path to
// calculate OPR_Dist2Root and OPR_Dist2Leaf for each OPR, then
// initializes _Lb, and each OPR_unsched_preds and OPR_unsched_succs.
//
void SCHED::Compute_Scheduling_Ranges()
{
  int cp = Compute_Critical_Path();

  if( talk >= VOLUBLE )
    printf( "critical path length: %d cycles\n", cp );

  _Lb = cp + 1;

  for( int indx = 0; indx < bb->Oprs(); indx++ ){
    OPR* op = (OPR*)bb->Opr( indx );

    OPR_unsched_preds( op ) = OPR_num_preds( op );
    OPR_unsched_succs( op ) = OPR_num_succs( op );

    // Don't count back arcs in OPT_unsched_preds.
    if( bb->is_cyclic() ){
      // otherwise, we need to work on OPR_unsched_succs.
      ASSERT( is_fwd );

      for( int arc = 0; arc < OPR_num_preds( op ); arc++ ){
	DEP* dep = OPR_pred( op, arc );
	OPR* pred_op = DEP_pred(dep);

	if( DEP_type( dep ) == DEP_SCC ||
	    DEP_omega( dep ) > 0 ){
	  OPR_unsched_preds( op )--;
	} else
	  ASSERT( OPR_indx( pred_op ) < OPR_indx( op ) );
      }
    }
  }
}


// SCHED::OPRs_Compare chooses which of OPRs a or b it prefers to schedule
// next.  This is strictly a helper procedure for SCHED::Select_Variable.
//
inline OPR* SCHED::OPRs_Compare( OPR* a, OPR* b ) const
{
  if( is_fwd ){
    // Pick the one with the earliest release time.
    if( OPR_release_time( a ) != OPR_release_time( b ) ){
      return ( OPR_release_time( a ) < OPR_release_time( b ) ) ? a : b;
    }

    // Pick the one with the earliest deadline
    if( OPR_deadline( a ) != OPR_deadline( b ) ){
      return ( OPR_deadline( a ) < OPR_deadline( b ) ) ? a : b;
    }

  } else {
    // For backward scheduling
    if( OPR_deadline( a ) != OPR_deadline( b ) ){
      return ( OPR_deadline( a ) < OPR_deadline( b ) ) ? b : a;
    }
    
    if( OPR_release_time( a ) != OPR_release_time( b ) ){
      return ( OPR_release_time( a ) < OPR_release_time( b ) ) ? b : a;
    }
  }

  // Pick the one with less hw resource.
  const RESOURCE_MASK a_mask = OPR_res_mask( a );
  const RESOURCE_MASK b_mask = OPR_res_mask( b );

  if( ( ~a_mask & b_mask ) == 0 )
    return b;

  if( ( a_mask & ~b_mask ) == 0 )
    return a;

  if( is_fwd ){
#if 0
    if( OPR_is_memory( a ) && !OPR_is_memory( b ) )
      return a;
    else if( !OPR_is_memory( a ) && OPR_is_memory( b ) )
      return b;
#else
    if( OPR_is_memory( a ) || OPR_is_memory( b ) ){
      if( OPR_is_load( a ) && !OPR_is_load( b ) )
	return a;
      else if( !OPR_is_load( a ) && OPR_is_load( b ) )
	return b;
      else if( OPR_is_memory( a ) && !OPR_is_memory( b ) )
	return a;
      else if( !OPR_is_memory( a ) && OPR_is_memory( b ) )
	return b;
    }
#endif

    // Last breaking.
    return ( OPR_indx( a ) < OPR_indx( b ) ) ? a : b;

  } else {
    if( OPR_is_memory( a ) && !OPR_is_memory( b ) )
      return b;
    else if( !OPR_is_memory( a ) && OPR_is_memory( b ) )
      return a;

    // Last breaking.
    return ( OPR_indx( a ) > OPR_indx( b ) ) ? a : b;
  }
}


// SCHED::Select_Variable selects one of the OPRs in ready_queue to be
// scheduled next.
//
OPR* SCHED::Select_Variable() const
{
  int num = ready_queue.Size() - 1;
  OPR* best_op = ready_queue[num];

  // should take register pressure into account!!!

  for( num--; num >= 0; num-- ){
    OPR* cur_op = ready_queue[num];
    best_op = OPRs_Compare( best_op, cur_op );
  }

  ASSERT( OPR_release_time( best_op ) <= OPR_deadline( best_op ) );

  return best_op;
}


// SCHED::Setup_Ready_Queue initializes ready_queue to contain exactly
// those OPRs which can be immediately scheduled without waiting for
// other OPRs to be scheduled first.
//
void SCHED::Setup_Ready_Queue()
{
  ready_queue.Reset();

  if( is_fwd ){
    for( int indx = 0; indx < bb->Oprs(); indx++ ){
      const OPR* op = bb->Opr( indx );
      int arc = 0;

      for( ; arc < OPR_num_preds( op ); arc++ ){
	DEP* dep = OPR_pred( op, arc );

	if( DEP_omega( dep ) == 0 &&
	    DEP_type( dep ) != DEP_SCC )
	  break;
      }
    
      if( arc == OPR_num_preds( op ) )
	ready_queue.Push( op );
    }

  } else {
    // Handle the bwd case here.
    for( int indx = 0; indx < bb->Oprs(); indx++ ){
      const OPR* op = bb->Opr( indx );
      int arc = 0;

      for( ; arc < OPR_num_succs( op ); arc++ ){
	DEP* dep = OPR_succ( op, arc );

	if( DEP_omega( dep ) == 0 &&
	    DEP_type( dep ) != DEP_SCC )
	  break;
      }
    
      if( arc == OPR_num_succs( op ) )
	ready_queue.Push( op );
    }
  }
  
  ASSERT( ready_queue.Size() > 0 );
}


bool SCHED::schedule( OPR* cur_op, bool bUp )
{
  OPR* child_op = NULL;
  // A res_mask could be changed by Find_First_Schedule_Cycle().
  const RESOURCE_MASK cur_op_res_mask = OPR_res_mask( cur_op );

  if( talk >= VOLUBLE ){
    printf( "[nautes %" SCNd64 "] op%d(%s) %s at cycle %d of [%d,%d]\n",
	    tree_nodes, OPR_indx( cur_op ), Opcode_Name(OPR_opcode( cur_op )),
	    bUp ? "up" : "down",
	    OPR_release_time( cur_op ),
	    OPR_release_time( cur_op ), OPR_deadline( cur_op ) );
  }

  if( tree_nodes++ > budget )
    return false;

  if( bUp ){
    ASSERT( OPR_release_time( cur_op ) <= OPR_deadline( cur_op ) );
    space.Push();

    ready_queue.Remove( cur_op );
    Set_OPR_scheduled( cur_op );
    mrt.Reserve_Resources( cur_op, OPR_release_time( cur_op ) );

    OPR_deadline( cur_op ) = OPR_release_time( cur_op );
    Set_OPR_order( cur_op, sch_order );
    sch_order++;

    if( !space.TightenUb( cur_op ) ){
      /* Impossible here for a large enough mii,
	 since (1) cost of back edges (parts of SCCs) <= 0;
	 and (2) release_time non-dec and deadline non-inc.
	 But how about resource constraints, like ldc and addiu? */
      ASSERT( false );
      goto TERMINATE;
    }

    // Update ready_queue after op is gone.

    for( int arc = 0; arc < OPR_num_succs(cur_op); arc++ ){
      DEP* dep = OPR_succ( cur_op, arc );
      OPR* succ_op = DEP_succ(dep);

      OPR_unsched_preds(succ_op)--;
      if( OPR_unsched_preds(succ_op) == 0 )
	ready_queue.Push( succ_op );

      int rt = OPR_release_time(cur_op) + DEP_latency(dep) - _mii * DEP_omega(dep);
      if( rt > OPR_release_time( succ_op ) ){
	OPR_release_time( succ_op ) = rt;
	if( !space.TightenLb( succ_op ) )
	  goto TERMINATE;
      }
    }
    
    if( ready_queue.Size() == 0 ){
      scheduled = true;
      return true;
    }

    // Tighten scheduling ranges based on the tigthened resources.

    for( int indx = 0; indx < bb->Oprs(); indx++ ){
      OPR* op = (OPR*)bb->Opr( indx );

      if( OPR_scheduled( op ) )
	continue;

      // Find_First_Schedule_Cycle checks data path constraints.
      int r = OPR_release_time( op );
      OPR_release_time( op ) = mrt.Find_First_Schedule_Cycle( op );
      if( r < OPR_release_time( op ) ){
	if( !space.TightenLb( op ) )
	  goto TERMINATE;
      }

      int d = OPR_deadline( op );
      OPR_deadline( op ) = mrt.Find_Last_Schedule_Cycle( op );
      if( d > OPR_deadline( op ) ){
	if( !space.TightenUb( op ) )
	  goto TERMINATE;
      }
    }

    if( !Adjust_space_hazards( cur_op ) ){
      ASSERT( false );
    }

  } else {
    // Handle the DOWN branch.

    int d = OPR_release_time( cur_op ) + _mii - 1;
    if( d < OPR_deadline( cur_op ) ){
      OPR_deadline( cur_op ) = d;
      if( !space.TightenUb( cur_op ) )
	goto TERMINATE;
    }

    OPR_release_time( cur_op )++;

    OPR_release_time( cur_op ) = mrt.Find_First_Schedule_Cycle( cur_op );

    if( !space.TightenLb( cur_op ) )
      goto TERMINATE;
  }

  child_op = Select_Variable();

  if( schedule( child_op, true ) || schedule( child_op, false ) ){
    return true;
  }

TERMINATE:

  if( !scheduled )
    Set_OPR_res_mask( cur_op, cur_op_res_mask );

  if( bUp ){
    Reset_OPR_scheduled( cur_op );
    mrt.Unreserve_Resources( cur_op, OPR_release_time( cur_op ) );
    ready_queue.Push( cur_op );

    sch_order--;
    ASSERT( OPR_order( cur_op ) == sch_order );
    Set_OPR_order( cur_op, -1 );

    for( int arc = 0; arc < OPR_num_succs(cur_op); arc++ ){
      DEP* dep = OPR_succ( cur_op, arc );
      OPR* succ_op = DEP_succ(dep);

      OPR_unsched_preds(succ_op)++;

      if( OPR_unsched_preds(succ_op) == 1 ){
	ready_queue.Remove( succ_op );
      }
    }

    space.Pop();
  }

  return false;
}


void SCHED::Print( FILE* f ) const
{
  fprintf( f, "\n====================== S C H E D ==========================\n" );

  if( bb->is_cyclic() ){
    fprintf( f, "%s Ops:%d MII:%d SC:%d Cost(%d,%d), LU:[%d,%d] nautes:%" SCNd64 " Total_Nautes:%" SCNd64 "\n",
	     scheduled ? "success" : "fail",
	     bb->Oprs(), _mii, stage_count, cost.length, cost.MaxLive, _Lb, _Ub,
	     tree_nodes, tree_nodes_total );
  } else {
    fprintf( f, "%s Ops:%d Length:%d MaxLive:%d, LB:%d\n",
	     is_fwd ? "fwd" : "bwd", bb->Oprs(), cost.length, cost.MaxLive, _Lb );
  }

  if( scheduled ){
    
    int to_cycle = bb->is_cyclic() ? _mii : cost.length;

    for( int cycle = 0; cycle < to_cycle; cycle++ ){
      fprintf( f, "cycle %d: ", cycle );

      for( int indx = 0; indx < bb->Oprs(); indx++ ){
	const OPR* op = bb->Opr( indx );

	if( OPR_release_time( op ) % to_cycle == cycle ){
	  fprintf( f, "op%d", indx );
	  if( bb->is_cyclic() ){
	    int stage = OPR_release_time( op ) / to_cycle;
	    fprintf( f, "[%d] ", stage );
	  }

	  fprintf( f, "(%s:%s) ", Opcode_Name( OPR_opcode(op) ),
		   MRT::res_per_cycle[OPR_res_id(op)]->RESOURCE_name() );
	}
      }
      
      fprintf( f, "\n" );
    }
  }

  fprintf( f, "===========================================================\n" );
}


// Initialize OPR_scheduled, OPR_release_time, and OPT_deadline for
// each OPR.
void SCHED::Setup_Release_Deadline( int U )
{
  for( int indx = 0; indx < bb->Oprs(); indx++ ){
    OPR* op = (OPR*)bb->Opr( indx );
    OPR_release_time( op ) = OPR_Dist2Root( op );
    OPR_deadline( op ) = U - 1 - OPR_Dist2Leaf( op );
    Reset_OPR_scheduled( op );
  }
}


/* Compute a schedule which can be fit into MII cycles. */
bool SCHED::Modulo_Scheduling()
{
  OPR* xfer_op = (OPR*)bb->Opr( bb->Oprs() - 1 );
  ASSERT( OPR_is_xfer(xfer_op) );

  mrt.Init( bb, _mii );

  // we should use LS to compute the UB !!!
  _Ub = mrt.Size();

  Setup_Ready_Queue();

  Compute_Scheduling_Ranges();

  /* Allocate space for scheduling-ranges at each instace. */
  space.Init( bb, _mii );

  OPR* first_op = Select_Variable();
  budget = bb->Oprs() << 7;

  while( _mii < _Ub ){
    stage_count = Ceiling_Divide( _Lb, _mii );
    //ASSERT( stage_count > 1 );
    cost.length = stage_count * _mii;
    tree_nodes = 0;

    // Define scheduling ranges here.
    Setup_Release_Deadline( cost.length );

    // Make sure xfer_op always stick at the last cycle.
    OPR_release_time( xfer_op ) = OPR_deadline( xfer_op ) = cost.length - 1;

    if( talk >= VOLUBLE )
      space.Print( &ready_queue );

    // Let the searching begin.

    if( schedule( first_op, true ) || schedule( first_op, false ) ){
      cost.MaxLive = mrt.Compute_MaxLive( bb );
    }
    
    tree_nodes_total += tree_nodes;

    // Print out searching results
    if( talk >= LACONIC ){
      Print();
      if( scheduled )
	mrt.Print();
    }

    if( scheduled )
      break;

    ASSERT( space.Stack_Is_Empty() );
    _mii++;
    mrt.Set_MII( _mii );
    space.Set_MII( _mii );
  }

  return scheduled;
}


/* Adjust the scheduling ranges given that op has been scheduled, and
   the underlying hw has some resource hazards.
*/
bool SCHED::Adjust_space_hazards( OPR* op )
{
#if 0
  if( !OPR_is_dmult( op ) &&
      !OPR_is_div( op ) )
    return true;
#else
  if ( OPR_ncycles( op ) < 2 )
    return true;
#endif

  for( int indx = 0; indx < bb->Oprs(); indx++ ){
    OPR* new_op = (OPR*)bb->Opr( indx );
    if( OPR_scheduled( new_op ) )
      continue;

    if( OPR_ncycles( op ) < 2 )
      continue;

    // Adjust the scheduling ranges here.

    if( is_fwd ){
      int clock = OPR_release_time( op ) + OPR_ncycles( op );
      if( clock > OPR_release_time( new_op ) ){
	OPR_release_time( new_op ) = clock;
	if( !space.TightenLb( new_op ) )
	  return false;
      }

    } else {
      int clock = OPR_deadline( op ) - OPR_ncycles( op );
      if( clock < OPR_deadline( new_op ) ){
	OPR_deadline( new_op ) = clock;
	if( !space.TightenUb( new_op ) )
	  return false;
      }
    }

  }

  return true;
}


void SCHED::List_Scheduling()
{
  mrt.Init( bb );
  _Ub = mrt.Size();

  Setup_Ready_Queue();

  Compute_Scheduling_Ranges();

  Setup_Release_Deadline( _Ub );
  space.Init( bb, _mii );

  if( talk >= VOLUBLE )
    space.Print( &ready_queue );

  while( true ){
    OPR* op = Select_Variable();
    int clock = is_fwd ? OPR_release_time(op) : OPR_deadline(op);

    ready_queue.Remove( op );
    Set_OPR_scheduled( op );
    Set_OPR_order( op, sch_order );
    sch_order++;
    mrt.Reserve_Resources( op, clock );

    if( !is_fwd )
      OPR_release_time( op ) = clock;

    if( talk >= VOLUBLE ){
      printf( "op%d: %d (%s)\n", OPR_indx(op), clock, ((MRT*)talk)->Resource_Name(op) );
    }

    // Update ready_queue and latency after op is scheduled.

    if( is_fwd ){
      for( int arc = 0; arc < OPR_num_succs(op); arc++ ){
	DEP* dep = OPR_succ( op, arc );
	OPR* succ_op = DEP_succ(dep);

	Adjust_Latency( dep );

	OPR_unsched_preds(succ_op)--;
	if( OPR_unsched_preds(succ_op) == 0 )
	  ready_queue.Push( succ_op );

	if( OPR_release_time( op ) + DEP_latency( dep ) > OPR_release_time( succ_op ) ){
	  ASSERT( false );
	  OPR_release_time( succ_op ) = OPR_release_time( op ) + DEP_latency( dep );
	  if( !space.TightenLb( succ_op ) )
	    ASSERT( false );
	}
      }

    } else {
      // for bwd
      for( int arc = 0; arc < OPR_num_preds(op); arc++ ){
	DEP* dep = OPR_pred( op, arc );
	OPR* pred_op = DEP_pred(dep);

	Adjust_Latency( dep );

	OPR_unsched_succs(pred_op)--;
	if( OPR_unsched_succs(pred_op) == 0 )
	  ready_queue.Push( pred_op );

	if( OPR_deadline( op ) - DEP_latency( dep ) < OPR_deadline( pred_op ) ){
	  ASSERT( false );
	  OPR_deadline( pred_op ) = OPR_deadline( op ) - DEP_latency( dep );
	  if( !space.TightenUb( pred_op ) )
	    ASSERT( false );
	}
      }
    }

    if( ready_queue.Size() == 0 ){
      cost.length = is_fwd ? clock + 1 : _Ub - clock;
      cost.MaxLive = mrt.Compute_MaxLive( bb );
      scheduled = true;

      if( !is_fwd && clock > 0 ){
	for( int indx = 0; indx < bb->Oprs(); indx++ ){
	  OPR* op = (OPR*)bb->Opr( indx );
	  OPR_release_time( op ) -= clock;
	  OPR_Dist2Root( op ) = OPR_release_time( op );
	}
      }

      break;
    }

    // Tighten scheduling ranges based on resource hazards. */

    if( !Adjust_space_hazards( op ) ){
      ASSERT( false );
    }

    // Tighten scheduling ranges based on the tigthened resources.

    for( int i = ready_queue.Size() - 1; i >= 0; i-- ){
      OPR* op = ready_queue[i];

      // Find_First_Schedule_Cycle checks data path constraints.
      int r = OPR_release_time( op );
      OPR_release_time( op ) = mrt.Find_First_Schedule_Cycle( op );

      if( r < OPR_release_time( op ) ){
	if( !space.TightenLb( op ) )
	  ASSERT( false );
      }

      int d = OPR_deadline( op );
      OPR_deadline( op ) = mrt.Find_Last_Schedule_Cycle( op );
      if( d > OPR_deadline( op ) ){
	if( !space.TightenUb( op ) )
	  ASSERT( false );
      }
    }
  }

  // Print out searching results
  if( talk >= LACONIC ){
    mrt.Print();
    Print();
  }
}
