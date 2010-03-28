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
* \file cg_swp_mindist.h                                               *
* \brief Header file for minimum distance calculation                  *
***********************************************************************/

#ifndef cg_swp_mindist_INCLUDED
#define cg_swp_mindist_INCLUDED "cg_swp_mindist.h"

/* Basic compiler include files */
#include "defs.h"
#include "mempool.h"

/* Software Pipelinig (SWP) related include files */
#include "cg_swp.h"
#include "cg_swp_op_vector.h"

/* Standard Template Library (STL) related include files */



class SWP_MINDIST {
  private:
  static const mINT16       _INFINITY = 9999;
  MEM_POOL                 *_m;
  SWP_OP_VECTOR::size_type  _num_ops;
  SWP_mINT16_VECTOR         _mindist;
    
  mINT16& 
  operator () (SWP_OP_VECTOR::size_type x, SWP_OP_VECTOR::size_type y);

  public:
  SWP_MINDIST (const SWP_OP_VECTOR& ops, UINT16 ii, MEM_POOL *m);
  ~SWP_MINDIST (void) { return; }

  void 
  Remove_Self_Recurrences (void);

  void 
  Print (FILE *f = stderr) const;
  void 
  Verify (void) const;

  mINT16 
  operator () (SWP_OP_VECTOR::size_type x, 
               SWP_OP_VECTOR::size_type y) const;
};


inline mINT16& 
SWP_MINDIST::operator () (SWP_OP_VECTOR::size_type x, 
                          SWP_OP_VECTOR::size_type y)
{
  SWP_OP_VECTOR::size_type idx = x * _num_ops + y;
  Is_True ((idx >= 0) && (idx < _mindist.size ()),
           ("SWP_MINDIST: Index out of bounds."));
  
  return _mindist[idx];
}


inline mINT16 
SWP_MINDIST::operator () (SWP_OP_VECTOR::size_type x, 
                          SWP_OP_VECTOR::size_type y) const
{
  SWP_OP_VECTOR::size_type idx = x * _num_ops + y;
  Is_True ((idx >= 0) && (idx < _mindist.size ()),
           ("SWP_MINDIST: Index out of bounds."));
  
  return _mindist[idx];
}

#endif /* cg_swp_mindist_INCLUDED */

