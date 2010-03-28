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
* \file cg_swp_mrt.cxx                                                 *
* \brief Implementation of the Modulo Reservation Table                *
***********************************************************************/

/* Basic compiler include files */
#include "defs.h"
#include "mempool.h"
#include "ti_res_res.h"

/* Software Pipelinig (SWP) related include files */
#include "cg_swp_op_vector.h"
#include "cg_swp_mrt.h"



SWP_MRT::SWP_MRT (const SWP_OP_VECTOR& ops, INT ii, MEM_POOL *m) :
  _m  (m),
  _ii (ii)
{
  /* Allocate resource mangament structure */
  _resources = TI_RES_RES_Alloc (SWP_Cyclic, _m);
  
  /* Before SWP can use the resource we have to call TI_RES_RES_Has_TOP
   * at least once for every unique opcode. 
   * For simplicity we call it for every OP in the loop (except for the 
   * fake START and STOP op).
   */
  for (INT i = 1; i < ops.size () - 1; ++i) {
    OP *op = ops(i).Get_OP ();
    FmtAssert (op != 0, ("SWP_MRT: Invalid OP pointer!"));
    TI_RES_RES_Has_TOP (_resources, OP_code (op));
  }

  /* We need to verify here.
   * If we have a bad II, TI_RES_RES_Set_BB_Cycle_Count could seg. fault
   */
  /* TODO: Check why TI_RES_RES_Set_BB_Cycle_Count seg. faults. */
  Verify ();

  /* Set the length to II and empty the resource reservation table */
  TI_RES_RES_Set_BB_Cycle_Count (_resources, _ii);
    
  return;
}


INT 
SWP_MRT::Find_Free_Resources (const SWP_OP& op, 
                              INT start, 
                              INT stop, 
                              SWP_SEARCH_DIRECTION direction) const
{
  INT inc;
  INT begin;
  INT finish;
  INT cycle = -1;

  if (direction == SWP_Forward) {
    inc    = 1;
    begin  = start;
    finish = stop + 1;
  }
  else if (direction == SWP_Backward) {
    inc    = -1;
    begin  = stop;
    finish = start - 1;
  }
  else {
    Fail_FmtAssertion ("SWP_MRT: Invalid search direction!");
  }

  for (INT i = begin; i != finish; i += inc) {
    if (Resources_Available (op, i)) {
      cycle = i;
      break;
    }
  }

  return cycle;
}

