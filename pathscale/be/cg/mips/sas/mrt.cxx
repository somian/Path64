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
#include "mrt.h"
#include "swp.h"
#include "sas_opcode.h"


#define MASK_EX   0x01
#define MASK_MD   0x02
#define MASK_FP   0x04
#define MASK_REG  0x40
#define MASK_FREG 0x80

#define FP_ID  2

//static RESOURCE resource( fu_mask, fu_name, fu_id, fu_avail_per_cycle );
static RESOURCE ex( MASK_EX, "EX", 0, 1  );
static RESOURCE md( MASK_MD, "MD", 1, 1  );
static RESOURCE fp( MASK_FP, "FP", FP_ID, 1  );
static RESOURCE reg( MASK_REG, "reg", 3, 32 );
static RESOURCE freg( MASK_FREG, "freg", 4, 32 );

/* Make sure that res_per_cycle[i].id == i. */
const RESOURCE* MRT::res_per_cycle[] =
  { &ex, &md, &fp, &reg, &freg };

const uint8_t MRT::Types_Per_Cycle = sizeof(res_per_cycle) / sizeof(res_per_cycle[0]);

#define RESOURCE_IS_EX(r)    ( (r)->RESOURCE_mask() == MASK_EX )
#define RESOURCE_IS_MD(r)    ( (r)->RESOURCE_mask() == MASK_MD )
#define RESOURCE_IS_FP(r)    ( (r)->RESOURCE_mask() == MASK_FP )
#define RESOURCE_IS_EXFP(r)  ( RESOURCE_IS_EX(r) || RESOURCE_IS_FP(r) )

#define OPR_is_alu(o)  \
  (((MRT::Reservation_Table[OPR_opcode(o)])->RESERVATION_mask()) & MASK_EX )
#define OPR_is_fpu(o)  \
  (((MRT::Reservation_Table[OPR_opcode(o)])->RESERVATION_mask()) & MASK_FP )

static const RESOURCE* res_ex[] = { &ex, NULL };
static const RESOURCE* res_md[] = { &md, NULL };
static const RESOURCE* res_fp[] = { &fp, NULL };
static const RESOURCE* res_null[] = { NULL };

/* general_latency is defined as:
   EX unit is 1-cycle latency
   MD unit is 4-cycle latentcy
   FP unit is 4-cycle latency */

/* CPU Instructions. */
//static RESERVATION op( op_name, general_latency, resources );
static RESERVATION Load( "load",    1, res_ex );
static RESERVATION IdxLS( "idx.ls", 0, res_ex );
static RESERVATION Store( "store",  1, res_ex );
static RESERVATION Logic( "logic",  0, res_ex );
static RESERVATION Add( "add",      0, res_ex );
static RESERVATION Slt( "slt",      1, res_ex );
static RESERVATION Mpy( "mpy",      1, res_ex );
static RESERVATION Move( "move",    1, res_ex );
static RESERVATION Mvhl( "mvhl",    1, res_ex );
static RESERVATION Shift( "shift",  1, res_ex );
static RESERVATION Branch( "branch",1, res_ex );
static RESERVATION Trap( "trap",    1, res_ex );

/* FPU(CP1) Instructions. */
static RESERVATION fLoad( "f.load",     1, res_ex );
static RESERVATION fStore( "f.store",   1, res_ex );
static RESERVATION fAdd( "f.add",       4, res_fp );
static RESERVATION Ccond( "c.cond",     4, res_fp );
static RESERVATION fMove( "f.move",     4, res_fp );
static RESERVATION cvt( "cvt",          4, res_fp );
static RESERVATION fBranch( "f.branch", 1, res_fp );

/* MIPS-3D ASE (Application Specific Extension) Instructions. */
static RESERVATION Ase( "ase", 4, res_fp );

/* Unknown Instructions. */
static RESERVATION Error( "Error", 0, res_null );

/* Reservation table for each op. Indexed by opcode. */
const RESERVATION* MRT::Reservation_Table[] = {
#define op(a,b,c,d,e,f,g) &(d),
#include "sas_op.h"
};


static inline int Cycle_Mod_II( int cyc, int ii )
{
  if( cyc < 0 ){
    do { cyc += ii; } while( cyc < 0 );
  } else {
    while( cyc >= ii ) cyc -= ii;
  }
  return cyc;
}


void MRT::Setup_Resource_Table()
{
  // Allocate space.
  if( Resource_Table == NULL ){
    Resource_Table = (Resource_Table_Entry *)
      MEM_POOL_Alloc( &sas_pool, sizeof(Resource_Table_Entry) * rtable_size );

    for( int i = 0; i < rtable_size; i++ ){
      int size = sizeof(Resource_Table[i].nRes[0]) * Types_Per_Cycle;
      Resource_Table[i].nRes = (uint8_t *) MEM_POOL_Alloc( &sas_pool, size );
    }
  }

  // Initialize hw resources for each cycle.
  for( int i = 0; i < rtable_size; i++ ){
    for( int j = 0; j < Types_Per_Cycle; j++ ){
      RESOURCE_ID id = res_per_cycle[j]->RESOURCE_id();
      ASSERT( id == j );
      Resource_Table[i].nRes[id] = res_per_cycle[j]->RESOURCE_avail_per_cycle();
      Resource_Table[i].nExFp = 2;  // Limited dual issue
    }
  }
}


void MRT::Init( SBB* bb, int mii )
{
  int max_resource_cycles = 0;

  if( rtable_size > 0 ){
    Setup_Resource_Table();
    return;
  }

  rtable_size = 0;
  MaxLive = 0;

  // start scheduling OPs at this <rtable_size>.

  for( int indx = 0; indx < bb->Oprs(); indx++ ){
    const OPR* op = bb->Opr( indx );
    int cur_resource_cycles = OPR_ncycles( op );
    max_resource_cycles = MAX( cur_resource_cycles, max_resource_cycles );
    int op_latency = cur_resource_cycles;

    for( int arc = 0; arc < OPR_num_succs(op); arc++ ){
      DEP* dep = OPR_succ( op, arc );
      op_latency = MAX( op_latency, DEP_latency(dep) );
    }

    rtable_size += op_latency;
  }

  /* increase table size by the maximum number of resource cycles needed by
     any OP. */

  // the latency varies depending on at which an op will be scheduled.
  rtable_size += max_resource_cycles + 5;

  Setup_Resource_Table();

  Set_MII( bb->is_cyclic() ? mii : rtable_size );
}


/* Check whether op can be scheduled at clock without any resources
   hazards corresponding to the predecessors of op.
*/
void MRT::Adjust_OPR_res_mask( OPR* op, int clock ) const
{
  const RESERVATION* r = MRT::Reservation_Table[OPR_opcode(op)];
  RESOURCE_MASK mask = r->RESERVATION_mask();

  ASSERT( clock >= 0 );

#if 0
  for( int arc = 0; arc < OPR_num_preds( op ); arc++ ){
    DEP* dep = OPR_pred( op, arc );
    if( !DEP_Is_True( dep ) )
      continue;

    OPR* pred_op = DEP_pred(dep);
    if( !OPR_scheduled( pred_op ) )
      continue;

    for( int i = 0; i < r->RESERVATION_size(); i++ ){
      const RESOURCE* res = r->RESERVATION_resource(i);

      if( !Consume_Resource( pred_op, op, clock, res ) ){
	mask &= ~(res->RESOURCE_mask());
      }
    }
  }
#endif

#if 0
  if( OPR_res_mask( op ) != mask ){
    printf( "Adjust res_mask for %d from %x to %x\n",
	    OPR_indx( op ), OPR_res_mask( op ), mask );
  }
#endif

  Set_OPR_res_mask( op, mask );
}


/* Configuration for mips5kf
   (1) limited dual issue;
   (2) two execution pipes:
       (a) one integer pipe, including one mult/div unit
       (b) one floating point pipe;
       (c) both fully pipelined with one(int) or four cycles(fp) of latency.
*/
bool MRT::Resources_Available( OPR* op, int clock, bool adjust_res )
{
  const RESERVATION* r = MRT::Reservation_Table[OPR_opcode(op)];

#if 0
  if( adjust_res ){
    Adjust_OPR_res_mask( op, clock );

    if( OPR_res_mask( op ) == 0 )
      return false;
  }
#endif

  clock = Cycle_Mod_II( clock, mii );

  // Limited dual issue
  if( Resource_Table[clock].nExFp < ( OPR_is_ndual( op ) ? 2 : 1 ) )
    return false;

  for( int i = 0; i < r->RESERVATION_size(); i++ ){
    const RESOURCE* res = r->RESERVATION_resource(i);
    const RESOURCE_ID res_id = res->RESOURCE_id();
    if( ( OPR_res_mask( op ) & res->RESOURCE_mask() ) == 0 )
      continue;
    if( Resource_Table[clock].nRes[res_id] > 0 )
      return true;
  }

  return false;
}


const char* MRT::Resource_Name( const OPR* op ) const
{
  static char buff[128];
  const RESERVATION* r = Reservation_Table[OPR_opcode(op)];

  buff[0] = '\0';

  for( int i = 0; i < r->RESERVATION_size(); i++ ){
    const RESOURCE* res = r->RESERVATION_resource(i);
    
    if( ( OPR_res_mask( op ) & res->RESOURCE_mask() ) == 0 )
      continue;

    if( OPR_scheduled( op ) &&
	( OPR_res_id( op ) == res->RESOURCE_id() ) )
      sprintf( buff, "%s %s*", buff, res->RESOURCE_name() );
    else
      sprintf( buff, "%s %s", buff, res->RESOURCE_name() );
  }

  return buff;
}


void MRT::Reserve_Resources( OPR* op, int clock )
{
  const RESERVATION* r = MRT::Reservation_Table[OPR_opcode(op)];
  clock = Cycle_Mod_II( clock, mii );

  for( int i = 0; i < r->RESERVATION_size(); i++ ){
    const RESOURCE* res = r->RESERVATION_resource(i);
    const RESOURCE_ID res_id = res->RESOURCE_id();

    // Make sure this op can really take this resource.
    if( ( OPR_res_mask( op ) & res->RESOURCE_mask() ) == 0 )
      continue;

    if( Resource_Table[clock].nRes[res_id] > 0 ){
      Resource_Table[clock].nRes[res_id]--;
      Set_OPR_res_id( op, res_id );
      Resource_Table[clock].nExFp -= OPR_is_ndual( op ) ? 2 : 1;
      return;
    }
  }

  ASSERT( false );
}


void MRT::Reserve_Register( int clock )
{
  clock = Cycle_Mod_II( clock, mii );
  Resource_Table[clock].nRes[reg.RESOURCE_id()]--;
}


void MRT::Unreserve_Resources( OPR* op, int clock )
{
  const RESOURCE* res = res_per_cycle[OPR_res_id(op)];
  clock = Cycle_Mod_II( clock, mii );
  Resource_Table[clock].nRes[OPR_res_id(op)]++;
  Resource_Table[clock].nExFp += OPR_is_ndual(op) ? 2 : 1;
}


int MRT::Find_First_Schedule_Cycle( OPR* op )
{
  for( int cycle = OPR_release_time(op); cycle <= OPR_deadline(op); cycle++ ){
    if( Resources_Available( op, cycle, true ) )
      return cycle;
  }

  return OPR_deadline(op) + 1;
}


int MRT::Find_Last_Schedule_Cycle( OPR* op )
{
  for( int cycle = OPR_deadline(op); cycle >= OPR_release_time(op); cycle-- ){
    if( Resources_Available( op, cycle, true ) )
      return cycle;
  }

  return -1;
}


void MRT::Print( FILE* f ) const
{
  fprintf( f, "\n===================== M R T ====================\n" );

  fprintf( f, "MII:%d rtable_size:%d MaxLive:%d\n", mii, rtable_size, MaxLive );

  fprintf( f, "\t" );

  for( int i = 0; i < Types_Per_Cycle; i++ ){
    const RESOURCE* res = res_per_cycle[i];
    fprintf( f, "%s[%d]\t", res->RESOURCE_name(), res->RESOURCE_avail_per_cycle() );
  }

  fprintf( f, "\n" );

  for( int i = 0; i < mii; i++ ){
    fprintf( f, "cycle%2d:", i );
    for( int j = 0; j < Types_Per_Cycle; j++ ){
      const RESOURCE* res = res_per_cycle[j];
      RESOURCE_ID id = res->RESOURCE_id();

      fprintf( f, "  %d\t", Resource_Table[i].nRes[id] );
    }

    fprintf( f, "\n" );
  }

  fprintf( f, "================================================\n" );
}


/* Compute the MaxLive.
   This routine should be done whenever an op is scheduled.
*/
int MRT::Compute_MaxLive( SBB* bb )
{
  MaxLive = 0;

  for( int indx = 0; indx < bb->Oprs(); indx++ ){
    const OPR* op = bb->Opr( indx );
    int define = OPR_release_time( op );
    int last_use = -1;

    for( int arc = 0; arc < OPR_num_succs(op); arc++ ){
      DEP* dep = OPR_succ( op, arc );
      if( DEP_Is_True( dep ) ){
	OPR* succ_op = DEP_succ(dep);
	last_use = MAX( last_use, OPR_release_time( succ_op ) );
      }
    }

    for( int live_range = define; live_range < last_use; live_range++ ){
      Reserve_Register( live_range );
    }
  }

  for( int i = 0; i < mii; i++ ){
    int live = reg.RESOURCE_avail_per_cycle() - Resource_Table[i].nRes[reg.RESOURCE_id()];
    MaxLive = MAX( MaxLive, live );
  }

  return MaxLive;
}


static const int8_t Special_Latency[] = {
#define op(a,b,c,d,e,f,g) e,
#include "sas_op.h"
};


/* Here from_op is a predecessor of the to_op in the ddg.
 */
void Adjust_Latency( DEP* dep )
{
  if( !DEP_Is_True( dep ) )
    return;

  const OPR* from = DEP_pred( dep );
  const OPR* to = DEP_succ( dep );
  const STOP opcode = OPR_opcode( from );
  int8_t latency = Special_Latency[opcode];

  // First, consider data dependencies.

  if( latency < 0 ){
    latency = MRT::Reservation_Table[opcode]->RESERVATION_latency();
    ASSERT( latency >= 0 );
  }

  // handle more special data dependencies cases
  // Note: OPR_is_mult/div includes dmult/ddiv;
  //       OPR_is_madd includes msub, maddu, and msubu;
  //       OPR_is_mt includes mtlo and mthi.

  if( OPR_is_mul( from ) ){
    if( OPR_is_mult( to ) || OPR_is_mul( to ) || OPR_is_madd( to )
	|| OPR_is_mt( to ) )
      latency = 1;
    else if( OPR_is_div( to ) )
      latency = 3;
    else
      latency = 4;

  } else if( OPR_is_dmult( from ) ){
    if( OPR_is_mf( to ) )
      latency = 10;
    else if( OPR_is_mult( to ) || OPR_is_mul( to ) )
      latency = 8;
    else if( OPR_is_div( to ) )
      latency = 11;
    else if( OPR_is_madd( to ) || OPR_is_mt( to ) )
      latency = 9;

  } else if( OPR_is_mult( from ) || OPR_is_madd( from ) ){
    if( OPR_is_mf( to ) )
      latency = 3;
    else if( OPR_is_mult( to ) || OPR_is_mul( to )
	     || OPR_is_madd( to ) || OPR_is_mt( to ) )
      latency = 1;
    else if( OPR_is_div( to ) )
      latency = 3;

  } else if( OPR_is_mt( from ) ){
    if( OPR_is_div( to ) )
      latency = 2;

  } else if( OPR_is_ddiv( from ) ){
    if( OPR_is_mf( to ) )
      latency = 68;
    else if( OPR_is_mult( to ) )
      latency = 66;
    else if( OPR_is_div( to ) )
      latency = 68;
    else if( OPR_is_madd( to ) || OPR_is_mt( to ) )
      latency = 67;
  } else if( OPR_is_div( from ) ){
    if( OPR_is_mf( to ) )
      latency = 36;
    else if( OPR_is_mult( to ) )
      latency = 34;
    else if( OPR_is_div( to ) )
      latency = 36;
    else if( OPR_is_madd( to ) || OPR_is_mt( to ) )
      latency = 35;

  }

  if( from == to )
    latency = MAX( latency, 1 );

#if 0
  if( DEP_latency( dep ) != latency ){
    printf( "Adjust latency for %d -> %d from %d to %d\n",
	    OPR_indx( from ), OPR_indx( to ), DEP_latency( dep ), latency );
  }
#endif

  DEP_latency( dep ) = latency;
}


#if 0
bool MRT::Consume_Resource( OPR* from, OPR* to, int clock, const RESOURCE* res ) const
{
  const RESOURCE* from_res = MRT::res_per_cycle[OPR_res_id(from)];
  const int from_clk = OPR_release_time( from );
  
  return true;
}
#endif
