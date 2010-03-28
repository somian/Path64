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
* \file cg_swp_allocator.h                                             *
* \brief Header file for the Register Allocator                        *
*                                                                      *
* The register allocator is based on the following paper:              *
* B. R. Rau, M. Lee, P. P. Tirumalai, M. S. Schlansker, "Register      *
* Allocation for Software Pipelined Loops", PLDI '92                   *
*                                                                      *
* Right now the register allocator is only implemented for MVE kernels.*
***********************************************************************/

#ifndef cg_swp_allocator_INCLUDED
#define cg_swp_allocator_INCLUDED "cg_swp_allocator.h"

/* Basic compiler include files */
#include "defs.h"
#include "mempool.h"

/* Code Generator (CG) related include files */
#include "register.h"
#include "tn.h"

/* Software Pipelinig (SWP) related include files */
#include "cg_swp_op_vector.h"
#include "cg_swp_loop.h"
#include "cg_swp_lifeness.h"
#include "cg_swp_lifetime.h"

/* Standard Template Library (STL) related include files */
#include <vector>
#include <map>
#include <functional>



enum SWP_ORDER_ALGO {
  Order_By_Start_Time,
  Order_By_Conflict,
  Order_By_Conflict_And_Start_Time,
  Order_By_Adjacency,
  Order_By_Adjacency_And_Start_Time,
  Order_By_Adjacency_And_Conflict,
  Order_By_Adjacency_And_Conflict_And_Start_Time
};


enum SWP_ALLOC_ALGO {
  Best_Fit_Algo,
  First_Fit_Algo,
  End_Fit_Algo
};


typedef mempool_allocator <std::pair <TN *, 
                                      REGISTER> > SWP_TN2REG_ALLOC;
typedef std::map <TN *,
                  REGISTER,
                  std::less <TN *>,
                  SWP_TN2REG_ALLOC>                 SWP_TN2REG_MAP;



class SORT_BY_LT_START_TIME : 
  public std::binary_function <const SWP_TN_LIFETIME&,
                               const SWP_TN_LIFETIME&,
                               BOOL>
{
  public:
  BOOL operator () (const SWP_TN_LIFETIME& lt1, 
                    const SWP_TN_LIFETIME& lt2) const 
  {
    if (lt1.Get_Start_Sub_Cycle () < lt2.Get_Start_Sub_Cycle ()) {
      return TRUE;
    }
    else if (lt1.Get_Start_Sub_Cycle () > lt2.Get_Start_Sub_Cycle ()) {
      return FALSE;
    }
    else if (lt1.Get_End_Sub_Cycle () < lt2.Get_End_Sub_Cycle ()) {
      return TRUE;
    }
    else {
      return FALSE;
    }
  }
};



class SWP_DIST {
  private:
  MEM_POOL       *_m;
  INT             _num_lt;
  SWP_INT_VECTOR  _dist;

  INT& operator () (INT x, INT y);

  public:
  SWP_DIST (const SWP_LIFETIME& lifetime, 
            const SWP_LOOP& swp_loop,
            MEM_POOL *m);
  ~SWP_DIST (void) { return; }

  void Print (FILE *f =  stderr) const;
  void Verify (void) const;
    
  INT operator () (INT x, INT y) const;
};


inline INT& 
SWP_DIST::operator () (INT x, INT y) {
  INT idx = x * _num_lt + y;
  Is_True ((idx >=0) && (idx < _dist.size ()),
           ("SWP_DIST: Out of bounds!"));

  return _dist[idx];
}
  
  
inline INT 
SWP_DIST::operator () (INT x, INT y) const {
  INT idx = x * _num_lt + y;
  Is_True ((idx >=0) && (idx < _dist.size ()),
           ("SWP_DIST: Out of bounds!"));

  return _dist[idx];
}



class SWP_ALLOCATOR {
  private:
  MEM_POOL            *_m;
  const SWP_LIFENESS&  _ln;
  const SWP_LOOP&      _swp_loop;
  SWP_LIFETIME        *_lt;
  SWP_DIST            *_dist;
  REGISTER_SET         _free_register;
  INT                  _register_count[ISA_REGISTER_CLASS_MAX+1];
  REGISTER_SET        *_lt_register_set;
  SWP_TN2REG_MAP       _reg_assign;
  SWP_ORDER_ALGO       _order_algo;
  SWP_ALLOC_ALGO       _alloc_algo;
  BOOL                 _successful;
  BOOL                 _trace;


  BOOL 
  Allocate_Loop_Invariant (ISA_REGISTER_CLASS);
  BOOL 
  Allocate_Loop_Variant (ISA_REGISTER_CLASS);

  void 
  Sort_By_Start_Time (void);

  REGISTER 
  Best_Fit (INT, ISA_REGISTER_CLASS rclass) const;
  REGISTER 
  First_Fit (INT, ISA_REGISTER_CLASS rclass) const;
  REGISTER 
  End_Fit (INT, ISA_REGISTER_CLASS rclass) const;

  void 
  Update_Free_Register_Set (INT, REGISTER);

  public:
  SWP_ALLOCATOR (const SWP_OP_VECTOR& mve_ops,
		 const SWP_MVE& mve,
                 const SWP_LIFENESS& lifeness,
                 const SWP_LOOP& swp_loop,
                 SWP_ORDER_ALGO,
                 SWP_ALLOC_ALGO,
                 MEM_POOL *m);
  ~SWP_ALLOCATOR (void) { return; }

  BOOL Get_Status (void) const { return _successful; }
  INT  Get_Register_Count (ISA_REGISTER_CLASS rclass) { 
    return _register_count[rclass]; 
  }

  void Print (FILE *f = stderr) const;
  void Verify (void) const;
};


inline void 
SWP_ALLOCATOR::Sort_By_Start_Time (void) {
  sort (_lt->begin (), _lt->end (), SORT_BY_LT_START_TIME ());

  return;
}

#endif /* cg_swp_allocator_INCLUDED */

