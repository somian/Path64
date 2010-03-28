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
* \file cg_swp_scheduler.h                                             *
* \brief Header file for the Modulo Scheduler                          *
*                                                                      *
* This Modulo Scheduler is based on                                    *
* Huff, Richard A., Lifetime-Sensitive Modulo Scheduling, ACM SIGPLAN  *
* Notices, volume 28, pages 258-267, 1993                              *
***********************************************************************/

#ifndef cg_swp_scheduler_INCLUDED
#define cg_swp_scheduler_INCLUDED "cg_swp_scheduler.h"

/* Basic compiler include files */
#include "defs.h"
#include "mempool.h"

/* Software Pipelinig (SWP) related include files */
#include "cg_swp_op_vector.h"



class SWP_SCHEDULER {
  private:
  MEM_POOL       *_m;
  SWP_OP_VECTOR&  _ops;
  INT             _ii;
  BOOL            _successful;

  public:
  SWP_SCHEDULER (SWP_OP_VECTOR& ops, 
                 INT ii, 
                 INT budget, 
                 MEM_POOL *m);
  ~SWP_SCHEDULER (void) { return; }

  BOOL 
  Get_Status (void) { return _successful; }

  void 
  Print (FILE *f = stderr) const;
  void 
  Verify (void) const;
};

#endif /* cg_swp_scheduler_INCLUDED */

