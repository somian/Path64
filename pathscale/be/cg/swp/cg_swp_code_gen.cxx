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
* \file cg_swp_code_gen.cxx                                            *
* \brief Implementation of the SWP Code Generation                     *
***********************************************************************/

/* Basic compiler include files */
#include "defs.h"
#include "mempool.h"
#include "cxx_memory.h"
#include "ti_res_count.h"
#include "targ_proc_properties.h"

/* Code Generator (CG) related include files */
#include "tn.h"
#include "bb.h"
#include "cg_loop.h"
#include "cgexp.h"
#include "freq.h"
#include "gra_live.h"

/* Software Pipelinig (SWP) related include files */
#include "cg_swp_op_vector.h"
#include "cg_swp_options.h"
#include "cg_swp_mve.h"
#include "cg_swp_note.h"
#include "cg_swp_code_gen.h"

/* Standard Template Library (STL) related include files */
#include <algorithm>

using namespace std;



SWP_CODE_GEN::SWP_CODE_GEN (SWP_LOOP& swp_loop,
                            const SWP_OP_VECTOR& ops,
                            const SWP_MVE& mve,
                            const SWP_LIFENESS& ln,
                            MEM_POOL *m) :
  _swp_loop     (swp_loop),
  _ops          (ops),
  _mve          (mve),
  _ln           (ln),
  _m            (m),
  _loop_descr   (swp_loop.loop_descr),
  _body         (LOOP_DESCR_loophead (swp_loop.loop_descr)),
  _prolog       (0),
  _kernel       (0),
  _epilog       (0),
  _old_prolog   (0),
  _old_epilog   (0),
  _num_prologs  (swp_loop.ps),
  _num_kernels  (swp_loop.kernel_unrollings),
  _num_epilogs  (swp_loop.ps + swp_loop.kernel_unrollings),
  _ii           (swp_loop.ii),
  _srcpos       (swp_loop.srcpos),
  _prologue_tns (0),
  _kernel_tns   (less <TN *> (), SWP_TN2TN_ALLOC (m)),
  _epilogue_tns (less <TN *> (), SWP_TN2TN_ALLOC (m)),
  _bp_used      (less <SWP_BP *> (), SWP_BP2BOOL_ALLOC (m))
{
  Init ();
  Create_Prologue_Kernel_Epilogue_BBs ();
  Update_CFG ();
  Generate_Glue_Copies_For_Loop_Invariants ();
  Fill_Prologues ();
  Fill_Kernels ();
  Fill_Epilogues ();
  Create_Kernel_Jump_Blocks ();
  Calculate_Code_Size ();
  Attach_Notes ();
  CG_LOOP_Recompute_Liveness (_loop_descr);

  return;
}


void
SWP_CODE_GEN::Init (void) {
  if (_num_prologs > 0) {
    _prologue_tns = CXX_NEW_ARRAY (SWP_TN2TN_MAP, _num_prologs, _m);
    for (INT i = 0; i < _num_prologs; ++i) {
      _prologue_tns[i] = SWP_TN2TN_MAP (less <TN *> (), 
                                        SWP_TN2TN_ALLOC (_m));
    }
  }

  SWP_BP *bp;
  for (bp = CG_LOOP_Backpatch_First (CG_LOOP_prolog, NULL);
       bp != 0;
       bp = CG_LOOP_Backpatch_Next (bp))
  {
    _bp_used[bp] = FALSE;
  }
  
  return;
}


/*******************************************************************//**
* \brief Create Prologue, Kernel and Epilogue BBs                      *
*                                                                      *
* \return void                                                         *
***********************************************************************/
void
SWP_CODE_GEN::Create_Prologue_Kernel_Epilogue_BBs (void) {
  /*********************************************************************
  * Create Prologue, Kernel and Epilogue BBs                           *
  *********************************************************************/
  FmtAssert (_num_kernels > 0, 
             ("SWP_CODE_GEN: Invalid number of kernels"));
  
  FmtAssert (_num_epilogs > 0, 
             ("SWP_CODE_GEN: Invalid number of epilogs"));

  
  _prolog = CXX_NEW_ARRAY (BB *, _num_prologs, _m);
  _kernel = CXX_NEW_ARRAY (BB *, _num_kernels, _m);
  _epilog = CXX_NEW_ARRAY (BB *, _num_epilogs, _m);
  

  /* We need for every prologue stage a BB, because we have early exit
   * checks after every stage and jump to a designated epilogue BB.
   */
  /* Create prologue BBs */
  for (INT i = 0; i < _num_prologs; ++i) {
    _prolog[i] = Gen_BB ();
    
    if (FREQ_Frequencies_Computed () || BB_freq_fb_based (_body)) {
      BB_freq (_prolog[i]) = BB_freq (_body);
    }

    if (BB_freq_fb_based (_body)) {
      Set_BB_freq_fb_based (_prolog [i]);
    }

    Set_BB_unrollings (_prolog[i], BB_unrollings (_body));
  }
  
  /* Create kernel BBs */
  for (INT i = 0; i < _num_kernels; ++i) {
    _kernel[i] = Gen_BB ();
    
    Set_BB_scheduled  (_kernel[i]);
    Set_BB_reg_alloc (_kernel[i]);
    Set_BB_swp_kernel (_kernel[i]);

    if (FREQ_Frequencies_Computed () || BB_freq_fb_based (_body)) {
      BB_freq (_kernel[i]) = BB_freq (_body);
    }

    if (BB_freq_fb_based (_body)) {
      Set_BB_freq_fb_based (_kernel [i]);
    }
    
    Set_BB_unrollings (_kernel[i], BB_unrollings (_body));
  }
  

  /* We need for every prologue stage a different epilogue and for 
   * every unrolled kernel version too. We won't reuse the already
   * existing epilogue BB, because we have to jump to it from our
   * several epilogue versions.
   */

  /* Create epilogue BBs */
  for (INT i = 0; i < _num_epilogs; ++i) {
    _epilog[i] = Gen_BB ();

    if (FREQ_Frequencies_Computed () || BB_freq_fb_based (_body)) {
      BB_freq (_epilog[i]) = BB_freq (_body);
    }

    if (BB_freq_fb_based (_body)) {
      Set_BB_freq_fb_based (_prolog [i]);
    }
    
    Set_BB_unrollings (_epilog[i], BB_unrollings (_body));
  }
  /********************************************************************/

  /*********************************************************************
  * Update CG LOOP related information                                 *
  *********************************************************************/
  /* Save old prologue and epilogue */
  _old_prolog = CG_LOOP_prolog;
  _old_epilog = CG_LOOP_epilog;


  /* Copy annotation from original loop body to the first MVE kernel */
  ANNOTATION *annot = ANNOT_Get (BB_annotations (_body), 
                                 ANNOT_LOOPINFO);
  LOOPINFO *info    = ANNOT_loopinfo (annot);
  BB_Add_Annotation (_kernel[0], ANNOT_LOOPINFO, info);
  
  /* Retarget the loop entrance to our unrolled MVE kernel and remove
   * the old loop body from the LOOP_DESCR
   */
  LOOP_DESCR_Retarget_Loop_Entrances (_loop_descr, _kernel[0]);
  LOOP_DESCR_Delete_BB (_loop_descr, _body);
  BB_next (_body) = BB_prev (_body) = 0;
  Unlink_Pred_Succ (_old_prolog, _body);
  Unlink_Pred_Succ (_body, _old_epilog);

  /* Add the new kernel BB(s) to the LOOP_DESCR */
  for (INT i = 0; i < _num_kernels; ++i) {
    LOOP_DESCR_Add_BB (_loop_descr, _kernel[i]);  
  }
  /********************************************************************/

  /*********************************************************************
  * Chain Prologue, Kernel and Epilogue BBs                            *
  *********************************************************************/
  /* Chain prologue BBs (if any) */
  for (INT i = 0; i < _num_prologs - 1; ++i) {
    Chain_BBs (_prolog[i], _prolog[i+1]);
  }

  /* Chain the old prologue to the first prologue or directly to the
   * kernel if there aren't any prologues.
   */
  if (_num_prologs > 0) {
    /* Chain to first prologue */
    Chain_BBs (_old_prolog, _prolog[0]);
    
    /* Chain the last prologue to the first MVE kernel */
    Chain_BBs (_prolog[_num_prologs-1], _kernel[0]);
  }
  else {
    /* Directly chain the old prologue to the first MVE kernel */
    Chain_BBs (_old_prolog, _kernel[0]);
  }

  /* Update CG_LOOP_prolog */
  if (_num_prologs > 0) {
    CG_LOOP_prolog = _prolog[_num_prologs-1];
  }

  /* Chain MVE kernels */
  for (INT i = 0; i < _num_kernels - 1; ++i) {
    Chain_BBs (_kernel[i], _kernel[i+1]);
  }
  Chain_BBs (_kernel[_num_kernels-1], _epilog[0]);

  /* Chain epilog BBs */
  for (INT i = 0; i < _num_epilogs - 1; ++i) {
    Chain_BBs (_epilog[i], _epilog[i+1]);
  }
  Chain_BBs (_epilog[_num_epilogs-1], _old_epilog);

  /* Update CG_LOOP_epilog */
  CG_LOOP_epilog = _epilog[0];
  /********************************************************************/

  return;
}


/*******************************************************************//**
* \brief Update CFG                                                    *
*                                                                      *
* Example CFG:                                                         *
* -------------------------------------------------------------------- *
* Prolog  : BB0                                                        *
*            |                                                         *
* Prolog 0: BB1--------\                                               *
*            |         |                                               *
* Prolog 1: BB2------\ |                                               *
*            |       | |                                               *
* Kernel 0: BB3----\ | |                                               *
*            |     | | |                                               *
* Kernel 1: BB4    | | |                                               *
*            |     | | |                                               *
* Epilog 0: BB5----|-|-|-\                                             *
*                  | | | |                                             *
* Epilog 1: BB6<---/ | | |                                             *
*            |       | | |                                             *
*            \-------|-|-|                                             *
*                    | | |                                             *
* Epilog 3: BB7<-----/ | |                                             *
*            |         | |                                             *
*            \---------|-|                                             *
*                      | |                                             *
* Epilog 4: BB8<-------/ |                                             *
*            |           |                                             *
* Epilog  : BB9<---------/                                             *
*                                                                      *
* \return void                                                         *
***********************************************************************/
void
SWP_CODE_GEN::Update_CFG (void) {
  ANNOTATION *annot = ANNOT_Get (BB_annotations (_body), 
                                 ANNOT_LOOPINFO);
  LOOPINFO *info    = ANNOT_loopinfo (annot);
  WN *wn            = LOOPINFO_wn (info);
  
  /* Link all the prologs BBs together down to the first kernel */
  Unlink_Pred_Succ (_old_prolog, _kernel[0]);
  Unlink_Pred_Succ (_kernel[0], _old_epilog);
  
  INT loop_count = WN_loop_trip_est (wn) ? WN_loop_trip_est (wn) : 100;
  float prob = static_cast<float> (_num_prologs + _num_kernels) / 
               loop_count;
  
  if (_num_prologs > 0) {
    Link_Pred_Succ_with_Prob (_old_prolog, _prolog[0], 1.0f);
    
    Link_Pred_Succ_with_Prob (_prolog[_num_prologs-1], 
                              _kernel[0], 
                              1.0f - prob);
  }
  else {
    Link_Pred_Succ_with_Prob (_old_prolog, _kernel[0], 1.0f); 
  }

  for (INT i = 0; i < _num_prologs - 1; ++i) {
    Link_Pred_Succ_with_Prob (_prolog[i], _prolog[i+1], 1.0f - prob);
  }

  /* Now connect every prologue BB with its dedicated epilogue BB */
  for (INT i = 0; i < _num_prologs; ++i) {
    Link_Pred_Succ_with_Prob (_prolog[i], 
                              _epilog[_num_epilogs-i-1], 
                              prob);
  }

  /* Conncet every kernel BB */
  for (INT i = 0; i < _num_kernels - 1; ++i) {
    Link_Pred_Succ_with_Prob (_kernel[i], _kernel[i+1], 1.0f - prob); 
  }
  Link_Pred_Succ_with_Prob (_kernel[_num_kernels-1], 
                            _kernel[0], 
                            1.0f - prob);
  
  /* Connect every kernel BB with its dedicated epilogue BB */
  for (INT i = 0; i < _num_kernels; ++i) {
    Link_Pred_Succ_with_Prob (_kernel[i], 
                              _epilog[_num_epilogs-_num_prologs-i-1], 
                              prob);
  }

  /* Finally we have to connect all epilogues to the old epilogue
   */
  for (INT i = 0; i < _num_epilogs; ++i) {
    Link_Pred_Succ_with_Prob (_epilog[i], _old_epilog, 1.0f);
  }

  return;
}


void
SWP_CODE_GEN::Generate_Glue_Copies_For_Loop_Invariants (void) {
  /* PathScale Bug 14709
   * We always need to make a copy of loop invariant TNs to garantee
   * that they are not assigned a different register by a following
   * SWP phase.
   */
  const TN_SET *invariant_tns = _ln.Get_Invariant_TNs ();
  TN *kernel_tn;
  FOR_ALL_TN_SET_members (invariant_tns, kernel_tn) {
    TN *original_tn = _mve.Get_Original_TN (kernel_tn);
    Add_Glue_Copy (kernel_tn, original_tn, _old_prolog, SWP_Append);
  }

  return;
}


/*******************************************************************//**
* \brief Fill Prologue BBs by kernel peeling                           *
*                                                                      *
* \return void                                                         *
***********************************************************************/
void
SWP_CODE_GEN::Fill_Prologues (void) {
  for (INT pro = 0; pro < _num_prologs; ++pro) {
    INT max_stage = min (pro, _swp_loop.sc);
    INT version   = (_num_kernels - 1) - (_num_prologs - 1) + pro;
    
    version = version % _num_kernels;
    while (version < 0) {
      version += _num_kernels;
    }

    /* Copy the TN prologue mapping */
    if (pro != 0) {
      _prologue_tns[pro] = _prologue_tns[pro - 1];
    }

    SWP_OP_VECTOR::size_type idx;
    FOR_ALL_SWP_OP_VECTOR_IDXs (_ops, idx) {
      if (   (   (_ops(idx).Get_Stage ()     <= max_stage)
              && (_ops(idx).Get_Unrolling () == version))
         )
      {
        OP *new_op = Duplicate_Prologue_OP (_ops(idx), pro);
        OP_scycle (new_op) = _ops(idx).Get_Start_Cycle ();
        BB_Append_Op (_prolog[pro], new_op);
      }
    } /* FOR_ALL_SWP_OP_VECTOR_IDXs (_ops, idx) */
   
    /* Update the branch instruction to jump to the dedicated
     * epilogue BB.
     */
    Negate_Logif_BB (_prolog[pro]);
    OP *br = BB_branch_op (_prolog[pro]);      
    TN *tn = Gen_Label_TN (
               Gen_Label_For_BB (_epilog[_num_epilogs - pro - 1]), 
               0);
    Set_OP_opnd (br,
                 Branch_Target_Operand (br),
                 tn);
  } /* for (INT pro = 0; pro < _num_prologs; ++pro) */

  return;
}


/*******************************************************************//**
* \brief Fill Kernel BBs by copying the MVE kernel                     *
*                                                                      *
* \return void                                                         *
***********************************************************************/
void
SWP_CODE_GEN::Fill_Kernels (void) {
  SWP_OP_VECTOR::size_type i;
  FOR_ALL_SWP_OP_VECTOR_IDXs (_ops, i) {
    if (i == _ops.Get_Start_Idx ()) continue;
    if (i == _ops.Get_Stop_Idx ()) continue;

    OP *op = _ops(i).Get_OP ();

    Process_Kernel_OP (_ops(i));

    if (OP_br (op)) {
      TN *tn;
      if (_ops(i).Get_Unrolling () < _num_kernels - 1) {
        INT idx = _num_epilogs - _num_prologs - 
                  _ops(i).Get_Unrolling () - 1;
        FmtAssert ((idx >= 0) && (idx < _num_epilogs),
                   ("SWP_CODE_GEN: Invalid epilog index!"));
        
        LABEL_IDX label = Gen_Label_For_BB (_epilog[idx]);
        tn = Gen_Label_TN (label, 0);
        
        TOP  old_top = OP_code(op);
        TOP  new_top = CGTARG_Invert(old_top);

        Is_True (new_top != TOP_UNDEFINED, 
	         ("unable to negate branch: %s", TOP_Name(old_top)));
        OP_Change_Opcode (op, new_top);
      }
      else {
        LABEL_IDX label = Gen_Label_For_BB (_kernel[0]);
        tn = Gen_Label_TN (label, 0);
      }

      Set_OP_opnd (op,
                   Branch_Target_Operand (op),
                   tn);
    }
    
    OP_scycle (op) = _ops(i).Get_Start_Cycle ();
    BB_Append_Op (_kernel[_ops(i).Get_Unrolling ()], op);
  }

  return;
}


/*******************************************************************//**
* \brief Fill Epilogue BBs by kernel peeling                           *
*                                                                      *
* \return void                                                         *
***********************************************************************/
void
SWP_CODE_GEN::Fill_Epilogues (void) {
  for (INT epi = 0; epi < _num_epilogs; ++epi) {
    _epilogue_tns.clear ();

    for (INT j = 0; j < _swp_loop.es; ++j) {
      INT min_stage = (j + 1) % _swp_loop.sc;
      INT max_stage = min (_swp_loop.sc - 1, j + 1 + (_num_epilogs - 1) - epi);
      INT version   = j - epi + _num_kernels;
      
      version = version % _num_kernels;
      while (version < 0) {
        version += _num_kernels;
      }
      
      SWP_OP_VECTOR::size_type i;
      FOR_ALL_SWP_OP_VECTOR_IDXs (_ops, i) {
        if (   (!OP_br (_ops(i).Get_OP ()))
            && (   (   (_ops(i).Get_Stage () >= min_stage)
                    && (_ops(i).Get_Stage () <= max_stage))
  /* PathScale Bug 15000
   * After offset adjustment, we have to garantee that there is always
   * an pointer update in every stage of the epilogue, because
   * the adjusted offsets of the loads and stores depend on this.
   * The update might be not necessary, because there might be no load
   * or store using it, but we don't care. EBO will clean that up for us
   * later.
   */
                || (   (_ops(i).Is_Pointer_Update ())
                    && (_ops(i).Get_Stage () <= max_stage)
                   )
  /* PathScale Bug 15000 */
               )
            && (_ops(i).Get_Unrolling () == version))
        {
          OP *op = Duplicate_Epilogue_OP (_ops(i), epi);
          OP_scycle (op) = _ops(i).Get_Start_Cycle () + j * _ii;
          BB_Append_Op (_epilog[epi], op);
        }
      } /* FOR_ALL_SWP_OP_VECTOR_IDXs (_ops, i) */
    
    } /* for (INT j = 0; j < _swp_loop.es; ++j) */

    
    /* Create copy code for epilogue TNs */
    CG_LOOP_BACKPATCH *bp;
    for (bp = CG_LOOP_Backpatch_First (CG_LOOP_epilog, NULL); 
         bp != 0; 
         bp = CG_LOOP_Backpatch_Next (bp))
    {
      INT  omega     = CG_LOOP_BACKPATCH_omega (bp);
      TN  *target_tn = CG_LOOP_BACKPATCH_non_body_tn (bp);
      TN  *source_tn = CG_LOOP_BACKPATCH_body_tn (bp);
      
      INT version = (_num_kernels - 1) - epi;
  
      version = version % _num_kernels;
      while (version < 0) {
        version += _num_kernels;
      }
      
      TN *kernel_tn = _mve.Get_Renamed_TN (source_tn, omega, version);
        
      /* PathScale Bug 14682:
       * Every epilogue uses new local TNs. The following code
       * does a map lookup to check if our TN has been renamed and
       * updates the TN if necessary.
       */
      SWP_TN2TN_MAP::const_iterator itr;
      itr = _epilogue_tns.find (kernel_tn);
      if (itr != _epilogue_tns.end ()) {
        source_tn = itr->second;
        Add_Copy (target_tn, source_tn, _epilog[epi], SWP_Append);
      }
      else if (epi >= _num_kernels) {
        INT pro = _num_epilogs -1 - epi;
        SWP_TN2TN_MAP::const_iterator itr;
        itr = _prologue_tns[pro].find (kernel_tn);
        if (itr != _prologue_tns[pro].end ()) {
          source_tn = itr->second;
          Add_Copy (target_tn, source_tn, _epilog[epi], SWP_Append);
        }
        else {
          Add_Copy (target_tn, kernel_tn, _epilog[epi], SWP_Append); 
        }
      }
      else {
        Add_Glue_Copy (target_tn, kernel_tn, _epilog[epi], SWP_Append);
      }
    }
    
    if (epi < _num_epilogs - 1) {
      OPS ops = OPS_EMPTY;
      Exp_Local_Jump (_old_epilog, 0, &ops);
      BB_Append_Ops (_epilog[epi], &ops);
    }
  }

  return;
}


void
SWP_CODE_GEN::Create_Kernel_Jump_Blocks (void) {
  if (   !SWP_Options.Enable_Jump_Block
      || !CGTARG_Branch_Always_Predicted_Taken ()
     ) 
  {
    return;
  }

  for (INT i = 0; i < _num_kernels - 1; ++i) {
    BB *ep = _epilog[_num_epilogs-_num_prologs-i-1];
    BB *bb = Gen_And_Insert_BB_After (_kernel[i]);
    

    Negate_Logif_BB (_kernel[i]);
    

    BBLIST *succs = BB_Find_Succ (_kernel[i], ep);
    float  prob   =  BBLIST_prob (succs);
    Unlink_Pred_Succ (_kernel[i], ep);
    Link_Pred_Succ_with_Prob (_kernel[i], bb, prob);
    
    OP *br          = BB_branch_op (_kernel[i]);
    LABEL_IDX label = Gen_Label_For_BB (_kernel[i+1]);
    TN *tn          = Gen_Label_TN (label, 0);

    Set_OP_opnd (br, Branch_Target_Operand (br), tn);


    Add_Goto (bb, _epilog[_num_epilogs-_num_prologs-i-1]);
    LOOP_DESCR_Add_BB (_loop_descr, bb);
  }

  return;
}
 

void
SWP_CODE_GEN::Calculate_Code_Size (void) {
  _swp_loop.size_original = BB_length (_body);
  for (INT i = 0; i < _num_prologs; ++i) {
    OP *op;
    FOR_ALL_BB_OPs (_prolog[i], op) {
      if (!OP_glue (op)) _swp_loop.size++;
    }
  }
  
  for (INT i = 0; i < _num_kernels; ++i) {
    _swp_loop.size += BB_length (_kernel[i]);
  }
  
  for (INT i = 0; i < _num_epilogs; ++i) {
    OP *op;
    FOR_ALL_BB_OPs (_epilog[i], op) {
      if (!OP_glue (op)) _swp_loop.size++;
    }
  }

  return;
}


void
SWP_CODE_GEN::Attach_Notes (void) {
  for (INT i = 0; i < _num_prologs; ++i) {
    SWP_Add_Prologue_Note (_prolog[i], _swp_loop, i);
  }

  SWP_Add_Loop_Note (_kernel[0], _swp_loop);

  for (INT i = 0; i < _num_kernels; ++i) {
    SWP_Add_Kernel_Note (_kernel[i], _swp_loop, i);
  }

  for (INT i = 0; i < _num_epilogs; ++i) {
    SWP_Add_Epilogue_Note (_epilog[i], _swp_loop, i);
  }

  return;
}


void 
SWP_CODE_GEN::Add_Copy (TN *target, 
                        TN *source,
                        BB *bb,
                        SWP_INSERT_TYPE insert) 
{
  OPS ops = OPS_EMPTY;
  Exp_COPY (target, source, &ops);
  
  if (insert == SWP_Append) {
    BB_Append_Ops (bb, &ops);
  }
  else {
    BB_Prepend_Ops (bb, &ops);
  }

  return;
}


void 
SWP_CODE_GEN::Add_Copy (TN *target, TN *source, OPS *ops) {
  Exp_COPY (target, source, ops);

  return;
}


void 
SWP_CODE_GEN::Add_Glue_Copy (TN *target, 
                             TN *source,
                             BB *bb,
                             SWP_INSERT_TYPE insert) 
{
  OPS ops = OPS_EMPTY;
  Exp_COPY (target, source, &ops);
  
  OP *op;
  FOR_ALL_OPS_OPs (&ops, op) {
    Set_OP_glue (op);
  }
  
  if (insert == SWP_Append) {
    BB_Append_Ops (bb, &ops);
  }
  else {
    BB_Prepend_Ops (bb, &ops);
  }

  return;
}


void 
SWP_CODE_GEN::Add_Glue_Copy (TN *target, TN *source, OPS *ops) {
  OPS glue_copy = OPS_EMPTY;
  Exp_COPY (target, source, &glue_copy);
  
  OP *op;
  FOR_ALL_OPS_OPs (&glue_copy, op) {
    Set_OP_glue (op);
  }
  
  OPS_Append_Ops (ops, &glue_copy);

  return;
}


/*******************************************************************//**
* \brief Duplicate a prologue OP                                       *
*                                                                      *
* Duplicate a given prologue OP, rename operands and results, and      *
* create (glue) copies from backpatches if necessary.                  *
*                                                                      *
* \param[in] swp_op                                                    *
* \param[in] num                                                       *
* \return OP *                                                         *
***********************************************************************/
OP *
SWP_CODE_GEN::Duplicate_Prologue_OP (const SWP_OP& swp_op, INT num) {
  OP *op     = swp_op.Get_OP ();
  OP *new_op = Dup_OP (op);

  /*********************************************************************
  * 1.) Rename all operands                                            *
  *********************************************************************/
  UINT8 opr;
  FOR_ALL_OP_OPNDs (new_op, opr) {
    TN *tn = OP_opnd (new_op, opr);

    /* We can skip this step if the TN is:
     * - not a register (constants don't need renaming)
     * - a dedicated register (can't perform renaming for dedicated 
     *   registers)
     * - loop invariant (loop invariants don't need renaming)
     */
    if (!TN_is_register (tn)) continue;
    if (TN_is_dedicated (tn)) continue;
    if (!_ln.Is_Loop_Variant (tn)) continue;


    SWP_TN2TN_MAP::const_iterator itr;
    itr = _prologue_tns[num].find (tn);
    if (itr != _prologue_tns[num].end ()) {
      /* The TN has already been defined in one of the prologues before
       * or in this prologue.
       */
      Set_OP_opnd (new_op, opr, itr->second);
    }
    else {
      /* The TN hasn't been defined yet. There must be a backpatch, 
       * which defines this TN.
       */
      INT iteration  = num - swp_op.Get_Stage ();
      INT omega      = swp_op.Get_Original_Omega (opr) - iteration;
      
      TN *new_tn = 
        Create_Prologue_Backpatch_Copy_For_TN (tn, omega, num);
      if (new_tn != 0) {
        Set_OP_opnd (new_op, opr, new_tn);
      }
      else {
        /* Something went wrong. The TN:
         *  - is not loop invariant
         *  - has not been defined before
         *  - doesn't have a backpatch
         */
        Fail_FmtAssertion ("SWP_CODE_GEN: TN with unknown origin!");
      }
    }
  } /* FOR_ALL_OP_OPNDs (new_op, opr) */


  #if 0
  /*********************************************************************
  * 2.) Treat conditional defined results as operands                  *
  *********************************************************************/
  if (OP_cond_def (new_op)) {
    for (INT opr = 0; opr < OP_results (new_op); ++opr) {
      TN *tn = OP_result (new_op, opr);
      TN *new_tn; 

      if (TN_is_dedicated (tn)) continue;

      /* Check if it has been defined in one of the prologues before or
       * in this prologue.
       */
      SWP_TN2TN_MAP::const_iterator itr;
      itr = _prologue_tns[num].find (tn);
      if (itr != _prologue_tns[num].end ()) { 
        continue;
      }

      /* Check if there is a backpatch which defines this TN (not very
       * likely, but possible)
       */
      INT omega = num - swp_op.Get_Stage ();
      new_tn = Create_Prologue_Backpatch_Copy_For_TN (tn, omega, num);
      if (new_tn) {
        continue; 
      }

      /* There is no definition what so ever. In this case we just use
       * the original TN.
       */
      new_tn = _mve.Get_Original_TN (tn);
      _prologue_tns[num][tn] = new_tn;
    }
  }
  #endif

  /*********************************************************************
  * 3.) Rename all results                                             *
  *********************************************************************/ 
  UINT8 res;
  FOR_ALL_OP_RESULTs (new_op, res) {
    TN *tn = OP_result (new_op, res);
    
    if (TN_is_dedicated (tn)) continue;

    /* If an OP is conditional defined, then the result TN is
     * allowed to be the same TN as of an preceding OP.
     * i.e.
     * TN100 <- op1 ...
     * TN200 <- op2 TN100 ... (cond.)
     *
     * will be converted to
     *
     * TN500 <- op1 ...
     * TN500 <- op2 TN500 ... (cond.)
     */
    TN *new_tn;
    if (OP_cond_def (new_op)) {
      FmtAssert (OP_same_res (new_op),
                  ("SWP_CODE_GEN: OP must have same_res property."));
      UINT8 idx = OP_find_opnd_use (new_op, OU_same_res); 
      new_tn = OP_opnd (new_op, idx);
    }
    else {
      /* For every other op we just create a new result TN */
      new_tn = Dup_TN (tn);
    }

    /* PathScale Bug 14740 Workaround *********************************/
    #ifdef TARG_MIPS
    if (TN_register_class (tn) == ISA_REGISTER_CLASS_fcc) {
      Set_TN_register (new_tn, TN_register (tn));
    }
    #endif
    /******************************************************************/
    
    _prologue_tns[num][tn] = new_tn;
    Set_OP_result (new_op, res, new_tn);
  } /* FOR_ALL_OP_RESULTs (new_op, res) */

  return new_op;
}


void
SWP_CODE_GEN::Process_Kernel_OP (const SWP_OP& swp_op) {
  OP *op = swp_op.Get_OP ();

  /*********************************************************************
  * 1.) Check if all operands have definitions.                        *
  *********************************************************************/
  UINT8 opr;
  FOR_ALL_OP_OPNDs (op, opr) {
    TN *tn = OP_opnd (op, opr);

    if (!TN_is_register (tn)) continue;
    if (TN_is_dedicated (tn)) continue;
    if (!_ln.Is_Loop_Variant (tn)) continue;

    SWP_TN2TN_MAP::const_iterator itr;
    itr = _kernel_tns.find (tn);
    /* If the TN has been defined in one of the kernels, we can stop */
    if (itr != _kernel_tns.end ()) continue;

    /* PathScale bug 14740
     * Instead of creating copies from the prologue TN to the kernel TN,
     * we now do a reverse search for the last definition of the TN in 
     * the prologue and rename all occurences of the prologue TN to the
     * kernel TN.
     */
    if (Rename_Prologue_TN_to_Kernel_TN (tn)) {
      _kernel_tns[tn] = tn;
      continue;
    }

    INT iteration = _num_prologs - 
                    swp_op.Get_Stage () +
                    swp_op.Get_Unrolling ();

    INT omega     = swp_op.Get_Original_Omega (opr) - iteration;

    if (Create_Kernel_Backpatch_Copy_For_TN (tn, omega)) {
      continue;
    }
    else {
      Fail_FmtAssertion ("SWP_CODE_GEN: Unknown TN origin");
    }
  }

  #if 0
  /*********************************************************************
  * 2.) Check if a conditional defined result has a definition         *
  *********************************************************************/
  if (OP_cond_def (op)) {
    for (INT opr = 0; opr < OP_results (op); ++opr) {
      TN *tn = OP_result (op, opr);

      SWP_TN2TN_MAP::const_iterator itr;
      itr = _kernel_tns.find (tn);
      if (itr != _kernel_tns.end ()) continue;

#if 1
      /* PathScale bug 14740 */
      if (Rename_Prologue_TN_to_Kernel_TN (tn, ops)) {
        _kernel_tns[tn] = tn;
        continue;
      }
#else  // Restore pre-14740 fix code until fix is fixed.
      if (_num_prologs > 0) {
	itr = _prologue_tns[_num_prologs - 1].find (tn);
	if (itr != _prologue_tns[_num_prologs - 1].end ()) {
	  Add_Glue_Copy (tn, itr->second, ops);
	  _kernel_tns[tn] = tn;
	  continue;
	}
      }
#endif

      INT omega = _num_prologs - 
                  swp_op.Get_Stage () +
                  swp_op.Get_Unrolling ();

      if (Create_Kernel_Backpatch_Copy_For_TN (tn, omega, ops)) {
        continue;
      }

      TN *new_tn = _mve.Get_Original_TN (tn);
      Add_Glue_Copy (tn, new_tn, ops);
      _kernel_tns[tn] = tn;
    }
  }
  #endif


  /*********************************************************************
  * 3.) Mark results as defined                                        *
  *********************************************************************/ 
  UINT8 res;
  FOR_ALL_OP_RESULTs (op, res) {
    TN *tn = OP_result (op, res);

    if (TN_is_dedicated (tn)) continue;
    
    _kernel_tns[tn] = tn;
  } 

  return;
}


OP *
SWP_CODE_GEN::Duplicate_Epilogue_OP (const SWP_OP& swp_op, INT num) {  
  OP *op     = swp_op.Get_OP ();
  OP *new_op = Dup_OP (op);
  
  /*********************************************************************
  * 1.) Rename all operands                                            *
  *********************************************************************/
  UINT8 opr;
  FOR_ALL_OP_OPNDs (new_op, opr) {
    TN *tn = OP_opnd (new_op, opr);

    if (!TN_is_register (tn)) continue;
    if (TN_is_dedicated (tn)) continue;
    if (!_ln.Is_Loop_Variant (tn)) continue;

    /* Has the TN already been defined in this epilogue? */
    SWP_TN2TN_MAP::const_iterator itr;
    itr = _epilogue_tns.find (tn);
    if (itr != _epilogue_tns.end ()) {
      Set_OP_opnd (new_op, opr, itr->second);
      continue;
    }

    /* This handels kernel related epilogues */
    if (num < _num_kernels) {
      /* keep kernel TN */
      continue;
    }

    /* Handle prologue related epilogues */
    if (_num_prologs > 0) {
      /* Check if the TN has been defined in the corresponding 
       * prologue. 
       */
      INT pro = (_num_epilogs - 1) - num;
      itr = _prologue_tns[pro].find (tn);
      if (itr != _prologue_tns[pro].end ()) {
        Set_OP_opnd (new_op, opr, itr->second);
        continue;
      }
      
      INT iteration  = pro - swp_op.Get_Stage ();
      INT omega      = swp_op.Get_Original_Omega (opr) - iteration;
      
      TN *new_tn = Create_Epilogue_Backpatch_Copy_For_TN (tn, omega);
      if (new_tn != 0) {
        Set_OP_opnd (new_op, opr, new_tn);
        continue;
      }
    }

    /* Something went wrong. The TN:
     *  - is not loop invariant
     *  - has not been defined before
     */
    Fail_FmtAssertion ("SWP_CODE_GEN: TN with unknown origin!");
  } /* FOR_ALL_OP_OPNDs (new_op, opr) */


  #if 0
  /*********************************************************************
  * 2.) Treat conditional defined results as operands                  *
  *********************************************************************/
  if (OP_cond_def (op)) {
    for (INT opr = 0; opr < OP_results (new_op); ++opr) {
      TN *tn = OP_result (new_op, opr);

      if (TN_is_dedicated (tn)) continue;

      /* Has the TN already been defined in this epilogue? */
      SWP_TN2TN_MAP::const_iterator itr;
      itr = _epilogue_tns.find (tn);
      if (itr != _epilogue_tns.end ()) {
        //Set_OP_opnd (new_op, opr, itr->second);
        continue;
      }

      /* This handels kernel related epilogues */
      if (num < _num_kernels) {
        /* keep kernel TN */
        _epilogue_tns[tn] = tn;

        continue;
      }

      /* Handle prologue related epilogues */
      if (_num_prologs > 0) {
        /* Check if the TN has been defined in the corresponding 
         * prologue. 
         */
        INT pro = (_num_epilogs - 1) - num;
        itr = _prologue_tns[pro].find (tn);
        if (itr != _prologue_tns[pro].end ()) {
          //Set_OP_opnd (new_op, opr, itr->second);
          _epilogue_tns[tn] = itr->second;
          continue;
        }
      
        INT omega = pro - swp_op.Get_Stage ();
      
        TN *new_tn = 
          Create_Prologue_Backpatch_Copy_For_TN (tn, omega, num);
        if (new_tn != 0) {
          _epilogue_tns[tn] = new_tn;
          //Set_OP_opnd (new_op, opr, new_tn);
          continue;
        }

        new_tn = _mve.Get_Original_TN (tn);
        _epilogue_tns[tn] = new_tn;
        continue;
      }
    
      /* Something went wrong. The TN:
       *  - is not loop invariant
       *  - has not been defined before
       */
      Fail_FmtAssertion ("SWP_CODE_GEN: TN with unknown origin!");
    }
  }
  #endif

  /*********************************************************************
  * 3.) Rename all results                                             *
  *********************************************************************/ 
  UINT8 res;
  FOR_ALL_OP_RESULTs (new_op, res) {
    TN *tn = OP_result (new_op, res);
    
    if (TN_is_dedicated (tn)) continue;

    /* Check if we already have the TN in our TN map. If so,
     * update the result TN, otherwise we just create a new TN */
    TN *new_tn;
    if (OP_cond_def (new_op)) {
      FmtAssert (OP_same_res (new_op),
                  ("SWP_CODE_GEN: OP must have same_res property."));
      UINT8 idx = OP_find_opnd_use (new_op, OU_same_res); 
      new_tn = OP_opnd (new_op, idx);
    }
    else {
      new_tn = Dup_TN (tn);
    }

    _epilogue_tns[tn] = new_tn;
    Set_OP_result (new_op, res, new_tn);
  } /* FOR_ALL_OP_RESULTs (new_op, res) */

  return new_op;
}


TN *
SWP_CODE_GEN::Create_Prologue_Backpatch_Copy_For_TN (TN *kernel_tn, 
                                                     INT omega,
                                                     INT num) 
{
  /*********************************************************************
  * 1.) Early exit check. If we have a zero omega, there is no need to *
  *     even look for a backpatch.                                     *
  *********************************************************************/
  if (omega <= 0) return 0;

  /*********************************************************************
  * 2.) Look for backpatches related to body_tn and with the same omega*
  *     and create a copy for them.                                    *
  *********************************************************************/
  TN *body_tn = _mve.Get_Original_TN (kernel_tn);
  
  /* search only for backpatches related to body_tn */ 
  SWP_BP *bp;
  for (bp = CG_LOOP_Backpatch_First (CG_LOOP_prolog, body_tn);
       bp != 0;
       bp = CG_LOOP_Backpatch_Next (bp))
  {
    if (CG_LOOP_BACKPATCH_omega (bp) == omega) {
      break;
    }
  }

  if (bp == 0) {
    return 0;
  }

  SWP_BP *actual_bp = _CG_LOOP_BP_actual_ptr (bp);

  /* Check if we already have created a copy for this backpatch */
  SWP_BP2BOOL_MAP::const_iterator itr;
  itr = _bp_used.find (actual_bp);
  if (   itr != _bp_used.end ()
      && itr->second == TRUE) 
  {
    Fail_FmtAssertion ("SWP_CODE_GEN: Backpatch copy already exist.");
  }

  TN *source_tn = CG_LOOP_BACKPATCH_non_body_tn (actual_bp);
  TN *target_tn = Dup_TN (body_tn);
  Add_Glue_Copy (target_tn, source_tn, _old_prolog, SWP_Append);

  for (INT i = 0; i <= num; ++i) {
    _prologue_tns[i][kernel_tn] = target_tn;
  }

  _bp_used[actual_bp] = TRUE;

  return target_tn;
}


TN *
SWP_CODE_GEN::Create_Kernel_Backpatch_Copy_For_TN (TN *kernel_tn,
                                                   INT omega)
{
  /*********************************************************************
  * 1.) Early exit check. If we have a zero omega, there is no need to *
  *     even look for a backpatch.                                     *
  *********************************************************************/
  if (omega <= 0) return 0;

  /*********************************************************************
  * 2.) Look for backpatches related to body_tn and with the same omega*
  *     and create a copy for them.                                    *
  *********************************************************************/
  TN *body_tn = _mve.Get_Original_TN (kernel_tn);
  
  /* search only for backpatches related to body_tn */ 
  SWP_BP *bp;
  for (bp = CG_LOOP_Backpatch_First (CG_LOOP_prolog, body_tn);
       bp != 0;
       bp = CG_LOOP_Backpatch_Next (bp))
  {
    if (CG_LOOP_BACKPATCH_omega (bp) == omega) {
      break;
    }
  }

  if (bp == 0) {
    return 0;
  }
  
  SWP_BP *actual_bp = _CG_LOOP_BP_actual_ptr (bp);

  /* Check if we already have created a copy for this backpatch */
  SWP_BP2BOOL_MAP::const_iterator itr;
  itr = _bp_used.find (actual_bp);
  if (   itr != _bp_used.end ()
      && itr->second == TRUE) 
  {
    Fail_FmtAssertion ("SWP_CODE_GEN: Backpatch copy already exist.");
  }

  TN *source_tn = CG_LOOP_BACKPATCH_non_body_tn (bp);

  Add_Glue_Copy (kernel_tn, source_tn, _old_prolog, SWP_Append);

  _kernel_tns[kernel_tn] = kernel_tn;

  for (INT i = 0; i < _num_prologs; ++i) {
    _prologue_tns[i][kernel_tn] = kernel_tn;
  }

  _bp_used[actual_bp] = TRUE;

  return kernel_tn;
}

TN *
SWP_CODE_GEN::Create_Epilogue_Backpatch_Copy_For_TN (TN *kernel_tn, 
                                                     INT omega) 
{
  /*********************************************************************
  * 1.) Early exit check. If we have a zero omega, there is no need to *
  *     even look for a backpatch.                                     *
  *********************************************************************/
  if (omega <= 0) return 0;

  /*********************************************************************
  * 2.) Look for backpatches related to body_tn and with the same omega*
  *     and create a copy for them.                                    *
  *********************************************************************/
  TN *body_tn = _mve.Get_Original_TN (kernel_tn);
  
  /* search only for backpatches related to body_tn */ 
  SWP_BP *bp;
  for (bp = CG_LOOP_Backpatch_First (CG_LOOP_prolog, body_tn);
       bp != 0;
       bp = CG_LOOP_Backpatch_Next (bp))
  {
    if (CG_LOOP_BACKPATCH_omega (bp) == omega) {
      break;
    }
  }

  if (bp == 0) {
    return 0;
  }

  SWP_BP *actual_bp = _CG_LOOP_BP_actual_ptr (bp);

  TN *source_tn = CG_LOOP_BACKPATCH_non_body_tn (actual_bp);
  TN *target_tn = Dup_TN (body_tn);
  Add_Glue_Copy (target_tn, source_tn, _old_prolog, SWP_Append);

  _bp_used[actual_bp] = TRUE;

  return target_tn;
}


void 
SWP_CODE_GEN::Print (FILE *f) const {
  fprintf (f, "%sTN mapping:\n%s", DBar, DBar);
  SWP_TN2TN_MAP::const_iterator itr;
  for (INT i = 0; i < _num_prologs; ++i) {
    fprintf (f, "Prologue %d:\n", i);
    for (itr = _prologue_tns[i].begin (); 
         itr != _prologue_tns[i].end (); 
         ++itr) 
    {
      fprintf (f, 
               "TN%d -> TN%d\n", 
               TN_number (itr->first), 
               TN_number (itr->second)); 
    }
    fprintf (f, "\n");
  }
  fprintf (f, "%s\n", DBar);


  fprintf (f, "%sBackpatches:\n%s", DBar, DBar);
  SWP_BP2BOOL_MAP::const_iterator itr2;
  for (itr2 = _bp_used.begin (); itr2 != _bp_used.end (); ++itr2) {
    CG_LOOP_Backpatch_Trace (CG_LOOP_prolog, itr2->first, f);
    if (itr2->second) {
      fprintf (f, "used ");
      fprintf (f, "%p \n", itr2->first);
    }
    else {
      fprintf (f, "not used ");
      fprintf (f, "%p \n", itr2->first);
    }
  }

  return;
}
  

void 
SWP_CODE_GEN::Verify (void) const {
  SWP_BP2BOOL_MAP::const_iterator itr;
  for (itr = _bp_used.begin (); itr != _bp_used.end (); ++itr) {
    if (!itr->second) {
      SWP_BP *bp = _CG_LOOP_BP_actual_ptr (itr->first);
      DevWarn ("Backpatch TN%d <- TN%d[%d] is not used!",
               TN_number (bp->non_body_tn),
               TN_number (bp->body_tn), 
               bp->omega);
    }
  }

  return;
}


BOOL
SWP_CODE_GEN::Rename_Prologue_TN_to_Kernel_TN (TN *kernel_tn) {
  if (_num_prologs > 0) {
    SWP_TN2TN_MAP::iterator itr;
    itr = _prologue_tns[_num_prologs - 1].find (kernel_tn);
    
    BOOL found_last_def = FALSE;
    BOOL found_cond_def = FALSE;
    OP *op; 

    /* Check if the kernel TN has been defined in any of the prologues */
    if (itr != _prologue_tns[_num_prologs - 1].end ()) {
      /* Do a reverse search of the prologues */
      for (INT pro = _num_prologs - 1; pro >= 0; --pro) {
        /* Do a reverse search of the OPs inside the prologue */
        FOR_ALL_BB_OPs_REV (_prolog[pro], op) {  
          UINT8 opr;
          FOR_ALL_OP_OPNDs (op, opr) {
            TN *opnd = OP_opnd (op, opr);
            
            /* If an operand TN matches our mapped kernel TN,
             * then rename it to the kernel TN 
             */
            if (opnd == itr->second) {
              Set_OP_opnd (op, opr, kernel_tn);
            }
          }

          UINT8 res;
          FOR_ALL_OP_RESULTs (op, res) {
            TN *result = OP_result (op, res);

            /* If the result TN matches our mapped kernel TN,
             * then we might have found the defining OP and
             * can stop
             */
            if (result == itr->second) {
              Set_OP_result (op, res, kernel_tn);
              found_last_def = TRUE;
            }
          }

          if (found_last_def) {
            if (!OP_cond_def (op)) {
              /* The OP is not conditional defined. That means this
               * OP is definitely the defining OP of our kernel TN
               * and we can stop our search here.
               */
              found_cond_def = FALSE;
              break;
            }
            else {
              /* The OP is conditional defined. That means there
               * must be an other OP which defines this TN too.
               * We have to continue searching.
               */
              found_last_def = FALSE;
              found_cond_def = TRUE;
            }
          }
        } /* FOR_ALL_BB_OPs_REV (_prolog[pro], op) */

        /* We have definitely found the defining OP. Update the
         * prologue mapping for this kernel TN for the current prologue
         * and all following prologues.
         */
        if (found_last_def) {
          for (INT j = pro; j < _num_prologs; ++j) {
            _prologue_tns[j][kernel_tn] = kernel_tn;
          }

          return TRUE;
        }
      } /* for (INT pro = _num_prologs - 1; pro >= 0; --pro) */

      /* We haven't found a definite definition of the kernel TN in the
       * prologues. Now we have to search the old prolgue where all the
       * glue copies are.
       */
      FOR_ALL_BB_OPs_REV (_old_prolog, op) {  
        UINT8 opr;
        FOR_ALL_OP_OPNDs (op, opr) {
          TN *opnd = OP_opnd (op, opr);
          
          /* If an operand TN matches our mapped kernel TN,
           * then rename it to the kernel TN 
           */
          if (opnd == itr->second) {
            Set_OP_opnd (op, opr, kernel_tn);
          }
        }

        UINT8 res;
        FOR_ALL_OP_RESULTs (op, res) {
          TN *result = OP_result (op, res);
            
          /* If the result TN matches our mapped kernel TN,
           * then we might have found the defining OP and
           * can stop
           */
          if (result == itr->second) {
            Set_OP_result (op, res, kernel_tn);
            found_last_def = TRUE;
          }
        }

        if (found_last_def) {
          if (!OP_cond_def (op)) {
            /* The OP is not conditional defined. That means this
             * OP is definitely the defining OP of our kernel TN
             * and we can stop our search here.
             */
            found_cond_def = FALSE;
            break;
          }
          else {
            /* The OP is conditional defined. That means there
             * must be an other OP which defines this TN too.
             * We have to continue searching.
             */
            found_last_def = FALSE;
            found_cond_def = TRUE;
          }
        }
      } /* FOR_ALL_BB_OPs_REV (_prolog[i], op) */
      
      for (INT j = 0; j < _num_prologs; ++j) {
        _prologue_tns[j][kernel_tn] = kernel_tn;
      }

      if (found_last_def) return TRUE;

      if (found_cond_def) {
        TN *source_tn = _mve.Get_Original_TN (kernel_tn);
        Add_Glue_Copy (kernel_tn, source_tn, _old_prolog, SWP_Append);
        
        return TRUE;
      }
    } /* if (itr != _prologue_tns[_num_prologs - 1].end ()) */
  } /* if (_num_prologs > 0) */
    
  return FALSE;
}

