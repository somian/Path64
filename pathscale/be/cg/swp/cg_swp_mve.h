/* 
 *  Copyright (C) 2008  PathScale, LLC. All Rights Reserved.
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
* \file cg_swp_mve.h                                                   *
* \brief Header file for the Modulo Variable Expansion                 *
***********************************************************************/

#ifndef cg_swp_mve_INCLUDED
#define cg_swp_mve_INCLUDED "cg_swp_mve.h"

/* Basic compiler include files */
#include "defs.h"
#include "mempool.h"

/* Software Pipelinig (SWP) related include files */
#include "cg_swp_op_vector.h"
#include "cg_swp_lifeness.h"
#include "cg_swp_lifetime.h"

/* Standard Template Library (STL) related include files */
#include <map>
#include <functional>



typedef mempool_allocator < std::pair <const TN *, 
                                       TN **> >    SWP_TN2TN_ARRAY_ALLOC;
                                    
typedef std::map <const TN *,
                  TN **,
                  std::less <const TN *>,
                  SWP_TN2TN_ARRAY_ALLOC>           SWP_TN2TN_ARRAY_MAP;


class SWP_MVE {
  private:
  MEM_POOL             *_m;
  const SWP_OP_VECTOR&  _ops;
  SWP_OP_VECTOR&        _mve_ops;
  INT                   _k_min;
  SWP_TN2TN_ARRAY_MAP   _renamed_tns;
  SWP_TN2TN_MAP         _reverse_tns;
  const SWP_LIFENESS&   _ln;
  const SWP_LIFETIME&   _lt;
  INT                   _ii;

  void 
  Determine_Unroll_Factor (void);
  void
  Unroll_Kernel (void);
  void
  Update_Omega (void);
  void 
  Duplicate_OP (const SWP_OP& swp_op, INT unrolling);
  void 
  Generate_Renaming_TNs (void);
  TN * 
  Get_Result_TN (const SWP_OP& swp_op, UINT8 idx);
  TN * 
  Get_Operand_TN (const SWP_OP& swp_op, UINT8 idx);

  public:
  SWP_MVE (const SWP_OP_VECTOR& ops_in, 
           SWP_OP_VECTOR& ops_out, 
           const SWP_LIFENESS& lifeness,
           const SWP_LIFETIME& lifetime,
           INT ii,
           MEM_POOL *m);
  ~SWP_MVE (void) { return; }
    

  TN *
  Get_Renamed_TN (TN *tn, INT omega, INT unrolling) const;
  TN *
  Get_Original_TN (TN *tn) const;
  INT 
  Get_Kernel_Unrollings (void) const { return _k_min; }

  void 
  Print (FILE *f = stderr) const;
  void 
  Verify (void) const;
};


inline TN * 
SWP_MVE::Get_Renamed_TN (TN *tn, INT omega, INT unroll) const {
  if (TN_is_dedicated (tn)) return tn;
  
  INT idx = unroll - omega + _k_min;
  
  idx = idx % _k_min;
  while (idx < 0) {
    idx += _k_min;
  }
  
  FmtAssert ((idx >= 0) && (idx < _k_min),
             ("SWP_MVE: Invalid index!"));
  SWP_TN2TN_ARRAY_MAP::const_iterator itr;
  itr = _renamed_tns.find (tn);
  FmtAssert (itr != _renamed_tns.end (), ("SWP_MVE:Unknown TN!"));
  
  return (itr->second)[idx];
}


inline TN *
SWP_MVE::Get_Original_TN (TN *tn) const {
  if (TN_is_dedicated (tn)) return tn;

  SWP_TN2TN_MAP::const_iterator itr;
  itr = _reverse_tns.find (tn);
  FmtAssert (itr != _reverse_tns.end (), ("SWP_MVE:Unknown TN!"));
  
  return itr->second;
}

#endif /* cg_swp_mve_INCLUDED */

