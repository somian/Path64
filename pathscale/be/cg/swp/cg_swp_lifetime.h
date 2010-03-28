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
 * \file cg_swp_lifetime.h                                             *
 * \brief Header file for LIFETIME                                     *
 **********************************************************************/

#ifndef cg_swp_lifetime_INCLUDED
#define cg_swp_lifetime_INCLUDED "cg_swp_lifetime.h"

/* Basic compiler include files */
#include "defs.h"
#include "mempool.h"

/* Code Generator (CG) related include files */
#include "cgtarget.h"
#include "register.h"
#include "tn.h"
#include "tn_set.h" 
#include "op.h"
#include "bb.h"

/* Software Pipelining (SWP) related include files */
#include "cg_swp.h"
#include "cg_swp_op_vector.h"
#include "cg_swp_lifeness.h"

/* Standard Template Library (STL) related include files */
#include <vector>
#include <map>
#include <algorithm>
#include <functional>
#include <cmath>


/* 15030: SWP_SUBCYCLES_PER_CYCLE must be at least as large as
 * the maximum number of OPs that can be scheduled into a single
 * cycle.  This includes fake OPs such as mov_hilo (otherwise,
 * we could just use CGTARG_Issue_Width()).  However, it should
 * not be too large, or the subcycle numbers could overflow the
 * data type for _start and _end in SWP_TN_LIFETIME.
 */

#define SWP_SUBCYCLES_PER_CYCLE 8


enum SWP_TN_TYPE {
  SWP_Result,
  SWP_Operand
};
 
class SWP_TN_LIFETIME;

typedef mempool_allocator <SWP_TN_LIFETIME> SWP_TN_LIFETIME_ALLOC;
typedef std::vector <SWP_TN_LIFETIME, 
                     SWP_TN_LIFETIME_ALLOC> SWP_TN_LIFETIME_VECTOR;



class SWP_TN_LIFETIME {
  public:
  static mUINT16 _num_lt;

  private:
  mUINT16       _idx;
  mUINT16       _start;
  mUINT16       _end;
  mUINT8        _omega;
  mUINT8        _alpha;
  SWP_TN_VECTOR _tn;
  
  public:
  SWP_TN_LIFETIME (MEM_POOL *m);
  ~SWP_TN_LIFETIME (void)             { return; }

  mUINT16
  Get_Idx (void) const                { return _idx; }

  void
  Set_Start_Sub_Cycle (mUINT16 start) { _start = start; }
  mUINT16
  Get_Start_Sub_Cycle (void) const    { return _start; }
  mUINT16
  Get_Start_Cycle (void) const
  {
    return _start / SWP_SUBCYCLES_PER_CYCLE;
  }

  void
  Set_End_Sub_Cycle (mUINT16 end)     { _end = end; }
  mUINT16
  Get_End_Sub_Cycle (void) const      { return _end; }
  mUINT16
  Get_End_Cycle (void) const
  {
    return _end / SWP_SUBCYCLES_PER_CYCLE;
  }

  void
  Set_Omega (mUINT8 omega)            { _omega = omega; }
  mUINT8
  Get_Omega (void) const              { return _omega; }

  void
  Set_Alpha (mUINT8 alpha)            { _alpha = alpha; }
  mUINT8
  Get_Alpha (void) const              { return _alpha; }

  void
  Add_TN (TN *tn)
  {
    if (find (_tn.begin (), _tn.end(), tn) == _tn.end ()) {
      _tn.push_back (tn);
    }

    return;
  }
  
  TN *
  Get_TN (SWP_TN_VECTOR::size_type idx) const
  {
    Is_True ((idx >= 0) && (idx < _tn.size ()), 
             ("SWP_TN_LIFETIME: Index out of bounds."));

    return _tn[idx];
  }

  SWP_TN_VECTOR::size_type
  Get_TN_Size (void) const            { return _tn.size (); }

  REGISTER
  Get_Register (void) const
  {
    Is_True (_tn.size () > 0, ("SWP_TN_LIFETIME: Undefined TN."));

    return TN_register (_tn[0]);
  }

  void
  Print (FILE *f = stderr) const;
  void
  Verify (void) const;

  mUINT16
  Get_TN_Lifetime (void) const;
  mUINT16
  operator () (void) const            { Get_TN_Lifetime (); }
};


inline mUINT16
SWP_TN_LIFETIME::Get_TN_Lifetime (void) const {
  mINT32 lifetime = _end - _start;
  Is_True (lifetime >= 0,
           ("SWP_LIFETIME: Negative lifetime."));

  return (mUINT16) std::ceil ((float)lifetime / SWP_SUBCYCLES_PER_CYCLE);
}


class SWP_MVE;

class SWP_LIFETIME {
  private:
  MEM_POOL *_m;
  INT       _ii;
  BB       *_head;
  BB       *_tail;
  INT       _reg_class;
    
  const SWP_OP_VECTOR&   _ops;
  const SWP_MVE         *_mve;
  const SWP_LIFENESS&    _ln;
  SWP_TN_LIFETIME_VECTOR _lt;
  SWP_TN2INT_MAP         _tn2lt;
  SWP_mUINT16_VECTOR     _op_sub_cycle;

  void
  Update_TN_Lifetime (mUINT16 cycle,
                      mUINT8 omega,
                      TN *tn,
                      SWP_TN_TYPE type);

  public:
  SWP_LIFETIME (const SWP_OP_VECTOR& ops,
		const SWP_MVE *mve,
                const SWP_LIFENESS& ln,
                INT ii,
                MEM_POOL *m,
                INT reg_class = ISA_REGISTER_CLASS_UNDEFINED);
  ~SWP_LIFETIME (void) { return; }

  void
  Print (FILE *f = stderr) const;
  void
  Verify (void) const;

  const SWP_TN_LIFETIME&
  operator () (const TN *) const;
  SWP_TN_LIFETIME&
  operator () (const TN *);

  const SWP_TN_LIFETIME&
  operator () (SWP_TN_LIFETIME_VECTOR::size_type idx) const;
  SWP_TN_LIFETIME&
  operator () (SWP_TN_LIFETIME_VECTOR::size_type idx);
    
  SWP_TN_LIFETIME_VECTOR::size_type
  size (void) const    { return _lt.size (); }

  SWP_TN_LIFETIME_VECTOR::iterator
  begin (void)         { return _lt.begin (); }
  SWP_TN_LIFETIME_VECTOR::iterator
  end (void)           { return _lt.end (); }
};


inline const SWP_TN_LIFETIME&
SWP_LIFETIME::operator () (const TN *tn) const {
  SWP_TN2INT_MAP::const_iterator itr = _tn2lt.find (tn);
  FmtAssert (itr != _tn2lt.end (),
             ("SWP::LIFETIME: Unknown TN."));
  Is_True ((itr->second >= 0) && (itr->second < _lt.size ()),
           ("SWP_LIFETIME: Index out of bounds."));

  return _lt[itr->second];
}


inline SWP_TN_LIFETIME&
SWP_LIFETIME::operator () (const TN *tn) {
  SWP_TN2INT_MAP::const_iterator itr = _tn2lt.find (tn);
  FmtAssert (itr != _tn2lt.end (),
             ("SWP::LIFETIME: Unknown TN."));
  Is_True ((itr->second >= 0) && (itr->second < _lt.size ()),
           ("SWP_LIFETIME: Index out of bounds."));
    
  return _lt[itr->second];
}


inline const SWP_TN_LIFETIME&
SWP_LIFETIME::operator () (SWP_TN_LIFETIME_VECTOR::size_type idx) const
{
  Is_True ((idx >= 0) && (idx < _lt.size ()),
           ("SWP_LIFETIME: Index out of bounds."));

  return _lt[idx];
}


inline SWP_TN_LIFETIME&
SWP_LIFETIME::operator () (SWP_TN_LIFETIME_VECTOR::size_type idx) {
  Is_True ((idx >= 0) && (idx < _lt.size ()),
           ("SWP_LIFETIME: Index out of bounds."));

  return _lt[idx];
}

#endif /* cg_swp_lifetime_INCLUDED */

