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
#include "scc.h"
#include "mrt.h"
#include "modulo.h"
#include "cg_vector.h"
#include "cg_sas.h"


#define IS_DIGIT(d)   ((d)>='0' && (d)<='9')


void Setup_OPR( OPR* op, const char* src )
{
  // Setup opcode
  STOP opcode = Find_Opcode( src );
  ASSERT( opcode != STOP_unknown );
  Set_OPR_opcode( op, opcode );

  Reset_OPR_scheduled( op );

  const RESERVATION* r = MRT::Reservation_Table[OPR_opcode(op)];
  const RESOURCE_MASK mask = r->RESERVATION_mask();

  Set_OPR_res_mask( op, mask );
}


void Print_OPR( FILE* f, const OPR* op )
{ 
  fprintf( f, "op%d[%d,%d] ", OPR_indx( op ), OPR_release_time( op ), OPR_deadline( op ) );
  fprintf( f, "fu:%s uses:%d\n", ((MRT*)op)->Resource_Name( op ), OPR_uses(op) );

  for( int i = 0; i < OPR_results(op); i++ ){
    const STN* result = OPR_result( op );
    fprintf( f , "%s ", STN_string( result ) );
  }

  fprintf( f, "= " );
  fprintf( f, "%s ", Opcode_Name(OPR_opcode(op)) );

  for( int i = 0; i < OPR_opnds(op); i++ ){
    const STN* opnd = OPR_opnd( op, i );
    fprintf( f, "%s", STN_string( opnd ) );
    fprintf( f, " " );
  }

  fprintf( f, "\n" );
}


// SBB:Emit_Schedule copies the cycle and order numbers from this SBB
// to the BB cg_bb.  It is invoked after scheduling of SBB is complete.

void SBB::Emit_Schedule( BB * cg_bb ) const
{
  OP * cg_op = BB_first_op (cg_bb);
  for( int id = 0; id < Oprs(); id++ ){
    const OPR* op = Opr_Blk( id );

    ASSERT( OPR_order(op) >= 0 );
    ASSERT( OPR_scheduled(op) );

    OP_info_cycle (cg_op) = OPR_release_time (op);
    OP_info_order (cg_op) = OPR_order (op);

    cg_op = OP_next (cg_op);
  }
}


// Assign op->OPR_indx() and bb->Opr() here.
void SBB::Topological_Sort()
{
  // For cyclic case, just maintain current ordering
  if( is_cyclic() ){
    for( int i = 0; i < Oprs(); i++ ){
      OPR* op = (OPR*)Opr_Blk( i );
      Set_OPR_indx( op, i );
      Set_Opr( i, op );
    }
    return;
  }

  // First, identify xfer_op (branch instruction) and delay_op (op in
  // delay slot after branch).
  int indx = Oprs() - 1;
  OPR* xfer_op = (OPR*)Opr_Blk( indx );
  OPR* delay_op = NULL;
  if( !OPR_is_xfer( xfer_op ) && indx > 0 ){
    delay_op = xfer_op;
    indx--;
    xfer_op = (OPR*)Opr_Blk( indx );
  }
  if( OPR_is_xfer( xfer_op ) ){
    if( delay_op == NULL ){
      // If branch has no delay op, try to assign one now.
      for( indx--; indx >= 0; indx-- ){
	OPR* op = (OPR*)Opr_Blk( indx );
	if( OPR_num_succs( op ) > 0 )
	  continue;
	if( ( delay_op == NULL )                  ||
	    ( OPR_opcode( delay_op ) == STOP_nop ) ||
	    ( OPR_is_memory(delay_op) && !OPR_is_memory(op) ) ){
	  delay_op = op;
	}
      }
    }
  } else {
    xfer_op = delay_op = NULL;
  }

  // For this initial "scheduling" pass, we will just order the ops so
  // that each dependence arc has predecessor ordered before successor.
  // Care must be taken to preserve the positions of xfer_op and
  // delay_op at the end of the block.

  // NOTE: I don't understand why this topological sort is necessary in
  // the non-cyclic case.  Aren't the OPs in the original BB already in
  // topological order?  Can dependence arcs go backwards?

  // Allocate a queue to hold OPRs ready to be placed (with all
  // predecessors already scheduled.
  int last_one = -1;  // last_one is index of last element of queue
  OPR** queue = (OPR**)MEM_POOL_Alloc( &sas_pool, sizeof(queue[0]) * Oprs() );

  // Initialize number of unscheduled predecessors for each OPR.
  // If none, insert OPR into queue.
  for( int i = 0; i < Oprs(); i++ ){
    OPR* op = (OPR*)Opr_Blk( i );

    OPR_unsched_preds( op ) = OPR_num_preds( op );
    OPR_unsched_succs( op ) = OPR_num_succs( op );  // unused

    if( op != xfer_op  &&
	op != delay_op &&
	OPR_unsched_preds( op ) == 0 ){
      queue[++last_one] = op;
    }
  }

  // Iterate through ops ready for scheduling
  indx = 0;
  while( indx <= last_one ){
    OPR* op = queue[indx];

    // Schedule next op on queue by assigning an index
    Set_OPR_indx( op, indx );
    Set_Opr( indx, op );
    ASSERT( Opr( indx ) == Opr_Blk( OPR_id( op ) ) );

    // For each successor, update number of unscheduled predessors;
    // If no more, insert into queue.
    for( int arc = 0; arc < OPR_num_succs(op); arc++ ){
      DEP* dep = OPR_succ( op, arc );
      OPR* succ_op = DEP_succ(dep);
      if( succ_op == xfer_op || succ_op == delay_op )
	continue;
      OPR_unsched_preds(succ_op)--;
      if( OPR_unsched_preds(succ_op) == 0 ){
	queue[++last_one] = succ_op;
      }
    }

    indx++;
  }

  // Schedule xfer_op and delay_op
  if( xfer_op != NULL ){
    Set_OPR_indx( xfer_op, indx );
    Set_Opr( indx, xfer_op );
    indx++;

    if( delay_op != NULL ){
      Set_OPR_indx( delay_op, indx );
      Set_Opr( indx, delay_op );
      indx++;
    }
  }

  ASSERT( indx == Oprs() );

  // Finally, add pseudo arcs to maintain proper positions of xfer_op and
  // delay_op during scheduling.

  if( xfer_op != NULL ){
    // Add pseudo arcs from leaf ops to xfer_op
    for( indx = OPR_indx( xfer_op ) - 1; indx >= 0; indx-- ){
      OPR* op = (OPR*)Opr( indx );
      if( OPR_num_succs( op ) == 0 )
	Add_Pseudo_Arc( op, xfer_op );
    }
  }
  // Add pseudo arc  xfer_op --> delay_op
  if( delay_op != NULL ){
    Add_Pseudo_Arc( xfer_op, delay_op );

    // Prevent OPR from being scheduled between xfer_op and delay_op
    for( int arc = 0; arc < OPR_num_preds(delay_op); arc++ ){
      DEP* dep = OPR_pred( delay_op, arc );
      OPR* pred_op = DEP_pred(dep);
      if( pred_op != xfer_op )
	Add_Pseudo_Arc( pred_op, xfer_op );
    }
  }
}


// SSB::Refine_Latency invokes Adjust_Latency on each true dependence
// arc in the SBB.

void SBB::Refine_Latency()
{
  for( int indx = 0; indx < Oprs(); indx++ ){
    OPR* op = (OPR*)Opr( indx );

    for( int arc = 0; arc < OPR_num_succs(op); arc++ ){
      DEP* dep = OPR_succ( op, arc );
      if( DEP_Is_True( dep ) ){
	Adjust_Latency( dep );
      }
    }
  }
}


// SSB::Remove_Arcs deletes all dependence arcs of the given type.

void SBB::Remove_Arcs( DEP_TYPE type )
{
  for( int indx = 0; indx < Oprs(); indx++ ){
    OPR* op = (OPR*)Opr( indx );
    for( int arc = 0; arc < OPR_num_succs(op); ){
      DEP* dep = OPR_succ( op, arc );

      if( DEP_type( dep ) == type ){
	OPR_num_succs(op)--;
	OPR_succ( op, arc ) = OPR_succ( op, OPR_num_succs(op) );

      } else
	arc++;
    }

    for( int arc = 0; arc < OPR_num_preds(op); ){
      DEP* dep = OPR_pred( op, arc );

      if( DEP_type( dep ) == type ){
	OPR_num_preds(op)--;
	OPR_pred( op, arc ) = OPR_pred( op, OPR_num_preds(op) );

      } else
	arc++;
    }
  }
}


/* Exist arcs A->C and A->B->C, then arc A->C is redundant if
   A: w23 = ...
   B: w23 = w23 ...
   C:     = w23 ...
   This algorithm can be simplifed greatly if the arcs are inserted in
   topological order.
*/
void SBB::Prune_Redundant_Arcs()
{
  // First, prune off useless arcs.
  for( int indx = Oprs() - 1; indx >= 0; indx-- ){
    const OPR* op = Opr( indx );
    int i = 0;
    int opnds = OPR_opnds( op );

    //if( OPR_cond_def( op ) )      i = -1;

    for( ; i < opnds; i++ ){
      const STN* opnd = ( i < 0 ? OPR_result( op ) : OPR_opnd( op, i ) );
      DEP* last_dep = NULL;
      OPR* last_op  = NULL;

      if( !STN_is_register(opnd) )
	continue;

      /* If a STN is defined more than once, then keep the last one, and
	 prune off the others. */

      for( int arc = 0; arc < OPR_num_preds(op); arc++ ){
	DEP* dep = OPR_pred( op, arc );
	OPR* pred_op = DEP_pred(dep);

	/* They don't have to be
	   REGs_Are_Equivalent( opnd, OPR_result( pred_op ) ),
	   since d40 relies on both w80 and w81 */

	if( !DEP_Is_True( dep ) || DEP_opnd( dep ) != i )
	  continue;

	if( last_dep == NULL ){
	  last_dep = dep;
	  last_op  = pred_op;

	} else {

	  // idx is assigned in topological order ...
	  if( OPR_indx( last_op ) > OPR_indx( pred_op ) ){
	    DEP_type( dep ) = DEP_IGNORABLE;

	    if( talk >= VOLUBLE ){
	      printf( "Arc: op%d -> op%d is ignorable\n",
		      OPR_indx(pred_op), OPR_indx(op) );
	    }

	  } else {
	    DEP_type( last_dep ) = DEP_IGNORABLE;
	    last_dep = dep;
	    last_op  = pred_op;

	    if( talk >= VOLUBLE ){
	      printf( "Arc: op%d -> op%d is ignorable\n",
		      OPR_indx(pred_op), OPR_indx(op) );
	    }
	  }
	}

	last_dep = dep;
      }  // for each arc
    }  // for each opnd
  }  // for each op

#if 0
  // remove all the redundant arcs from op to a fixed op
  // NOTE: As written, I think this code deletes all REGIN arcs,
  // which is clearly wrong.  Delete?
  for( int indx = 0; indx < Oprs(); indx++ ){
    const OPR* fixed_op = Opr( indx );

    for( int arc = 0; arc < OPR_num_preds(fixed_op); arc++ ){
      DEP* dep = OPR_pred( fixed_op, arc );
      OPR* pred_op = DEP_pred( dep );

      if( DEP_Is_True( dep ) ||
	  DEP_type( dep ) == DEP_IGNORABLE )
	continue;

      for( int ign = 0; ign < OPR_num_succs( pred_op ); ign++ ){
	DEP* ign_dep = OPR_succ( pred_op, ign );
	if( DEP_Is_True( ign_dep ) ){
	  DEP_type( dep ) = DEP_IGNORABLE;

	  if( talk >= VOLUBLE ){
	    printf( "Arc: op%d -> op%d is ignorable\n", OPR_indx(pred_op), OPR_indx(op) );
	  }

	  break;
	}
      }
    }
  }
#endif

  // remove all the DEP_IGNORABLE arcs
  Remove_Arcs();

  // After pruning, we can setup the definer and uses.
  // NOTE: Neither STN_definer nor OPR_uses is currently used anywhere.
  for( int indx = 0; indx < Oprs(); indx++ ){
    OPR* op = (OPR*)Opr( indx );

    for( int arc = 0; arc < OPR_num_succs( op ); arc++ ){
      DEP* dep = OPR_succ( op, arc );
      if( !DEP_Is_True( dep ) )
	continue;

      const OPR* succ_op = DEP_succ( dep );
      STN* opnd = (STN*)OPR_opnd( succ_op, DEP_opnd(dep) );
      Set_STN_definer( opnd, op );

      Inc_OPR_uses( op );
    }
  }
}


void SBB::Add_Pseudo_Arc( OPR* from_op, OPR* to_op )
{
  Add_Arc( from_op, to_op, DEP_MISC, MAX_OPNDS, 0, 0, 0 );
}


void SBB::Add_Arc( OPR* from_op, OPR* to_op,
		  DEP_TYPE type, int opnd, int latency, int omega, int mii )
{
  const uint32_t total_arcs = Oprs() * Oprs();
  DEP* new_dep = (DEP*)Dep( last_arc );

  // After topological sort, OPR_indx >= 0.
  if( OPR_indx( from_op ) >= 0 &&
      ( omega > 0 || OPR_indx( from_op ) > OPR_indx( to_op ) ) ){
    ASSERT( is_cyclic() );
  }

  if( type == DEP_REGANTI && omega > 0 ){
    ASSERT( OPR_is_mult( to_op ) );
  }

#if 0
  if( type == DEP_REGIN && OPR_is_load( to_op ) ){
    Set_OPR_is_addr_incr( from_op );
  }
#endif

  DEP_type( new_dep ) = type;
  DEP_opnd( new_dep ) = opnd;
  DEP_latency( new_dep ) = latency;
  DEP_omega( new_dep ) = omega;

  DEP_pred( new_dep ) = from_op;
  DEP_succ( new_dep ) = to_op;

  for( int arc = 0; arc < OPR_num_succs( from_op ); arc++ ){
    DEP* dep = OPR_succ( from_op, arc );
    OPR* succ_op = DEP_succ(dep);

    if( succ_op != to_op )
      continue;

    if( DEP_omega( dep ) == DEP_omega( new_dep ) ){
      // Check which dep dominate which one.
      if( DEP_latency( dep ) < DEP_latency( new_dep ) ){
	*dep = *new_dep;
      }

      return;

    } else if( DEP_latency( dep ) == DEP_latency( new_dep ) ){
      // Check which dep dominate which one.
      if( DEP_omega( dep ) > DEP_omega( new_dep ) ){
	*dep = *new_dep;
      }

      return;

    } else if( type == DEP_SCC ){
      ASSERT( mii > 0 );
      if( DEP_omega( new_dep) <= DEP_omega( dep ) &&
	  ( DEP_latency( new_dep ) - DEP_latency( dep ) >
	    ( DEP_omega( new_dep ) - DEP_omega( dep ) ) * mii ) ){
	*dep = *new_dep;
      }
      return;
    } else Fail_FmtAssertion ("Handle this duplicate dependency");
  }

  OPR_succ( from_op, OPR_num_succs( from_op ) ) = new_dep;
  OPR_num_succs( from_op )++;
  OPR_pred( to_op, OPR_num_preds( to_op ) ) = new_dep;
  OPR_num_preds( to_op )++;


  if( talk >= VOLUBLE ){
    static const char* DEP_name[] = {
      "ignore",
      "regin", "regout", "reganti",
      "memin", "memout",
      "misc", "scc" };
    int from = OPR_indx(from_op) >= 0 ? OPR_indx(from_op) : OPR_id(from_op);
    int to   = OPR_indx(to_op) >= 0 ? OPR_indx(to_op) : OPR_id(to_op);

    printf( "Arc%d: op%d (%s)-> op%d (%d l:%d o:%d)\n",
	    last_arc, from, DEP_name[DEP_type(new_dep)], to,
	    DEP_opnd(new_dep), DEP_latency(new_dep), DEP_omega(new_dep) );
  }

  last_arc++;
  ASSERT( last_arc <= total_arcs );
  ASSERT( OPR_num_succs( from_op ) <= Oprs() );
  ASSERT( OPR_num_preds( to_op ) <= Oprs() );

  /* Given code segment like
     op1: b = a[1] ...
     op2: a = ...
     op3:   = b ...
     A DEP_REGANTI-type arc should be added from op1 to op2, since a[1] and
     a will be the same register w/o the help of rotating register file. */

  if( type == DEP_REGIN &&
      omega > 0         &&
      OPR_id( from_op ) > OPR_id( to_op ) ){
    Add_Arc( to_op, from_op, DEP_REGANTI, opnd, 0, 0, 0 );
  }
}


void SBB::Draw_Node( FILE* f, const OPR* op, char* color, char* shape ) const
{
  fprintf( f, "\t node: { " );
  fprintf( f, "title:\"%i\" ", OPR_indx(op) );
  fprintf( f, "label:\"op%d:%s\" ", OPR_indx(op), Opcode_Name(OPR_opcode(op)) );

  if( color != NULL )
    fprintf( f, "color: %s ", color );

  if( shape == NULL )
    fprintf( f, " }\n" );
  else
    fprintf( f, "shape:%s }\n", shape );
}


void SBB::Draw_Arc( FILE* f, DEP* dep, char* pattern, char* color ) const
{
  OPR* from = DEP_pred( dep );
  OPR* to   = DEP_succ( dep );

  fprintf( f, "\t edge: { " );
  fprintf( f, "sourcename:\"%i\" ", OPR_indx( from ) );
  fprintf( f, "targetname:\"%i\" ", OPR_indx( to ) );
  fprintf( f, "label: \"%i,%i\" ", DEP_latency( dep ), DEP_omega( dep ) );
  fprintf( f, "color: %s ", color );
  fprintf( f, "linestyle: %s}\n", pattern );
}
  

void SBB::Draw_DDG() const
{
  //const char* vcgfnm = tempnam( "/tmp/", "ddg" );
  const char* vcgfnm = "/tmp/ddg";
  const char* vcg = "/home/jliu/bin/xvcg";
  FILE* vcgfp = NULL;
  pid_t childpid;

  /* Write the ddg to a file first. */

  const char* ddg_vcg_header[] = {
    "\t title: \"Data Dependence Graph\"\n",
    "\t height: 500\n",
    "\t width:  400\n",
    "\t yspace: 20\n",
    "\t ybase: 70\n",
    "\t color:  white\n",
    "\t node.textcolor: blue\n",
    "\t node.shape: ellipse\n",
    "\t edge.color:  red\n",
    "\t edge.thickness: 2\n",
    "\t near_edges: no\n",
    "\t splines: yes\n", 
    "\t orientation: top_to_bottom\n",
    "\t display_edge_labels: no\n\n",
    "\t late_edge_labels: yes\n\n",
    "\t layoutalgorithm: minbackward\n\n"
  }; 

  ASSERT( Oprs() > 0 );

  /**
   * output vcg file
   */ 
  if( ( vcgfp = fopen( vcgfnm, "w+" ) ) == NULL )
    ASSERT( false );

  fprintf( vcgfp, "graph: {\n" );

  for( unsigned indx = 0; indx < sizeof( ddg_vcg_header ) / sizeof( char* ); indx++ ){
    fprintf( vcgfp, "%s", ddg_vcg_header[indx] );
  }
  
  for( int indx = 0; indx < Oprs(); indx++ ){
    const OPR* op = Opr( indx );

    /* Use different shape based on different type of node. */
    //Draw_Node( vcgfp, op, "yellow", "box" );
    Draw_Node( vcgfp, op, NULL, "box" );
  }
 
  /* Only consider the left down triangle part of the ddg matrix.   */

  for( int indx = 0; indx < Oprs(); indx++ ){
    const OPR* op = Opr( indx );

    for( int arc = 0; arc < OPR_num_succs( op ); arc++ ){
      DEP* dep = OPR_succ( op, arc );

      if( DEP_omega( dep ) > 0 )
	Draw_Arc( vcgfp, dep, "dotted", "red" );

      else if( DEP_Is_True( dep ) )
	Draw_Arc( vcgfp, dep, "solid", "blue" );

      else
	Draw_Arc( vcgfp, dep, "solid", "black" );

    }
  }
  
  fprintf( vcgfp, "}\n" );  
  fclose( vcgfp );

  /* Call the xvcg to draw the ddg. */

  if( (childpid = fork() ) == 0){
    close(0);    close(1);
    //close(2);
    execlp( vcg, vcg, vcgfnm, (char *)0 );
  }  else
    ASSERT( childpid > 0 );

  return;
}


void SBB::Print( FILE* f ) const
{
  fprintf( f, "\n===================== D D G ====================\n" );

  for( int indx = 0; indx < Oprs(); indx++ ){
    const OPR* op = Opr( indx );

    Print_OPR( f, op );

    if( OPR_num_succs( op ) > 0 )
      fprintf( f, "\tsuccs: " );

    for( int arc = 0; arc < OPR_num_succs( op ); arc++ ){
      DEP* dep = OPR_succ( op, arc );
      OPR* succ_op = DEP_succ(dep);
      fprintf( f, "op%d(%d,%d) ", OPR_indx(succ_op), DEP_latency(dep), DEP_omega(dep) );
    }

    fprintf( f, "\n\n" );
  }

  fprintf( f, "================================================\n" );
}


#if 0
static void Dump_TN( char* buf, int size, TN* tn )
{
  char *result = buf;

  ASSERT( tn != NULL );

  if( TN_is_constant(tn) ){
    if( TN_has_value(tn) ){
      buf += sprintf ( buf, "(0x%" SCNx64 ")", TN_value(tn) );
      if (TN_size(tn) == 4 && 
	  TN_value(tn) >> 32 != 0 &&
	  TN_value(tn) >> 31 != -1)
	buf += sprintf ( buf, "!!! TN_value=0x%" SCNx64 " is too big to fit in a word",
			 TN_value(tn));
    }
    else if (TN_is_enum(tn)) {
      buf += sprintf ( buf, "(enum:%s)", ISA_ECV_Name(TN_enum(tn)) );
    }
    else if ( TN_is_label(tn) ) {
      LABEL_IDX lab = TN_label(tn);
      const char *name = LABEL_name(lab);
      INT64 offset = TN_offset(tn);
      if ( offset == 0 ) {
	buf += sprintf ( buf, "(lab:%s)", name );
      }
      else {
	buf += sprintf ( buf, "(lab:%s+%" SCNd64 ")", name, offset );
      }
    } 
    else if ( TN_is_tag(tn) ) {
      LABEL_IDX lab = TN_label(tn);
      const char *name = LABEL_name(lab);
      buf += sprintf ( buf, "(tag:%s)", name );
    }
    else if ( TN_is_symbol(tn) ) {
      ST *var = TN_var(tn);
      buf += sprintf ( buf, "(sym" );
      if (ST_class(var) == CLASS_CONST)
      	buf += sprintf ( buf, ":%s)", Targ_Print(NULL, ST_tcon_val(var)));
      else
      	buf += sprintf ( buf, ":%s%+lld)", ST_name(var), TN_offset(tn) );
    } 
    else {
      ASSERT( false );
    }
  }
#ifdef TARG_OSP192
  else if (TN_is_multidirect(tn)) {  
    ST *var = TN_var(tn);
    if (TN_offset(tn))
      buf += sprintf ( buf, "(mdsym:%s%+lld)", ST_name(var), TN_offset(tn) );
    else buf += sprintf ( buf, "(mdsym:%s)", ST_name(var));
  }
#endif
  else {  /* register TN */
    if (TN_is_global_reg(tn)) {
      buf += sprintf ( buf, "GTN%d", TN_number(tn) );
    }
    else {
      buf += sprintf ( buf, "TN%d", TN_number(tn) );
    }
#ifdef TARG_OSP192
    if (TN_register_class(tn) == ISA_REGISTER_CLASS_pointer)
      buf += sprintf ( buf, "p");
    else if (TN_size(tn) == 8)
      buf += sprintf ( buf, "d");
#endif
    if (TN_register(tn) != REGISTER_UNDEFINED) {
      if (TN_register(tn) <= REGISTER_CLASS_last_register(TN_register_class(tn))) {
#ifdef TARG_OSP192
	char *letterp = buf+1;;
#endif
	buf += sprintf (buf, "(%s)", 
		REGISTER_name(TN_register_class(tn), TN_register(tn)));
#ifdef TARG_OSP192
	if (TN_size(tn) == 8)
	  *letterp = 'd';
#endif
      } else {
	buf += sprintf (buf, "(%d,%d)", TN_register_class(tn), TN_register(tn));
      }
    }
    if (TN_is_save_reg(tn)) {
	buf += sprintf (buf, "(sv:%s)", 
		REGISTER_name(TN_save_rclass(tn), TN_save_reg(tn)));
    }
#ifdef TARG_OSP192
    if (TN_is_transient(tn))
	buf += sprintf (buf, "(trans)"); 
#endif
  }
  
  ASSERT( buf - result < size );
}

static char func_name[256];

/* An embedded function for Pro64 to dump the source basic block. */
void Dump_Src_BB( SBB* bb, FILE* t )
{
  OP* op;
  char buf[1024];
  int size = sizeof(buf);

  fprintf( t, "\n{\n" );

  fprintf( t, "\tbb:  %s:%s:%d\n", "file.cxx", func_name, id(bb) );
  fprintf( t, "\tops: %d\n", length(bb) );

  FOR_ALL_OPs( bb, op ){
    ASSERT( id(OP_bb(op)) == id(bb) );

    // Print OP No SrcLine

    bool cg_loop_op = Is_CG_LOOP_Op(op);

    fprintf( t, "\top%d: ", OP_map_idx( op ) );

    for( int i = 0; i < OP_results(op); i++ ){
      Dump_TN( buf, size, OP_result(op,i) );
      fprintf( t , "%s ", buf );
    }

    fprintf(t, "= ");
    fprintf(t, "%s ", Opcode_Name(OP_code(op)));
    for( int i=0; i<OP_opnds(op); i++ ){
      TN *tn = OP_opnd(op,i);
      Dump_TN( buf, size, tn );
      fprintf( t, "%s ", buf );
      if ( cg_loop_op ) {
	INT omega = TN_is_symbol(tn) ? OP_restore_omega(op) : OP_omega(op,i);
	if (omega) fprintf(t, "[%d]", omega);
      }
      //if (OP_Defs_TN(op, tn)) fprintf(t, "<defopnd>");
      fprintf(t, " ");
    }

#if 0
    /* print flags */
    if (OP_glue(op)) fprintf (t, " glue");
    if (OP_no_alias(op)) fprintf (t, " noalias");
    if (OP_copy(op)) fprintf (t, " copy");
    if (OP_volatile(op)) fprintf (t, " volatile");
    if (OP_side_effects(op)) fprintf (t, " side_effects");
    if (OP_hoisted(op)) fprintf (t, " hoisted");
    if (OP_cond_def(op)) fprintf (t, " cond_def");
    if (OP_end_group(op)) fprintf (t, " end_group");
    if (OP_tail_call(op)) fprintf (t, " tail_call");
    if (OP_no_move_before_gra(op)) fprintf (t, " no_move");
    if (OP_spadjust_plus(op)) fprintf (t, " spadjust_plus");
    if (OP_spadjust_minus(op)) fprintf (t, " spadjust_minus");

    if (OP_unrolling(op)) {
      UINT16 unr = OP_unrolling(op);
      fprintf(t, " %d%s unrolling", unr,
	      unr == 1 ? "st" : unr == 2 ? "nd" : unr == 3 ? "rd" : "th");
    }
#endif
    fprintf( t, " {" );

    // Print succs

    for( ARC_LIST* arcs = OP_succs(op); arcs != NULL; arcs = ARC_LIST_rest(arcs) ){
      ARC* arc = ARC_LIST_first(arcs);
      OP* succ_op = ARC_succ(arc);
      UINT16 succ_id = OP_map_idx( succ_op );
      CG_DEP_KIND kind = ARC_kind(arc);

      fprintf( t, "\n\t\top%d: ", succ_id );

      if( kind == CG_DEP_REGIN )
	fprintf( t, "raw opnd(%d) ", ARC_opnd(arc) );
      else if( kind == CG_DEP_REGANTI )
	fprintf( t, "war opnd(%d) ", ARC_opnd(arc) );
      else if ( kind == CG_DEP_REGOUT )
	fprintf( t, "waw " );
      else
	fprintf( t, "misc " );

      fprintf( t, "latency(%d) omega(%d) ", ARC_latency(arc), ARC_omega(arc) );
    }

    if( OP_succs(op) != NULL )
      fprintf( t, "\n\t" );

    fprintf( t, "}\n" );
  }

  fprintf( t, "}\n" );  
}
#endif


// Dump the results
void SCHED::Emit_Schedule( BB * cg_bb ) const
{
  bb->Emit_Schedule( cg_bb );
}


static int sort_by_res_mask( const void* a, const void* b )
{
  OPR* op1 = *(OPR**)a;
  OPR* op2 = *(OPR**)b;

  // Pick the one with less hw resource.
  const RESOURCE_MASK a_mask = OPR_res_mask( op1 );
  const RESOURCE_MASK b_mask = OPR_res_mask( op2 );

  if( ( a_mask & ~b_mask ) == 0 )
    return -1;

  if( ( ~a_mask & b_mask ) == 0 )
    return 1;

  return ( OPR_indx( op1 ) < OPR_indx( op2 ) ) ? -1 : 1;
}


void SCHED::Compute_ResMII()
{
  MRT mrt( MUTE );
  int max_resource_cycles = 0;

  mrt.Init( bb, bb->Oprs() );
  _res_mii = 0;

  ready_queue.Reset();

  for( int indx = 0; indx < bb->Oprs(); indx++ ){
    OPR* op = (OPR*)bb->Opr( indx );
    OPR_release_time( op ) = 0;
    OPR_deadline( op ) = bb->Oprs() - 1;
    max_resource_cycles = MAX( max_resource_cycles, OPR_ncycles(op) );

    ready_queue.Push( op );
  }  

  relaxed.Sort( sort_by_res_mask );

  for( int indx = 0; indx < ready_queue.Size(); indx++ ){
    OPR* op = ready_queue[indx];
    int cycle = OPR_release_time( op );

    while( !mrt.Resources_Available( op, cycle, false ) ){
      ASSERT( cycle <= OPR_deadline(op) );
      cycle++;
    }

    mrt.Reserve_Resources( op, cycle );
    OPR_release_time( op ) = OPR_deadline( op ) = cycle;
    _res_mii = MAX( _res_mii, cycle );

    //Print_OPR( stdout, op );
  }

  _res_mii++;
  _res_mii = MAX( _res_mii, max_resource_cycles );
  _mii = MAX( _mii, _res_mii );

  //if( talk >= VOLUBLE )    mrt.Print();

  if( talk >= TACITURN )
    printf( "ResMII: %d\n", _res_mii );
}


void SCHED::Compute_RecMII()
{
  Compute_SCCs scc( bb, _mii, MUTE );
  
  _rec_mii = scc.Compute_RecMII_through_SCCs();

  _mii = MAX( _mii, _rec_mii );

  if( talk >= TACITURN )
    printf( "RecMII: %d\n", _rec_mii );
}


void SCHED::Compute_MinDist()
{
  int** min_dist = (int**) MEM_POOL_Alloc( &sas_pool, sizeof(int*) * bb->Oprs() );

  for( int i = 0; i < bb->Oprs(); i++ ){
    min_dist[i] = (int*) MEM_POOL_Alloc ( &sas_pool, sizeof(int) * bb->Oprs() );
  }

TRY_AGAIN:

  for( int i = 0; i < bb->Oprs(); i++ ){
    for( int j = 0; j < bb->Oprs(); j++)
      min_dist[i][j] = neg_inf;
  }

  for( int i = 0; i < bb->Oprs(); i++ ){
    const OPR* op = bb->Opr( i );

    for( int arc = 0; arc < OPR_num_succs( op ); arc++ ){
      DEP* dep = OPR_succ( op, arc );
      OPR* succ_op = DEP_succ(dep);
      int  dist = DEP_latency(dep) - DEP_omega(dep) * _mii;

      min_dist[i][OPR_indx(succ_op)] = MAX( min_dist[i][OPR_indx(succ_op)], dist );
    }
  }

  /* Run Floyd's all-pairs-shortest-path algorithm. */

  for( int k = 0; k < bb->Oprs(); k++ ){
    for( int i = 0; i < bb->Oprs(); i++ ){
      for( int j = 0; j < bb->Oprs(); j++){
	if( min_dist[i][k] == neg_inf ||
	    min_dist[k][j] == neg_inf )
	  continue;

	int dist = min_dist[i][k] + min_dist[k][j];
	min_dist[i][j] = MAX( min_dist[i][j], dist );
      }
    }
  }

  for( int i = 0; i < bb->Oprs(); i++ ){
    /* Since t_j + II*omega >= t_i + latency(i,j), or
       t_j - t_i >= latency(i,j) - II*omega .
       Since latency(i,i) <= II*omega,
       t_i - t_i >= latency(i,i) - II*omega <= 0
       If min_dist[i][i] > 0, a contradiction drawn with a too small II.
    */
    if( min_dist[i][i] > 0 ){
      _mii++;
      goto TRY_AGAIN;
    }
  }

  return;

  if( talk >= LACONIC ){
    FILE* fp = stdout;
    const int n_col = 16;

    fprintf( fp, "\n===================== D I S T ====================\n" );

    fprintf( fp, "     " );

    for( int j = 0; j < bb->Oprs(); j++){
      if( j != 0 && j % n_col == 0 )
	fprintf( fp, "\n     " );
      fprintf( fp, "%4d", j );
    }

    fprintf( fp, "\n" );

    for( int i = 0; i < bb->Oprs(); i++){
      fprintf( fp, "%3d: ", i );
      for( int j = 0; j < bb->Oprs(); j++){
	if( j != 0 && j % n_col == 0 )
	  fprintf( fp, "\n     " );
	if( min_dist[i][j] == neg_inf )
	  fprintf( fp, "    " );
	else
	  fprintf( fp, "%4d", min_dist[i][j] );
      }
      fprintf( fp, "\n" );
    }

    fprintf( fp, "==================================================\n" );
  }
}


void SCHED::Perform_SWP()
{
  Compute_ResMII();
  Compute_RecMII();

  ASSERT( _mii > 0 );

  Compute_MinDist();

  if( !Modulo_Scheduling() )
    ASSERT( false );
}


void SCHED::Perform_LS()
{
  COST fwd_cost;
  is_fwd = true;
  List_Scheduling();
  fwd_cost = cost;

  return;

  if( cost.length == _Lb )
    return;

  int16_t* fwd_sched = (int16_t *)
    MEM_POOL_Alloc( &sas_pool, bb->Oprs() * sizeof(fwd_sched[0]) );
  for( int indx = 0; indx < bb->Oprs(); indx++ ){
    OPR* op = (OPR*)bb->Opr( indx );
    fwd_sched[indx] = OPR_release_time( op );
  }  

  COST bwd_cost;
  is_fwd = false;
  List_Scheduling();

  bwd_cost = cost;

  // If the fwd schedule is better, then restore the schedule.
  if( fwd_cost.length <= bwd_cost.length ){
    for( int indx = 0; indx < bb->Oprs(); indx++ ){
      OPR* op = (OPR*)bb->Opr( indx );
      OPR_release_time( op ) = OPR_deadline( op ) = fwd_sched[indx];
    }
  }

  //ASSERT( fwd_cost.MaxLive == bwd_cost.MaxLive );
}


SCHED::SCHED( SBB* bb, TALK talk ) : mrt( talk )
{
  // Initialize data
  this->bb = bb;
  this->talk = talk;
  _mii = 0;

  bb->Topological_Sort();

  scheduled = false;
  is_fwd = true;
  ready_queue.Init( bb->Oprs() );

  // The latency got from pro64 might not be accurate.
  bb->Refine_Latency();

  // DDG pruning
  bb->Prune_Redundant_Arcs();

  sch_order = 0;

  //bb->Draw_DDG();

  // Perform scheduling
  if( bb->is_cyclic() ){
    Perform_SWP();

  } else {
    Perform_LS();
  }

  // Make sure we have scheduled all the ops.
  ASSERT( sch_order == bb->Oprs() );

  if( talk >= LACONIC )
    bb->Print();
}

