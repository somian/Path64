/*
 * Copyright (C) 2007, 2009 Pathscale, LLC.  All Rights Reserved.
 */

/*
 *  Copyright (C) 2007. QLogic Corporation. All Rights Reserved.
 */

/*
 * Copyright 2002, 2003, 2004, 2005, 2006 PathScale, Inc.  All Rights Reserved.
 */

/*

  Copyright (C) 2000, 2001 Silicon Graphics, Inc.  All Rights Reserved.

   Path64 is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   Path64 is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
   or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
   License for more details.

   You should have received a copy of the GNU General Public License
   along with Path64; see the file COPYING.  If not, write to the Free
   Software Foundation, 51 Franklin Street, Fifth Floor, Boston, MA
   02110-1301, USA.

   Special thanks goes to SGI for their continued support to open source

*/


/* ====================================================================
 * ====================================================================
 *
 * Module: calls.cxx
 * $Revision: 1.65 $
 * $Date: 05/08/31 17:55:24-07:00 $
 * $Author: tkong@hyalite.keyresearch $
 * $Source: be/cg/SCCS/s.calls.cxx $
 *
 * Revision history:
 *  04-Oct-91 - Original Version
 *  24-Oct-91 - Added support for entry/exit code.
 *  27-May-93 - Added support for PIC entry/exit code.
 *
 * Description:
 *
 * General support for call-related functionality in the back end.
 * Utilities for manipulating the call descriptors.
 *
 * ====================================================================
 * ====================================================================
 */

#include "defs.h"
#include "erglob.h"
#include "erbe.h"
#include "glob.h"
#include "tracing.h"
#include "config_targ.h"
#include "config.h"

#include "symtab.h"
#include "strtab.h"
#include "be_symtab.h"
#include "targ_isa_lits.h"
#include "topcode.h"
#include "cg.h"
#include "cg_internal.h"
#include "cg_flags.h"
#include "register.h"
#include "tn.h"
#include "tn_set.h"
#include "bb.h"
#include "op.h"
#include "tn_map.h"

#include "cgemit.h"
#include "ttype.h"
#include "calls.h"
#include "const.h"
#include "data_layout.h"
#include "cgexp.h"
#include "targ_sim.h"
#include "whirl2ops.h"
#include "cg_spill.h"
#include "reg_live.h"
#include "lra.h"
#include "cgtarget.h"
#include "entry_exit_targ.h"
#include "targ_abi_properties.h"
#include "cxx_template.h"

#ifdef KEY
#include "gtn_universe.h"
#include "gtn_set.h"
#endif

INT64 Frame_Len;

/* Callee-saved register <-> save symbol/TN map: */
SAVE_REG *Callee_Saved_Regs;
INT32 Callee_Saved_Regs_Count;

#ifdef KEY
STACK<SAVE_REG_LOC> Saved_Callee_Saved_Regs(Malloc_Mem_Pool);
#endif

/* Special PREGs associated with save locations for Callee Saved registers */
PREG_NUM *Callee_Saved_Pregs;
PREG_NUM Caller_FP_Preg;
PREG_NUM Caller_GP_Preg;
PREG_NUM Return_Preg;
PREG_NUM GP_Preg;
PREG_NUM Return_Int_Preg[2];
PREG_NUM Return_Float_Preg[2];

/* Return address register map: */
static SAVE_REG Return_Address_Reg_Map;
SAVE_REG *Return_Address_Reg = &Return_Address_Reg_Map;

/* Do we need to setup a new GP for this procedure? */
static enum {
	undefined_code,	/* not set yet */
	never_code,	/* can access gp without setup */
	no_code,	/* doesn't have a gp setup */
	need_code,	/* needs a gp setup */
	final_code	/* has final version of gp setup */
} GP_Setup_Code = undefined_code;

BOOL LC_Used_In_PU;

/* TNs to save the callers GP and FP if needed */
#ifdef TARG_MIPS
TN *Caller_GP_TN;
#else
static TN *Caller_GP_TN;
#endif
static TN *Caller_FP_TN;
static TN *Caller_Pfs_TN;
static TN *ra_intsave_tn;

/* Keep track of a TN with the value of the current PU's stack pointer
 * adjustment (i.e. the frame length):
 */
static TN *Frame_Len_TN;
static TN *Neg_Frame_Len_TN;

BOOL Gen_Frame_Pointer;

/* Trace flags: */
static BOOL Trace_EE = FALSE;	/* Trace entry/exit processing */

/* macro to test if we will use a scratch register to hold gp for
 * the pu.  we do this in leaf routines if there are no regions
 * and gra will be run.
 */
#define Use_Scratch_GP(need_gp_setup) \
((need_gp_setup) && !PU_Has_Calls && !PU_has_region(Get_Current_PU()))

/* ====================================================================
 *
 * Init_Pregs
 *
 * Initialize various pregs for PU wide use.
 * ====================================================================
 */

static void
Init_Pregs ( void )
{
  INT i;
  
  Caller_FP_Preg = Create_Preg( TY_mtype( Spill_Int_Type ), "caller_FP");
  Caller_GP_Preg = Create_Preg( TY_mtype( Spill_Int_Type ), "caller_GP");

  /* Now initialize the return address map: */
  Return_Preg = Create_Preg( TY_mtype( Spill_Int_Type ), "return");

  for ( i = 0; i < 2; i++ ) {
    Return_Int_Preg[i]   = Create_Preg( TY_mtype( Spill_Int_Type ),   "return_int");
    Return_Float_Preg[i] = Create_Preg( TY_mtype( Spill_Float_Type ), "return_float");
  }

  /* set up preg for gp tn */
  GP_Preg = Create_Preg( TY_mtype( Spill_Int_Type ), "GP");
}


#ifndef TARG_X8664
/* =======================================================================
 *
 *  Setup_GP_TN_For_PU
 *
 *  set up GT_TN for the PU.  we may be able to place it in a caller saved
 *  register, and if so, allocate one for that purpose.  if not, use $gp.  
 *
 * =======================================================================
 */
static void
Setup_GP_TN_For_PU( ST *pu)
{
  REGISTER reg;

  if (GP_Setup_Code != undefined_code) {
	/* This can only happen if called more than once per PU,
	 * i.e. when regions.  In this case, keep the initial setting
	 * (so that all bbs are consistent), and then possibly adjust
	 * everything in the PU in Adjust_GP_Entry_Exit */
	;
  }
  else if (Force_GP_Prolog) {
	GP_Setup_Code = need_code;
  }
  else if (!Is_Caller_Save_GP &&
	(Gen_PIC_Call_Shared || Gen_PIC_Shared) &&
#ifdef TARG_MIPS
	/* Bug 12724: For C++, a function that is apparently invisible
	   outside may still be called from outside, via the C++
	   exceptions runtime. So we need to save/restore GP. */
	(ST_visible_outside_dso(pu) || PU_cxx_lang(Pu_Table[ST_pu(pu)]))
#else
	ST_visible_outside_dso(pu)
#endif
	)
  {
	/* if non-leaf that is visible outside dso, then must setup gp
	 * because may call internal routine that uses gp. */
	if (PU_Has_Calls || PU_References_GP) {
		PU_References_GP = TRUE;
		GP_Setup_Code = need_code;
	}
	else {
		/* Don't setup GP unless sure that it is needed.
		 * Adjust_GP_Entry_Exit can add gp setup/save/restore code
		 * at end after we are sure that it is needed.
		 * Otherwise we have to remove gp code, and that gets
		 * complicated with regions, because it can also affect
		 * the boundary sets. */
		GP_Setup_Code = no_code;
	}
  }
  else {
	GP_Setup_Code = never_code;
  }

  /* initialize the gp map */
  TN_MAP_Set( TN_To_PREG_Map, GP_TN, (void *)GP_Preg );
  PREG_To_TN_Array[ GP_Preg ] = GP_TN;
  PREG_To_TN_Mtype[ GP_Preg ] = TY_mtype(Spill_Int_Type);

  /* we will put gp in a scratch register if this is a leaf routine
   * and gra is to be called (check for REGISTER_UNDEFINED protects
   * agains multiple entry PUs).  we also make gp available as a callee
   * saved register so that using the scratch register will not cause
   * any degradation due to register constraints.  we don't currently
   * have enough information to perform this optimization if regions
   * are present.
   */
  if ( Use_Scratch_GP(GP_Setup_Code == need_code) ) {
    REGISTER_SET caller_saves;
    REGISTER_SET func_val;
    REGISTER_SET func_arg;

    REGISTER_Set_Allocatable(REGISTER_CLASS_gp, REGISTER_gp, TRUE);

    /* exclude function return and argument registers from our choices */
    caller_saves = REGISTER_CLASS_caller_saves(REGISTER_CLASS_gp);
    func_val = REGISTER_CLASS_function_value(REGISTER_CLASS_gp);
    caller_saves = REGISTER_SET_Difference(caller_saves, func_val);
    func_arg = REGISTER_CLASS_function_argument(REGISTER_CLASS_gp);
    caller_saves = REGISTER_SET_Difference(caller_saves, func_arg);

    reg = REGISTER_SET_Choose(caller_saves);
    if ( reg == REGISTER_UNDEFINED ) {
      /* no caller saved register available for some reason.  this
       * should not happen, but we'll fail gracefully and just use
       * gp.
       */
      DevWarn("No caller saved register to replace $gp in leaf routine.\n");
      reg = REGISTER_gp;
    }
  } else {
    /* use gp */
    reg = REGISTER_gp;
  }
  REGISTER_Set_Allocatable(REGISTER_CLASS_gp, reg, FALSE);
  Set_TN_register(GP_TN, reg);
}
#endif


/* =======================================================================
 *
 *  Init_Callee_Saved_Regs_for_REGION 
 *
 *  See interface description.
 *
 * =======================================================================
 */
void
Init_Callee_Saved_Regs_for_REGION ( ST *pu, BOOL is_region )
{
  INT i;
  ISA_REGISTER_CLASS cl;
  TN *stn;

#ifndef TARG_X8664  // x86_64 does not use GP
  Setup_GP_TN_For_PU( pu );
#endif

  if (NULL != RA_TN /* IA-32 doesn't have ra reg. */) {
    /* initialize the return address map: */
    if ( stn = PREG_To_TN_Array[ Return_Preg ] )
      SAVE_tn(Return_Address_Reg) = stn;
    else {
      SAVE_tn(Return_Address_Reg) = Build_TN_Like(RA_TN);
      Set_TN_save_creg (SAVE_tn(Return_Address_Reg), TN_class_reg(RA_TN));
      TN_MAP_Set( TN_To_PREG_Map, SAVE_tn(Return_Address_Reg),
		  (void *)Return_Preg );
      PREG_To_TN_Array[ Return_Preg ] = SAVE_tn(Return_Address_Reg);
      PREG_To_TN_Mtype[ Return_Preg ] = Spill_Int_Mtype;
    }
  }
  ra_intsave_tn = NULL;
  Caller_GP_TN = NULL;
  Caller_FP_TN	= NULL;
  Caller_Pfs_TN = NULL;
  if (Pfs_TN) {
  	Caller_Pfs_TN = Gen_Register_TN (ISA_REGISTER_CLASS_integer, Pointer_Size);
  	Set_TN_save_creg (Caller_Pfs_TN, TN_class_reg(Pfs_TN));
  }
 
  /* if called for a region, no need to set up TN's for callee saved
   * registers.  these will be handled in the PU.
   */
  if (is_region) {
    return;
  }

#ifdef TARG_X8664
  if( CG_opt_level > 0  &&
      Is_Target_32bit() &&
      Gen_PIC_Shared    &&
      !PU_References_GOT ){
    TN* ebx_tn = Ebx_TN();
    REGISTER_Set_Allocatable( TN_register_class(ebx_tn),
			      TN_register(ebx_tn),
			      TRUE );
  }  
#endif

  /* allocate the callee-saved register map: */
  Callee_Saved_Regs = (SAVE_REG *)Pu_Alloc (  (ISA_REGISTER_CLASS_MAX + 1) 
					    * (REGISTER_MAX + 1)
					    * sizeof(SAVE_REG) );

  /* build and initialize the save tn's for the callee saved registers */
  Callee_Saved_Regs_Count = 0;
  i = 0;
  FOR_ALL_ISA_REGISTER_CLASS( cl ) {
    REGISTER reg;
    REGISTER_SET regset = REGISTER_CLASS_callee_saves(cl);

    if (REGISTER_CLASS_multiple_save(cl)) continue;

    for ( reg = REGISTER_SET_Choose(regset);
	  reg != REGISTER_UNDEFINED;
	  reg = REGISTER_SET_Choose_Next(regset, reg), ++i
    ) {
      TN *callee_tn;
      TN *ded_tn = Build_Dedicated_TN(cl, reg, 0);
      DevAssert(ded_tn, ("Missing dedicated TN for callee-saved register %s",
			REGISTER_name(cl, reg)));
      CALLEE_ded_tn(i) = ded_tn;
      callee_tn = Build_RCLASS_TN(cl);
      Set_TN_save_creg (callee_tn, TN_class_reg(ded_tn));
      CALLEE_tn(i) = callee_tn;
    }

  }
  if (LC_TN) {
  	// Add ar.lc which is callee-save but not allocatable
  	// so doesn't show up in callee-save regset.
	// We set save_reg so that the copy will automatically
	// be generated, however it is not a normal copy in that
	// it is a copy from ar to int class.  Therefore the normal
	// preferencing will not make it look like a dead copy.
	// So instead of letting gra/lra remove it if unused,
	// we instead will have a separate pass to remove it after cgprep.
	// We could have instead only generated the save/restore after
	// cgprep when we know we need it, but then also need to update
	// all the gra live tn sets which is expensive for just one
	// new tn.  So instead is simpler to generate it up front
	// (so automatically added to tn sets), then remove it if not needed
	// (will still be in tn sets but no references).
	TN *callee_tn = Build_RCLASS_TN (ISA_REGISTER_CLASS_integer);
        Set_TN_save_creg (callee_tn, TN_class_reg(LC_TN));
	CALLEE_tn(i) = callee_tn;
	CALLEE_ded_tn(i) = LC_TN;
  	++i;
  }
  Callee_Saved_Regs_Count = i;
}



#ifdef KEY
struct Save_user_allocated_saved_regs
{
  OPS *ops;
  BB *bb;
  Save_user_allocated_saved_regs(OPS *o, BB *b) : ops(o), bb(b) { }
  void operator() (UINT, ST_ATTR *st_attr) const {
    if (ST_ATTR_kind (*st_attr) != ST_ATTR_DEDICATED_REGISTER)
      return;
    PREG_NUM preg = ST_ATTR_reg_id(*st_attr);
    ISA_REGISTER_CLASS rclass;
    REGISTER reg;
    CGTARG_Preg_Register_And_Class(preg, &rclass, &reg);
    if (! ABI_PROPERTY_Is_callee(rclass, preg-REGISTER_MIN))
      return;
    SAVE_REG_LOC sr;
    sr.ded_tn = Build_Dedicated_TN(rclass, reg, 0);
    DevAssert(sr.ded_tn, ("Missing dedicated TN for callee-saved register %s",
		      REGISTER_name(rclass, reg)));
    if (Is_Unique_Callee_Saved_Reg (sr.ded_tn)) {
      sr.temp = CGSPILL_Get_TN_Spill_Location (sr.ded_tn, CGSPILL_LCL);
      sr.user_allocated = TRUE;
      /* Generate the spill ops */
      CGSPILL_Store_To_Memory (sr.ded_tn, sr.temp, ops, CGSPILL_LCL, bb);
      Set_OP_no_move_before_gra(OPS_last(ops));
      Saved_Callee_Saved_Regs.Push(sr);
    }
  }
};
#endif

/* ====================================================================
 *
 * Generate_Entry
 *
 * Generate entry code (in SGIR form) in the given BB for the given
 * entry symbol.  This currently includes the following:
 *
 *   1) Insert an update of the stack pointer.  If a frame pointer is
 *	required, first copy it from the stack pointer.  This also
 *	requires saving the caller's FP, since it's a callee-saved
 *	register, so we copy it to another temporary register.  (We
 *	can't just store it, since our stack frame hasn't been
 *	established yet.)
 *
 *   2)	Insert stores from dedicated parameter registers for all
 *	declared and possible undeclared formals for varargs routines.
 *
 *   3) If <gra_run>, then GRA will be run and each of the callee_saves
 *	registers is copied to its save TN.
 *
 * ====================================================================
 */

static void
Generate_Entry (BB *bb, BOOL gra_run )
{
  INT callee_num;
  TN *stn;
  OP *op;
  OPS ops = OPS_EMPTY;
  ANNOTATION *ant = ANNOT_Get (BB_annotations(bb), ANNOT_ENTRYINFO);
  ENTRYINFO *ent_info = ANNOT_entryinfo(ant);
  ST *st = ENTRYINFO_name(ent_info);	/* The entry's symtab entry */

  if ((BB_rid(bb) != NULL) && ( RID_level(BB_rid(bb)) >= RL_CGSCHED )) {
  	/* don't change bb's which have already been through CG */
  	return;
  }

  if ( Trace_EE ) {
    #pragma mips_frequency_hint NEVER
    fprintf ( TFile,
	      "\n%s<calls> Generating entry for %s (BB:%d)\n",
	      DBar, ST_name(st), BB_id(bb) );

  }

  if (!BB_handler(bb)) {

    EETARG_Save_Pfs (Caller_Pfs_TN, &ops);	// alloc

#ifdef TARG_X8664
    /* Initialize the frame pointer if required: */
    if ( Gen_Frame_Pointer && PUSH_FRAME_POINTER_ON_STACK ) {
      Build_OP( Is_Target_64bit() ? TOP_pushq : TOP_pushl, SP_TN, FP_TN, SP_TN,
      		&ops );
      Exp_COPY( FP_TN, SP_TN, &ops );
    }
#endif

    /* Initialize the stack pointer (this is a placeholder; Adjust_Entry
     * will replace it to the actual sequence once we know the size of
     * the frame):
     */
    Exp_Spadjust (SP_TN, Neg_Frame_Len_TN, V_NONE, &ops);

    /* Initialize the frame pointer if required: */
    if ( Gen_Frame_Pointer && !PUSH_FRAME_POINTER_ON_STACK ) {
      // check if fp is callee reg
      // (check abi_property rather than register set
      //  because register_set unsets fp as callee when already allocated).
      if (ABI_PROPERTY_Is_callee (TN_register_class(FP_TN),
	  REGISTER_machine_id(TN_register_class(FP_TN), TN_register(FP_TN)) ))
      {
	// save old fp
      	if ( Caller_FP_TN == NULL ) {
      		/* Build a temp symbol for caller's FP if needed: */
        	if ( stn = PREG_To_TN_Array[ Caller_FP_Preg ] )
  			Caller_FP_TN = stn;
        	else {
			// Bug 13316: FP can hold 64 bit (non pointer) value
  			Caller_FP_TN = Gen_Register_TN ( 
				ISA_REGISTER_CLASS_integer,
				TY_size(Spill_Int_Type));
  			Set_TN_save_creg (Caller_FP_TN, TN_class_reg(FP_TN));
  			TN_MAP_Set( TN_To_PREG_Map, Caller_FP_TN, 
				(void *)Caller_FP_Preg );
  			PREG_To_TN_Array[ Caller_FP_Preg ] = Caller_FP_TN;
  			PREG_To_TN_Mtype[ Caller_FP_Preg ] = TY_mtype(Spill_Int_Type);
        	}
	}
      	/* Save the caller's FP in a temporary: */
      	Exp_COPY (Caller_FP_TN, FP_TN, &ops);
	Set_OP_no_move_before_gra(OPS_last(&ops));
      }
  
      /* Now recover the new FP from the new SP: */
      Exp_Spadjust (FP_TN, Frame_Len_TN, V_NONE, &ops);
    }
    ENTRYINFO_sp_adj(ent_info) = OPS_last(&ops);

#ifdef KEY
    // bug 4583: save callee-saved registers that may get clobbered 
    // by inline asm
    for (INT i = 0; i < Saved_Callee_Saved_Regs.Elements(); i++) {
      SAVE_REG_LOC sr = Saved_Callee_Saved_Regs.Top_nth(i);

#ifdef TARG_X8664
      if (sr.temp == NULL)
	continue; // handled by push/pop under CG_push_pop_int_saved_regs
#endif
      CGSPILL_Store_To_Memory (sr.ded_tn, sr.temp, &ops, CGSPILL_LCL, bb);
      Set_OP_no_move_before_gra(OPS_last(&ops));
    }

    /* save callee-saved registers allocated to local user variables */
    if ( ST_ATTR_Table_Size (CURRENT_SYMTAB)) {
      For_all_entries(*Scope_tab[CURRENT_SYMTAB].st_attr_tab, 
		      Save_user_allocated_saved_regs(&ops, bb), 1);
    }
#endif
  }

  if ( gra_run ) {
    /* Copy from the callee saves registers to register TNs */
    for ( callee_num = 0; callee_num < Callee_Saved_Regs_Count; ++callee_num ) {
      TN *callee_tn = CALLEE_tn(callee_num);
      if (    TN_is_save_reg(callee_tn) 
	  && !REGISTER_CLASS_multiple_save(TN_register_class(callee_tn)))
      {
        Exp_COPY ( callee_tn, CALLEE_ded_tn(callee_num), &ops );
	Set_OP_no_move_before_gra(OPS_last(&ops));
      }
    }
  }

  /* If the return address builtin is required, save RA_TN to the 
   * memory location for __return_address. Otherwise, we copy RA_TN
   * to the save-tn for it. 
   */
#ifdef TARG_MIPS
  // Don't save RA_TN if it is an exception handler entry point.
  if (!BB_handler(bb))
#endif
  if (NULL != RA_TN) {
    if ( PU_has_return_address(Get_Current_PU()) ) {
      // This is broken for IA-32.  On IA-32, the return address is always
      // at a constant offset from the frame pointer, specifically it is
      // accessible as 4(%ebp), but it is never in a register.  Nor
      // does it need to be saved.
      ST *ra_sv_sym = Find_Special_Return_Address_Symbol();
      TN *ra_sv_tn = Build_TN_Like(RA_TN);
      Set_TN_save_creg (ra_sv_tn, TN_class_reg(RA_TN));
      Set_TN_spill(ra_sv_tn, ra_sv_sym);
      Exp_COPY (ra_sv_tn, RA_TN, &ops);
      if (MTYPE_byte_size(Pointer_Mtype) < MTYPE_byte_size(Spill_Int_Mtype) ) {
	/* In n32 the __return_address is 4 bytes (pointer),
	 * but we need 8-byte save/restore to make kernel and dbx happy.
	 * So use dummy 8-byte base that was created. */
	ra_sv_sym = ST_base(ra_sv_sym);		/* use 8-byte block */
	Set_TN_spill(ra_sv_tn, ra_sv_sym);	/* so dwarf uses new addr */
      }
      CGSPILL_Store_To_Memory (ra_sv_tn, ra_sv_sym, &ops, CGSPILL_LCL, bb);
    }
    else {
      if (gra_run && PU_Has_Calls 
	&& TN_register_class(RA_TN) != ISA_REGISTER_CLASS_integer)
      {
	// because has calls, gra will need to spill this.
	// but if it is not already in an integer reg,
	// then gra will spill to memory whereas for ia64
	// it could use a stacked reg; ideally gra would handle
	// this, but it doesn't and is easy to just copy to int reg
	// by hand and then let gra use stacked reg.
	if (ra_intsave_tn == NULL) {
        	ra_intsave_tn = Build_RCLASS_TN (ISA_REGISTER_CLASS_integer);
		Set_TN_save_creg (ra_intsave_tn, TN_class_reg(RA_TN));
	}
	Exp_COPY (ra_intsave_tn, RA_TN, &ops );
      } else {
        Exp_COPY (SAVE_tn(Return_Address_Reg), RA_TN, &ops );
      }
      Set_OP_no_move_before_gra(OPS_last(&ops));
    }
  }

  if ( gra_run ) 
    EETARG_Save_Extra_Callee_Tns (&ops);

#ifndef TARG_X8664  // x86_64 does not use GP
  // Don't save GP if it is an exception handler entry point.
  if (!BB_handler(bb)) {
    /* Save the old GP and setup a new GP if required */
    if (GP_Setup_Code == need_code) {

      /* we will put the gp in any available register if a leaf routine
       * to avoid the save/restore of the gp.  we will also make gp
       * available as a callee saved register so that in the worst case,
       * we will not generate worse code by using another register for
       * gp.
       */
      if ( !Use_Scratch_GP( TRUE ) ) {
	if ( Caller_GP_TN == NULL ) {
	  if ( stn = PREG_To_TN_Array[ Caller_GP_Preg ] )
	    Caller_GP_TN = stn;
	  else {
	    Caller_GP_TN = Gen_Register_TN (
		    ISA_REGISTER_CLASS_integer, Pointer_Size);
	    Set_TN_save_creg (Caller_GP_TN, TN_class_reg(GP_TN));
	    TN_MAP_Set( TN_To_PREG_Map, Caller_GP_TN, (void *)Caller_GP_Preg );
	    PREG_To_TN_Array[ Caller_GP_Preg ] = Caller_GP_TN;
	    PREG_To_TN_Mtype[ Caller_GP_Preg ] = TY_mtype(Spill_Int_Type);
	  }
	}
	Exp_COPY (Caller_GP_TN, GP_TN, &ops);
      }

      // is possible for altentry to be marked as not_used
      // but we have gp_prolog for whole PU.  In that case,
      // don't generate prolog (that references altentry ST).
      if (ST_is_not_used(st))
	;
      else if (Gen_PIC_Call_Shared && CGEXP_gp_prolog_call_shared 
	&& (MTYPE_byte_size(Pointer_Mtype) == MTYPE_byte_size(MTYPE_I4)) )
      {
	/* pv 466125 */
	/* we actually ignore the st in this case, but pass it anyways
	 * so that tn structure is regular. */
	TN *gp_value_tn = Gen_Symbol_TN(st, 0, TN_RELOC_GPIDENT);
	Set_TN_size(gp_value_tn, 4);  /* text addresses are always 32bits */
	Exp_OP1 (OPC_I4INTCONST, GP_TN, gp_value_tn, &ops);
      }
      else {
	/* Create a symbolic expression for ep-gp */
	TN *cur_pu_got_disp_tn = Gen_Symbol_TN(st, 0, TN_RELOC_GPSUB);
	TN *got_disp_tn = Gen_Register_TN (
		ISA_REGISTER_CLASS_integer, Pointer_Size);
	// text addresses are always 32bits
	Set_TN_size(cur_pu_got_disp_tn, 4);  
	Exp_OP1 (OPC_I4INTCONST, got_disp_tn, cur_pu_got_disp_tn, &ops);

	/* Add it to ep to get the new GP: */
	Exp_ADD (Pointer_Mtype, GP_TN, Ep_TN, got_disp_tn, &ops);
      }
    } 
    else if (Is_Caller_Save_GP && PU_Has_Calls && !Constant_GP
	&& PREG_To_TN_Array[ Caller_GP_Preg ] != NULL) 
    {
	// need to save old gp but don't need to setup new gp.
	// caller_gp_tn should already be created by call code.
	Caller_GP_TN = PREG_To_TN_Array[ Caller_GP_Preg ];
      	Exp_COPY (Caller_GP_TN, GP_TN, &ops);
    }
  }
#endif

  /* set the srcpos field for all the entry OPs */
  FOR_ALL_OPS_OPs(&ops, op)
    OP_srcpos(op) = ENTRYINFO_srcpos(ent_info);

  /* If we're tracing, print the new stuff before merging it: */
  if ( Trace_EE ) {
    #pragma mips_frequency_hint NEVER
    Print_OPS (&ops);
  }

#ifdef TARG_X8664
  if( Is_Target_32bit() && Gen_PIC_Shared ){
    EETARG_Generate_PIC_Entry_Code( bb, &ops );
  }
#endif

  /* Merge the new operations into the beginning of the entry BB: */
  BB_Prepend_Ops(bb, &ops);
}


/* ====================================================================
 *
 * Is_Function_Value
 *
 * Return a boolean that indicates if <tn> is a function value TN.
 *
 * ====================================================================
 */
inline BOOL
Is_Function_Value(TN *tn)
{
  if (TN_is_dedicated(tn)) {
    REGISTER reg = TN_register(tn);
    ISA_REGISTER_CLASS rc = TN_register_class(tn);
    return REGISTER_SET_MemberP(REGISTER_CLASS_function_value(rc), reg);
  }
  return FALSE;
}


/* ====================================================================
 *
 * Find_Call_Addr_Load
 *
 * 'call_bb' contains a call. Search the BB for the load of t9 and
 * return it (by value) along with the index of the operand through the
 * out parameter 'iopnd'. If no definition is found, NULL is returned.
 *
 * ====================================================================
 */
static OP *
Find_Call_Addr_Load(BB *call_bb, INT *iopnd)
{
  OP *op;

  for (op = BB_last_op(call_bb); op; op = OP_prev(op)) {
    if (OP_results(op) == 1) {
      TN *tn = OP_result(op,0);
      if (TN_is_ep_reg(tn)) {
	INT i;

	if (!OP_load(op)) break;

	for (i = 0; i < OP_opnds(op); ++i) {
          tn = OP_opnd(op, i);
	  if (TN_is_symbol(tn)) {
	    *iopnd = i;
	    return op;
	  }
	}

	break;
      }
    }
  }

  return NULL;
}


/* ====================================================================
 *
 * Can_Be_Tail_Call
 *
 * Determine if the specified exit block <exit_bb> can be converted
 * to a tail call. If so, the corresponding call BB is returned;
 * otherwise, NULL is returned.
 *
 * ====================================================================
 */
static BB *
Can_Be_Tail_Call(ST *pu_st, BB *exit_bb)
{
  OP *op;
  hTN_MAP fvals;
  PLOC ploc;
  TY_IDX func_type;
  ST *st;
  INT addr_opnd;
  OP *addr_op;
  ANNOTATION *ant;
  CALLINFO *call_info;
  ST *call_st;
  WN *call_wn;
  BB *pred;

  /* The exit block can have only one pred, and it must be a call block.
   */
  pred = BB_Unique_Predecessor(exit_bb);
  if (!pred || !BB_call(pred)) return NULL;

  /* Bug 13846: The tail-call transformation discards the exit block's
   * labels.  Make sure a label is not marked addr_saved.
   */
  if (BB_Has_Addr_Taken_Label(exit_bb)) return NULL;

  /* Get some info about the call and the callee.
   */
  ant = ANNOT_Get(BB_annotations(pred), ANNOT_CALLINFO);
  call_info = ANNOT_callinfo(ant);
  call_st = CALLINFO_call_st(call_info);
  call_wn = CALLINFO_call_wn(call_info);

  /* Assume a call sequence A->B->C (i.e. A calls B which calls C)
   * We would like to change the call B->C to be a tail-call.
   *
   * If C does not setup a new gp (static/internal), it can be a tail-call
   * only if B also does not setup a new GP. Otherwise, C would get A's
   * GP which might be wrong.
   *
   * One complication is that there is another scenario where C will
   * use the caller's GP: if C is preemptible, then its RLD stub
   * uses GP as part of its interface. In these cases we [later] change
   * the relocation for the symbol so it is treated as a data reference,
   * and no stub is generated.
   */
  addr_op = NULL;
  if (   !Is_Caller_Save_GP
      && (Gen_PIC_Call_Shared || Gen_PIC_Shared)
      && ST_visible_outside_dso(pu_st))
  {
    /* If we're making an indirect call to 'C' then we can't be sure
     * whether or not it sets up a GP.
     */
    if (call_st == NULL) return NULL;

    /* 'C' does not setup a GP, so if we make 'B' into a tail call,
     * then 'C' may get an incorrect GP.
     */
    if (!ST_visible_outside_dso(call_st)) return NULL;

    /* Determine if valid GP might be necessary for an RLD stub.
     */
    if (ST_is_preemptible(call_st)) {
      TN *tn;

#ifdef TARG_X8664
      addr_op = BB_xfer_op( pred );
      addr_opnd = 0;
      if( addr_op == NULL ){
	return NULL;
      }
#else
      addr_op = Find_Call_Addr_Load(pred, &addr_opnd);
      if (addr_op == NULL) return NULL;
      tn = OP_opnd(addr_op, addr_opnd);
      if (TN_is_reloc_call16(tn)) {
#if 1
	/* RE: pv812245, originally we changed the preemptible symbol
	 * to non-preemptible and made it weak. The later causes
	 * symbol preemption to behave differently than it should
	 * so we now just reject the case. The old code remains
	 * in case we decide to do it under some special switch.
	 */
	return NULL;
#else
	if (!Enable_GOT_Call_Conversion) return NULL;
#endif
      } else if (TN_is_reloc_got_disp(tn)) {
	/* ok as is */
	addr_op = NULL;
      } else {
	return NULL;
      }
#endif
    }
  }

#ifdef KEY
  /* Bug 12718: If the callee uses __builtin_return_address, we need
   * to preserve the call. */
  if (call_st && PU_has_return_address(Pu_Table[ST_pu(call_st)]))
    return NULL;


  // bug 14606: we do not need these tests for C++ thunk PUs. Below
  // -O2, we may not have the addr flags set.
  if (!PU_is_thunk(Get_Current_PU())) {
#endif
  /* If any stack variables have had their address taken, it is
   * possible they might be used by the called PU, but if we do the
   * tail call conversion, the stack frame will have been removed.
   */
  if (!BE_ST_pu_has_valid_addr_flags(Get_Current_PU_ST())) return NULL;
  INT i;
  FOREACH_SYMBOL (CURRENT_SYMTAB, st, i) {     /* all local symbols in Current_Symtab */
    if (ST_class(st) != CLASS_VAR) continue;
    if (BE_ST_addr_used_locally(st) || BE_ST_addr_passed(st)) return NULL;
  }

#ifdef KEY
  }
#endif

  /* Make sure we don't use the stack to pass arguments to the called PU.
   * Defs have parm info in TY, but calls without prototypes do not,
   * so use whirl call node in that case.
   */
  func_type = call_st ? ST_pu_type(call_st) : WN_ty(call_wn);
  ploc = Setup_Output_Parameter_Locations(func_type);
  if (call_wn == NULL) {
    TYLIST_IDX tl;
    for (tl = TY_parms(func_type); tl != (TYLIST_IDX) NULL; tl = TYLIST_next(tl)) {
      ploc = Get_Output_Parameter_Location(TYLIST_item(tl));
      if (PLOC_on_stack(ploc)) return NULL;
    }
  } else {
    INT i;
    INT num_parms = WN_num_actuals(call_wn);
    for (i = 0; i < num_parms; i++) {
      ploc = Get_Output_Parameter_Location (TY_Of_Parameter(WN_actual(call_wn,i)));
      if (PLOC_on_stack(ploc)) return NULL;
    }
  }

#ifdef TARG_X8664
  /* Don't perform tail call optimization if the caller does not write the result to
     the x87 stack, but the callee does. (bug#2841)
   */
  if( Is_Target_32bit() ){
    bool caller_uses_stack = false;

    const RETURN_INFO caller_return_info =
      Get_Return_Info( TY_ret_type(ST_pu_type(pu_st)),
		       No_Simulated,
		       PU_ff2c_abi(Pu_Table[ST_pu(pu_st)]) );

    for( int i = 0; i < RETURN_INFO_count(caller_return_info); i++ ){
      const TYPE_ID type = RETURN_INFO_mtype( caller_return_info, i );
      if( MTYPE_is_float( type ) ){
	caller_uses_stack = true;
	break;
      }
    }

    const RETURN_INFO callee_return_info =
      Get_Return_Info( TY_ret_type(func_type),
		       No_Simulated,
		       call_st ? PU_ff2c_abi(Pu_Table[ST_pu(call_st)]) : FALSE ); 

    for( int i = 0; i < RETURN_INFO_count(callee_return_info); i++ ){
      const TYPE_ID type = RETURN_INFO_mtype( callee_return_info, i );
      if( MTYPE_is_float( type ) ){
	if( !caller_uses_stack )
	  return NULL;
      }
    }
  }

  /* Under -m32 -fpic, don't do tail call optimization, because the caller
     needs to restore GOT before return.
  */
  if( Is_Target_32bit() &&
      PU_References_GOT ){
    return NULL;
  }
#endif // TARG_X8664

  /* We need to make sure that the function values for the current
   * PU are the same or a subset of the function values for the
   * called PU. We accomplish this by examining uses and defs
   * of the function value TNs. We only allow copies were the
   * ultimate source and destination TN are the same function value
   * TN (the copy may be through an interim local TN). If a
   * function value TN is used in any other way, we reject this case.
   */
  MEM_POOL_Push(&MEM_local_pool);
  fvals = hTN_MAP_Create(&MEM_local_pool);

  FOR_ALL_BB_OPs_FWD(exit_bb, op) {
    if (OP_copy(op)) {
      TN *src = OP_opnd(op,OP_COPY_OPND);
      TN *dst = OP_result(op,0);
      BOOL src_is_fval = Is_Function_Value(src);
      BOOL dst_is_fval = Is_Function_Value(dst);

      if (!src_is_fval) {
	src = (TN *) hTN_MAP_Get(fvals, src);
	if (src) src_is_fval = TRUE;
      }

      if (dst_is_fval) {
	if (   src_is_fval
	    && (TN_register_and_class(src) == TN_register_and_class(dst))
	) continue;
      } else if (src_is_fval) {
	hTN_MAP_Set(fvals, dst, src);
	continue;
      }
    }

    MEM_POOL_Pop(&MEM_local_pool);
    return NULL;
  }
  MEM_POOL_Pop(&MEM_local_pool);

#ifndef TARG_X8664
  /* If we had preemptible symbol for the callee, then change
   * its relocation so we avoid generating a stub for it.
   */
  if (addr_op) {
    TN *old_tn = OP_opnd(addr_op, addr_opnd);
    TN *new_tn = Dup_TN(old_tn);
    Set_TN_is_reloc_got_disp(new_tn);
    Set_OP_opnd(addr_op, addr_opnd, new_tn);
    EMT_Change_Symbol_To_Weak(call_st);
    Set_ST_is_weak_symbol(call_st);
  }
#endif

  return pred;
}


//
// Determine if there are some conditions for the whole PU that
// would disallow tail-call optimization. This acts as a first
// level filter before we analyze each exit block to see if there
// is a potential tail-call there.
//
static BOOL Can_Do_Tail_Calls_For_PU ()
{
  // If the PU performs dynamic stack allocations then it could somehow
  // pass a pointer to the stack to the tail-called routine.
  if (Current_PU_Stack_Model == SMODEL_DYNAMIC) return FALSE;

  // If PU has regions, cannot do tail-call optimization.
  if (PU_has_region(Get_Current_PU())) return FALSE;

  // If the PU calls a routine with the the following pragma:
  //	#pragma unknown_control_flow (func)
  // then we cannot do tail-call optimization for it.
  if (PU_has_unknown_control_flow (Get_Current_PU())) return FALSE;

  // If a PU has a call to setjmp and some other tail call from this
  // PU ultimately has a call to longjmp, then the stack frame will be
  // gone when we get back to the code following the setjmp. Therefore,
  // disable tail call opt for any PU with a setjmp. Also note that
  // setjmp can come in several variants (setjmp, _setjmp, sigsetjmp, etc)
  // so just look for any symbol ending in "setjmp" -- this may be
  // over cautious, but it's better to err on the safe side.
  for (BB *bb = REGION_First_BB; bb; bb = BB_next(bb)) {
    if (BB_call(bb)) {
      const char *name;
      INT len;
      ANNOTATION *callant = ANNOT_Get(BB_annotations(bb), ANNOT_CALLINFO);
      CALLINFO *callinfo = ANNOT_callinfo(callant);
      ST *st = CALLINFO_call_st(callinfo);
      if (st == NULL) continue;
      name = ST_name(st);
      len = strlen(name);
      if (len >= 6 && strcmp(name + len - 6, "setjmp") == 0) return FALSE;
      if (len >= 10 && strcmp(name+len-10, "getcontext") == 0) return FALSE;
    }
  }
  return TRUE;
}


/* ====================================================================
 *
 * Optimize_Tail_Calls
 *
 * Optimize tail calls for a PU by converting the sequence of a call
 * immediately followed by a return into a jump.
 *
 * Note that since we may reset PU_Has_Calls, it requires that
 * the tail call optimization occur before the normal entry/exit
 * processing because stack allocation and GP logic is intertwinned
 * with it.
 *
 * ====================================================================
 */
void
Optimize_Tail_Calls(ST *pu)
{
  BB_LIST *elist;
  BOOL have_tail_call = FALSE;

  // Don't optimize if disabled, -O0 or cannot do tail-calls for PU.
  // KEY:
  // But don't disable if it is a C++ thunk (bug 14606). C++ thunks
  // do not call their target from an exception region, so the thunk
  // should do a jmp instead of a call to the target function.
  if ((  !CG_tail_call
      || CG_opt_level == 0 
      || !Can_Do_Tail_Calls_For_PU ())
#ifdef KEY // bug 14606
      && !PU_is_thunk(Get_Current_PU())
#endif
     ) return;

  /* Check each of the exit blocks
   */
  for ( elist = Exit_BB_Head; elist; elist = BB_LIST_rest(elist) ) {
    BB *call_bb;
    BB *exit_bb = BB_LIST_first(elist);

    /* If we have a tail call we can optimize, then do so.
     */
    if (call_bb = Can_Be_Tail_Call(pu, exit_bb)) {
      OP *jmp_op;
      OP *call_op = BB_last_op(call_bb);
      Is_True(OP_call(call_op), ("call block didn't end in a call inst"));

      if (Trace_EE) {
	#pragma mips_frequency_hint NEVER
	fprintf(TFile, "%s<calls> call block before tail call optimization:\n",
			DBar);
	Print_BB(call_bb);
	fprintf(TFile, "\n<calls> exit block before tail call optimization:\n");
	Print_BB(exit_bb);
      }

      /* Replace the call OP with a jump.
       */
      jmp_op = EETARG_Build_Jump_Instead_Of_Call (call_op);
      Set_OP_tail_call(jmp_op);
      BB_Insert_Op_Before(call_bb, call_op, jmp_op);
      BB_Remove_Op(call_bb, call_op);

      /* Transfer the exit info from the exit block to the call block.
       * The call block becomes the new exit block. The exit block
       * is removed from the succ chain and will be removed by cflow later.
       */
      BB_Transfer_Exitinfo(exit_bb, call_bb);
      Unlink_Pred_Succ(call_bb, exit_bb);
      Exit_BB_Head = BB_LIST_Delete(exit_bb, Exit_BB_Head);
      Exit_BB_Head = BB_LIST_Push(call_bb, Exit_BB_Head, &MEM_pu_pool);
      Remove_BB(exit_bb);

      if (Trace_EE) {
	#pragma mips_frequency_hint NEVER
	fprintf(TFile, "\n<calls> exit block after tail call optimization:\n");
	Print_BB(call_bb);
      }

      have_tail_call = TRUE;
    }
  }

  /* If we optimized at least one tail call, then we might have
   * removed the last "normal" call, in which case we might not
   * need a stack. Reseting PU_Has_Calls will make the right
   * things happen.
   */
  if (have_tail_call) {
    BB *bb;
    for (bb = REGION_First_BB; bb; bb = BB_next(bb)) {
      if (BB_call(bb) && !BB_exit(bb)) goto done;
    }
    PU_Has_Calls = FALSE;
  done:
    ;
  }
}


/* ====================================================================
 *
 * Target_Unique_Exit
 *
 * 'bb' is an exit block. Modify it so that it jumps to the unique
 * return block. This would be simple except that the dedicated return
 * value TNs cannot be live out of this block.
 *
 * ====================================================================
 */
static void
Target_Unique_Exit (
  BB *bb,
  BB *unique_exit_bb,
  TN *rtn_tns[ISA_REGISTER_CLASS_MAX+1][REGISTER_MAX+1])
{
  OP *op;

  /* Scan the OPs (in reverse order) in the exit block and handle any 
   * reads of or writes to a function value TN.
   */
  FOR_ALL_BB_OPs_REV(bb, op) {
    INT i;

    /* Handle writes to a function value TN.
     */
    for (i = OP_results(op) - 1; i >= 0; --i) {
      TN *tn = OP_result(op,i);
      if ( Is_Function_Value(tn) ) {
	TN *new_tn;
	REGISTER reg = TN_register(tn);
	ISA_REGISTER_CLASS rc = TN_register_class(tn);

	/* We have a write to a function value TN. These are special
	 * and can't be live out of this block, so we must make a copy.
	 * Get the TN for the copy.
	 */
	new_tn = rtn_tns[rc][reg];
	if ( new_tn == NULL ) {
	  new_tn = Dup_TN_Even_If_Dedicated(tn);
	  if (TN_is_float(tn)) {
	    /* For bug#478
	       OP_result_size measures a TN size in bits; yet
	       TN_size measures a TN size in bytes.
	     */
	    INT tn_size = OP_result_size(op,i) / 8;
#ifdef KEY
	    // When storing a value into a function return register where the
	    // size of the value is smaller than the size of the return
	    // register, Handle_STID will store the value into a temp TN and
	    // copy the TN to the return register.  Get the size from this TN
	    // since it has the correct size.  The correct size is needed to
	    // select the correct move OP code when copying from the temp TN to
	    // the return register in the exit BB.  Bug 14259.
	    if (OP_copy(op))
	      tn_size = TN_size(OP_opnd(op, OP_COPY_OPND));
#endif
	    Set_TN_size(new_tn, tn_size);
	  }
	}

	/* Adjust the result TN of the original function value write.
	 */
	Set_OP_result(op, i, new_tn);

	/* If this is the first time we've encountered this function
	 * value TN then make sure it's set, from the copy,
	 * in the new unique return block.
	 */
	if ( rtn_tns[rc][reg] == NULL ) {
	  ANNOTATION *ant = ANNOT_Get (BB_annotations(unique_exit_bb),
				       ANNOT_EXITINFO);
	  EXITINFO *exit_info = ANNOT_exitinfo(ant);
	  OPS ops = OPS_EMPTY;
	  rtn_tns[rc][reg] = new_tn;
	  Exp_COPY(tn, new_tn, &ops);
	  OP_srcpos(OPS_last(&ops)) = EXITINFO_srcpos(exit_info);
	  BB_Prepend_Ops(unique_exit_bb, &ops);
	}
      }
    }

    /* A select or unaligned load may have a use of func value TN.
     */
    if ( OP_same_res(op) ) {
      INT i;

      for ( i = 0; i < OP_opnds(op); ++i ) {
	TN *tn = OP_opnd(op,i);
	if ( Is_Function_Value(tn) ) {
	  REGISTER reg = TN_register(tn);
	  ISA_REGISTER_CLASS rc = TN_register_class(tn);
	  TN *new_tn = rtn_tns[rc][reg];
	  FmtAssert(new_tn, ("use before def of return value TN in BB:%d",
			    BB_id(bb)));
	  Set_OP_opnd(op, i, new_tn);
	}
      }
    }
  }

  /* Make sure this exit gets to the unique exit.
   */
  if ( BB_next(bb) == unique_exit_bb ) {
    Target_Simple_Fall_Through_BB(bb, unique_exit_bb);
  } else {
    Add_Goto(bb, unique_exit_bb);
  }
}


/* ====================================================================
 *
 * Generate_Unique_Exit
 *
 * Generate a unique exit and re-target all existing exits to it.
 *
 * ====================================================================
 */
static void
Generate_Unique_Exit(void)
{
  BB_LIST *elist;
  BB *unique_exit_bb;
  INT exits;

  /* Keep track of things we need for creating a unique exit from a PU.
   * We track the return value TNs as well as the BB containing the
   * real exit.
   */
  TN *rtn_tns[ISA_REGISTER_CLASS_MAX+1][REGISTER_MAX+1];

  /* Should we generate a unique exit?
   */
  if (   !CG_unique_exit
      || PU_has_region(Get_Current_PU())) /* until we're sure it works with
					  * regions.
					  */
  {
    return;
  }

  /* If there is less than 2 simple (not tail-call) exits, then this is
   * a pointless exercise.
   */
  exits = 0;
  for ( elist = Exit_BB_Head; elist; elist = BB_LIST_rest(elist) ) {
    BB *bb = BB_LIST_first(elist);
    exits += !BB_call(bb);
  }
  if (exits < 2) return;

  /* Generate the unique exits.
   */
  bzero(rtn_tns, sizeof(rtn_tns));
  unique_exit_bb = NULL;
  for ( elist = Exit_BB_Head; elist; elist = BB_LIST_rest(elist) ) {
    BB *bb = BB_LIST_first(elist);

    /* Exclude tail call blocks
     */
    if (BB_call(bb)) continue;

    /* This block will no longer be a exit block. So for the first
     * block, transfer the exitinfo to the unique exit block. For
     * the others, just remove the exitinfo.
     */
    if (unique_exit_bb == NULL) {
      unique_exit_bb = Gen_And_Insert_BB_After(bb);
      BB_rid(unique_exit_bb) = BB_rid(bb);
      BB_Transfer_Exitinfo(bb, unique_exit_bb);
      Exit_BB_Head = BB_LIST_Push(unique_exit_bb, Exit_BB_Head, &MEM_pu_pool);
    } else {
      ANNOTATION *ant = ANNOT_Get(BB_annotations(bb), ANNOT_EXITINFO);
      BB_annotations(bb) = ANNOT_Unlink(BB_annotations(bb), ant);
      Reset_BB_exit(bb);
    }
    Exit_BB_Head = BB_LIST_Delete(bb, Exit_BB_Head);

    /* Target the unique exit block.
     */
    Target_Unique_Exit(bb, unique_exit_bb, rtn_tns);
  }
}

#ifdef TARG_X8664
void Adjust_SP_After_Call( BB* bb )
{
  OP* op = BB_last_op(bb);
  if( op == NULL || !OP_call( op ) )
    return;

  const ANNOTATION* ant = ANNOT_Get( BB_annotations(bb), ANNOT_CALLINFO );
  const CALLINFO* call_info = ANNOT_callinfo(ant);
  const ST* call_st = CALLINFO_call_st(call_info);
  const WN* call_wn = CALLINFO_call_wn(call_info);
  const TY_IDX call_ty = call_st != NULL ? ST_pu_type(call_st) : WN_ty(call_wn);
  const BOOL ff2c_abi =
    call_st != NULL ? PU_ff2c_abi( Pu_Table[ST_pu(call_st)] ) : FALSE;
  const RETURN_INFO return_info = Get_Return_Info( TY_ret_type(call_ty),
						   No_Simulated,
						   ff2c_abi );

  /* The C++ front-end will add the first fake param, then convert the
     function return type to void. (bug#2424)
   */
  if( RETURN_INFO_return_via_first_arg(return_info) ||
      TY_return_to_param( call_ty ) ){
    if (call_st != NULL && strncmp(ST_name(call_st), "_TRANSFER", 9) == 0)
      return; // bug 6153
    OPS ops = OPS_EMPTY;
    Exp_SUB( Pointer_Mtype, SP_TN, SP_TN, Gen_Literal_TN(4,0), &ops );
    BB_Append_Ops( bb, &ops );

    if( Trace_EE ){
#pragma mips_frequency_hint NEVER
      fprintf( TFile, "%sDecrease SP by 4 bytes after call in BB:%d\n",
	       DBar, BB_id(bb) );
      Print_OPS( &ops );
    }
  }
}
#endif


/* ====================================================================
 *
 * Generate_Exit
 *
 * Generate exit code (in SGIR form) in the given BB for the given
 * subprogram.  This currently includes the following:
 *
 *   1) Insert an update of the stack pointer.
 *
 *   2) Restore GP if needed.
 *
 *   3) If <gra_run>, then GRA will be run and each of the callee_saves
 *	registers is copied its save TN.
 *
 * ====================================================================
 */

static void
Generate_Exit (
  ST *st,		/* The subprogram's symtab entry */
  BB *bb,		/* The exit BB to receive code */
  BOOL gra_run,         /* Make the preferencing copies for GRA */
  BOOL is_region)       /* Is a nested region */

{
  INT callee_num;
  TN *stn;
  OP *op;
  OPS ops = OPS_EMPTY;
  ANNOTATION *ant = ANNOT_Get (BB_annotations(bb), ANNOT_EXITINFO);
  EXITINFO *exit_info = ANNOT_exitinfo(ant);
  BB *bb_epi;

  if ( is_region && gra_run ) {
    /* get out if region and running gra.  epilog code handled with
     * PU in gra.
     */
    return;
  }

  if ((BB_rid(bb) != NULL) && ( RID_level(BB_rid(bb)) >= RL_CGSCHED )) {
    if (gra_run) {
      /* if the exit is from a region, then we will create a new block
       * that is part of the PU to hold the exit code for gra.
       */
      bb_epi = Gen_And_Insert_BB_After(bb);
      BB_Transfer_Exitinfo(bb, bb_epi);
      Target_Simple_Fall_Through_BB(bb,bb_epi);
      BB_rid(bb_epi) = Current_Rid;
      Exit_BB_Head = BB_LIST_Delete(bb, Exit_BB_Head);
      Exit_BB_Head = BB_LIST_Push(bb_epi, Exit_BB_Head, &MEM_pu_pool);
    } else {
      /* if gra is not being run, then the epilog code has been added
       * to the return block.
       */
      return;
    }
  } else {
    bb_epi = bb;
  }

  if ( Trace_EE ) {
    #pragma mips_frequency_hint NEVER
    fprintf ( TFile,
	      "\n%s<calls> Generating exit for %s (BB:%d)\n",
	      DBar, ST_name(st), BB_id(bb_epi) );
  }

  /* Restore the caller's GP: */
  if (GP_Setup_Code == need_code) {
    /* we will put the gp in any available register if a leaf routine
     * to avoid the save/restore of the gp.  we will also make gp
     * available as a callee saved register so that in the worst case,
     * we will not generate worse code by using another register for
     * gp.  we don't yet have the information necessary to perform
     * this optimization in the presence of regions.
     */
    if ( !Use_Scratch_GP( TRUE ) ) {
      if ( Caller_GP_TN == NULL ) {
	if ( stn = PREG_To_TN_Array[ Caller_GP_Preg ] )
	  Caller_GP_TN = stn;
	else {
	  Caller_GP_TN = Gen_Register_TN (
		ISA_REGISTER_CLASS_integer, Pointer_Size);
	  TN_MAP_Set( TN_To_PREG_Map, Caller_GP_TN, (void *)Caller_GP_Preg );
	  PREG_To_TN_Array[ Caller_GP_Preg ] = Caller_GP_TN;
	  PREG_To_TN_Mtype[ Caller_GP_Preg ] = TY_mtype(Spill_Int_Type);
	}
      }
      Exp_COPY (GP_TN, Caller_GP_TN, &ops);
    }
  }

  if ( gra_run )
    EETARG_Restore_Extra_Callee_Tns (&ops);

#ifdef TARG_X8664
  if( Is_Target_32bit() && Gen_PIC_Shared ){
    EETARG_Generate_PIC_Exit_Code( bb_epi, &ops );
  }
#endif

  if (NULL != RA_TN) {
    if ( PU_has_return_address(Get_Current_PU()) ) {
      /* If the return address builtin is required, restore RA_TN from the 
       * memory location for __return_address. 
       */
      ST *ra_sv_sym = Find_Special_Return_Address_Symbol();
      TN *ra_sv_tn = Build_TN_Like(RA_TN);
      Set_TN_save_creg (ra_sv_tn, TN_class_reg(RA_TN));
      Set_TN_spill(ra_sv_tn, ra_sv_sym);
      if (MTYPE_byte_size(Pointer_Mtype) < MTYPE_byte_size(Spill_Int_Mtype) ) {
  	/* In n32 the return_address is 4 bytes (pointer),
  	 * but we need 8-byte save/restore to make kernel and dbx happy. */
  	ra_sv_sym = ST_base(ra_sv_sym);		/* use 8-byte block */
  	Set_TN_spill(ra_sv_tn, ra_sv_sym);	/* so dwarf uses new addr */
      }
      CGSPILL_Load_From_Memory (ra_sv_tn, ra_sv_sym, &ops, CGSPILL_LCL, bb_epi);
      Exp_COPY (RA_TN, ra_sv_tn, &ops);
    }
    else {
      if (gra_run && PU_Has_Calls 
	&& TN_register_class(RA_TN) != ISA_REGISTER_CLASS_integer)
      {
	// because has calls, gra will need to spill this.
	// but if it is not already in an integer reg,
	// then gra will spill to memory whereas for ia64
	// it could use a stacked reg; ideally gra would handle
	// this, but it doesn't and is easy to just copy to int reg
	// by hand and then let gra use stacked reg.
	Exp_COPY (RA_TN, ra_intsave_tn, &ops );
      } else {
        /* Copy back the return address register from the save_tn. */
      	Exp_COPY ( RA_TN, SAVE_tn(Return_Address_Reg), &ops );
      }
      Set_OP_no_move_before_gra(OPS_last(&ops));
    }
  }

  if ( gra_run ) {
    /* Copy from register TNs to the callee saves registers */
    for ( callee_num = 0; callee_num < Callee_Saved_Regs_Count; ++callee_num ) {
      TN *callee_tn = CALLEE_tn(callee_num);
      if (    TN_is_save_reg(callee_tn) 
	  && !REGISTER_CLASS_multiple_save(TN_register_class(callee_tn)))
      {
        Exp_COPY ( CALLEE_ded_tn(callee_num), callee_tn, &ops );
	Set_OP_no_move_before_gra(OPS_last(&ops));
      }
    }
  }

#ifdef KEY
  /* restore callee-saved registers allocated to local user variables */
  for (INT i = 0; i < Saved_Callee_Saved_Regs.Elements(); i++) {
    SAVE_REG_LOC sr = Saved_Callee_Saved_Regs.Top_nth(i);
#ifdef TARG_X8664
    if (sr.temp == NULL)
      continue; // handled by push/pop under CG_push_pop_int_saved_regs
#endif
    if (! sr.user_allocated)
      continue;
    /* generate the reload ops */
    CGSPILL_Load_From_Memory (sr.ded_tn, sr.temp, &ops, CGSPILL_LCL, bb_epi);
    Set_OP_no_move_before_gra(OPS_last(&ops));
  }
#endif

  if (PU_Has_Calls) {
  	EETARG_Restore_Pfs (Caller_Pfs_TN, &ops);
  }

  /* Restore the stack pointer.
   */
  if ( Gen_Frame_Pointer && !PUSH_FRAME_POINTER_ON_STACK ) {
    Exp_COPY (SP_TN, FP_TN, &ops);
  } 
  else {

    /* This is a placeholder; Adjust_Exit will replace it with the 
     * actual sequence once we know the size of the frame.
     */
    Exp_Spadjust (SP_TN, Frame_Len_TN, V_NONE, &ops);
  }
  EXITINFO_sp_adj(exit_info) = OPS_last(&ops);

  /* Restore the caller's frame pointer register if we used FP: */
  if ( Gen_Frame_Pointer 
        && !PUSH_FRAME_POINTER_ON_STACK
        && ABI_PROPERTY_Is_callee (
	TN_register_class(FP_TN),
	REGISTER_machine_id(TN_register_class(FP_TN), TN_register(FP_TN)) ))
  {
    if ( Caller_FP_TN == NULL ) {
      if ( stn = PREG_To_TN_Array[ Caller_FP_Preg ] )
	Caller_FP_TN = stn;
      else {
	// Bug 13316: FP can hold 64 bit (non pointer) value
	Caller_FP_TN = Gen_Register_TN (
		ISA_REGISTER_CLASS_integer, TY_size(Spill_Int_Type));
	TN_MAP_Set( TN_To_PREG_Map, Caller_FP_TN, (void *)Caller_FP_Preg );
	PREG_To_TN_Array[ Caller_FP_Preg ] = Caller_FP_TN;
  	PREG_To_TN_Mtype[ Caller_FP_Preg ] = TY_mtype(Spill_Int_Type);
      }
    }
    Exp_COPY (FP_TN, Caller_FP_TN, &ops);
    Set_OP_no_move_before_gra(OPS_last(&ops));
  }

  /* Generate the return instruction, unless is this a tail call
   * block, in which case the xfer instruction is already there.
   */
  if (!BB_call(bb_epi)) { 
#ifdef TARG_X8664
    int sp_adjust = 0;

    if( Is_Target_32bit() ){
      const TY_IDX call_ty = ST_pu_type(st);
      const BOOL ff2c_abi = PU_ff2c_abi(Pu_Table[ST_pu(st)]);
      const RETURN_INFO return_info = Get_Return_Info( TY_ret_type(call_ty),
						       No_Simulated,
						       ff2c_abi );
      if( RETURN_INFO_return_via_first_arg(return_info) ||
	  TY_return_to_param( call_ty ) ){
	sp_adjust = Pointer_Size;
      }
    }

    Exp_Return( RA_TN, sp_adjust, &ops );
#else
    Exp_Return (RA_TN, &ops);
#endif // TARG_X8664
  }

  /* set the srcpos field for all the exit OPs */
  FOR_ALL_OPS_OPs(&ops, op)
    OP_srcpos(op) = EXITINFO_srcpos(exit_info);

  /* If we're tracing, print the new stuff before merging it: */
  if ( Trace_EE ) {
    #pragma mips_frequency_hint NEVER
    Print_OPS(&ops);
  }

  /* Merge the new operations into the end of the exit BB: */
  if (BB_call(bb_epi)) {

    /* If it's a tail call block we insert the new operations in front
     * of the jump.
     */
    OP *point = BB_last_op(bb_epi);
    Is_True(OP_br(point), ("last tail call OP of BB:%d not a jump", BB_id(bb_epi)));
    BB_Insert_Ops_Before(bb_epi, point, &ops);
  } else {
    BB_Append_Ops(bb_epi, &ops);
  }
}

// sets spadjust TN values
extern void 
Set_Frame_Len (INT64 val)
{
#ifdef TARG_X8664
  if (CG_min_stack_size &&
      !Stack_Frame_Has_Calls()) {	// Align stack before calls.  Bug 8017.
    extern BOOL Is_Stack_Used();
    if( !Is_Stack_Used() )
      val = 0;
  }
#endif

  Frame_Len = val;
  Set_TN_value(Frame_Len_TN, val);
  Set_TN_value(Neg_Frame_Len_TN, -val);
}

/* we now generate the final code after pu is processed,
 * but still need to init some stuff before processing pu. */
void
Init_Entry_Exit_Code (WN *pu_wn)
{
  Trace_EE = Get_Trace ( TP_CGEXP, 64 );

  GP_Setup_Code = undefined_code;
  Caller_GP_TN = NULL;
  Caller_FP_TN	= NULL;

  // initialize values to dummy value
  Frame_Len_TN = Gen_Unique_Literal_TN(0,8);
  Neg_Frame_Len_TN = Gen_Unique_Literal_TN(0,8);

  Gen_Frame_Pointer = (Current_PU_Stack_Model != SMODEL_SMALL);
#ifdef TARG_X8664
  if (Opt_Level == 0 || Force_Frame_Pointer || Call_Mcount ||
      Debug_Level > 0)
    Gen_Frame_Pointer = TRUE;// because return address always stored at offset 0
#endif
#ifdef KEY
  Saved_Callee_Saved_Regs.Clear();
#endif

  // target-specific code (e.g. for stacked registers)
  EETARG_Init_Entry_Exit_Code (pu_wn, Gen_Frame_Pointer);

  Init_Pregs ();

  LC_Used_In_PU = FALSE;
}


/* ====================================================================
 *
 * Generate_Entry_Exit_Code
 *
 * Generate entry and exit code for the current PU.  This involves
 * calling Generate_Entry for each entry BB, and Generate_Exit
 * for each exit BB.
 *
 * ====================================================================
 */

void
Generate_Entry_Exit_Code ( ST *pu, BOOL is_region )
{
  BB_LIST *elist;
  BOOL gra_run = ! CG_localize_tns;

  Is_True(pu != NULL,("Generate_Entry_Exit_Code, null PU ST"));
  /* assume GP_Setup_Code already set */

  for ( elist = Entry_BB_Head; elist; elist = BB_LIST_rest(elist) ) {
    Generate_Entry (BB_LIST_first(elist), gra_run );
  }

  Generate_Unique_Exit ();

  for ( elist = Exit_BB_Head; elist; elist = BB_LIST_rest(elist) ) {
    Generate_Exit (pu, BB_LIST_first(elist), gra_run, is_region );
  }

 if (GP_Setup_Code == need_code && !is_region)
	/* don't change setting till done with regions */
	GP_Setup_Code = final_code;
}


static void
Adjust_GP_Entry(BB *bb)
{
  if (GP_Setup_Code == need_code) {
	/* added gp reference after usual gp setup time,
	 * so now need to add in gp setup. */
  	/* Save the old GP and setup a new GP */
	/* Create a symbolic expression for ep-gp */
	ST *st = ENTRYINFO_name(	/* The entry's symtab entry */
		ANNOT_entryinfo(ANNOT_Get(BB_annotations(bb),ANNOT_ENTRYINFO)));
	TN *got_disp_tn = Gen_Register_TN (
		ISA_REGISTER_CLASS_integer, Pointer_Size);

	ST *mem_loc = CGSPILL_Get_TN_Spill_Location (GP_TN, CGSPILL_LCL);
	OPS ops = OPS_EMPTY;
	TN *cur_pu_got_disp_tn = Gen_Symbol_TN(st, 0, TN_RELOC_GPSUB);

	/* Generate the spill ops */
	CGSPILL_Store_To_Memory (GP_TN, mem_loc, &ops, CGSPILL_LCL, bb);

	// text addresses are always 32bits
	Set_TN_size(cur_pu_got_disp_tn, 4);  
	Exp_OP1 (OPC_I4INTCONST, got_disp_tn, cur_pu_got_disp_tn, &ops);

    	/* Add it to ep to get the new GP: */
    	Exp_ADD (Pointer_Mtype, GP_TN, Ep_TN, got_disp_tn, &ops);

	/* allocate registers for any temps used in spill sequence */
	// Assign_Temp_Regs (&ops, bb);

	/* insert the ops in the op list for the current BB */
	CGSPILL_Prepend_Ops (bb, &ops);

	if (Trace_EE) {
		#pragma mips_frequency_hint NEVER
    		fprintf(TFile, "%s<calls> Insert spill and setup of GP for BB:%d\n", DBar, BB_id(bb));
		Print_OPS(&ops);
	}
  }
}

static void
Adjust_GP_Exit (BB *bb)
{
  if (GP_Setup_Code == need_code) {
	/* added gp reference after usual gp setup time,
	 * so now need to add in gp setup. */
	ST *mem_loc = CGSPILL_Get_TN_Spill_Location (GP_TN, CGSPILL_LCL);
	OPS ops = OPS_EMPTY;

	/* generate the reload ops */
	CGSPILL_Load_From_Memory (GP_TN, mem_loc, &ops, CGSPILL_LCL, bb);
	/* allocate registers for any temps used in spill sequence */
	// Assign_Temp_Regs (&ops, bb);

	/* insert the ops in the op list for the current BB */
	CGSPILL_Append_Ops (bb, &ops);

	if (Trace_EE) {
		#pragma mips_frequency_hint NEVER
    		fprintf(TFile, "%s<calls> Insert restore of GP for BB:%d\n", DBar, BB_id(bb));
		Print_OPS (&ops);
	}
  }
}

/* possibly add the GP setup code */
void
Adjust_GP_Setup_Code (ST *pu, BOOL allocate_registers)
{
  BB_LIST *elist;

  if (GP_Setup_Code == no_code && PU_References_GP) {
	/*
	 * This can happen for several reasons:
	 * 1) when regions, don't see full PU at first;
	 * 2) spilling a fcc introduces a gp reference;
	 * 3) rematerialization may use gp.
	 * So we can introduce GP references as late as LRA.
	 * In this case, we have to add the gp setup/save/restore code,
	 * and allocate registers for it (since after GRA/LRA).
	 * The placement may not be optimal, but GRA can't shrinkwrap
	 * anyways since new uses may occur after GRA.
	 */
	/*
	 * CHANGE:  setup gp after cgprep, and don't allow new gp
	 * uses from fcc spills.  This way LRA sees that t9 and gp
	 * are used.
	 */
	FmtAssert(!allocate_registers, ("Created new GP reference during register allocation"));
	// Is_True(allocate_registers, ("expect to allocate registers now"));
	GP_Setup_Code = need_code;
  }
  else return;

  for (elist = Entry_BB_Head; elist; elist = BB_LIST_rest(elist)) {
    Adjust_GP_Entry(BB_LIST_first(elist));
  }

  for (elist = Exit_BB_Head; elist; elist = BB_LIST_rest(elist)) {
    Adjust_GP_Exit(BB_LIST_first(elist));
  }

  if (GP_Setup_Code == need_code)
	GP_Setup_Code = final_code;
}


static void
Adjust_LC_Entry (BB *bb)
{
  OP *op;
  INT i;
  FOR_ALL_BB_OPs_FWD(bb, op) {
    for ( i = 0; i < OP_opnds(op); ++i ) {
	if (OP_opnd(op,i) == LC_TN && OP_no_move_before_gra(op)) {
    		BB_Remove_Op(bb, op);
		if (Trace_EE) {
			#pragma mips_frequency_hint NEVER
    			fprintf(TFile, "<calls> remove save of LC in BB:%d\n", BB_id(bb));
		}
	}
    }
  }
}

static void
Adjust_LC_Exit (BB *bb)
{
  OP *op;
  INT i;
  FOR_ALL_BB_OPs_FWD(bb, op) {
    for ( i = 0; i < OP_results(op); ++i ) {
	if (OP_result(op,i) == LC_TN && OP_no_move_before_gra(op)) {
    		BB_Remove_Op(bb, op);
		if (Trace_EE) {
			#pragma mips_frequency_hint NEVER
    			fprintf(TFile, "<calls> remove restore of LC in BB:%d\n", BB_id(bb));
		}
	}
    }
  }
}

/* possibly remove save/restore of LC. */
/* Must call this after cgprep */
void
Adjust_LC_Setup_Code (void)
{
  if (LC_TN == NULL) return;	// doesn't exist for target
  if (LC_Used_In_PU) return;	// keep save/restore
  if (CG_localize_tns)	return;	// never generated initial save/restore

  BB_LIST *elist;
  for (elist = Entry_BB_Head; elist; elist = BB_LIST_rest(elist)) {
    Adjust_LC_Entry(BB_LIST_first(elist));
  }
  for (elist = Exit_BB_Head; elist; elist = BB_LIST_rest(elist)) {
    Adjust_LC_Exit(BB_LIST_first(elist));
  }
}


/* ====================================================================
 *
 * Assign_Prolog_Temps
 *
 * Very simplistic register allocation. Intended soley for use
 * on the sequence of insts to load the stack frame size constant.
 * TODO:  merge this with Assign_Temp_Regs.
 *
 * ====================================================================
 */
static void
Assign_Prolog_Temps(OP *first, OP *last, REGISTER_SET *temps)
{
  OP *op;
  ISA_REGISTER_CLASS cl;
  REGISTER reg;

  /* Assume we will only need one register and that its class will
   * be the same as $sp. Reserve the register now. Note that no
   * checking is performed to validate this assumption.
   */
  cl = TN_register_class(SP_TN);
  reg = REGISTER_SET_Choose(temps[cl]);
  FmtAssert(reg != REGISTER_UNDEFINED, ("no free temps"));
  temps[cl] = REGISTER_SET_Difference1(temps[cl], reg);

  /* Loop over the OPs in the sequence, allocating registers.
   */
  for (op = first; op != OP_next(last); op = OP_next(op)) {
    TN *tn;
    INT k;

    REGISTER_CLASS_OP_Update_Mapping(op);

    for (k = 0; k < OP_results(op); k++) {
      tn = OP_result(op,k);
      if (TN_register(tn) == REGISTER_UNDEFINED) {
	FmtAssert(TN_register_class(tn) == cl,
		  ("unexpected register class for unallocated register"));
	TN_Allocate_Register(tn, reg);
      }
    }

    for (k = 0; k < OP_opnds(op); k++) {
      tn = OP_opnd(op,k);
      if (TN_is_register(tn) && TN_register(tn) == REGISTER_UNDEFINED) {
	FmtAssert(TN_register_class(tn) == cl,
		  ("unexpected register class for unallocated register"));
	TN_Allocate_Register(tn, reg);
      }
    }
  }
}


/* ====================================================================
 *
 * Gen_Prolog_LDIMM64
 *
 * Generate an instruction sequence to load an arbitrary 64-bit value
 *
 * ====================================================================
 */
static TN *
Gen_Prolog_LDIMM64(UINT64 val, OPS *ops)
{
  TN *src = Gen_Literal_TN(val, 8);
  TN *result = Build_TN_Of_Mtype (MTYPE_I8);

  Exp_Immediate (result, src, TRUE, ops);

  return result;
}

/* ====================================================================
 *
 * Adjust_Entry
 *
 * Adjust the stack frame allocation code as necessary now that the
 * actual frame size is known.
 *
 * ====================================================================
 */
static void
Adjust_Entry(BB *bb)
{
  UINT64 frame_len = Frame_Len;
  ANNOTATION *ant = ANNOT_Get(BB_annotations(bb), ANNOT_ENTRYINFO);
  ENTRYINFO *ent_info = ANNOT_entryinfo(ant);
  OP *ent_adj = ENTRYINFO_sp_adj(ent_info);
  OP *fp_adj;
  OP *sp_adj;
  TN *sp_incr;
  TN *fp_incr;

  if (BB_handler(bb)) return;

  if (Trace_EE) {
    #pragma mips_frequency_hint NEVER
    fprintf(TFile,
	    "\n%s<calls> Adjusting entry for %s (BB:%d)\n",
	    DBar, ST_name(ENTRYINFO_name(ent_info)), BB_id(bb));
    fprintf(TFile, "\nFinal frame size: %" SCNd64 " (0x%" SCNx64 ")\n", frame_len, frame_len);
  }

#ifdef TARG_X8664
  if (CG_push_pop_int_saved_regs && ! Gen_Frame_Pointer) {
    OPS ops = OPS_EMPTY;
    for (INT i = 0; i < Saved_Callee_Saved_Regs.Elements(); i++) {
      SAVE_REG_LOC sr = Saved_Callee_Saved_Regs.Top_nth(i);
      if (sr.temp != NULL)
	continue;
      Build_OP(Is_Target_64bit() ? TOP_pushq : TOP_pushl, SP_TN, sr.ded_tn, 
      	       SP_TN, &ops);
    }
    BB_Insert_Ops_Before(bb, ent_adj, &ops);
  }
#endif

  /* The ENTRYINFO annotation identifies the last instruction of the
   * stack frame allocation sequence. Therefore the instruction could
   * be either the adjustment of SP or FP. Find both the FP and SP
   * adjust OPs (will be the same when we have a virtual FP).
   */
  sp_adj = ent_adj;
  fp_adj = sp_adj;
  if ( Gen_Frame_Pointer && !PUSH_FRAME_POINTER_ON_STACK ) {
    do {
      sp_adj = OP_prev(sp_adj);
      //
      // Spills can be introduced now by GRA.  Skip 'em.
      //
    } while (!OP_result(sp_adj, 0) || !TN_is_sp_reg(OP_result(sp_adj,0)));
  }

  /* Get the operands that are the frame size increment.
   */
  sp_incr = OP_opnd(sp_adj, OP_find_opnd_use(sp_adj, OU_opnd2));
  fp_incr = (fp_adj != sp_adj) 
	? OP_opnd(fp_adj, OP_find_opnd_use(fp_adj, OU_opnd2))
	: NULL;

  /* Trace the sequence before we modify it.
   */
  if (Trace_EE) {
    #pragma mips_frequency_hint NEVER
    fprintf(TFile, "\nOld stack frame allocation:\n");
    Print_OP_No_SrcLine(sp_adj);
    if (fp_adj != sp_adj) Print_OP_No_SrcLine(fp_adj);
  }

  /* We make assumptions about what we generated in Generate_Entry.
   * Try to make sure our assumptions were right:
   *   SP-adjust: TOP_spadjust $sp, $sp, -Frame_Len_TN
   *   FP-adjust: TOP_spadjust $fp, $sp, Frame_Len_TN
   */
  FmtAssert(   OP_code(sp_adj) == TOP_spadjust
	    && OP_results(sp_adj) == 1
	    && TN_is_sp_reg(OP_result(sp_adj,0))
	    && TN_is_sp_reg(OP_opnd(sp_adj,
		OP_find_opnd_use(sp_adj, OU_opnd1)))
	    && sp_incr == Neg_Frame_Len_TN
	    && ( ! OP_has_predicate(sp_adj) 
		|| OP_opnd(sp_adj, OP_PREDICATE_OPND) == True_TN), 
	    ("Unexpected form of entry SP-adjust OP"));
  if (fp_adj != sp_adj) {
    FmtAssert(   OP_code(fp_adj) == TOP_spadjust 
	      && OP_results(fp_adj) == 1
	      /* && OP_result(fp_adj,0) == FP_TN */
	      && TN_is_dedicated_class_and_reg (OP_result(fp_adj,0), 
					TN_register_and_class(FP_TN))
	      && TN_is_sp_reg(OP_opnd(fp_adj,
		OP_find_opnd_use(fp_adj, OU_opnd1)))
	      && fp_incr != NULL 
	      && fp_incr == Frame_Len_TN 
	      && ( ! OP_has_predicate(fp_adj) 
		  || OP_opnd(fp_adj, OP_PREDICATE_OPND) == True_TN), 
	      ("Unexpected form of entry FP-adjust OP"));
  }

  /* Perform any adjustments. We will either remove, change, or
   * leave alone, the adjustment OP.
   */
  if (frame_len == 0) {
    BB_Remove_Op(bb, sp_adj);

    if (sp_adj == fp_adj) {
      ent_adj = NULL;
    } else {
      OPS ops = OPS_EMPTY;

      /* Replace the FP adjust placeholder with the new adjustment OP.
       * Note that we just do this for an artifical situation that
       * at one time could be created with -TENV:large_stack, and in
       * in fact even that currently doesn't cause a problem. But just
       * in case, we make the case work.
       */
      Exp_COPY(FP_TN, SP_TN, &ops);
      ent_adj = OPS_last(&ops);
      OP_srcpos(ent_adj) = OP_srcpos(fp_adj);
      BB_Insert_Ops_Before(bb, fp_adj, &ops);
      BB_Remove_Op(bb, fp_adj);
    }

    if (Trace_EE) {
      #pragma mips_frequency_hint NEVER
      fprintf(TFile, "\nNew stack frame allocation:\n"
		     "-- removed --\n");
    }
  } else {
    OPS ops = OPS_EMPTY;
    TN *incr;
    OP *op;

    /* Load the increment into a register if it is too large to
     * be used as an immediate operand.
     */
    if (!CGTARG_Can_Fit_Immediate_In_Add_Instruction (frame_len)) {
      REGISTER_SET temps[ISA_REGISTER_CLASS_MAX+1];

      /* Get the frame size into a register
       */
      REG_LIVE_Prolog_Temps(bb, sp_adj, fp_adj, temps);
      if (Trace_EE) {
	#pragma mips_frequency_hint NEVER
	ISA_REGISTER_CLASS cl;

	fprintf(TFile, "\nInteger temp register usage at prolog SP adjust:\n");
	FOR_ALL_ISA_REGISTER_CLASS(cl) {
	  if (cl == TN_register_class(SP_TN)) {
	    fprintf(TFile, "  avail=");
	    REGISTER_SET_Print(temps[cl], TFile);
	    fprintf(TFile, "\n");
	  }
	}
      }

      incr = Gen_Prolog_LDIMM64(frame_len, &ops);
      Assign_Prolog_Temps(OPS_first(&ops), OPS_last(&ops), temps);
    } else {

      /* Use the frame size symbol
       */
      incr = Frame_Len_TN;
    }

    /* Replace the SP adjust placeholder with the new adjustment OP
     */
    Exp_SUB (Pointer_Mtype, SP_TN, SP_TN, incr, &ops);
    FOR_ALL_OPS_OPs_FWD(&ops, op) OP_srcpos(op) = OP_srcpos(sp_adj);
    ent_adj = OPS_last(&ops);
    BB_Insert_Ops_Before(bb, sp_adj, &ops);
    BB_Remove_Op(bb, sp_adj);
    if (Trace_EE) {
      #pragma mips_frequency_hint NEVER
      fprintf(TFile, "\nNew stack frame allocation:\n");
      FOR_ALL_OPS_OPs_FWD(&ops, op) Print_OP_No_SrcLine(op);
    }

    if (fp_adj != sp_adj) {

      /* Replace the FP adjust placeholder with the new adjustment OP
       */
      Exp_ADD (Pointer_Mtype, FP_TN, SP_TN, incr, OPS_Init(&ops));
      ent_adj = OPS_last(&ops);
      OP_srcpos(ent_adj) = OP_srcpos(fp_adj);
      BB_Insert_Ops_Before(bb, fp_adj, &ops);
      BB_Remove_Op(bb, fp_adj);
      if (Trace_EE) {
	#pragma mips_frequency_hint NEVER
      	FOR_ALL_OPS_OPs_FWD(&ops, op) Print_OP_No_SrcLine(op);
      }
    }
  }

  /* Point to the [possibly] new SP adjust OP
   */
  ENTRYINFO_sp_adj(ent_info) = ent_adj;

  // possible do target-dependent fixups
  EETARG_Fixup_Entry_Code (bb);
}


/* ====================================================================
 *
 * Adjust_Exit
 *
 * Adjust the stack frame de-allocation code as necessary now that the
 * actual frame size is known.
 *
 * ====================================================================
 */
static void
Adjust_Exit(ST *pu_st, BB *bb)
{
  ANNOTATION *ant = ANNOT_Get(BB_annotations(bb), ANNOT_EXITINFO);
  EXITINFO *exit_info = ANNOT_exitinfo(ant);
  OP *sp_adj = EXITINFO_sp_adj(exit_info);
  UINT64 frame_len = Frame_Len;
  TN *incr = NULL;

  /* Trace the sequence before we modify it.
   */
  if (Trace_EE) {
    #pragma mips_frequency_hint NEVER
    fprintf(TFile,
	    "\n%s<calls> Adjusting exit for %s (BB:%d)\n",
            DBar, ST_name(pu_st), BB_id(bb));
    fprintf(TFile, "\nFinal frame size: %" SCNd64 " (0x%" SCNx64 ")\n", frame_len, frame_len);
    fprintf(TFile, "\nOld stack frame de-allocation:\n");
    Print_OP_No_SrcLine(sp_adj);
  }

  /* Get the operand that is the frame size increment.
   */
  if ( !Gen_Frame_Pointer || PUSH_FRAME_POINTER_ON_STACK) {
    incr = OP_opnd(sp_adj, OP_find_opnd_use(sp_adj, OU_opnd2));
  }

  /* We make assumptions about what we generated in Generate_Exit.
   * Try to make sure our assumptions were right:
   *   TOP_spadjust $sp, $sp, Frame_Len_TN
   * or
   *   <copy> $sp, ...
   */
  if (Gen_Frame_Pointer && !PUSH_FRAME_POINTER_ON_STACK) {
    FmtAssert(OP_copy(sp_adj) &&
	      TN_is_sp_reg(OP_result(sp_adj,0)),
	      ("Unexpected exit SP adjust OP"));
  } else {
    FmtAssert(   OP_code(sp_adj) == TOP_spadjust 
	      && OP_results(sp_adj) == 1
	      && TN_is_sp_reg(OP_result(sp_adj,0))
	      && TN_is_sp_reg(OP_opnd(sp_adj,
		OP_find_opnd_use(sp_adj, OU_opnd1)))
	      && incr == Frame_Len_TN
	      && ( ! OP_has_predicate(sp_adj) 
		  || OP_opnd(sp_adj, OP_PREDICATE_OPND) == True_TN), 
	      ("Unexpected form of exit SP-adjust OP"));
  }

#ifdef TARG_X8664
  if (CG_push_pop_int_saved_regs && ! Gen_Frame_Pointer) {
    OPS popops = OPS_EMPTY;
    for (INT i = Saved_Callee_Saved_Regs.Elements()-1; i >= 0; i--) {
      SAVE_REG_LOC sr = Saved_Callee_Saved_Regs.Top_nth(i);
      if (sr.temp != NULL)
	continue;
      Build_OP(Is_Target_64bit() ? TOP_popq : TOP_popl, sr.ded_tn, SP_TN, SP_TN, &popops);
    }
    BB_Insert_Ops_After(bb, sp_adj, &popops);
  }
#endif

  /* Perform any adjustments. We will either remove the adjustment
   * or leave it unchanged.
   */
  if (Gen_Frame_Pointer && PUSH_FRAME_POINTER_ON_STACK) {
    OP* op = EETARG_High_Level_Procedure_Exit ();
#ifdef KEY // bug 3600
    OP_srcpos(op) = OP_srcpos(sp_adj);

    if (W2OPS_Pragma_Preamble_End_Seen())
      Set_OP_first_after_preamble_end(op);
#endif
    BB_Insert_Op_After (bb, sp_adj, op);
    BB_Remove_Op (bb, sp_adj);
    sp_adj = op;
  } else if (Gen_Frame_Pointer) {
    if ( Trace_EE ) {
      #pragma mips_frequency_hint NEVER
      fprintf(TFile, "\nNew stack frame de-allocation:\n"
		     "-- unchanged --\n");
    }
  } else if (frame_len == 0) {
    BB_Remove_Op(bb, sp_adj);
    sp_adj = NULL;

    if (Trace_EE) {
      #pragma mips_frequency_hint NEVER
      fprintf(TFile, "\nNew stack frame de-allocation:\n"
		     "-- removed --\n");
    }
  } else if (CGTARG_Can_Fit_Immediate_In_Add_Instruction (frame_len)) {
    OPS ops = OPS_EMPTY;
    OP *op;

    /* Replace the SP adjust placeholder with the new adjustment OP
     */
    Exp_ADD (Pointer_Mtype, SP_TN, SP_TN, incr, &ops);
    BB_Insert_Ops_Before(bb, sp_adj, &ops);
    BB_Remove_Op(bb, sp_adj);
    FOR_ALL_OPS_OPs_FWD(&ops, op) OP_srcpos(op) = OP_srcpos(sp_adj);
    sp_adj = OPS_last(&ops);

    if ( Trace_EE ) {
      #pragma mips_frequency_hint NEVER
      fprintf(TFile, "\nNew stack frame de-allocation:\n");
      FOR_ALL_OPS_OPs_FWD(&ops, op) Print_OP_No_SrcLine(op);
    }
  } else {
    FmtAssert(FALSE, ("Can't handle stack de-allocation of 0x%" SCNx64 "", frame_len));
  }

  /* Point to the [possibly] new SP adjust OP
   */
  EXITINFO_sp_adj(exit_info) = sp_adj;
}

static void
Adjust_Alloca_Code (void)
{
  BB *bb;
  OP *op;
  OPS ops;
  OP *new_op;
  for (bb = REGION_First_BB; bb != NULL; bb = BB_next(bb)) {
	// When we remove op and insert a new one,
	// that confuses the BB_OPs iterator.
	// So iterate by hand instead.
	op = BB_first_op(bb);
	while (op != NULL) {
		if (OP_code(op) != TOP_spadjust) {
			op = OP_next(op);
			continue;
		}
  		OPS_Init(&ops);
#ifndef KEY
		if (OP_spadjust_plus(op)) {
#else
		if (OP_variant(op) == V_ADJUST_PLUS) {
#endif
        		// dealloca does copy of kid to $sp 
			// (op1 is old sp value)
        		Exp_COPY (OP_result(op,0), 
			  	OP_opnd(op, OP_find_opnd_use(op, OU_opnd2)),
			  	&ops);
		}
#ifndef KEY
		else if (OP_spadjust_minus(op)) {
#else
                else if (OP_variant(op) == V_ADJUST_MINUS) {
#endif
    			Exp_SUB (Pointer_Mtype, OP_result(op,0),
			  	OP_opnd(op, OP_find_opnd_use(op, OU_opnd1)),
			  	OP_opnd(op, OP_find_opnd_use(op, OU_opnd2)),
				&ops);
		}
		else {
			FmtAssert(FALSE, ("non-alloca spadjust"));
		}
		FOR_ALL_OPS_OPs_FWD(&ops, new_op) {
			OP_srcpos(new_op) = OP_srcpos(op);
			Is_True(OP_has_predicate(new_op) == OP_has_predicate(op),
				("spadjust can't copy predicates"));
			// copy predicate to new copy/sub ops
			if (OP_has_predicate(new_op))
				Set_OP_opnd (new_op, OP_PREDICATE_OPND,
					OP_opnd(op, OP_PREDICATE_OPND) );
        	}
		BB_Insert_Ops_Before(bb, op, &ops);
    		BB_Remove_Op(bb, op);
		op = OPS_last(&ops);
	}
  }
}


/* ====================================================================
 *
 * Adjust_Entry_Exit_Code
 *
 * Adjust entry and exit code for the current PU.  This involves
 * calling Adjust_Entry for each entry BB, and Adjust_Exit
 * for each exit BB.
 *
 * ====================================================================
 */
void
Adjust_Entry_Exit_Code( ST *pu )
{
  BB_LIST *elist;

  for (elist = Entry_BB_Head; elist; elist = BB_LIST_rest(elist)) {
    Adjust_Entry(BB_LIST_first(elist));
  }

  for (elist = Exit_BB_Head; elist; elist = BB_LIST_rest(elist)) {
    Adjust_Exit(pu, BB_LIST_first(elist));
  }

  FmtAssert (!(GP_Setup_Code == no_code && PU_References_GP),
		("had gp reference without setting up new gp"));

  if (PU_has_alloca(Get_Current_PU())) {
	// replace spadjusts with "real" code.
	Adjust_Alloca_Code ();
  }
}

#ifdef KEY
// See the interface description

BOOL Is_Unique_Callee_Saved_Reg (TN * new_tn)
{
  for (INT i=0; i<Saved_Callee_Saved_Regs.Elements(); i++)
    if (TNs_Are_Equivalent (Saved_Callee_Saved_Regs.Top_nth(i).ded_tn,
                            new_tn))
      return FALSE;

  return TRUE;
}

INT Cgdwarf_Num_Callee_Saved_Regs (void)
{
  return Saved_Callee_Saved_Regs.Elements();
}

struct tn* Cgdwarf_Nth_Callee_Saved_Reg (INT n)
{
  return Saved_Callee_Saved_Regs.Top_nth(n).ded_tn;
}

ST* Cgdwarf_Nth_Callee_Saved_Reg_Location (INT n)
{
  return Saved_Callee_Saved_Regs.Top_nth(n).temp;
}
#endif

#ifdef TARG_X8664
INT Push_Pop_Int_Saved_Regs(void)
{
  INT size = 0;
  if (CG_push_pop_int_saved_regs && ! Gen_Frame_Pointer) {
    for (INT i=0; i<Saved_Callee_Saved_Regs.Elements(); i++) {
      SAVE_REG_LOC sr = Saved_Callee_Saved_Regs.Top_nth(i);
      if (sr.temp == NULL)
	size++;
    }
  }
  return size;
}
#endif
