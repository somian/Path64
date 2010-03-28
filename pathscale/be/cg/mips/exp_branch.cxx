/*
 * Copyright (C) 2008 PathScale, LLC.  All Rights Reserved.
 */

/*
 * Copyright 2002, 2003, 2004 PathScale, Inc.  All Rights Reserved.
 */

/*

  Copyright (C) 2000, 2001 Silicon Graphics, Inc.  All Rights Reserved.

  This program is free software; you can redistribute it and/or modify it
  under the terms of version 2 of the GNU General Public License as
  published by the Free Software Foundation.

  This program is distributed in the hope that it would be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  

  Further, this software is distributed without any warranty that it is
  free of the rightful claim of any third person regarding infringement 
  or the like.  Any license provided herein, whether implied or 
  otherwise, applies only to this software file.  Patent licenses, if 
  any, provided herein do not apply to combinations of this program with 
  other software, or any other product whatsoever.  

  You should have received a copy of the GNU General Public License along
  with this program; if not, write the Free Software Foundation, Inc., 59
  Temple Place - Suite 330, Boston MA 02111-1307, USA.

  Contact information:  Silicon Graphics, Inc., 1600 Amphitheatre Pky,
  Mountain View, CA 94043, or:

  http://www.sgi.com

  For further information regarding this notice, see:

  http://oss.sgi.com/projects/GenInfo/NoticeExplan

*/


/* CGEXP routines for expanding branches */

#include "defs.h"
#include "erglob.h"
#include "ercg.h"
#include "tracing.h"
#include "config.h"
#include "config_targ_opt.h"
#include "tn.h"
#include "cg_flags.h"
#include "cg_loop.h"
#include "op.h"
#include "cgexp.h"
#include "cgexp_internals.h"

void
Initialize_Branch_Variants(void)
{
	// nothing to do
}


// Normally, we convert a branch into a comparison TOP (like cmp or
// c_lt_s) followed by a branch TOP (like bne or bltz).  Sometimes
// we can eliminate the comparison TOP (especially for beq, bne, or
// when one of the operands is zero.
//
// Pick_Compare_TOP returns the TOP to use for the comparison
// (or TOP_UNDEFINED if no comparison is necessary).
// Pick_Compare_TOP may also update the branch variant and the
// operands to ensure that comparison and branch have the proper
// format (e.g.: only the second argument can be an integer value).
// If it can eliminate the comparison by modifying the operands
// src1 and src2 and the branch variant, it will do so and return
// TOP_UNDEFINED.
//
// V_br_condition has already been applied to "variant".

TOP
Pick_Compare_TOP (VARIANT *variant, TN **src1, TN **src2, OPS *ops)
{
  // If both src1 and src2 are values, fold comparison.
  // Bug 14568: This can (rarely) happen.
  if ( *src1 != NULL && *src2 != NULL &&
       ( TN_has_value(*src1) || TN_is_zero(*src1) ) &&
       ( TN_has_value(*src2) || TN_is_zero(*src2) ) ) {
    INT32  tn1_i4 = 0, tn2_i4 = 0;
    INT64  tn1_i8 = 0, tn2_i8 = 0;
    UINT32 tn1_u4 = 0, tn2_u4 = 0;
    UINT64 tn1_u8 = 0, tn2_u8 = 0;
    if ( TN_has_value(*src1) ) {
      tn1_i4 = TN_value(*src1);
      tn1_i8 = TN_value(*src1);
      tn1_u4 = TN_value(*src1);
      tn1_u8 = TN_value(*src1);
    }
    if ( TN_has_value(*src2) ) {
      tn2_i4 = TN_value(*src2);
      tn2_i8 = TN_value(*src2);
      tn2_u4 = TN_value(*src2);
      tn2_u8 = TN_value(*src2);
    }
    BOOL taken;
    switch ( *variant ) {
    case V_BR_I4EQ:  taken = ( tn1_i4 == tn2_i4 );  break;
    case V_BR_I4NE:  taken = ( tn1_i4 != tn2_i4 );  break;
    case V_BR_I4GE:  taken = ( tn1_i4 >= tn2_i4 );  break;
    case V_BR_I4GT:  taken = ( tn1_i4 >  tn2_i4 );  break;
    case V_BR_I4LE:  taken = ( tn1_i4 <= tn2_i4 );  break;
    case V_BR_I4LT:  taken = ( tn1_i4 <  tn2_i4 );  break;
    case V_BR_I8EQ:  taken = ( tn1_i8 == tn2_i8 );  break;
    case V_BR_I8NE:  taken = ( tn1_i8 != tn2_i8 );  break;
    case V_BR_I8GE:  taken = ( tn1_i8 >= tn2_i8 );  break;
    case V_BR_I8GT:  taken = ( tn1_i8 >  tn2_i8 );  break;
    case V_BR_I8LE:  taken = ( tn1_i8 <= tn2_i8 );  break;
    case V_BR_I8LT:  taken = ( tn1_i8 <  tn2_i8 );  break;
    case V_BR_U4EQ:  taken = ( tn1_u4 == tn2_u4 );  break;
    case V_BR_U4NE:  taken = ( tn1_u4 != tn2_u4 );  break;
    case V_BR_U4GE:  taken = ( tn1_u4 >= tn2_u4 );  break;
    case V_BR_U4GT:  taken = ( tn1_u4 >  tn2_u4 );  break;
    case V_BR_U4LE:  taken = ( tn1_u4 <= tn2_u4 );  break;
    case V_BR_U4LT:  taken = ( tn1_u4 <  tn2_u4 );  break;
    case V_BR_U8EQ:  taken = ( tn1_u8 == tn2_u8 );  break;
    case V_BR_U8NE:  taken = ( tn1_u8 != tn2_u8 );  break;
    case V_BR_U8GE:  taken = ( tn1_u8 >= tn2_u8 );  break;
    case V_BR_U8GT:  taken = ( tn1_u8 >  tn2_u8 );  break;
    case V_BR_U8LE:  taken = ( tn1_u8 <= tn2_u8 );  break;
    case V_BR_U8LT:  taken = ( tn1_u8 <  tn2_u8 );  break;
    default:
      FmtAssert( FALSE, ("unimplemented branch variant in Pick_Compare_TOP") );
    }
    *variant = taken ? V_BR_ALWAYS : V_BR_NEVER;
    return TOP_UNDEFINED;
  }

  // Try to swap operands and change variant to move value to src2.
  BOOL swap = FALSE;
  if ( *src1 != NULL && *src2 != NULL && ! TN_has_value(*src2)
       && ( TN_has_value(*src1) || TN_is_zero(*src1) ) ) {
    TN *tn_tmp = *src1;
    *src1 = *src2;
    *src2 = tn_tmp;
    *variant = Invert_BR_Variant( *variant );
  }

  // Handle all cases in which a branch with a constant operand
  // can be converted to ALWAYS, NEVER, or a Zero_TN operand.
  // First check for zero arguments:
  if ( *src2 == NULL ) ;
  else if ( TN_is_zero(*src2) ) {
    *src2 = Zero_TN;
    switch ( *variant ) {
    case V_BR_U8LT:
    case V_BR_U4LT:  *variant = V_BR_NEVER;  break;
    case V_BR_U8GE:
    case V_BR_U4GE:  *variant = V_BR_ALWAYS; break;
    case V_BR_U8LE:  *variant = V_BR_U8EQ0;  break;
    case V_BR_U4LE:  *variant = V_BR_U4EQ0;  break;
    case V_BR_U8GT:  *variant = V_BR_U8NE0;  break;
    case V_BR_U4GT:  *variant = V_BR_U4NE0;  break;
    case V_BR_I8LE:  *variant = V_BR_I8LE0;  break;
    case V_BR_I4LE:  *variant = V_BR_I4LE0;  break;
    case V_BR_I8LT:  *variant = V_BR_I8LT0;  break;
    case V_BR_I4LT:  *variant = V_BR_I4LT0;  break;
    case V_BR_I8GE:  *variant = V_BR_I8GE0;  break;
    case V_BR_I4GE:  *variant = V_BR_I4GE0;  break;
    case V_BR_I8GT:  *variant = V_BR_I8GT0;  break;
    case V_BR_I4GT:  *variant = V_BR_I4GT0;  break;
    }
  }
  // Then check for nonzero integer arguments:
  else if ( ! TN_has_value(*src2) ) ;
  else if ( TN_value(*src2) == 1 ) {
    switch ( *variant ) {
    case V_BR_U8LT:  *variant = V_BR_U8EQ0;  break;
    case V_BR_U4LT:  *variant = V_BR_U4EQ0;  break;
    case V_BR_U8GE:  *variant = V_BR_U8NE0;  break;
    case V_BR_U4GE:  *variant = V_BR_U4NE0;  break;
    case V_BR_I8LT:  *variant = V_BR_I8LE0;  break;
    case V_BR_I4LT:  *variant = V_BR_I4LE0;  break;
    case V_BR_I8GE:  *variant = V_BR_I8GT0;  break;
    case V_BR_I4GE:  *variant = V_BR_I4GT0;  break;
    }
  }
  else if ( TN_value(*src2) == -1 ) {
    switch ( *variant ) {
    case V_BR_U8LE:
    case V_BR_U4LE:  *variant = V_BR_ALWAYS; break;
    case V_BR_U8GT:
    case V_BR_U4GT:  *variant = V_BR_NEVER;  break;
    case V_BR_I8LE:  *variant = V_BR_I8LT0;  break;
    case V_BR_I4LE:  *variant = V_BR_I4LT0;  break;
    case V_BR_I8GT:  *variant = V_BR_I8GE0;  break;
    case V_BR_I4GT:  *variant = V_BR_I4GE0;  break;
    }
  }
  else if ( TN_value(*src2) == (INT64) -1 << 63 ) {
    switch ( *variant ) {
    case V_BR_I8GE:  *variant = V_BR_ALWAYS; break;
    case V_BR_I8LT:  *variant = V_BR_NEVER;  break;
    case V_BR_U8GE:  *variant = V_BR_I8LT0;  break;
    case V_BR_U8LT:  *variant = V_BR_I8GE0;  break;
    }
  }
  else if ( TN_value(*src2) == (INT64) -1 << 31 ) {
    switch ( *variant ) {
    case V_BR_I4GE:  *variant = V_BR_ALWAYS;  break;
    case V_BR_I4LT:  *variant = V_BR_NEVER;  break;
    case V_BR_U4GE:  *variant = V_BR_I4LT0;  break;
    case V_BR_U4LT:  *variant = V_BR_I4GE0;  break;
    }
  }
  else if ( ~ TN_value(*src2) == (INT64) -1 << 63 ) {
    switch ( *variant ) {
    case V_BR_I8LE:  *variant = V_BR_ALWAYS;  break;
    case V_BR_I8GT:  *variant = V_BR_NEVER;  break;
    case V_BR_U8LE:  *variant = V_BR_I8GE0;  break;
    case V_BR_U8GT:  *variant = V_BR_I8LT0;  break;
    }
  }
  else if ( ~ TN_value(*src2) == (INT64) -1 << 31 ) {
    switch ( *variant ) {
    case V_BR_I4LE:  *variant = V_BR_ALWAYS;  break;
    case V_BR_I4GT:  *variant = V_BR_NEVER;  break;
    case V_BR_U4LE:  *variant = V_BR_I4GE0;  break;
    case V_BR_U4GT:  *variant = V_BR_I4LT0;  break;
    }
  }

  switch ( *variant ) {

  // Quit early if the comparison OP is unnecessary
  case V_BR_ALWAYS: case V_BR_NEVER:
  case V_BR_I8GE0:  case V_BR_I4GE0:
  case V_BR_I8GT0:  case V_BR_I4GT0:
  case V_BR_I8LE0:  case V_BR_I4LE0:
  case V_BR_I8LT0:  case V_BR_I4LT0:
  case V_BR_I8EQ0:  case V_BR_I4EQ0:
  case V_BR_U8EQ0:  case V_BR_U4EQ0:
  case V_BR_I8NE0:  case V_BR_I4NE0:
  case V_BR_U8NE0:  case V_BR_U4NE0:
    return TOP_UNDEFINED;

  // Convert integer GT --> GE or LT, LE --> LT or GE
  // Convert float GT --> LT, GE --> LE
  case V_BR_I8GT:  case V_BR_I4GT:
  case V_BR_I8LE:  case V_BR_I4LE:
  case V_BR_U8GT:  case V_BR_U4GT:
  case V_BR_U8LE:  case V_BR_U4LE:
    if ( TN_has_value(*src2) ) {
      INT64 val = TN_value( *src2 );
      *src2 = Gen_Literal_TN( val + 1, TN_size(*src2) );
      switch ( *variant ) {
      case V_BR_I8GT:  *variant = V_BR_I8GE;  break;
      case V_BR_I4GT:  *variant = V_BR_I4GE;  break;
      case V_BR_I8LE:  *variant = V_BR_I8LT;  break;
      case V_BR_I4LE:  *variant = V_BR_I4LT;  break;
      case V_BR_U8GT:  *variant = V_BR_U8GE;  break;
      case V_BR_U4GT:  *variant = V_BR_U4GE;  break;
      case V_BR_U8LE:  *variant = V_BR_U8LT;  break;
      case V_BR_U4LE:  *variant = V_BR_U4LT;  break;
      }
      break;
    }
    // If src2 of integer GT/LE not constant value, fall through
    // to use same swapping code as float GT/GE case.

  case V_BR_FGT:  case V_BR_DGT:
  case V_BR_FGE:  case V_BR_DGE:
    // Swap operands and change variant
    TN *tn_tmp = *src1;
    *src1 = *src2;
    *src2 = tn_tmp;
    *variant = Invert_BR_Variant( *variant );
    break;

  }

  // Choose TOP for this variant.  Integer GT and LE and float
  // GT and GE have already been converted to other variants.
  // Integer EQ and NE don't need comparison OP.
  TOP cmp = TOP_UNDEFINED, cmp_i = TOP_UNDEFINED;
  switch ( *variant ) {
  case V_BR_I8LT:  case V_BR_I4LT:
  case V_BR_I8GE:  case V_BR_I4GE:
    cmp_i = TOP_slti;  cmp = TOP_slt;
    break;
  case V_BR_U8LT:  case V_BR_U4LT:
  case V_BR_U8GE:  case V_BR_U4GE:
    cmp_i = TOP_sltiu;  cmp = TOP_sltu;
    break;
  case V_BR_FEQ:  cmp = TOP_c_eq_s;   break;
  case V_BR_DEQ:  cmp = TOP_c_eq_d;   break;
  case V_BR_FLT:  cmp = TOP_c_lt_s;   break;
  case V_BR_DLT:  cmp = TOP_c_lt_d;   break;
  case V_BR_FLE:  cmp = TOP_c_le_s;   break;
  case V_BR_DLE:  cmp = TOP_c_le_d;   break;
  case V_BR_FNE:  cmp = TOP_c_neq_s;  break;
  case V_BR_DNE:  cmp = TOP_c_neq_d;  break;
  }

  // If src2 is immediate and fits, use immediate form of top
  if ( TN_has_value(*src2) ) {
    if ( cmp_i != TOP_UNDEFINED &&
	 ISA_LC_Value_In_Class( TN_value(*src2), LC_simm16 ) ) {
      cmp = cmp_i;
    } else {
      BOOL is_64bit = TN_size( *src1 ) == 8;
      *src2 = Expand_Immediate_Into_Register( *src2, is_64bit, ops );
    }
  }

  return cmp;
}


void
Expand_Branch ( TN *targ, TN *src1, TN *src2, VARIANT variant, OPS *ops)
{
  BOOL false_br = V_false_br( variant ) != 0;
  VARIANT cond = V_br_condition( variant );

  if ( Trace_Exp2 ) {
    fprintf ( TFile, "<cgexp> Translating %s branch:\n",
	      false_br ? "false" : "true" );
  }

  FmtAssert( cond <= V_BR_LAST, ("unexpected variant in Expand_Branch") );
  FmtAssert( cond != V_BR_NONE, ("BR_NONE variant in Expand_Branch") );

  TOP cmp = Pick_Compare_TOP ( &cond, &src1, &src2, ops );
  if ( Trace_Exp2 && cond != variant ) {
    fprintf( TFile, "<cgexp> transformed branch cond = %" SCNd64 "\n", cond );
  }

  if ( cond == V_BR_ALWAYS || cond == V_BR_NEVER ) {

    Is_True( cmp == TOP_UNDEFINED,
	     ( "Expand_Branch: Unexpected compare op for %s",
	       BR_Variant_Name(cond) ) );
    if ( (cond == V_BR_ALWAYS) ^ false_br ) {
      // Unconditional branch for ALWAYS/!false_br and NEVER/false_br
      Build_OP (TOP_j, targ, ops);
    }

  } else if ( cmp != TOP_UNDEFINED && TOP_is_flop(cmp) ) { 
    // floating point conditional branch

    // c_neq_s and c_neq_d are not recognized by the Octane assembler;
    if ( cmp == TOP_c_neq_s ) {
      cmp = TOP_c_eq_s;
      false_br = ! false_br;
    } else if ( cmp == TOP_c_neq_d ) {
      cmp = TOP_c_eq_d;
      false_br = ! false_br;
    }
    TN *fcc_tmp = Gen_Register_TN( ISA_REGISTER_CLASS_fcc, 1 );
    Build_OP( cmp, fcc_tmp, src1, src2, ops );
    Build_OP( false_br ? TOP_bc1f : TOP_bc1t, fcc_tmp, targ, ops );

  } else {
    // integer conditional branch

    if ( cmp != TOP_UNDEFINED ) {
      // Build comparison OP
      TN *tn_tmp = Build_TN_Of_Mtype( MTYPE_I4 );
      Build_OP( cmp, tn_tmp, src1, src2, ops );
      src1 = tn_tmp;
      src2 = Zero_TN;
    }

    // Select branch TOP
    TOP top = TOP_UNDEFINED;
    switch ( cond ) {
    case V_BR_I8GE0:  case V_BR_I4GE0:  top = TOP_bgez;  break;
    case V_BR_I8GT0:  case V_BR_I4GT0:  top = TOP_bgtz;  break;
    case V_BR_I8LE0:  case V_BR_I4LE0:  top = TOP_blez;  break;
    case V_BR_I8LT0:  case V_BR_I4LT0:  top = TOP_bltz;  break;
    case V_BR_I8EQ0:  case V_BR_U8EQ0:
    case V_BR_I4EQ0:  case V_BR_U4EQ0:  src2 = Zero_TN;  // FALL-THRU
    case V_BR_I8GE:   case V_BR_U8GE:
    case V_BR_I4GE:   case V_BR_U4GE:
    case V_BR_I8EQ:   case V_BR_U8EQ:
    case V_BR_I4EQ:   case V_BR_U4EQ:   top = TOP_beq;  break;
    case V_BR_I8NE0:  case V_BR_U8NE0:
    case V_BR_I4NE0:  case V_BR_U4NE0:  src2 = Zero_TN;  // FALL-THRU
    case V_BR_I8LT:   case V_BR_U8LT:
    case V_BR_I4LT:   case V_BR_U4LT:
    case V_BR_I8NE:   case V_BR_U8NE:
    case V_BR_I4NE:   case V_BR_U4NE:   top = TOP_bne;  break;
    default:
      FmtAssert( FALSE, ("unimplemented branch variant in Expand_Branch") );
    }

#if 1
    // Negate_BR_Variant doesn't set false_br for integer compares.
    FmtAssert( ! false_br, ( "Expand_Branch: Unexpected false branch"
			     " for %s", BR_Variant_Name(cond) ) );
#else
    if ( false_br ) {
      switch ( top ) {
      case TOP_bgez:  top = TOP_bltz;  break;
      case TOP_bgtz:  top = TOP_blez;  break;
      case TOP_blez:  top = TOP_bgtz;  break;
      case TOP_bltz:  top = TOP_bgez;  break;
      case TOP_beq:   top = TOP_bne;   break;
      case TOP_bne:   top = TOP_beq;   break;
      }
    }
#endif

    if ( top == TOP_bne || top == TOP_beq ) {
      Build_OP( top, src1, src2, targ, ops );
    } else {
      Build_OP( top, src1, targ, ops );
    }

  }
}


void Exp_Indirect_Branch (TN *targ_reg, OPS *ops)
{
  Build_OP(TOP_jr, targ_reg, ops);
}


void Exp_Local_Jump(BB *bb, INT64 offset, OPS *ops) {
  FmtAssert (offset == 0, ("NYI: Offset support for Exp_Local_Jump."));
  
  OP *jmp = Mk_OP (TOP_j);
  TN *tn  = Gen_Label_TN (Gen_Label_For_BB (bb), 0);
  Set_OP_opnd (jmp, 
               Branch_Target_Operand (jmp),
               tn);
  OPS_Append_Op (ops, jmp);

  return;
}


void Exp_Return (TN *return_address, OPS *ops)
{
  Build_OP(TOP_jr, return_address, ops);
}


void Exp_Call (OPERATOR opr, TN *return_address, TN *target, OPS *ops)
{
  TOP top;
  switch (opr) {
  case OPR_CALL:
    top = TOP_jal;
    break;
  case OPR_ICALL:
#if 0
    if ( ! Get_Trace (TP_CGEXP, 256)) {
      // target is at 0(target), gp at 8(target)
      OPCODE opc = OPCODE_make_op (OPR_LDID, Pointer_Mtype, Pointer_Mtype);
      TN *tmp1 = Build_TN_Of_Mtype (Pointer_Mtype);
      Expand_Load (opc, GP_TN, target, Gen_Literal_TN(8, 4), V_NONE, ops);
      Expand_Load (opc, tmp1, target, Gen_Literal_TN(0, 4), V_NONE, ops);
      target = tmp1;
    }
#endif
    /*FALLTHROUGH*/
  case OPR_PICCALL:
    top = TOP_jalr;
    break;
  default:
    FmtAssert(FALSE, ("unexpected opr in Exp_Call"));
    /*NOTREACHED*/
  }
  Build_OP (top, return_address, target, ops);
}
