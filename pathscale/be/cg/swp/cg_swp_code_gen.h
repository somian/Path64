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
* \file cg_swp_code_gen.h                                              *
* \brief Header file for the SWP Code Generation                       *
***********************************************************************/

#ifndef cg_swp_code_gen_INCLUDED
#define cg_swp_code_gen_INCLUDED "cg_swp_code_gen.h"

/* Basic compiler include files */
#include "defs.h"
#include "mempool.h"

/* Code Generator (CG) related include files */
#include "tn.h"
#include "bb.h"

/* Software Pipelinig (SWP) related include files */
#include "cg_swp.h"
#include "cg_swp_driver.h"
#include "cg_swp_op_vector.h"
#include "cg_swp_loop.h"
#include "cg_swp_mve.h"



enum SWP_INSERT_TYPE {
  SWP_Append,
  SWP_Prepend
};



class SWP_CODE_GEN {
  private:
  const SWP_MVE&        _mve;
  SWP_LOOP&             _swp_loop;
  const SWP_OP_VECTOR&  _ops;
  const SWP_LIFENESS&   _ln;
  
  MEM_POOL         *_m;
  LOOP_DESCR       *_loop_descr;
  BB               *_body;
  BB              **_prolog;
  BB              **_kernel;
  BB              **_epilog;
  BB               *_old_prolog;
  BB               *_old_epilog;
  INT               _num_prologs;
  INT               _num_kernels;
  INT               _num_epilogs;
  INT               _ii;
  SRCPOS            _srcpos;
  SWP_TN2TN_MAP    *_prologue_tns;
  SWP_TN2TN_MAP     _kernel_tns;
  SWP_TN2TN_MAP     _epilogue_tns;
  SWP_BP2BOOL_MAP   _bp_used;

  void
  Init (void);
  void
  Create_Prologue_Kernel_Epilogue_BBs (void);
  void
  Update_CFG (void);
  void
  Generate_Glue_Copies_For_Loop_Invariants (void);
  void
  Fill_Prologues (void);
  void
  Fill_Kernels (void);
  void
  Fill_Epilogues (void);
  void
  Create_Kernel_Jump_Blocks (void);
  void
  Calculate_Code_Size (void);
  void
  Attach_Notes (void);
  TN *
  Create_Prologue_Backpatch_Copy_For_TN (TN *kernel_tn, 
                                         INT omega, 
                                         INT num); 
  TN *
  Create_Kernel_Backpatch_Copy_For_TN (TN *kernel_tn,
                                       INT omega);
  
  TN *
  Create_Epilogue_Backpatch_Copy_For_TN (TN *kernel_tn, 
                                         INT omega); 
  void
  Process_Kernel_OP (const SWP_OP& swp_op);
  void
  Add_Copy (TN *target, TN *source, BB *bb, SWP_INSERT_TYPE type);
  void
  Add_Copy (TN *target, TN *source, OPS *ops);
  void
  Add_Glue_Copy (TN *target, TN *source, BB *bb, SWP_INSERT_TYPE type);
  void 
  Add_Glue_Copy (TN *target, TN *source, OPS *ops);
  OP *
  Duplicate_Prologue_OP (const SWP_OP& swp_op, INT num);
  OP *
  Duplicate_Epilogue_OP (const SWP_OP& swp_op, INT num);
  BOOL
  Rename_Prologue_TN_to_Kernel_TN (TN *kernel_tn);

  public:
  SWP_CODE_GEN (SWP_LOOP& swp_loop, 
                const SWP_OP_VECTOR& ops,
                const SWP_MVE& mve,
                const SWP_LIFENESS& ln,
                MEM_POOL *m);
  ~SWP_CODE_GEN (void) { return; }

  void Print (FILE *f = stderr) const;
  void Verify (void) const;
};

#endif /* cg_swp_code_gen_INCLUDED */

