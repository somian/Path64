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
* \file cg_swp_lifeness.h                                              *
* \brief Calculate the lifness information for a given SWP_OP_Vector.  *
***********************************************************************/


#ifndef cg_swp_lifeness_INCLUDED
#define cg_swp_lifeness_INCLUDED "cg_swp_lifeness.h"

/* Basic compiler include files */
#include "defs.h"
#include "mempool.h"

/* Code Generator (CG) related include files */
#include "tn.h"
#include "tn_set.h" 

/* Software Pipelining (SWP) related include files */
#include "cg_swp_op_vector.h"



class SWP_LIFENESS {
  private:
  MEM_POOL *_m;

  TN_SET   *_tn_defined;
  TN_SET   *_tn_used;
  TN_SET   *_tn_dedicated;
  TN_SET   *_tn_invariant;
  TN_SET   *_tn_variant;
  
  void 
  Calculate_Lifeness (const SWP_OP_VECTOR::const_iterator,
                      const SWP_OP_VECTOR::const_iterator);

  public:
  SWP_LIFENESS (const SWP_OP_VECTOR::const_iterator begin, 
                const SWP_OP_VECTOR::const_iterator end,
                MEM_POOL *m);
  SWP_LIFENESS (const SWP_OP_VECTOR& ops,
                MEM_POOL *m);
  ~SWP_LIFENESS (void) { return; }

  BOOL
  Is_Loop_Variant (const TN *tn) const;
  BOOL 
  Is_Loop_Defined (const TN *tn) const;
  BOOL
  Is_Conditional_Defined (const TN *tn) const;
  const 
  TN_SET *Get_Dedicated_TNs (void) const { return _tn_dedicated; }
  const 
  TN_SET *Get_Invariant_TNs (void) const { return _tn_invariant; }
  const 
  TN_SET *Get_Variant_TNs (void) const   { return _tn_variant; }

  void 
  Print (FILE *f = stderr) const;
  void 
  Verify (void) const;
};


inline BOOL 
SWP_LIFENESS::Is_Loop_Variant (const TN *tn) const {
  
  return TN_SET_MemberP (_tn_variant, tn);
}

inline BOOL 
SWP_LIFENESS::Is_Loop_Defined (const TN *tn) const {
  
  return TN_SET_MemberP (_tn_defined, tn);
}

#endif /* cg_swp_lifeness_INCLUDED */

