/*
 * Copyright (C) 2005, 2007 PathScale Inc.  All Rights Reserved.
 */
/* 
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

#ifndef _IDF_MD
#define _IDF_MD

#include <stdio.h>
#include <stdint.h>
#include "sas_opcode.h"
#include "bb.h"


inline void My_Stop()
{
  return;
}


#define STOP(f)                                             \
  do {                                                      \
    if( !(f) ){                                             \
      fprintf( stdout, "STOP: [%s] ", #f );                 \
      fprintf( stdout, "at %s %d\n", __FILE__, __LINE__ );  \
      My_Stop();                                            \
    }                                                       \
  } while(0)


// Provide a place to stop after error happens.
inline void My_Abort()
{
  abort();
}


#ifdef ASSERT
#undef ASSERT
#endif

#define ASSERT(f) FmtAssert (f, ("SWP assertion failure"));

typedef struct _OPR OPR;

class ARRAY {
 private:
  const void** elements;  // the actual array of objects
  uint32_t size;          // # of elements in the array
  uint32_t max_size;      // max allocated

  typedef int (*ARRAY_ELEMENT_CMP_FUNC)(const void*, const void*);

 public:
  const int16_t Size() const       { return size; }
  void Reset()                     { size = 0; }
  OPR* operator[]( int i ) const   { return (OPR*)elements[i]; }
  void Push( const void* e )       { ASSERT( size < max_size ); elements[size++] = e; }
  void Pop()                       { size--; }

  ARRAY() { size = max_size = 0, elements = NULL; }
  ~ARRAY() {}

  void Sort( ARRAY_ELEMENT_CMP_FUNC cmp_func )
    {
      qsort( elements, size, sizeof(void*), cmp_func );
    }

  void Init( size_t length )
    {
      if( max_size > 0 ){
	ASSERT( max_size >= length );
	size = 0;
	return;
      }

      ASSERT( length > 0 );
      max_size = length;
      elements = (const void **) MEM_POOL_Alloc( &sas_pool,
						 sizeof(void *) * length );
      size = 0;
    }

  void Remove( void* element )
    {
      for( unsigned int i = 0; i < size; i++ ){
	if( elements[i] == element ){
	  elements[i] = elements[size-1];
	  size--;
	  return;
	}
      }

      ASSERT( false );
    }

  const bool Find( void* element ) const
    {
      for( unsigned int i = 0; i < size; i++ ){
	if( elements[i] == element )
	  return true;
      }

      return false;
    }
};


typedef uint16_t ATTR_TN;
typedef uint16_t REG;
typedef uint64_t REGs;
typedef uint8_t  REG_CLASS;

typedef struct {
  int8_t  size;       // size of the TN in bits
  uint8_t omega;
  int64_t value;      // TN_has_value
  ATTR_TN attr;       // attributes flags for TN
  int8_t  offset;     // for memory access op
  int8_t  post_mod;   // the post-modification value for memory access op
  REG     reg;        // the physical register
  REG     pseudo_reg; // the pseudo register
  REG_CLASS reg_class;
  OPR*    definer;    // the OPR which defines the value of this TN

  char string[20];
} STN;

#define REG_MIN   1
#define REG_MAX   256

#define STN_string(s)               ((s)->string)
#define Set_STN_string(s,v)         (strncpy( (s)->string, v, 20 ))
#define Set_STN_omega(s,v)          ((s)->omega = (v))
#define STN_is_preg(s)              (((s)->reg >= REG_MIN) && ((s)->reg <= REG_MAX))
#define Reset_STN_is_preg(s)        ((s)->reg = (s)->pseudo_reg)
#define Set_STN_pseudo_reg(s,p)     ((s)->pseudo_reg = (p))
#define STN_pseudo_reg(s)           ((s)->pseudo_reg+0)
#define STN_post_mod(s)             ((s)->post_mod)
#define STN_offset(s)               ((s)->offset)
#define STN_size(s)                 ((s)->size+0)
#define STN_attr(s)                 ((s)->attr)
#define STN_value(s)                ((s)->value+0)
#define Set_STN_value(s,v)          ((s)->value = v)
#define STN_register(s)             ((s)->reg+0)
#define Set_STN_register(s,r)       ((s)->reg = r)
#define STN_register_class(s)       ((s)->reg_class+0)
#define Set_STN_register_class(s,c) ((s)->reg_class = c)
#define Set_STN_value(s,v)          ((s)->value = v)
#define Set_STN_size(s,b)           ((s)->size = b)
#define Set_STN_definer(s,d)        ((s)->definer = d)
#define STN_definer(s)              ((s)->definer+0)

#define STN_REGISTER      0x0001
#define STN_is_register(s)       (STN_attr(s) & STN_REGISTER)
#define Set_STN_is_register(s)   (STN_attr(s) |= STN_REGISTER)
#define Reset_STN_is_register(s) (STN_attr(s) &= ~STN_REGISTER)
#define STN_SAVE_REG      0x0002
#define STN_is_save_reg(s)       (STN_attr(s) & STN_SAVE_REG)
#define Set_STN_is_save_reg(s)   (STN_attr(s) |= STN_SAVE_REG)
#define Reset_STN_is_save_reg(s) (STN_attr(s) &= ~STN_SAVE_REG)
#define STN_SYMBOL        0x0008
#define STN_is_symbol(s)         (STN_attr(s) & STN_SYMBOL)
#define Set_STN_is_symbol(s)     (STN_attr(s) |= STN_SYMBOL)
#define STN_LABEL         0x0010
#define STN_is_label(s)          (STN_attr(s) & STN_LABEL)
#define Set_STN_is_label(s)      (STN_attr(s) |= STN_LABEL)
#define STN_HAS_VALUE     0x0020
#define STN_has_value(s)         (STN_attr(s) & STN_HAS_VALUE)
#define Set_STN_has_value(s)     (STN_attr(s) |= STN_HAS_VALUE)
#define STN_IS_PTR_REG    0x0040
#define STN_is_pointer_register(s)       (STN_attr(s) & STN_IS_PTR_REG)
#define Set_STN_is_pointer_register(s)   (STN_attr(s) |= STN_IS_PTR_REG)
#define Reset_STN_is_pointer_register(s) (STN_attr(s) &= ~STN_IS_PTR_REG)
#define STN_IS_MEM        0x0080
#define STN_is_mem(s)       (STN_attr(s) & STN_IS_MEM)
#define Set_STN_is_mem(s)   (STN_attr(s) |= STN_IS_MEM)
#define STN_IS_CONSTANT   0x0100
#define STN_is_constant(s)       (STN_attr(s) & STN_IS_CONSTANT)
#define Set_STN_is_constant(s)   (STN_attr(s) |= STN_IS_CONSTANT)
#define STN_IS_MULTIDIRECT 0x0200
#define STN_is_multidirect(s)       (STN_attr(s) & STN_IS_MULTIDIRECT)
#define Set_STN_is_multidirect(s)   (STN_attr(s) |= STN_IS_MULTIDIRECT)
#define STN_IS_MP_MEM        0x0400
#define STN_is_mp_mem(s)     (STN_attr(s) & STN_IS_MP_MEM)
#define Set_STN_is_mp_mem(s) (STN_attr(s) |= STN_IS_MP_MEM)


typedef enum {
  DEP_IGNORABLE = 0,
  DEP_REGIN, DEP_REGOUT, DEP_REGANTI,
  DEP_MEMIN, DEP_MEMOUT,
  DEP_MISC,  DEP_SCC
} DEP_TYPE;


typedef struct {
  OPR*     pred;
  OPR*     succ;
  int8_t   opnd;
  DEP_TYPE type;
  int8_t   latency;
  int8_t   omega;
} DEP;

#define DEP_Is_True(dep)      (DEP_type(dep) == DEP_REGIN)
#define DEP_pred(a)           ((a)->pred)
#define DEP_succ(a)           ((a)->succ)
#define DEP_latency(a)        ((a)->latency)
#define DEP_omega(a)          ((a)->omega)
#define DEP_type(a)           ((a)->type)
#define DEP_opnd(a)           ((a)->opnd)


typedef uint8_t  RESOURCE_ID;
typedef uint8_t  RESOURCE_MASK;


// The maximum number of operands per op
#define MAX_OPNDS   5
#define MAX_RESULTS 2

struct _OPR {
  STOP     opcode;        // which is actually OP_code
  int16_t id;            // assigned by the input schedule file
  int16_t indx;          // assigned by sas in topological order
  int16_t order;         // notify the caller the order when this op is scheduled
  int16_t release_time;
  int16_t deadline;
  int16_t Dist2Leaf;
  int16_t Dist2Root;
  uint16_t height;       // the hight to a leaf node, determined by #successors

  uint8_t results;
  STN     result[MAX_RESULTS];
  uint8_t opnds;
  STN     opnd[MAX_OPNDS];

  RESOURCE_ID   res_id;    // at which resource this op is scheduled
  RESOURCE_ID   res1_id;   // at which additional resource this op is scheduled
  RESOURCE_MASK res_mask;  // which resources this op can be scheduled on

  uint16_t unsched_preds;  // how many predecessors have not been scheduled
  uint16_t unsched_succs;  // how many successors have not been scheduled
  uint16_t num_succs;
  uint16_t num_preds;
  DEP**    pred;   // predecessors of this OPR
  DEP**    succ;   // successors of this OPR
  uint16_t uses;   // among those successors, how many of them are true data dep.

  uint8_t ncycles;   // resource will be occupied for ncycles.

  bool scheduled;    // if this op is scheduled
  bool is_addr_incr; // if this op define the addr of a load op
};


#define OPR_ncycles(o)        ((o)->ncycles+0)
#define Set_OPR_ncycles(o,i)  ((o)->ncycles = (i))
#define OPR_order(o)          ((o)->order+0)
#define Set_OPR_order(o,i)    ((o)->order = (i))
#define OPR_res_mask(o)       ((o)->res_mask+0)
#define Set_OPR_res_mask(o,d) ((o)->res_mask = (d))
#define OPR_res_id(o)         ((o)->res_id+0)
#define Set_OPR_res_id(o,i)   ((o)->res_id = (i))
#define OPR_res1_id(o)        ((o)->res1_id+0)
#define Set_OPR_res1_id(o,i)  ((o)->res1_id = (i))
#define OPR_height(o)         ((o)->height)
#define Inc_OPR_uses(o)       ((o)->uses++)
#define OPR_uses(o)           ((o)->uses+0)
#define OPR_unsched_preds(o)  ((o)->unsched_preds)
#define OPR_unsched_succs(o)  ((o)->unsched_succs)
#define OPR_preds(o)          ((o)->pred)
#define OPR_succs(o)          ((o)->succ)
#define OPR_pred(o,i)         ((o)->pred[i])
#define OPR_succ(o,i)         ((o)->succ[i])
#define OPR_num_succs(o)      ((o)->num_succs)
#define OPR_num_preds(o)      ((o)->num_preds)
#define OPR_Dist2Leaf(o)      ((o)->Dist2Leaf)
#define OPR_Dist2Root(o)      ((o)->Dist2Root)
#define OPR_release_time(o)   ((o)->release_time)
#define OPR_deadline(o)       ((o)->deadline)
#define OPR_opcode(o)         ((o)->opcode)
#define Set_OPR_opcode(o,c)   ((o)->opcode=c)
#define OPR_indx(o)           ((o)->indx+0)
#define Set_OPR_indx(o,i)     ((o)->indx = (i))
#define OPR_id(o)             ((o)->id+0)
#define Set_OPR_id(o,i)       ((o)->id = (i))
#define OPR_results(o)        ((o)->results)
#define OPR_result(o)         (&(o)->result[0])
#define OPR_opnds(o)          ((o)->opnds)
#define OPR_opnd(o,i)         (&(o)->opnd[i])
#define OPR_result_size(o)    ((o)->result.size)
#define OPR_opnd_size(o,i)    ((o)->opnd[i].size)
#define OPR_scheduled(o)        ((o)->scheduled)
#define Set_OPR_scheduled(o)    ((o)->scheduled = true)
#define Reset_OPR_scheduled(o)  ((o)->scheduled = false)
#define OPR_is_addr_incr(o)     ((o)->is_addr_incr)
#define Set_OPR_is_addr_incr(o) ((o)->is_addr_incr = true)


class SBB {
 private:
  uint16_t nOprs;  // number of oprs inside this BB
  uint32_t last_arc;

  bool cyclic;     // perform list scheduling or modulo scheduling on it

  OPR*  oprs_blk;  // hold the ops in the original given order
  OPR** oprs;      // hold the ops in topological order
  DEP*  deps;

  void Draw_Node( FILE*, const OPR*, char*, char* ) const;
  void Draw_Arc( FILE*, DEP*, char*, char* ) const;

  TALK talk;

 public:

  SBB( TALK talk ) : talk( talk ) {}

  const bool is_cyclic() const    { return cyclic; }
  const OPR* Opr( int i ) const   { return oprs[i]; }
  const OPR* Opr_Blk( int i ) const    { return &oprs_blk[i]; }
  const OPR* operator[]( int i ) const { return oprs[i]; }
  void Set_Opr( int i, OPR* op )  { oprs[i] = op; }
  const uint16_t Oprs() const     { return nOprs; }
  void Set_Oprs( int n )          { nOprs = n; }
  const DEP* Dep( int i ) const   { return &deps[i]; }

  void Draw_DDG() const;
  void Print( FILE* f = stdout ) const;
  void Emit_Schedule( BB* ) const;

  void Prune_Redundant_Arcs();
  void Remove_Arcs( DEP_TYPE = DEP_IGNORABLE );
  void Topological_Sort();
  void Refine_Latency();
  void Add_Pseudo_Arc( OPR*, OPR* );
  void Add_Arc( OPR*, OPR*, DEP_TYPE, int, int, int, int = 0 );

  void Init( int N, bool cyclic )
    {
      ASSERT( N > 0 );

      Set_Oprs( N );
      this->cyclic = cyclic;
      last_arc = 0;

      /* Allocate space for the latency table. */

      oprs_blk = (OPR *) MEM_POOL_Alloc( &sas_pool, sizeof(OPR) * N );
      oprs = (OPR **) MEM_POOL_Alloc( &sas_pool, sizeof(OPR *) * N );
      deps = (DEP *) MEM_POOL_Alloc( &sas_pool, sizeof(DEP) * N * N );

      for( int indx = 0; indx < N; indx++ ){
	OPR* op = &oprs_blk[indx];

	Set_OPR_id( op, indx );

	// Initialize op, which will be setup by SBB::Topological_Sort().
	oprs[indx] = NULL;
	Set_OPR_indx( op, -1 );
	Set_OPR_order( op, -1 );
       
	// Ncycles, the throughput, will be set in Recreate_DDG.
	Set_OPR_ncycles( op, 999 );

	// Setup dep arcs
	OPR_preds( op ) = (DEP **) MEM_POOL_Alloc( &sas_pool, sizeof(DEP*) * N );
	OPR_succs( op ) = (DEP **) MEM_POOL_Alloc( &sas_pool, sizeof(DEP*) * N );
	OPR_num_succs( op ) = OPR_num_preds( op ) = 0;
      }
    }
};


extern "C" {
  void  Setup_OPR( OPR*, const char* );
}


#endif
