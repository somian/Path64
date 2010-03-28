/*
 *  Copyright (C) 2008 PathScale, LLC.  All Rights Reserved.
 */

/*
 * Copyright 2002, 2003, 2004 PathScale, Inc.  All Rights Reserved.
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
 *  Target Specific Miscellany Declarations which include target
 *  dependencies.
 *
 *  THIS FILE IS ONLY TO BE INCLUDE BY ../cgtarget.h!!!!
 *
 * ====================================================================
 * ====================================================================
 */

#define INST_BYTES 4
#define DEFAULT_LONG_BRANCH_LIMIT (0x1ffffff * INST_BYTES)
#define DEFAULT_BRP_BRANCH_LIMIT DEFAULT_LONG_BRANCH_LIMIT
#define MIN_BRANCH_DISP (2097151 * INST_BYTES)

inline BOOL CGTARG_Branch_Always_Predicted_Taken(void)
{
  if (Is_Target_Mips5kf())
    return TRUE;
  else if (Is_Target_twc9a())
    return FALSE;
  FmtAssert(FALSE, ("CGTARG_Branch_Always_Predicted_Taken: unknown target"));
  return FALSE;
}

inline INT32 CGTARG_Branch_Taken_Penalty(void)
{
  if (Is_Target_Mips5kf() ||
      Is_Target_twc9a()) {
    return CGTARG_branch_taken_penalty_overridden ?
	     CGTARG_branch_taken_penalty : 0;
  }
  FmtAssert(FALSE, ("CGTARG_Branch_Taken_Penalty: unknown target"));
  return -1;
}

inline INT32 CGTARG_Issue_Width(void)
{
  if (Is_Target_Mips5kf() ||
      Is_Target_twc9a()) {
    return 2;
  }
  FmtAssert(FALSE, ("CGTARG_Issue_Width: unknown target"));
  return -1;
}

inline BOOL 
CGTARG_Use_Brlikely(float branch_taken_probability)
{
  return FALSE;
}

inline BOOL
CGTARG_Can_Predicate_Calls()
{
  return FALSE;
}


inline BOOL
CGTARG_Can_Predicate_Returns()
{
  return FALSE;
}

inline BOOL
CGTARG_Can_Predicate_Branches()
{
  return FALSE;
}

inline INT
CGTARG_Text_Alignment (void)
{
  return 4;
}

inline TOP
CGTARG_Noop_Top(void)
{
  return TOP_nop;	// return simulated nop
}

inline TOP
CGTARG_Noop_Top(ISA_EXEC_UNIT_PROPERTY unit)
{
  return CGTARG_Noop_Top();
}

inline TOP
CGTARG_Simulated_Top(OP *op, ISA_EXEC_UNIT_PROPERTY unit)
{

  TOP top = OP_code(op);
 
  /* Prune the obvious cases. */
  if (!TOP_is_simulated(top)) return top;

  /* Placeholder for itemizing specific simulated ops */
  return top;
}

/*
 *     TN *CGTARG_Return_Enum_TN(OP *op, ISA_ENUM_CLASS class, INT *pos)
 *     if 'op' has an enum operand which matches the ISA_ENUM_CLASS 
 *     <class> type, return the enum TN and its operand position.
 */
inline TN* 
CGTARG_Return_Enum_TN(OP *op, ISA_ENUM_CLASS_VALUE enum_class, INT *pos)
{
  INT i;

  for (i = 0; i < OP_opnds(op); ++i) {
    TN *opnd_tn = OP_opnd(op, i);
    if (TN_is_enum(opnd_tn) && (enum_class == TN_enum(opnd_tn))) {
      *pos = i;
      return opnd_tn;
    }
  }

  return NULL;
}

inline BOOL 
CGTARG_Is_OP_Barrier(OP *op)
{
  if (OP_code(op) == TOP_asm) {
    extern OP_MAP OP_Asm_Map;
    ASM_OP_ANNOT* asm_info = (ASM_OP_ANNOT*) OP_MAP_Get(OP_Asm_Map, op);
    return (WN_Asm_Clobbers_Mem(ASM_OP_wn(asm_info)));
  } else
    return FALSE;
}

inline BOOL
CGTARG_Is_OP_Intrinsic(OP *op)
{
  return OP_code(op) == TOP_intrncall;
}

inline BOOL
CGTARG_Is_Bad_Shift_Op (OP *op)
{
  return FALSE;
}

inline BOOL
CGTARG_Is_Right_Shift_Op (OP *op)
{
  switch (OP_code(op)) {
  case TOP_sra:
  case TOP_srav:
  case TOP_srl:
  case TOP_srlv:
  case TOP_dsra:
  case TOP_dsra32:
  case TOP_dsrav:
  case TOP_dsrl:
  case TOP_dsrl32:
  case TOP_dsrlv:
    return TRUE;
  }
  return FALSE;
}
    
inline BOOL 
CGTARG_Is_OP_Addr_Incr(OP *op)
{
  TOP top = OP_code(op);
  return((top == TOP_addi || top == TOP_addiu || 
	  top == TOP_daddi || top == TOP_daddiu) &&
	 TN_has_value(OP_opnd(op, 1)) &&
	 (OP_result(op, 0) == OP_opnd(op, 0)));
}

inline BOOL
CGTARG_Can_daddu_Be_Folded(OP *op1, OP *op2)
{
  return FALSE;
}

inline TOP
CGTARG_Copy_FP_Op(TYPE_ID mtype)
{
  TOP top = TOP_UNDEFINED;
  if (mtype == MTYPE_F4) top = TOP_mov_s;
  else if (mtype == MTYPE_F8) top = TOP_mov_d;
  else if (mtype == MTYPE_V8F4) top = TOP_mov_ps;
  return top;
}

#if 0
inline TOP
CGTARG_Copy_Op(TYPE_ID mtype)
{
  if (MTYPE_is_float(mtype)) return CGTARG_Copy_FP_Op(mtype);
  else return MTYPE_byte_size(mtype) < 8 ? TOP_add : TOP_dadd;
}
#endif

inline BOOL
CGTARG_Have_Indexed_Mem_Insts(void)
{
  if (Is_Target_Mips5kf() ||
      Is_Target_twc9a()) {
    return TRUE;
  }
  FmtAssert(FALSE, ("CGTARG_Have_Indexed_Mem_Insts: unknown target"));
  return FALSE;
}

inline BOOL
Has_Single_FP_Condition_Code (void)
{
  return FALSE;
}

inline BOOL
CGTARG_Is_OP_daddu(OP *op)
{
  TOP top = OP_code(op);
  return top == TOP_daddu;
}

inline BOOL
CGTARG_Can_Predicate()
{
  return FALSE;
}

extern TOP CGTARG_Get_unc_Variant(TOP top);
extern void Make_Branch_Conditional(BB *bb);

inline BOOL
CGTARG_Has_Branch_Predict(void)
{
  return TRUE;
}
