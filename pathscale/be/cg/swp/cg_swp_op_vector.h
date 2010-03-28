/*
 *  Copyright (C) 2008, 2009 PathScale, LLC.  All Rights Reserved.
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


/*******************************************************************//**
* \file cg_swp_op_vector.h                                             *                  
* \brief SWP_OP_VECTOR is a utility class which is used by several     *
*        classes during SWP.                                           *
***********************************************************************/

#ifndef cg_swp_op_vector_INCLUDED
#define cg_swp_op_vector_INCLUDED "cg_swp_op_vector.h"

/* Basic compiler include files */
#include "defs.h"
#include "mempool.h"
#include "mempool_allocator.h"

/* Code Generator (CG) related include files */
#include "cg_loop.h"
#include "op.h"
#include "bb.h"

/* Software Pipelining (SWP) related include files */
#include "cg_swp.h"

/* Standard Template Library (STL) related include files */
#include <vector>
#include <map>
#include <functional>

#define MAX_OPERANDS ISA_OPERAND_max_operands

#define FOR_ALL_SWP_OP_VECTOR_IDXs_FWD(op_vec, idx) \
  for (idx = 0; idx < op_vec.size (); ++idx)

#define FOR_ALL_SWP_OP_VECTOR_IDXs_REV(op_vec, idx) \
  for (idx = op_vec.size () - 1; idx >= 0; --idx)

#define FOR_ALL_SWP_OP_VECTOR_IDXs FOR_ALL_SWP_OP_VECTOR_IDXs_FWD



class SWP_OP;

typedef mempool_allocator <SWP_OP>                SWP_STL_OP_ALLOC;
typedef std::vector <SWP_OP, SWP_STL_OP_ALLOC>    SWP_STL_OP_VECTOR;

typedef mempool_allocator <std::pair <const OP *, 
                                      INT> >      SWP_OP2INT_ALLOC;
typedef std::map <const OP *, 
                  INT, 
                  std::less <const OP *>, 
                  SWP_OP2INT_ALLOC>               SWP_OP2INT_MAP;



class SWP_OP {
  private:
  OP      *_op;
  mINT16   _cycle;
  mINT16   _scycle;
  mUINT16  _num_tries;
  mUINT8   _stage;
  mUINT8   _unrolling;
  mBOOL    _scheduled;
  mBOOL    _p_update;
  mINT8    _original_omega[MAX_OPERANDS];
  mINT8    _omega[MAX_OPERANDS];

  public:
  SWP_OP (OP *op);
  SWP_OP (const SWP_OP& swp_op);
  SWP_OP (OP *op, const SWP_OP& swp_op); 
  SWP_OP (OP *op, 
          INT16 cycle, 
          INT16 scycle, 
          UINT8 stage, 
          BOOL scheduled,
          BOOL p_update,
          UINT16 num_tries, 
          UINT8 unrolling, 
          const SWP_OP& swp_op); 
  ~SWP_OP (void)                    { return; }

  OP* 
  Get_OP (void) const               { return _op; }

  INT16  
  Get_Cycle (void) const            { return _cycle; }
  INT16  
  Get_Start_Cycle (void) const      { return _scycle; }
  UINT8  
  Get_Stage (void) const            { return _stage; }
  void 
  Set_Cycle (INT16 cycle, INT ii)
  {
    _cycle  = cycle;
    _scycle = cycle % ii;
    _stage  = cycle / ii;

    return;
  } 
    
  void 
  Set_Scheduled (void)              { _scheduled = TRUE; }
  void 
  Reset_Scheduled (void)            { _scheduled = FALSE; }
  BOOL 
  Is_Scheduled (void) const         { return _scheduled; }
  
  void 
  Set_Pointer_Update (void)         { _p_update = TRUE; }
  void 
  Reset_Pointer_Update (void)       { _p_update = FALSE; }
  BOOL 
  Is_Pointer_Update (void) const    { return _p_update; }

  void 
  Inc_Try_Counter (void)            { ++_num_tries; }
  void 
  Reset_Try_Counter (void)          { _num_tries = 0; }
  UINT16  
  Get_Try_Counter (void) const      { return _num_tries; }

  UINT8  
  Get_Unrolling (void) const        { return _unrolling; }
  void 
  Set_Unrolling (UINT8 unrolling)   { _unrolling = unrolling; }

  void 
  Set_Omega (UINT8 opr, INT8 omega) 
  { 
    Is_True (opr < MAX_OPERANDS, ("SWP_OP: Index out of bounds."));
    _omega[opr] = omega;

    return;
  }

  INT8  
  Get_Omega (UINT8 opr) const 
  { 
    Is_True (opr < MAX_OPERANDS, ("SWP_OP: Index out of bounds."));

    return _omega[opr];
  }

  void
  Set_Original_Omega (UINT8 opr, INT8 omega) 
  { 
    Is_True (opr < MAX_OPERANDS, ("SWP_OP: Index out of bounds."));
    _original_omega[opr] = omega;

    return;
  }

  INT8
  Get_Original_Omega (UINT8 opr) const
  { 
    Is_True (opr < MAX_OPERANDS, ("SWP_OP: Index out of bounds."));

    return _original_omega[opr];
  }

  void 
  Print (FILE *f = stderr) const;
  void 
  Verify (void) const;
};



class SWP_OP_LESS : 
  public std::binary_function <const SWP_OP&, const SWP_OP&, BOOL> 
{
  private:
  INT _ii;

  public:
  bool operator () (const SWP_OP& sop1, const SWP_OP& sop2) const {
    return (sop1.Get_Start_Cycle () < sop2.Get_Start_Cycle ());
  }
};



class SWP_OP_VECTOR : public SWP_STL_OP_VECTOR {
  private:
  MEM_POOL *_m;
  OP       *_start;
  OP       *_stop;
  OP       *_branch;
    
  size_type _start_idx;
  size_type _stop_idx;
  size_type _branch_idx;
  
  SWP_OP2INT_MAP _op_map;

  void
  Recalculate_OP_Indices (void);

  public:
  SWP_OP_VECTOR (MEM_POOL *m);
  SWP_OP_VECTOR (BB *body, MEM_POOL *m);
  ~SWP_OP_VECTOR (void) { return; }

  size_type 
  Get_Start_Idx (void) const  { return _start_idx; }
  size_type 
  Get_Stop_Idx (void) const   { return _stop_idx; }
  size_type 
  Get_Branch_Idx (void) const { return _branch_idx; }

  size_type 
  OP_Idx (const OP *) const;
  void 
  Sort_OPs_By_Cycles (void);
  void 
  Sort_OPs_Within_Cycles (void);
  void 
  Update_Omega (void);
  void 
  Print (const char *string, FILE *f = stderr) const;
  void 
  Verify (void) const;

  SWP_OP&
  operator () (size_type);
  const SWP_OP&
  operator () (size_type) const;

  void 
  push_back (const SWP_OP& x);
};


inline SWP_OP_VECTOR::size_type 
SWP_OP_VECTOR::OP_Idx (const OP *op) const {
  SWP_OP2INT_MAP::const_iterator result;
  result = _op_map.find (op);
  FmtAssert (result != _op_map.end (),
             ("SWP_OPS: OP is not in OP map!"));
    
  return result->second;
}
  
  
inline void 
SWP_OP_VECTOR::Sort_OPs_By_Cycles (void) {
  SWP_OP_VECTOR::iterator itr1 = begin ();
  SWP_OP_VECTOR::iterator itr2 = end ();
  
  if (_start && _stop) {
    ++itr1;
    --itr2;
  }
  // Use stable_sort to preserve dependencies between OPs
  // assigned to the same cycle.
  stable_sort (itr1, itr2, SWP_OP_LESS ());

  Recalculate_OP_Indices ();

  return;
}
  

inline SWP_OP& 
SWP_OP_VECTOR::operator () (size_type x) {
  Is_True ((x >= 0) && (x < size ()),
           ("SWP_OP: Index out of bounds."));
  
  return (*this)[x];
}
  

inline const SWP_OP& 
SWP_OP_VECTOR::operator () (size_type x) const {
  Is_True ((x >= 0) && (x < size ()),
           ("SWP_OP: Index out of bounds."));
    
  return (*this)[x];
}


inline void
SWP_OP_VECTOR::push_back (const SWP_OP& x) { 
  OP *op = x.Get_OP ();
  _op_map[op] = size ();

  SWP_STL_OP_VECTOR::push_back (x);
}

#endif /* cg_swp_op_vector_INCLUDED */

