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


/* =======================================================================
 * =======================================================================
 *
 *  Module: reg_live.h
 *  $Revision$
 *  $Date$
 *  $Author$
 *  $Source$
 *
 *  Description:
 *  ============
 *
 *  Interface to physical register global live range analysis.
 *  The routine to compute the register liveness information is:
 *
 *     void REG_LIVE_Analyze_Region(void)
 *
 *  When the register liveness information is no longer needed,
 *  the following routine is used to allow the facility to clean up.
 *
 *     void REG_LIVE_Finish(void)
 *
 * Utilities:
 * ==========
 *
 *  The following two routines check if a given <cl,reg> is
 *  live at entry/exit to a basic block <bb>. These can be 
 *  called only after register liveness has been computed
 *  using REG_LIVE_Analyze_Region.
 *
 *     BOOL REG_LIVE_Into_BB(ISA_REGISTER_CLASS cl, REGISTER reg, BB *bb);
 *     BOOL REG_LIVE_Outof_BB(ISA_REGISTER_CLASS cl, REGISTER reg, BB *bb);
 *
 *
 *  The following two routines check for implicit uses and defs
 *  of <cl,reg> at the boundaries of basic blocks. These are intended
 *  for use by clients before register allocation to check on the
 *  liveness for dedicated TNs. The GRA_LIVE sets are not fully
 *  accurate for dedicated TNs. The GRA_LIVE routines for checking
 *  liveness call the following routines for dedicated TNs.
 *  The implicit uses are for parameters and return registers for
 *  procedures, sp, gp, fp.
 *
 *     BOOL REG_LIVE_Implicit_Use_Outof_BB (
 *		ISA_REGISTER_CLASS cl, REGISTER reg, BB *bb);
 *     BOOL REG_LIVE_Implicit_Def_Into_BB (
 *		ISA_REGISTER_CLASS cl, REGISTER reg, BB *bb);
 *
 *
 *   The following two interfaces are to support the adjustment
 *   of the entry/exit stack allocation/deallocation (at some point
 *   this needs to be better integrated into the reg_live package):
 *
 *   void REG_LIVE_Prolog_Temps(BB *bb, OP *first, OP *last, 
 *				REGISTER_SET *temps)
 *	Determine the temp registers which are available for use
 *	in the allocation of the stack frame. <bb> is the entry BB.
 *	<first> and <last> are the first and last OPs of the allocation
 *	sequence. <temps> points to an array of REGISTERs to hold the
 *	temps for each register class.
 *
 *
 *  void REG_LIVE_Epilog_Temps(ST *pu_st, BB *bb, OP *adj_op, 
 *			       REGISTER_SET *temps)
 *	Determine the temp registers which are available for use
 *	in the deallocation of the stack frame. <pu_st> is the symbol
 *	for the PU. <bb> is the exit BB. <adj_op> is the stack frame
 *	deallocation OP. <temps> points to an array of REGISTERs to hold the
 *      temps for each register class.
 *
 * =======================================================================
 * =======================================================================
 */
#ifndef reg_live_INCLUDED
#define reg_live_INCLUDED

#include "register.h"
#include "bb.h"

/* Define REGSET to be an array of REGISTER_SETs. */
typedef REGISTER_SET *REGSET;

/* Utility functions to manipulate REGSETS (arrays of REGISTER_SETS). */
extern void REGSET_ASSIGN(REGSET set1, REGSET set2);
extern void REGSET_CLEAR(REGSET set);
extern void REGSET_OR(REGSET set1, REGSET set2);
extern BOOL REGSET_EQUALS(REGSET set1, REGSET set2);
extern BOOL REGSET_INTERSECT(REGSET set1, REGSET set2);
extern void REGSET_Print(REGSET set);

void REG_LIVE_Analyze_Region(void);
void REG_LIVE_Finish(void);
BOOL REG_LIVE_Into_BB(ISA_REGISTER_CLASS cl, REGISTER reg, BB *bb);
BOOL REG_LIVE_Outof_BB(ISA_REGISTER_CLASS cl, REGISTER reg, BB *bb);
BOOL REG_LIVE_Implicit_Use_Outof_BB (ISA_REGISTER_CLASS cl, REGISTER reg, BB *bb);
BOOL REG_LIVE_Implicit_Def_Into_BB (ISA_REGISTER_CLASS cl, REGISTER reg, BB *bb);
void REG_LIVE_Update(ISA_REGISTER_CLASS cl, REGISTER reg, BB *bb);

void REG_LIVE_Prolog_Temps(BB *bb, OP *first, OP *last, REGISTER_SET *temps);
void REG_LIVE_Epilog_Temps(ST *pu_st, BB *bb, OP *adj_op, REGISTER_SET *temps);

#endif /* reg_live_INCLUDED */
