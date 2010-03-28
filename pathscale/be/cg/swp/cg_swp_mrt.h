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
* \file cg_swp_mrt.h                                                   *
* \brief Header file for the Modulo Reservation Table                  *
***********************************************************************/

#ifndef cg_swp_mrt_INCLUDED
#define cg_swp_mrt_INCLUDED "cg_swp_mrt.h"

/* Basic compiler include files */
#include "defs.h"
#include "mempool.h"
#include "ti_res_res.h"

/* Software Pipelinig (SWP) related include files */
#include "cg_swp_op_vector.h"



enum SWP_RESOURCE_TYPE {
  SWP_Cyclic     = TRUE,
  SWP_Non_Cyclic = FALSE
};


enum SWP_SEARCH_DIRECTION {
  SWP_Forward  = 0,
  SWP_Backward = 1
};



class SWP_MRT {
  private:
  MEM_POOL   *_m;
  INT         _ii;
  TI_RES_RES *_resources;

  public:
  SWP_MRT (const SWP_OP_VECTOR& ops, INT ii, MEM_POOL *m);
  ~SWP_MRT (void) { return; }

  void 
  Reserve_Resources (SWP_OP& op);
  void 
  Unreserve_Resources (SWP_OP& op);

  BOOL 
  Resources_Available (const SWP_OP& op, INT cycle) const;
  BOOL 
  Resources_Relevant (const SWP_OP& op1, const SWP_OP& op2) const;
  BOOL 
  Resources_Equivalent (const SWP_OP& op1, const SWP_OP& op2) const;

  INT
  Find_Free_Resources (const SWP_OP& op, 
                       INT start, 
                       INT stop, 
                       SWP_SEARCH_DIRECTION dir) const; 

  void 
  Verify (void) const;
  void 
  Print (FILE *f = stderr);
};


inline void 
SWP_MRT::Reserve_Resources (SWP_OP& op) {
  Is_True (Resources_Available (op, op.Get_Cycle ()) == TRUE,
           ("SWP_MRT: No resources available!"));
  TI_RES_RES_Reserve_Resources (_resources,
                                OP_code (op.Get_OP ()),
                                op.Get_Cycle ());

  op.Set_Scheduled ();
  op.Inc_Try_Counter ();

  return;
}


inline void 
SWP_MRT::Unreserve_Resources (SWP_OP& op) {
  TI_RES_RES_Unreserve_Resources (_resources,
                                  OP_code (op.Get_OP ()),
                                  op.Get_Cycle ());

  op.Reset_Scheduled ();

  return;
}


inline BOOL 
SWP_MRT::Resources_Available (const SWP_OP& op, INT cycle) const {
    
  return TI_RES_RES_Resources_Available (_resources, 
                                         OP_code (op.Get_OP ()),
                                         cycle);
}


inline BOOL 
SWP_MRT::Resources_Relevant (const SWP_OP& op1, const SWP_OP& op2) const
{
  BOOL return_val = FALSE;

  if (op1.Get_Cycle () < op2.Get_Cycle ()) {
    INT offset = op2.Get_Cycle () - op1.Get_Cycle ();

    return_val =  
      TI_RES_RES_Resources_Relevant (_resources, 
                                     OP_code (op1.Get_OP ()), 
                                     OP_code (op2.Get_OP ()),
                                     offset);
  }
  else {
    INT offset = op1.Get_Cycle () - op2.Get_Cycle ();

    return_val =
      TI_RES_RES_Resources_Relevant (_resources, 
                                     OP_code (op2.Get_OP ()),
                                     OP_code (op1.Get_OP ()),
                                     offset);
  }

  return return_val;
}


inline BOOL 
SWP_MRT::Resources_Equivalent (const SWP_OP& op1, 
                               const SWP_OP& op2) const
{
  return    (op1.Get_OP () == op2.Get_OP ()) 
         || TI_RES_RES_Resources_Equivalent (_resources,
                                             OP_code (op1.Get_OP ()),
                                             OP_code (op2.Get_OP ()));
}


inline void 
SWP_MRT::Verify (void) const {
  FmtAssert (!TI_RES_RES_Is_Bad_II (_resources, _ii), 
             ("SWP_MRT: Bad II"));

  return;
}


inline void 
SWP_MRT::Print (FILE *f) {
  TI_RES_RES_Print (f, _resources);
  
  return;
}

#endif /* cg_swp_mrt_INCLUDED */

