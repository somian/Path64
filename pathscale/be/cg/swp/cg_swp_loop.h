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
* \file cg_swp_loop.h                                                  *
* \brief Header file for SWP_LOOP.                                     *  
***********************************************************************/

#ifndef cg_swp_loop_INCLUDED
#define cg_swp_loop_INCLUDED "cg_swp_loop.h"

/* Basic compiler include files */
#include "defs.h"
#include "mempool.h"

/* Code Generator (CG) related include files */
#include "cg_loop.h"
#include "op.h"
#include "bb.h"

/* Software Pipelining (SWP) related include files */
#include "cg_swp.h"



class SWP_LOOP {
  public:
  LOOP_DESCR      *loop_descr;
  SWP_LOOP_TYPE    loop_type;
  SWP_STATUS_CODE  status_code;
  SRCPOS           srcpos;
  
  INT register_count[ISA_REGISTER_CLASS_MAX+1];

  INT mii;
  INT rec_mii;
  INT res_mii;
  INT ii;

  INT sc;
  INT lps;
  INT les;
  INT ps;
  INT es;

  INT kernel_unrollings;
  INT loop_unrollings;

  double time_ddg;
  double time_mii;
  double time_scheduler;
  double time_mve;
  double time_register;
  double time_code_gen;
  double time_total;

  INT size_original;
  INT size;

    
  SWP_LOOP (void);
  ~SWP_LOOP (void) { return; }
  
  void 
  Print (FILE *f = stderr) const;
  void 
  Print_MII (FILE *f = stderr) const;
  void
  Print_Time (FILE *f = stderr) const;
  void 
  Verify (void) const;
};

#endif /* cg_swp_loop_INCLUDED */

