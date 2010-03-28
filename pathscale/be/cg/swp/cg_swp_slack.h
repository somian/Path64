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
* \file cg_swp_slack.h                                                 *
* \brief Header file for SLACK                                         *
***********************************************************************/

#ifndef cg_swp_slack_INCLUDED
#define cg_swp_slack_INCLUDED "cg_swp_slack.h"

/* Basic compiler include files */
#include "defs.h"
#include "mempool.h"

/* Software Pipelinig (SWP) related include files */
#include "cg_swp.h"
#include "cg_swp_op_vector.h"
#include "cg_swp_mindist.h"



class SWP_SLACK {
  private:
  MEM_POOL             *_m;
  const SWP_OP_VECTOR&  _ops;
  const SWP_MINDIST&    _mindist;
  INT                   _start;
  INT                   _stop;
  SWP_INT_VECTOR        _estart;
  SWP_INT_VECTOR        _lstart;
    
  public:
  SWP_SLACK (const SWP_OP_VECTOR& ops, 
             const SWP_MINDIST& mindist, 
             INT ii,
             MEM_POOL *);
  ~SWP_SLACK (void) { return; }

  void 
  Init (INT);

  INT& 
  Estart (INT);
  INT& 
  Lstart (INT);
  INT  
  Estart (INT) const;
  INT  
  Lstart (INT) const;

  void 
  Update_Slack (void);
  void 
  Update_Slack (INT);
  void 
  Update_Slack (SWP_INT_VECTOR& adjust);

  void 
  Print (FILE *f = stderr) const;
  void 
  Verify (void) const;

  INT 
  operator () (INT) const;
};


inline INT& 
SWP_SLACK::Estart (INT x) {
  Is_True ((x >= 0) && (x < _estart.size ()),
           ("SWP_SLACK::Estart: Out of bounds!"));

  return _estart[x];
}


inline INT& 
SWP_SLACK::Lstart (INT x) {
  Is_True ((x >= 0) && (x < _lstart.size ()),
           ("SWP_SLACK::Lstart: Out of bounds!"));

  return _lstart[x];
}


inline INT 
SWP_SLACK::Estart (INT x) const {
  Is_True ((x >= 0) && (x < _estart.size ()),
           ("SWP_SLACK::Estart: Out of bounds!"));

  return _estart[x];
}

  
inline INT 
SWP_SLACK::Lstart (INT x) const {
  Is_True ((x >= 0) && (x < _lstart.size ()),
           ("SWP_SLACK::Lstart: Out of bounds!"));

  return _lstart[x];
}


inline INT 
SWP_SLACK::operator () (INT x) const {
  Is_True ((x >= 0) && (x < _estart.size ()),
           ("SWP_SLACK::Estart: Out of bounds!"));
  Is_True ((x >= 0) && (x < _lstart.size ()),
           ("SWP_SLACK::Lstart: Out of bounds!"));
  
  return _lstart[x] - _estart[x];
}

#endif /* cg_swp_slack_INCLUDED */

