/*
 *  Copyright (C) 2008 PathScale, LLC.  All Rights Reserved.
 */

/*
 * Copyright 2006.  QLogic Corporation.  All Rights Reserved.
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

//  Coloring the interference graph
/////////////////////////////////////
//
//  Description:
//
//      Basically Chaiten/Briggs with hooks for preferencing and splitting.
//
/////////////////////////////////////


//  $Revision: 1.17 $
//  $Date: 05/06/13 17:31:01-07:00 $
//  $Author: tkong@hyalite.keyresearch $
//  $Source: be/cg/gra_mon/SCCS/s.gra_color.cxx $

#ifdef USE_PCH
#include "cg_pch.h"
#endif // USE_PCH
#pragma hdrstop

#ifdef _KEEP_RCS_ID
static char *rcs_id = "$Source: be/cg/gra_mon/SCCS/s.gra_color.cxx $ $Revision: 1.17 $";
#endif

#include <limits.h>
#include "defs.h"
#include "errors.h"
#include "tracing.h"
#include "mempool.h"
#include "register.h"
#include "priority_queue.h"
#include "cg_flags.h"
#include "gra_bb.h"
#include "gra.h"
#include "gra_region.h"
#include "gra_trace.h"
#include "gra_lrange.h"
#include "gra_split.h"
#include "gra_grant.h"
#include "gra_spill.h"
#include "gra_interfere.h"
#ifdef TARG_X8664
#include "targ_sim.h"
#include "whirl2ops.h"
#endif

#ifdef KEY
BOOL GRA_reclaim_register = FALSE;
BOOL GRA_reclaim_register_set = FALSE;
#endif

INT32 GRA_local_forced_max = DEFAULT_FORCED_LOCAL_MAX;
    // How many locals to force allocate (out of the number requested by LRA)?

#ifdef TARG_X8664
BOOL GRA_grant_special_regs = FALSE;
BOOL GRA_local_forced_max_set = FALSE;
#endif

BOOL GRA_avoid_glue_references_for_locals = TRUE;
  // Try to grant the forced locals from the set of registers not also used in 
  // glue copies in the same block

static REGISTER_SET non_prefrenced_regs[ISA_REGISTER_CLASS_MAX + 1];
  // Registers that noone prefers yet.

static REGISTER_SET callee_saves_used[ISA_REGISTER_CLASS_MAX + 1];
  // Callee saves registers that someone has already used.  This makes it free
  // for someone else to use them.
static REGISTER_SET regs_used[ISA_REGISTER_CLASS_MAX + 1]; // statistics only

static MEM_POOL prq_pool;

static GRA_REGION *GRA_current_region;  // The current region for which we are 
					// allocating registers.

// Use to iterate over the members of a LRNAGE's preference class
// that have already been allocated a register in their
// _Preference_Priority order.
class LRANGE_PREF_ITER: public GRA_PREF_LRANGE_ITER {
public:
  LRANGE_PREF_ITER(void) {}
  ~LRANGE_PREF_ITER(void) {}

  void Init(LRANGE *lrange)	{ GRA_PREF_LRANGE_ITER::Init(lrange->Pref()); }
};

//  Choosing registers ...
//////////////////////////////////////////////////////////////////////////
//
//  ... trying to pay attention to preferences.  It goes like this:
//
//      1.  If another LRANGE in its preference class already has a register
//          and it's allowed, we'll take this register, or
//
//      2.  Try to pick a register not prefered by its neighbors or the
//          preferences of its neighbors, or
//
//      3.  Try to pick a register prefered by noone yet, or
//
//      4.  Try to pick a caller saves register, or
//
//      5.  Try to pick a callee saves register that we've already used, or
//
//      6.  Take what's left.
//
//////////////////////////////////////////////////////////////////////////


/////////////////////////////////////
static void
Initialize(void)
/////////////////////////////////////
//
//  Initialize register sets used by preferencing and callee_saves
//  Description
/////////////////////////////////////
{
  ISA_REGISTER_CLASS rc;
  static BOOL prq_initialized = FALSE;

  if ( ! prq_initialized ) {
    prq_initialized = TRUE;
    MEM_POOL_Initialize(&prq_pool,"GRA LRANGE priority queue",FALSE);
  }

  FOR_ALL_ISA_REGISTER_CLASS( rc ) {
    non_prefrenced_regs[rc] = REGISTER_CLASS_allocatable(rc);
    
    callee_saves_used[rc] = REGISTER_SET_EMPTY_SET;

    regs_used[rc] = REGISTER_SET_EMPTY_SET;
  }
}

/////////////////////////////////////
static void
Update_Register_Info( LRANGE* lrange, REGISTER reg, BOOL reclaim = FALSE )
/////////////////////////////////////
//
//  Update some information assiciated primarily with the register and used
//  for preferencing.
//
/////////////////////////////////////
{
  ISA_REGISTER_CLASS rc = lrange->Rc();

  lrange->Allocate_Register(reg, reclaim);

  if ( lrange->Has_Preference() ) {
    non_prefrenced_regs[rc] =
        REGISTER_SET_Difference1(non_prefrenced_regs[rc],reg);
  }

  if (!lrange->Has_Wired_Register() && !lrange->Tn_Is_Save_Reg()) {
    regs_used[rc] = REGISTER_SET_Union1(regs_used[rc], reg);
    if ((REGISTER_SET_MemberP(REGISTER_CLASS_callee_saves(rc),reg)
#ifdef HAS_STACKED_REGISTERS
       || REGISTER_Is_Stacked_Local(rc, reg)
#endif
	)) 
      callee_saves_used[rc] = REGISTER_SET_Union1(callee_saves_used[rc],reg);
  }
}

/////////////////////////////////////
static BOOL
Choose_Best_Register(REGISTER* reg, ISA_REGISTER_CLASS rc,
                     REGISTER_SET allowed , BOOL remove_global_callees,
                     BOOL prefer_caller_stacked)
/////////////////////////////////////
//
//  Find the best register in <allowed>, a register set of registers in
//  <rc>.  If one is found (it will be if <allowd> is non-empty), return
//  it by reference in <reg> and return TRUE.
//
/////////////////////////////////////
{
  if ( REGISTER_SET_EmptyP(allowed) )
    return FALSE;

#ifdef HAS_STACKED_REGISTERS
  if (! REGISTER_Has_Stacked_Registers(rc)) {
#endif
    *reg = REGISTER_SET_Choose_Intersection(allowed,
					    REGISTER_CLASS_caller_saves(rc));
    if ( *reg != REGISTER_UNDEFINED )
      return TRUE;
      
    *reg = REGISTER_SET_Choose_Intersection(allowed,callee_saves_used[rc]);
    if ( *reg != REGISTER_UNDEFINED )
      return TRUE;
      
    *reg = REGISTER_SET_Choose(allowed);
    if ( *reg != REGISTER_UNDEFINED )
      return TRUE;
    
    return FALSE;
#ifdef HAS_STACKED_REGISTERS
  }
  else {
    // first, try to use any caller-saved regs that have already been allocated
    REGISTER_SET callers =
      REGISTER_SET_Union(REGISTER_CLASS_caller_saves(rc),
                         REGISTER_Get_Stacked_Avail_Set(ABI_PROPERTY_caller,
                                                        rc));
    *reg = REGISTER_SET_Choose_Intersection(allowed, callers);
    if ( *reg != REGISTER_UNDEFINED )
      return TRUE;

    // If we'd prefer to use a stacked caller over anything remaining, then
    // try to get a new one by enlarging the stack; it must not be in allowed
    if (prefer_caller_stacked) {
      *reg = REGISTER_Request_Stacked_Register(ABI_PROPERTY_caller, rc);
      if ( *reg != REGISTER_UNDEFINED )
        return TRUE;
    }
      
    // next, try to use any callee-saved regs that have already been allocated
    *reg = REGISTER_SET_Choose_Intersection(allowed,callee_saves_used[rc]);
    if ( *reg != REGISTER_UNDEFINED )
      return TRUE;

    if (remove_global_callees) { // not using non-stacked callee-saved regs
      allowed = REGISTER_SET_Difference(allowed,REGISTER_CLASS_callee_saves(rc));
    }
    // choose from previously un-used callee-saved regs
    *reg = REGISTER_SET_Choose(allowed);
    if ( *reg != REGISTER_UNDEFINED )
      return TRUE;

    // try to get a new callee-saved one by enlarging the stack
    *reg = REGISTER_Request_Stacked_Register(ABI_PROPERTY_callee, rc);
    if ( *reg != REGISTER_UNDEFINED )
      return TRUE;
  
    return FALSE;
  }
#endif
}

/////////////////////////////////////
static BOOL
Choose_Preference( LRANGE* lrange, REGISTER_SET allowed, GRA_REGION* region
#ifdef KEY
		   , BOOL reclaim = FALSE
#endif
		 )
/////////////////////////////////////
//
//  If one of the registers used by a member of <lrange>'s preference class is
//  in <allowed>, assign it and return TRUE.
//
/////////////////////////////////////
{
  LRANGE_PREF_ITER iter;

  for (iter.Init(lrange); ! iter.Done(); iter.Step()) {
    LRANGE* p_lrange = iter.Current();

    if ( REGISTER_SET_MemberP(allowed,p_lrange->Reg()) ) {
#ifdef KEY
      if (reclaim) {
	LRANGE_Split_Reclaimed_BBs(lrange, p_lrange->Reg());
      }
#endif
      Update_Register_Info(lrange, p_lrange->Reg(), reclaim);
      GRA_Trace_Preference_Attempt(lrange, p_lrange, region, TRUE);
      return TRUE;
    }
    GRA_Trace_Preference_Attempt(lrange, p_lrange, region, FALSE);
  }

  return FALSE;
}

/////////////////////////////////////
static BOOL
Choose_Avoiding_Neighbor_Preferences( LRANGE* lrange, REGISTER_SET allowed
#ifdef KEY
				      , BOOL reclaim = FALSE
#endif
				    )
/////////////////////////////////////
//
//  Try to find a register for <lrange> in the <allowed> set that is preferred
//  by as few of <lrange>'s neighbors as possible.
//
//  NOTE that this is a little different from the modulo rename preferencing
//  used in SWP.  There may be some fun left here...
//
/////////////////////////////////////
{
  ISA_REGISTER_CLASS rc = lrange->Rc();
  REGISTER reg;
  LRANGE_PREF_ITER p_iter;
  LRANGE_NEIGHBOR_ITER n_iter;

  for (n_iter.Init(lrange, GRA_current_region); ! n_iter.Done(); n_iter.Step()){
    LRANGE* nlr = n_iter.Current();

    if ( nlr->Allocated() )    	    // We avoid it's actual choice, so
      continue;                     //   no need to avoid its preference.

    // Everthing is a neighbor of these, so what's the point?  We'll tend to
    // avoid them in the Choose_Best_Register code...
    if (nlr->Type() == LRANGE_TYPE_COMPLEMENT && TN_is_save_reg(nlr->Tn())
    ) {
      continue;
    }

    for (p_iter.Init(nlr); ! p_iter.Done(); p_iter.Step()) {
      LRANGE* plr = p_iter.Current();
      REGISTER_SET new_allowed;

      if ( plr->Allocated() && plr->Rc() == lrange->Rc() )
        new_allowed = REGISTER_SET_Difference1(allowed, plr->Reg());
      if ( !REGISTER_SET_EmptyP(new_allowed) ) {
        allowed = new_allowed;
      }

    }
  }

  if (Choose_Best_Register(&reg, rc, allowed,
			   REGISTER_Has_Stacked_Registers(rc),
			   !(lrange->Spans_A_Call() || lrange->Spans_Infreq_Call()))) {
#ifdef KEY
    if (reclaim) {
      LRANGE_Split_Reclaimed_BBs(lrange, reg);
    }
#endif
    Update_Register_Info(lrange, reg, reclaim);
    return TRUE;
  }

  return FALSE;
}

/////////////////////////////////////
static BOOL
Allocate_Stacked_Register(LRANGE* lrange)
/////////////////////////////////////
//
//  Try to get a new stacked register for the lrange if profitable,
//  and it exists.
//
/////////////////////////////////////
{
#ifdef HAS_STACKED_REGISTERS
  if (!GRA_use_stacked_regs ||
      !REGISTER_Has_Stacked_Registers(lrange->Rc())) {
    return FALSE;
  }
  if (lrange->Spans_A_Setjmp()) 
    return FALSE;

  INT abi_property;
  if (lrange->Spans_A_Call()) {
    abi_property = ABI_PROPERTY_callee;
  } else {
    abi_property = ABI_PROPERTY_caller;
  }
  REGISTER reg = REGISTER_Request_Stacked_Register(abi_property,
						   lrange->Rc());
  if (reg != REGISTER_UNDEFINED) {
    Update_Register_Info(lrange, reg);
    return TRUE;
  }
#endif
  return FALSE;
}

/////////////////////////////////////
static BOOL
Choose_Noones_Preference( LRANGE* lrange, REGISTER_SET allowed
#ifdef KEY
			  , BOOL reclaim = FALSE
#endif
			)
/////////////////////////////////////
//
//  See if there is a register in 'allowed' that is not yet the preference of
//  any live range.  If there is one, assign it and return TRUE.
//
/////////////////////////////////////
{
  ISA_REGISTER_CLASS rc = lrange->Rc();
  REGISTER     reg;
  REGISTER_SET npr = non_prefrenced_regs[lrange->Rc()];

  allowed = REGISTER_SET_Intersection(allowed,npr);
  if ( Choose_Best_Register(&reg,rc,allowed,
			    REGISTER_Has_Stacked_Registers(rc),
			    (lrange->Spans_A_Call() || lrange->Spans_Infreq_Call())) ) {
#ifdef KEY
    if (reclaim) {
      LRANGE_Split_Reclaimed_BBs(lrange, reg);
    }
#endif
    Update_Register_Info(lrange, reg, reclaim);
    return TRUE;
  }
  return FALSE;
}

/////////////////////////////////////
static BOOL
Choose_Anything( LRANGE* lrange, REGISTER_SET allowed
#ifdef KEY
		 , BOOL reclaim = FALSE
	       )
#endif
/////////////////////////////////////
//
//  Take any register from the 'allowed' set for 'lrange'.  Return TRUE if
//  successful.
//
/////////////////////////////////////
{
  REGISTER reg;
  ISA_REGISTER_CLASS rc = lrange->Rc();
  BOOL has_stacked = REGISTER_Has_Stacked_Registers(rc);

  if ( Choose_Best_Register(&reg,rc,allowed, has_stacked,
			    !(lrange->Spans_A_Call() || lrange->Spans_Infreq_Call())) ) {
#ifdef KEY
    if (reclaim) {
      LRANGE_Split_Reclaimed_BBs(lrange, reg);
    }
#endif
    Update_Register_Info(lrange, reg, reclaim);
    return TRUE;
  }

  //
  // If stacked registers are available, try to get a new one.
  //
#ifdef KEY
  if (!reclaim)
#endif
  if (has_stacked && Allocate_Stacked_Register(lrange)) {
    return TRUE;
  }

  //
  // Now allow "global" callee saved registers to be freely used (they
  // could have been used to satisfy a preference).
  //
  if (Choose_Best_Register(&reg,rc,allowed, FALSE, FALSE)) {
#ifdef KEY
    if (reclaim) {
      LRANGE_Split_Reclaimed_BBs(lrange, reg);
    }
#endif
    Update_Register_Info(lrange, reg, reclaim);
    return TRUE;
  }

  return FALSE;
}

/////////////////////////////////////
static BOOL
Choose_Register( LRANGE* lrange, GRA_REGION* region )
/////////////////////////////////////
//
//  Choose a register for the given 'lrange'.  Return TRUE if successful.
//
/////////////////////////////////////
{
  REGISTER_SET allowed = lrange->Allowed_Registers(GRA_current_region);

  if ( lrange->Has_Wired_Register() ) {
#ifdef KEY
    if (! PU_Has_Nonlocal_Goto_Target)
#endif
    DevAssert( REGISTER_SET_MemberP(allowed, lrange->Reg()),
               ("LRANGE not allowed its wired register"));
    Update_Register_Info(lrange, lrange->Reg());
    return TRUE;
  }
#ifdef KEY
  GRA_Trace_LRANGE_Choose(lrange, allowed);
#endif
#ifdef HAS_STACKED_REGISTERS
  if (REGISTER_SET_EmptyP(allowed) ) {
    //
    // Try to get a stacked register.
    //
    return Allocate_Stacked_Register(lrange);
  }      
#endif

#ifdef KEY
  // SWP may have assigned a register to the TN.  GRA should trust SWP and
  // choose this register no matter what.  (Maybe better to use GRA's
  // Has_Wired_Register framework, which currently covers LOCAL lranges only.)
  if (Enable_SWP &&
      lrange->Type() == LRANGE_TYPE_COMPLEMENT) {
    REGISTER reg = TN_register(lrange->Tn());
    if (reg != REGISTER_UNDEFINED) {
      Update_Register_Info(lrange, reg);
      return TRUE;
    }
  }
#endif

  if ( Choose_Preference(lrange, allowed, region) )
    return TRUE;
  else if ( Choose_Avoiding_Neighbor_Preferences(lrange,allowed) )
    return TRUE;
  else if ( Choose_Noones_Preference(lrange,allowed) )
    return TRUE;
  else
    return Choose_Anything(lrange,allowed);
}

#ifdef TARG_X8664
/////////////////////////////////////
static REGISTER_SET
Find_Single_Register_Subclasses(GRA_BB *gbb)
/////////////////////////////////////
//
// Find operands and results that must use a special register.  Return these
// registers.
//
/////////////////////////////////////
{
  // Hard code rax/rcx/rdx into the search for speed.
  int rax = 0, rcx = 0, rdx = 0;
  REGISTER_SET grants = REGISTER_SET_EMPTY_SET;
  OP *op;

  if (GRA_grant_special_regs) {
    // Force GRA to grant the special registers.
    rax = rcx = rdx = 1;
  } else {
    FOR_ALL_BB_OPs (gbb->Bb(), op) {
      ASM_OP_ANNOT *asm_info = (OP_code(op) == TOP_asm) ? 
		      (ASM_OP_ANNOT*) OP_MAP_Get(OP_Asm_Map, op) : NULL;
      INT i;
      for (i = 0; i < OP_opnds(op); i++) {
        ISA_REGISTER_SUBCLASS subclass = asm_info ?
          ASM_OP_opnd_subclass(asm_info)[i] : OP_opnd_reg_subclass(op, i);
        rax |= (subclass == ISA_REGISTER_SUBCLASS_rax);
        rcx |= (subclass == ISA_REGISTER_SUBCLASS_rcx);
        rdx |= (subclass == ISA_REGISTER_SUBCLASS_rdx);
      }
      for (i = 0; i < OP_results(op); i++) {
        ISA_REGISTER_SUBCLASS subclass = asm_info ?
          ASM_OP_result_subclass(asm_info)[i] : OP_result_reg_subclass(op, i);
        rax |= (subclass == ISA_REGISTER_SUBCLASS_rax);
        rcx |= (subclass == ISA_REGISTER_SUBCLASS_rcx);
        rdx |= (subclass == ISA_REGISTER_SUBCLASS_rdx);
      }
      if (rax & rcx & rdx)
        break;
    }
  }

  // Stuff the registers into the register set.
  if (rax)
    grants = REGISTER_SET_Union1(grants, RAX);
  if (rcx)
    grants = REGISTER_SET_Union1(grants, RCX);
  if (rdx)
    grants = REGISTER_SET_Union1(grants, RDX);

  // If the last OP in BB is a ijump or icall, make sure LRA have enough
  // register(s) for it.  LRA cannot spill around the OP because it cannot
  // insert reloads after it.  Don't give parameter registers, and don't give
  // RAX which is set to 0 before the call.  Bug 7366.
  OP *last_op = BB_last_op(gbb->Bb());
  if (last_op) {
    switch (OP_code(last_op)) {
      case TOP_icall:
	// Grant a non-parameter register.
	grants = REGISTER_SET_Union1(grants, Is_Target_32bit() ? RSI : R10);
	break;
      case TOP_ijmpx:
      case TOP_ijmpxxx:
      case TOP_icallx:
      case TOP_icallxxx:
	// One register for either base or index.
	grants = REGISTER_SET_Union1(grants, Is_Target_32bit() ? RCX : R10);
	break;
      case TOP_ijmpxx:
      case TOP_icallxx:
	// One register for base and one for index.
	grants = REGISTER_SET_Union1(grants, Is_Target_32bit() ? RCX : R10);
	grants = REGISTER_SET_Union1(grants, Is_Target_32bit() ? RDX : R11);
	break;
    }
  }

  return grants;
}
#endif

/////////////////////////////////////
static void
Force_Color_Some_Locals( GRA_REGION* region, ISA_REGISTER_CLASS rc )
/////////////////////////////////////
//
//  The first GRA_local_forced_max locals in each BB are not represented by
//  LRANGEs for compile speed reasons.  We'll just choose colors for these
//  right before we start coloring the complement live ranges.
//
/////////////////////////////////////
{
  GRA_REGION_GBB_ITER iter;
  INT rc_local_forced_max;

  //
  // If it's been overridden, use that value.
  //
  // TODO:  This really needs to be rethought ... IA-64 makes this not
  // so simple of a choice, particularly in the general purpose registers
  // with the register stack.
  //
  if (GRA_LOCAL_FORCED_MAX(rc) == DEFAULT_FORCED_LOCAL_MAX) {
    INT rc_size = (REGISTER_CLASS_last_register(rc) - REGISTER_MIN) + 1;
#ifdef KEY
    rc_local_forced_max = GRA_LOCAL_FORCED_MAX(rc);
#else
    rc_local_forced_max = Min(GRA_LOCAL_FORCED_MAX(rc), rc_size/8);
#endif
  } else {
#ifndef KEY // cannot use this code because with user-assigned registers via
	// asm("reg-name"), there will be less allocatable registers resulting
	// in 0 register granted to LRA (bug 3663)
    INT rc_size = REGISTER_SET_Size(REGISTER_CLASS_allocatable(rc));
    if (rc_size <= 8) {
     /* There are not enough registers in this class to support this option. */
      return;
    }
    if (rc_size < GRA_LOCAL_FORCED_MAX(rc)*2) {
     /* Don't allow a request for more than half of the available registers. */
      rc_local_forced_max = rc_size/2;
    } else {
#endif
      rc_local_forced_max = GRA_LOCAL_FORCED_MAX(rc);
#ifndef KEY
    }
#endif
  }
  
  for (iter.Init(region); ! iter.Done(); iter.Step()) {
    INT i;
    GRA_BB* gbb = iter.Current();
    INT regs_to_grant = Min(gbb->Register_Girth(rc),rc_local_forced_max);
#ifdef TARG_X8664
    // Give the special-purpose registers referenced in the BB to LRA.  On the
    // x86, only integer registers have special purpose.
    if (rc == ISA_REGISTER_CLASS_integer) {
      REGISTER_SET special_regs = Find_Single_Register_Subclasses(gbb);
      for (REGISTER reg = REGISTER_SET_Choose(special_regs);
	   reg != REGISTER_UNDEFINED;
	   reg = REGISTER_SET_Choose_Next(special_regs, reg)) {
	gbb->Make_Register_Used(ISA_REGISTER_CLASS_integer, reg);
	if (GRA_reclaim_register)
	  gbb->Make_Register_Referenced(ISA_REGISTER_CLASS_integer, reg);
	GRA_GRANT_Local_Register(gbb, ISA_REGISTER_CLASS_integer, reg);
	regs_to_grant--;
      }
    }
#endif
    for ( i = regs_to_grant;
          i > 0;
          --i
    ) {
      REGISTER reg;
      REGISTER_SET allowed = REGISTER_CLASS_allocatable(rc);
#ifdef HAS_STACKED_REGISTERS
      if (REGISTER_Has_Stacked_Registers(rc)) {
	allowed = REGISTER_SET_Difference(allowed, REGISTER_CLASS_stacked(rc));
	allowed = REGISTER_SET_Union(allowed, regs_used[rc]);
      }
#endif
      allowed = REGISTER_SET_Difference(allowed,
                                gbb->Registers_Used(rc));
#ifdef KEY  // this is needed to avoid insufficient regs granted to LRA
      allowed = REGISTER_SET_Difference(allowed, 
	      			REGISTER_CLASS_function_value(rc));
#endif
      BOOL non_glue_found = FALSE;

      REGISTER_SET npr = non_prefrenced_regs[rc];
      REGISTER_SET gru = gbb->Glue_Registers_Used(rc);

      if ( GRA_avoid_glue_references_for_locals ) {
	REGISTER_SET npr_gru = REGISTER_SET_Difference(npr, gru);
	if ( Choose_Best_Register(&reg,rc,
				  REGISTER_SET_Intersection(allowed, npr_gru),
				  FALSE, FALSE)
	    || Choose_Best_Register(&reg,rc,
				    REGISTER_SET_Difference(allowed, gru),
				    FALSE, FALSE)) {
	  non_glue_found = TRUE;
	}
      }
      if ( non_glue_found
	  || Choose_Best_Register(&reg,rc,
				  REGISTER_SET_Intersection(allowed, npr),
				  FALSE, FALSE)
          || Choose_Best_Register(&reg,rc,allowed,FALSE,FALSE)
      ) {
        gbb->Make_Register_Used(rc,reg);
#ifdef KEY
	if (GRA_reclaim_register)
	  gbb->Make_Register_Referenced(rc, reg);
#endif
        GRA_GRANT_Local_Register(gbb,rc,reg);
      }
      else {
#ifdef TARG_X8664
	if( Is_Target_64bit() ){
	  DevWarn("Couldn't force allocate %d registers in rc %d for BB:%d",
		  i,rc,BB_id(gbb->Bb()));
	}
#endif
        break;
      }
    }
  }
}

#ifdef KEY
//  Reclaim registers ...
//////////////////////////////////////////////////////////////////////////
//
//  Choose a register from the reclaimable register set.  A register is
//  reclaimable for a LRANGE X if, throughout X's BBs, the register is
//  allocated to another LRANGE Y but is not referenced by Y (used or defined).
//  Choose_Reclaimable_Register works just like Choose_Register, except that
//  Choose_Reclaimable_Register searches for register candidates over the
//  reclaimable registers instead of the unused registers.
//
//////////////////////////////////////////////////////////////////////////


/////////////////////////////////////
static BOOL
Choose_Reclaimable_Register (LRANGE *lrange, GRA_REGION *region)
/////////////////////////////////////
//
// Reclaim a register for the given 'lrange'.  Return TRUE if successful.
//
/////////////////////////////////////
{
  // Don't reclaim a register for local lranges.
  if (lrange->Type() == LRANGE_TYPE_LOCAL)
    return FALSE;

  REGISTER_SET reclaimable = lrange->Reclaimable_Registers(GRA_current_region);

  // Choose_Register should have taken care of LRANGEs with wired registers.
  Is_True(!lrange->Has_Wired_Register(),
	  ("Choose_Reclaimable_Register: LRANGE has wired register"));

  GRA_Trace_LRANGE_Choose_Reclaimable(lrange, reclaimable);

  if (REGISTER_SET_EmptyP(reclaimable)) {
    return FALSE;
  }      

  if (Choose_Preference(lrange, reclaimable, region, TRUE))
    return TRUE;
  else if (Choose_Avoiding_Neighbor_Preferences(lrange, reclaimable, TRUE))
    return TRUE;
  else if (Choose_Noones_Preference(lrange, reclaimable, TRUE))
    return TRUE;
  else
    return Choose_Anything(lrange, reclaimable, TRUE);
}
#endif

//  Simplification
//////////////////////////////////////////////////////////////////////////


TYPE_PRQ(LRANGE,LRPRQ)  // Generate a priority queue type for live ranges.


/////////////////////////////////////
static BOOL
Compare_Priorities( LRANGE* lrange0, LRANGE* lrange1 )
/////////////////////////////////////
//
//  LRNAGE priority comparison function for use with priority queues.
//
/////////////////////////////////////
{
  return lrange0->Priority() < lrange1->Priority();
}


/////////////////////////////////////
static void
Initialize_Priority_Queue( LRPRQ* q, GRA_REGION* region, ISA_REGISTER_CLASS cl )
/////////////////////////////////////
//
//  Initialize the priority queue <q> so it can hold all the live ranges of
//  the given <cl> in the given <region>.
//
/////////////////////////////////////
{
  LRPRQ_Initialize(q,Compare_Priorities,
                     NULL,
                     NULL,
                     &prq_pool,
		     region->Lrange_Count(cl),
                     200);
}


void
LRANGE_CLIST::Print_Clist(void)
{
  LRANGE *lr;
  for (lr = first; lr != last; lr = lr->clist_next) {
    printf("%d ", lr->id);
  }
  if (lr != NULL) {
    printf("%d \n", lr->id);
  }
}

/////////////////////////////////////
static void
Simplify_Initialize( GRA_REGION* region, ISA_REGISTER_CLASS rc,
                                         LRANGE_CLIST*      cl,
                                         LRANGE_CLIST*      pcl,
                                         LRPRQ*             ready,
                                         LRPRQ*             not_ready )
/////////////////////////////////////
//
//  Get ready for simplification of the given <region> and ISA_REGISTER_CLASS,
//  <rc>.  Initialize the two priority queues and enqueue the live ranges to
//  <ready> that have fewer neighbors than there are registers available (to
//  the particular live range).  <cl> is the coloring list and initialized to
//  be empty.  <pcl> is the coloring list for preallocated live ranges.  We'll
//  put any of these we encounter on <pcl> and it can be prepended to <cl>
//  after simplification.  <not_ready> gets any other live ranges in <region>.
//
/////////////////////////////////////
{
  GRA_REGION_RC_LRANGE_ITER iter;
  GRA_REGION_GBB_ITER gbb_iter;

  cl->Initialize();
  pcl->Initialize();
  lrange_mgr.Clear_One_Set();

  Initialize_Priority_Queue(ready,region,rc);
  Initialize_Priority_Queue(not_ready,region,rc);

  //
  // put unpreferenced wired locals on the preallocated live range coloring
  // list.  these are never part of the conflict graph for compile time
  // reasons.
  //
  for (gbb_iter.Init(region); ! gbb_iter.Done(); gbb_iter.Step()) {
    GRA_BB* gbb = gbb_iter.Current();
    LRANGE_LIST *l;

    for (l = gbb->Unpreferenced_Wired_LRANGEs(rc);
	 l != NULL;
	 l = LRANGE_LIST_rest(l)
	 ) {
      pcl->Push(LRANGE_LIST_first(l));
    }
  }

  for (iter.Init(region,rc); ! iter.Done(); iter.Step()) {
    LRANGE* lr = iter.Current();

    if ( lr->Has_Wired_Register() ) {

      // This is a LRANGE that needs to be allocated to a particular register.
      // These are always local and no two that interfere require the same
      // register.  We'll hold these out during the simplification phase and
      // put them on front of the coloring list.  We may cause more Briggs
      // points this way, but that's life.  We really have no choice about
      // these and anybody who interferes with one won't be able to use its
      // register.

      if ( lr->Has_Preference() ) {
        // those without preferences added above
        pcl->Push(lr);
      }
    }
    else if ( ! lr->Region_Invariant() ) {
      FmtAssert(! lr->Allocated(),("LRANGE already allocated"));
      lr->Neighbors_Left_Initialize();
      lr->Calculate_Priority();

#ifndef KEY // for key, leave ready queue empty; rank purely based on priority
      if ( lr->Neighbors_Left() < lr->Candidate_Reg_Count() ) {
        LRPRQ_Insert(ready,lr);
        lrange_mgr.One_Set_Union1(lr);
      }
      else
#endif
        LRPRQ_Insert(not_ready,lr);
    }
  }
}


/////////////////////////////////////
static void
Simplify( GRA_REGION* region, ISA_REGISTER_CLASS rc, LRANGE_CLIST* cl )
/////////////////////////////////////
//
//  This is essentially Chaitin/Briggs simplification.  The live ranges in the
//  given <region> which want a register from the ISA_REGISTER_CLASS <rc> are
//  ordered onto the given coloring list 'cl'.  We are guaranteed that there
//  is no interference between live ranges belonging to different regions;
//  preference is used to express the relationship.
//
/////////////////////////////////////
{
  LRPRQ ready, not_ready;
  LRANGE* lr;
  LRANGE_NEIGHBOR_ITER iter;
  LRANGE_CLIST pcl;

  MEM_POOL_Push(&prq_pool);
  Simplify_Initialize(region,rc,cl,&pcl,&ready,&not_ready);

#ifndef KEY // for key, leave ready queue empty; rank purely based on priority
  for (;;) {
    if ( LRPRQ_Size(&ready) != 0 )
      lr = LRPRQ_Delete_Top(&ready);
    else if ( LRPRQ_Size(&not_ready) != 0) {
      // Chose one at a Briggs point.
      lr = LRPRQ_Delete_Top(&not_ready);
      GRA_Trace_Color_LRANGE("Briggs point.",lr);
    }
    else
      break;

    FmtAssert(! lr->Allocated(),("LRANGE already allocated"));

    cl->Push(lr);
    GRA_Trace_Color_LRANGE("Listing.",lr);
    lr->Listed_Set();

    for (iter.Init(lr, region); ! iter.Done(); iter.Step()) {
      LRANGE* nlr = iter.Current();

      FmtAssert(! nlr->Allocated(),("LRANGE already allocated"));

      // We want to preserve the neighbors left field of the simplified
      // LRANGEs.  They will be used during splitting.  See gra_lrange.h.
      if ( ! ( nlr->Listed() || nlr->Has_Wired_Register() ) &&
           nlr->Neighbors_Left_Decrement() == nlr->Candidate_Reg_Count() - 1) {
        LRPRQ_Remove(&not_ready,nlr);

        DevAssert(! lrange_mgr.One_Set_MemberP(nlr),
                  ("Adding a lrange to coloring ready queue twice"));
        lrange_mgr.One_Set_Union1(nlr);

        LRPRQ_Insert(&ready,nlr);

      }
    }
  }
#else
  while (LRPRQ_Size(&not_ready) != 0) { 
    lr = LRPRQ_Delete_Top(&not_ready);

    FmtAssert(! lr->Allocated(),("LRANGE already allocated"));

    cl->Push(lr);
    GRA_Trace_Color_LRANGE("Listing.",lr);
    lr->Listed_Set();
  }
#endif

  // Any live ranges that must be allocated to particular live ranges are now
  // prepended to the front of the coloring list.  Notice how these have been
  // forcefully held out of simplification.
  pcl.Append(cl);

  FmtAssert(LRPRQ_Size(&ready) == 0,
            ("Ready priority queue not empty after simplification"));
  FmtAssert(LRPRQ_Size(&not_ready) == 0,
            ("Not-ready priority queue not empty after simplification"));

  MEM_POOL_Pop(&prq_pool);
}


//  Coloring drivers
//////////////////////////////////////////////////////////////////////////

static void
Choose_Original_Registers( GRA_REGION* region, ISA_REGISTER_CLASS rc )
/////////////////////////////////////
//
//  We couldn't color the <rc> TNs in <region> using preferencing.  Just back
//  down to the original register use from the initial coloring.
//
/////////////////////////////////////
{
  GRA_REGION_RC_LRANGE_ITER iter;
  GRA_Trace_Color(0,"Using original registers");

  for (iter.Init(region,rc); ! iter.Done(); iter.Step()) {
    LRANGE* lr = iter.Current();

    DevAssert(lr->Type() == LRANGE_TYPE_REGION,
	      ("Choose_Original_Register for non-REGION LRANGE"));
    lr->Allocated_Reset();
    Update_Register_Info(lr, lr->Orig_Reg());
  }
}

/////////////////////////////////////
static BOOL
Color_Region_With_Preferences( GRA_REGION *region, ISA_REGISTER_CLASS rc )
/////////////////////////////////////
//
//  Try to using preferences to color the live ranges in <region> that need
//  registers in <rc>.  Return TRUE to indicate success.
//
/////////////////////////////////////
{
  LRANGE_CLIST cl;          // Coloring list
  LRANGE_CLIST_ITER iter;   // Iterator over above

  GRA_Trace_Color(0,"Color with preferences...");
  Simplify(region,rc,&cl);

  for (iter.Init(&cl); ! iter.Done(); iter.Step()) {
    if ( ! Choose_Register(iter.Current(), region) ) {
      GRA_Trace_Color(0,"Color with preferences failed.");
      return FALSE;
    }
  }

  GRA_Trace_Color(0,"Color with preferences succeeded.");
  return TRUE;
}

/////////////////////////////////////
static void
GRA_Color_Prev_Allocated_Region( GRA_REGION* region )
/////////////////////////////////////
//  No interface description.
/////////////////////////////////////
{
  ISA_REGISTER_CLASS rc;

  GRA_Trace_Color_REGION(region);
  GRA_current_region = region;

  FOR_ALL_ISA_REGISTER_CLASS( rc ) {
    if ( region->Prev_Alloc_By_GRA() )
      Choose_Original_Registers(region,rc);
    else if ( region->Lrange_Count(rc) == 0 )
      continue;
    else {
      // Skip the following to disable coloring of SWP region
#if 0
      if ( ! Color_Region_With_Preferences(region,rc) ) {
      // If we can't get what we want, just take what we already had.
      DevWarn("Couldn't recolor a previously allocated region. "
              "Keeping original.");
#endif
      Choose_Original_Registers(region,rc);
    }
  }
  region->Set_GRA_Colored();
}

/////////////////////////////////////
static BOOL
Must_Split( LRANGE* lrange )
/////////////////////////////////////
//
//  Should we always split instead of spilling?
//
//  Before attempting to color each live range, we check to make sure it's
//  priority is non-negative.  The priority is supposed to approximate the the
//  cost of spilling the live range (loading/restoring it in every block with
//  a reference) vs allocating it (loading/restoring at the points determined
//  by a previous live range split, if there was one.)  However this is a bit
//  crude, becase the spill/restore placement optimization can sometimes
//  improve rather a lot on the cost of allocating (by moving the spills and
//  restores outwared to less frequently traversed blocks).  This was
//  especially obvious with the callee-saves live ranges.  In effect, this
//  function is used to force shrink wrapping, regardless of the priority
//  calculated.
//
/////////////////////////////////////
{
  return    lrange->Type() == LRANGE_TYPE_COMPLEMENT
    && TN_is_save_reg(lrange->Tn()) && GRA_shrink_wrap;
}

/////////////////////////////////////
static void
GRA_Color_Complement( GRA_REGION* region )
/////////////////////////////////////
//  No interface description.
/////////////////////////////////////
{
  ISA_REGISTER_CLASS  rc;
  LRANGE_CLIST        cl;     // Coloring list
  LRANGE_CLIST_ITER   iter;   // Iterator over above
  char buff[100];

  priority_count = 0.0;

  GRA_Trace_Color(0,"Coloring complement region...");
  GRA_current_region = gra_region_mgr.Complement_Region();

  GRA_GRANT_Initialize();

  FOR_ALL_ISA_REGISTER_CLASS_IN_REVERSE( rc ) {
    BOOL forced_locals = FALSE;

    if ( region->Lrange_Count(rc) == 0 ) {
      Force_Color_Some_Locals(region,rc);
      continue;
    }
    GRA_Init_Trace_Memory();
    Simplify(region,rc,&cl);
    GRA_Trace_Memory("Simplify()");

    GRA_Init_Trace_Memory();
    for (iter.Init(&cl); ! iter.Done(); iter.Step()) {
      LRANGE* split_alloc_lr;
      LRANGE* lr = iter.Current();

      if ( ! (forced_locals || lr->Has_Wired_Register()
#ifdef KEY
	      // SWP may have allocated the register.
	      || (lr->Type() == LRANGE_TYPE_COMPLEMENT &&
		  TN_register(lr->Tn()) != REGISTER_UNDEFINED)
#endif
	 ) ) {
        forced_locals = TRUE;
        Force_Color_Some_Locals(region,rc);
      }

      GRA_Trace_Color_LRANGE("Coloring",lr);
      GRA_Trace_Complement_LRANGE_Neighbors(lr, region);

#ifdef TARG_X8664
      // If lrange is a x87/MMX lrange spanning a mixture of x87/MMX OPs, then
      // split the lrange into the part that contains the mixed OPs
      // (split_alloc_lr) and the part that does not.  The latter is put back
      // into the coloring list to be colored later like a regular lrange.
      if ((rc == ISA_REGISTER_CLASS_x87 && lr->Spans_mmx_OP()) ||
	  (rc == ISA_REGISTER_CLASS_mmx && lr->Spans_x87_OP())) {
	LRANGE_Split_Mixed_x87_MMX(lr, &iter, &split_alloc_lr);
	// Localize the part with mixed x87/MMX OPs.
	GRA_Note_Spill(split_alloc_lr);
	continue;
      }
#endif

      // 
      // can't spill wired registers under any circumstances.  the only
      // way we're going to get here is if the frequency on the block is
      // hosed.  we'll definitely get bad code if we've got these kind
      // of bogus frequencies, but its better than incorrect code.
      //
#ifdef KEY
      if (lr->Priority() < 0.0F && !lr->Has_Wired_Register() && !Must_Split(lr)
	  || lr->No_Appearance()) {
        GRA_Note_Spill(lr);
      } else if (lr->Spans_Infreq_Call() &&
                 (!lr->Tn_Is_Save_Reg() ||
                  !REGISTER_SET_MemberP(REGISTER_CLASS_callee_saves(lr->Rc()),TN_save_reg(lr->Tn()))) &&
		 LRANGE_Split(lr, &iter, &split_alloc_lr)) {
	BOOL did_choose = Choose_Register(split_alloc_lr, region);
	FmtAssert(did_choose,("Failed to choose a register for a split across infreq call of %s",
			      split_alloc_lr->Format(buff)));
	priority_count += split_alloc_lr->Priority();
      } else if (Choose_Register(lr, region)) {
	priority_count += lr->Priority();
      } else if (lr->Tn_Is_Save_Reg()) {  // bug 3552: never split saved-TNs
	GRA_Note_Spill(lr);
      } else if (LRANGE_Split(lr, &iter, &split_alloc_lr) &&
		 (split_alloc_lr->Priority() >= 0.0F ||
		  Must_Split(split_alloc_lr))) {
	BOOL did_choose = Choose_Register(split_alloc_lr, region);
	FmtAssert(did_choose,("Failed to choose a register for a split of %s",
			      split_alloc_lr->Format(buff)));
	priority_count += split_alloc_lr->Priority();
      }
#if 0
      // Allocate a reclaimable register to the lrange.  Skip this step because
      // finding a reclaimable register over the entire lrange is unlikely to
      // be successful due to high register pressure.  The code is fully
      // implemented but requires more testing.
      else if (GRA_reclaim_register &&
	       Choose_Reclaimable_Register(split_alloc_lr, region)) {
	priority_count += lr->Priority();
      }
#endif
      // Split lrange to fit in a reclaimable register.
      else if (GRA_reclaim_register &&
	       LRANGE_Split(lr, &iter, &split_alloc_lr, TRUE) &&
	       (split_alloc_lr->Priority() >= 0.0F ||
		Must_Split(split_alloc_lr))) {
	BOOL did_choose = Choose_Reclaimable_Register(split_alloc_lr, region);
	FmtAssert(did_choose,
		  ("Failed to choose a reclaimable register for a split of %s",
		   split_alloc_lr->Format(buff)));
	priority_count += split_alloc_lr->Priority();
      } else {
	GRA_Note_Spill(split_alloc_lr);
      }
#else	// KEY
      if (lr->Priority() < 0.0F && !lr->Has_Wired_Register() && !Must_Split(lr)
	  || lr->No_Appearance()) {
        GRA_Note_Spill(lr);
      } else if (lr->Spans_Infreq_Call() &&
                 (!lr->Tn_Is_Save_Reg() ||
                  !REGISTER_SET_MemberP(REGISTER_CLASS_callee_saves(lr->Rc()),TN_save_reg(lr->Tn()))) &&
		 LRANGE_Split(lr, &iter, &split_alloc_lr)) {
	BOOL did_choose = Choose_Register(split_alloc_lr, region);
	FmtAssert(did_choose,("Failed to choose a register for a split across infreq call of %s",
			      split_alloc_lr->Format(buff)));
	priority_count += split_alloc_lr->Priority();
      } else if (!Choose_Register(lr, region)) {
        if (!LRANGE_Split(lr,&iter,&split_alloc_lr) ||
	    (split_alloc_lr->Priority() < 0.0F &&
	     !Must_Split(split_alloc_lr))) {
          GRA_Note_Spill(split_alloc_lr);
        } else {
          BOOL did_choose = Choose_Register(split_alloc_lr, region);
	  FmtAssert(did_choose,("Failed to choose a register for a split of %s",
			        split_alloc_lr->Format(buff)));
	  priority_count += split_alloc_lr->Priority();
        }
      } else {
	priority_count += lr->Priority();
      }
#endif	// KEY
    }

    GRA_Trace_Memory("Complement coloring loop");
    GRA_Trace_Regs_Stats(rc, REGISTER_CLASS_allocatable(rc), regs_used[rc]);

    if (! forced_locals)        // Haven't done this yet if every lrange wired
      Force_Color_Some_Locals(region,rc);

    if (Is_Predicate_REGISTER_CLASS(rc) &&
	REGISTER_SET_EmptyP(callee_saves_used[rc]))
      GRA_Remove_Predicates_Save_Restore();  // because they're always generated
  }

  gra_region_mgr.Complement_Region()->Set_GRA_Colored();
}

/////////////////////////////////////
void
GRA_Color(void)
/////////////////////////////////////
//  See interface description.
/////////////////////////////////////
{
  GRA_REGION_PREV_ALLOC_ITER iter;

  Initialize();

  GRA_Init_Trace_Memory();
  for (iter.Init(); ! iter.Done(); iter.Step()) {
    GRA_REGION *region = iter.Current();
    GRA_Color_Prev_Allocated_Region(region);
  }
  GRA_Trace_Memory("Gra_Color_Prev_Allocate_Region()");

  GRA_Color_Complement(gra_region_mgr.Complement_Region());
}
