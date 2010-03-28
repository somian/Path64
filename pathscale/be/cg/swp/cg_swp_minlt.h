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
* \file cg_swp_minlt.h                                                 *
* \brief Header file for MINLT                                         *
***********************************************************************/

#ifndef cg_swp_minlt_INCLUDED
#define cg_swp_minlt_INCLUDED "cg_swp_minlt.h"

/* Basic compiler include files */
#include "defs.h"
#include "mempool.h"

/* Software Pipelinig (SWP) related include files */
#include "cg_swp.h"
#include "cg_swp_op_vector.h"
#include "cg_swp_mindist.h"



class SWP_MINLT {
  private:
  MEM_POOL             *_m;
  const SWP_OP_VECTOR&  _ops;
  SWP_INT_VECTOR        _minlt;
  
  INT& 
  operator () (INT);

  public:
  SWP_MINLT (const SWP_OP_VECTOR& ops, 
             const SWP_MINDIST& mindist, 
             INT ii,
             MEM_POOL *m);
  ~SWP_MINLT (void) { return; }

  void 
  Print (FILE *f = stderr) const;
  void 
  Verify (void) const;

  INT  
  operator () (INT) const;
};


inline INT& 
SWP_MINLT::operator () (INT x) {
  Is_True ((x >= 0) && (x < _minlt.size ()),
           ("SWP_MINLT: Out of bounds!"));
  
  return _minlt[x];
}


inline INT 
SWP_MINLT::operator () (INT x) const {
  Is_True ((x >= 0) && (x < _minlt.size ()),
           ("SWP_MINLT: Out of bounds!"));
  
  return _minlt[x];
}

#endif /* cg_swp_minlt_INCLUDED */

