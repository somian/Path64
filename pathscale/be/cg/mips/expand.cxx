/*
 * Copyright (C) 2007, 2008, 2009 Pathscale, LLC.  All Rights Reserved.
 */

/*
 * Copyright (C) 2007. QLogic Corporation. All Rights Reserved.
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


/* ====================================================================
 * ====================================================================
 *
 * Module: expand.c
 * $Revision: 1.28 $
 * $Date: 03/03/12 22:31:46-00:00 $
 * $Author: fchow@keyresearch.com $
 * $Source: /home/bos/bk/kpro64-tmp/be/cg/MIPS/SCCS/s.expand.cxx $
 *
 * Description:
 *
 * This file contains the internals of code expansion. Its interface
 * is 'Exp_OP', which takes an OP, expands it into a list of OPs which
 * are appended to the oplist passed in.
 *
 * It handles all the macro expansions, special handling during 
 * expansion and all the nitty gritty stuff that goes along with it.
 *
 * ====================================================================
 * ====================================================================
 */

#define __STDC_LIMIT_MACROS
#include <stdint.h>
#include "defs.h"
#include "config.h"
#include "config_targ_opt.h"
#include "erglob.h"
#include "ercg.h"
#include "glob.h"
#include "tracing.h"
#include "util.h"

#include "tn.h"
#include "cg_flags.h"
#include "bb.h"
#include "symtab.h"
#include "opcode.h"
#include "const.h"	/* needed to manipulate target/host consts */
#include "targ_const.h"	/* needed to manipulate target/host consts */
#include "op.h"
#include "data_layout.h"
#include "stblock.h"
#include "cgexp.h"
#include "cgexp_internals.h"
#include "w2op.h"
#include "label_util.h"
#include "cgtarget.h"
#include "whirl2ops.h"
#include "targ_sim.h"   /* To generate stores of param registers in builtin_apply_args */
#include "intrn_info.h" /* for INTRN_rt_name */

BOOL Reuse_Temp_TNs = FALSE;

BOOL Trace_Exp2 = FALSE;      /* extra cgexp trace*/

/* Dup_TN won't dup a dedicated tn, but for our purposes we
 * can just re-use the dedicated tn.  Don't want to re-use a
 * symbolic tn or it will mess up live ranges. */
/* DOESN'T WORK:  causes problems in Create_lvs because it causes
 * a use of a parm reg at the call-site, so it looks like the
 * parm-reg is incoming at the call?  This probably should work,
 * but for now we can use other routine that create a real dup tn. */
#define DUP_TN(tn)	Dup_TN_Even_If_Dedicated(tn)

void
Expand_Copy (TN *result, TN *src, TYPE_ID mtype, OPS *ops)
{
  if (MTYPE_is_float(mtype)) {
    TOP top = CGTARG_Copy_FP_Op(mtype);
    Build_OP(top, result, src, ops);
  } else {
    Build_OP(TOP_or, result, src, Zero_TN, ops);
  }
  Set_OP_copy (OPS_last(ops));
}

//
//  Helper routine to do proper sign extension
//  11679: All 32-bit MIPS values are sign extended (even U4-op results)
//
static void
Fixup_32_Bit_Op(TN *result,TN *src, TYPE_ID dest_type, OPS *ops)
{
  if (dest_type == MTYPE_I8 || dest_type == MTYPE_U8) {
    Expand_Copy(result,src,dest_type,ops);
  } else {
    Expand_Convert_Length (result, src, Gen_Literal_TN(MTYPE_size_reg(dest_type), 4),
			   dest_type, TRUE, ops);
			   // dest_type, MTYPE_is_signed(dest_type),ops);
  }
}


/* ====================================================================
 *
 * Expand_Convert_Length
 *
 * Generate code to expand an xCVTL operator.  The code generated is a
 * left shift to put the upper bit to be kept in the high bit of the
 * word or double-word, followed by a right shift back to either sign-
 * or zero-extend it as requested.
 *
 * ====================================================================
 */

void
Expand_Convert_Length ( TN *dest, TN *src, TN *length_tn, TYPE_ID mtype, BOOL signed_extension, OPS *ops)
{
  TN *lit_tn;
  FmtAssert (! MTYPE_float(mtype),
	     ("Expand_Convert_Length: illegal data type\n"));
  FmtAssert (TN_has_value(length_tn),
	     ("Expand_Convert_Length: non-constant length\n"));
  UINT64 val = TN_value(length_tn);
  if (val <= 16 && ! signed_extension) 
    Build_OP(TOP_andi, dest, src, Gen_Literal_TN((1 << val) - 1, 4), ops);
  else if (val == 32 && signed_extension && Sext_Allowed) {
    // Bug 13497: sll tgt_tn, src_tn, 0x0 safely sign extends
    Build_OP(TOP_sext, dest, src, ops);
  } else if (MTYPE_is_size_double(mtype) || val == 32) {
    if (val > 32) {
      TN *tmp = Build_TN_Of_Mtype(mtype);
      lit_tn = Gen_Literal_TN(64 - val, 4);
      Build_OP(TOP_dsll, tmp, src, lit_tn, ops);
      Build_OP(signed_extension ? TOP_dsra : TOP_dsrl, dest, tmp, lit_tn, 
	       ops);
    }
    else {
      TN *tmp = Build_TN_Of_Mtype(mtype);
      lit_tn = Gen_Literal_TN(32 - val, 4);
      Build_OP(TOP_dsll32, tmp, src, lit_tn, ops);
      Build_OP(signed_extension ? TOP_dsra32 : TOP_dsrl32, dest, tmp, lit_tn, 
	       ops);
    }
  }
  else {
    TN *tmp = Build_TN_Of_Mtype(mtype);
    lit_tn = Gen_Literal_TN(32 - val, 4);
    Build_OP(TOP_sll, tmp, src, lit_tn, ops);
    Build_OP(signed_extension ? TOP_sra : TOP_srl, dest, tmp, lit_tn, ops);
  }
}

static void
Exp_Immediate (TN *dest, TN *src, OPS *ops)
{
  INT64 val;
  TN *tmp = Build_TN_Like(dest);
  if ( TN_has_value(src) ) {
    val = TN_value(src);
  }
  else if ( TN_is_symbol(src) ) {
    ST *base;
    Base_Symbol_And_Offset_For_Addressing (TN_var(src), TN_offset(src), &base, 
		    &val);
  }
  else FmtAssert(FALSE,("unexpected constant in Exp_Immediate"));

  if (TN_is_symbol(src) && TN_relocs(src) == TN_RELOC_GPSUB) {
    Build_OP(TOP_lui, tmp, 
		    Gen_Symbol_TN(TN_var(src), 0, TN_RELOC_HI_GPSUB), ops);
    Build_OP(TOP_addiu, dest, tmp, Gen_Symbol_TN(TN_var(src), 0, 
			    TN_RELOC_LO_GPSUB), ops);
  }
  else if (ISA_LC_Value_In_Class (val, LC_simm16)) {
    Build_OP (TOP_addiu, dest, Zero_TN, src, ops);
  }
  else if (ISA_LC_Value_In_Class (val, LC_uimm16)) {
    Build_OP (TOP_ori, dest, Zero_TN, src, ops);
  }
  else if (val >= INT32_MIN && val <= INT32_MAX) {
    Build_OP (TOP_lui, tmp, Gen_Literal_TN((val >> 16)&0xffff, 4), ops);
    Build_OP (TOP_ori, dest, tmp, Gen_Literal_TN(val & 0xffff, 4), ops);
  }
  else if ((UINT64)val <= UINT32_MAX) {
    if (TN_size(dest) == 4)
      Build_OP (TOP_lui, tmp, Gen_Literal_TN((val >> 16) & 0xffff, 4), ops);
    else {
      Build_OP (TOP_ori, tmp, Zero_TN, 
		Gen_Literal_TN((val >> 16) & 0xffff, 4), ops);
      Build_OP (TOP_dsll, tmp, tmp, Gen_Literal_TN(16, 4), ops);
    }
    Build_OP (TOP_ori, dest, tmp, Gen_Literal_TN(val & 0xffff, 4), ops);
  }
  else if (Max_Sdata_Elt_Size >= 8 && Gspace_Available >= 8) {  // 14498
    TCON tcon = Host_To_Targ (MTYPE_I8, val);
    ST *sym = New_Const_Sym (Enter_tcon (tcon), Be_Type_Tbl(MTYPE_I8));
    Allocate_Object(sym);
    FmtAssert((ST_gprel(sym)), ("sym for 64-bit immediate not gp_rel"));
    Build_OP(TOP_ld, dest, GP_TN,
	     Gen_Symbol_TN(sym, 0, TN_RELOC_GPREL16), ops);
  }
  else if ( (INT32) val == (INT32) (val >> 32) ) {
    // Upper 32 bits match lower 32-bits (perhaps bitmask)
    TN *tmp1 = Build_TN_Like(dest);
    TN *tmp2 = Build_TN_Like(dest);
    Build_OP (TOP_lui, tmp, Gen_Literal_TN((val >> 16) & 0xffff, 4), ops);
    Build_OP (TOP_ori, tmp1, tmp, Gen_Literal_TN(val & 0xffff, 4), ops);
    Build_OP (TOP_dsll32, tmp2, tmp1, Gen_Literal_TN(0, 4), ops);
    Build_OP (TOP_or, dest, tmp1, tmp2, ops);
  }
  else {
    // In case of -G below 8, build 64-bit immediate the hard way
    TN *tmp1 = Build_TN_Like(dest);
    TN *tmp2 = Build_TN_Like(dest);
    TN *tmp3 = Build_TN_Like(dest);
    TN *tmp4 = Build_TN_Like(dest);
    Build_OP (TOP_lui, tmp, Gen_Literal_TN((val >> 48)&0xffff, 4), ops);
    Build_OP (TOP_ori, tmp1, tmp, Gen_Literal_TN((val >> 32)&0xffff, 4), ops);
    Build_OP (TOP_dsll, tmp2, tmp1, Gen_Literal_TN(16, 4), ops);
    Build_OP (TOP_ori, tmp3, tmp2, Gen_Literal_TN((val >> 16)&0xffff, 4), ops);
    Build_OP (TOP_dsll, tmp4, tmp3, Gen_Literal_TN(16, 4), ops);
    Build_OP (TOP_ori, dest, tmp4, Gen_Literal_TN(val & 0xffff, 4), ops);
  }
}

void
Exp_Immediate (TN *dest, TN *src, BOOL is_signed, OPS *ops)
{
  Expand_Immediate(dest, src, is_signed, ops);
}

/* 
 * Expand Immediate value.
 */
void
Expand_Immediate (TN *dest, TN *src, BOOL is_signed, OPS *ops)
{
  FmtAssert((TN_is_constant(src)),
	    ("unexpected non-constant in Expand_Immediate"));
  FmtAssert((TN_has_value(src) || TN_is_symbol(src)), 
	    ("expected value or const in Expand_Immediate"));
  Exp_Immediate (dest, src, ops);
}

TN*
Expand_Immediate_Into_Register (TN *src, BOOL is_64bit, OPS *ops)
{
  if (TN_value(src) == 0)
    return Zero_TN;
  /* load into reg and do reg case */
  TN *tmp = Build_TN_Of_Mtype (is_64bit ? MTYPE_I8 : MTYPE_I4);
  Expand_Immediate (tmp, src, TRUE, ops);
  return tmp;
}

void
Expand_Add (TN *result, TN *src1, TN *src2, TYPE_ID mtype, OPS *ops)
{
  FmtAssert ((MTYPE_bit_size(mtype) == 32 || MTYPE_bit_size(mtype) == 64),
	               ("Expand_Add: illegal result size\n"));
  TOP new_opcode;
  INT64 val;
  BOOL is_64bit = MTYPE_bit_size(mtype) == 64;
  if (TN_is_constant(src1)) {
    if (TN_has_value(src1)) {
      val = TN_value(src1);
      if (val == 0) {
	Expand_Copy (result, src2, mtype, ops);
	return;
      }
    } else if ( TN_is_symbol(src1) ) {
      /* symbolic constant, gp-relative or sp-relative */
      ST *base;
      INT64 val;
      TN *tmp = Build_TN_Of_Mtype (mtype);
      Base_Symbol_And_Offset_For_Addressing (TN_var(src1), TN_offset(src1), 
					     &base, &val);
      new_opcode = is_64bit ? TOP_daddiu : TOP_addiu;
      if (ISA_LC_Value_In_Class (val, LC_simm16) || 
	  ISA_LC_Value_In_Class (val, LC_uimm16)) {
	Build_OP (new_opcode, result, src2, src1, ops);
      } else if (val >= INT32_MIN && val <= INT32_MAX) {
	if (TN_relocs(src1) == TN_RELOC_LOW16) {
	  // Add only the low 16-bit.  Bug 10568.
	  Expand_Add(result, src2, Gen_Literal_TN(val & 0xffff, 4),
		     is_64bit ? MTYPE_U8 : MTYPE_U4, ops);
	} else {
	  Build_OP (TOP_lui, tmp, Gen_Literal_TN((val >> 16)&0xffff, 4), ops);
	  Build_OP (TOP_ori, tmp, tmp, Gen_Literal_TN(val & 0xffff, 4), ops);
	  Build_OP (is_64bit ? TOP_daddu : TOP_addu, result, tmp, src2, ops);
	}
      } else {
	TCON tcon = Host_To_Targ (MTYPE_I8, val);
	ST *sym = New_Const_Sym (Enter_tcon (tcon), Be_Type_Tbl(MTYPE_I8));
	Allocate_Object(sym);
	if (Use_32_Bit_Pointers)
	  Build_OP(TOP_lw, tmp, GP_TN,
		   Gen_Symbol_TN(sym, 0, TN_RELOC_GOT_DISP), ops);
	else
	  Build_OP(TOP_ld, tmp, GP_TN,
		   Gen_Symbol_TN(sym, 0, TN_RELOC_GOT_DISP), ops);
	Build_OP(TOP_ld, tmp, tmp, Gen_Literal_TN(0, 4), ops);
	Build_OP (is_64bit ? TOP_daddu : TOP_addu, result, tmp, src2, ops);
      }       
      return;
    } 
    else FmtAssert(FALSE,("unexpected constant in Expand_Add"));
    
    if (ISA_LC_Value_In_Class ( val, LC_simm16)) {
      new_opcode = is_64bit ? TOP_daddiu : TOP_addiu;
      Build_OP (new_opcode, result, src2, Gen_Literal_TN(val,4), ops);
    } else {
      src1 = Expand_Immediate_Into_Register (src1, is_64bit, ops);
      new_opcode = is_64bit ? TOP_daddu : TOP_addu;
      Build_OP (new_opcode, result, src2, src1, ops);
    }
  } else if (TN_is_constant(src2)) {
    // switch order of src so immediate is first
    Expand_Add (result, src2, src1, mtype, ops);
  } else {
  	Build_OP (is_64bit ? TOP_daddu : TOP_addu, result, src1, src2, ops);
  }
}

void
Expand_Sub (TN *result, TN *src1, TN *src2, TYPE_ID mtype, OPS *ops)
{
  FmtAssert ((MTYPE_bit_size(mtype) == 32 || MTYPE_bit_size(mtype) == 64),
	               ("Expand_Sub: illegal result size\n"));
  TOP new_opcode;
  INT64 val;
  BOOL is_64bit = MTYPE_bit_size(mtype) == 64;
  if (TN_is_constant(src2)) {
	if (TN_has_value(src2)) {
		val = - TN_value(src2);
		if (val == 0) {
			Expand_Copy (result, src1, mtype, ops);
			return;
		}
	} 
	else if ( TN_is_symbol(src2) ) {
		/* symbolic constant, gp-relative or sp-relative */
		ST *base;
		INT64 val;
		Base_Symbol_And_Offset_For_Addressing (TN_var(src2), TN_offset(src2), &base, &val);
		val = - val;
	} 
	else FmtAssert(FALSE,("unexpected constant in Expand_Sub"));

	if (ISA_LC_Value_In_Class ( val, LC_simm16)) {
		new_opcode = is_64bit ? TOP_daddiu : TOP_addiu;
		Build_OP (new_opcode, result, src1, Gen_Literal_TN(val,4), ops);
	} else {
		src2 = Expand_Immediate_Into_Register (src2, is_64bit, ops);
		new_opcode = is_64bit ? TOP_dsubu : TOP_subu;
		Build_OP (new_opcode, result, src1, src2, ops);
	}
  }
  else if (TN_is_constant(src1)) {
    TN *tmp = Build_TN_Of_Mtype (mtype);
  	// switch order of src so immediate is first
	Expand_Sub (tmp, src2, src1, mtype, ops);
	// generate a negate
	Build_OP(is_64bit ? TOP_dsubu : TOP_subu, result, Zero_TN, tmp, ops);
  } 
  else {
  	Build_OP (is_64bit ? TOP_dsubu : TOP_subu, result, src1, src2, ops);
  }
}


void
Expand_Neg (TN *result, TN *src, TYPE_ID mtype, OPS *ops)
{
  BOOL is_64bit = MTYPE_bit_size(mtype) == 64;
  FmtAssert ((MTYPE_bit_size(mtype) == 32 || MTYPE_bit_size(mtype) == 64),
	               ("Expand_Neg: illegal result size\n"));
  Build_OP (is_64bit ? TOP_dsubu : TOP_subu, result, Zero_TN, src, ops);
}


void
Expand_Abs (TN *dest, TN *src, TYPE_ID mtype, OPS *ops)
{
  TN *p1 = Build_TN_Of_Mtype(MTYPE_I4);
  BOOL is_signed = MTYPE_is_signed(mtype);
  BOOL is_double = MTYPE_is_size_double(mtype);
  if (src != dest) {
    Build_OP(is_double ? TOP_dsubu : TOP_subu, dest, Zero_TN, src, ops);
    Build_OP(is_signed?TOP_slt:TOP_sltu, p1, src, Zero_TN, ops);
    Build_OP(TOP_movz, dest, src, p1, dest, ops);
  }
  else {
    TN *tmp = Build_TN_Of_Mtype(mtype);
    Build_OP(is_double ? TOP_dsubu : TOP_subu, tmp, Zero_TN, src, ops);
    Build_OP(is_signed?TOP_slt:TOP_sltu, p1, src, Zero_TN, ops);
    Build_OP(TOP_movn, dest, tmp, p1, dest, ops);
  }
  Set_OP_cond_def_kind(OPS_last(ops), OP_ALWAYS_COND_DEF);      
}

void
Expand_Shift (TN *result, TN *src1, TN *src2, TYPE_ID mtype, SHIFT_DIRECTION kind, OPS *ops)
{
  WN *tree;
  TOP top;  
  BOOL is_64bit = MTYPE_is_size_double(mtype);

  if (TN_is_constant(src1))
    src1 = Expand_Immediate_Into_Register(src1, MTYPE_is_size_double(mtype), ops);
  if (TN_has_value(src2)) {
    // In mips, only the low log2(wordsize) bits of the shift count are used. 
    UINT64 val = TN_value(src2);
    switch (kind) {
    case shift_left:
      if (! is_64bit)
	top = TOP_sll;
      else if (val < 32) 
	top = TOP_dsll;
      else top = TOP_dsll32;
      break;
    case shift_aright:
      if (! is_64bit)
	top = TOP_sra;
      else if (val < 32) 
	top = TOP_dsra;
      else top = TOP_dsra32;
      break;
    case shift_lright:
      if (! is_64bit)
	top = TOP_srl;
      else if (val < 32) 
	top = TOP_dsrl;
      else top = TOP_dsrl32;
      break;
    }
    Build_OP(top, result, src1, Gen_Literal_TN(val & 31, 4), ops);
  }
  else {
    switch (kind) {
    case shift_left:
      top = is_64bit ? TOP_dsllv : TOP_sllv;
      break;
  
    case shift_aright:
      top = is_64bit ? TOP_dsrav : TOP_srav;
      break;
  
    case shift_lright:
      top = is_64bit ? TOP_dsrlv : TOP_srlv;
      break;
    }
    Build_OP(top, result, src1, src2, ops);
  }
}

inline void
Expand_G_To_F (TN *ftn, TN *gtn, OPS *ops)
{ FmtAssert(FALSE,("Unimplemented")); }

inline void
Expand_F_To_G (TN *gtn, TN *ftn, OPS *ops)
{ FmtAssert(FALSE,("Unimplemented")); }


/*
 * Expand_Multiply_Into_Shifts expands multiply into a series of shifts,
 * additions, and subtractions, and appends the generated code onto ops.
 * r = x * val.  mtype is the type of the multiplication operation
 * (which rarely may differ from the TN type).
 * Caller must handle cases of -1, 0, and 1.
 */

static void
Expand_Multiply_Into_Shifts(TN *r, TN *x, INT64 val, TYPE_ID mtype, OPS *ops)
{
  // Assumes val is not 0, 1, nor -1.

  OPS ops_r;  // We will construct list of OPs bottom up
  OPS_Init(&ops_r);
  OP *op;
  BOOL is64bit = MTYPE_size_reg(mtype) == 64;
  INT twop = 0;
  if (val & 0x1) {

    TN *r1 = Build_TN_Like(r);

    if ((val & 0x3) == 0x3) {
      val = val + 1;
      op = Mk_OP(is64bit ? TOP_dsubu : TOP_subu, r, r1, x);
    } else if (val > 0) {
      val = val - 1;
      op = Mk_OP(is64bit ? TOP_daddu : TOP_addu, r, r1, x);
    } else {
      val = 1 - val;
      op = Mk_OP(is64bit ? TOP_dsubu : TOP_subu, r, x, r1);
    }
    OPS_Prepend_Op(&ops_r, op);
    r = r1;
  }

  while (true) {

    // Trim powers of two off of val.
    do { val >>= 1; ++twop; } while (! (val & 0x1));

    if (val == 1) {
      if (twop < 32) {
	op = Mk_OP(is64bit ? TOP_dsll : TOP_sll,
		   r, x, Gen_Literal_TN(twop, 4));
      } else {
	op = Mk_OP(TOP_dsll32, r, x, Gen_Literal_TN(twop - 32, 4));
      }
      OPS_Prepend_Op(&ops_r, op);
      break;
    }

    TN *r1 = Build_TN_Like(r);

    if (val == -1) {
      if (twop < 32) {
	op = Mk_OP(is64bit ? TOP_dsll : TOP_sll,
		   r, r1, Gen_Literal_TN(twop, 4));
      } else {
	op = Mk_OP(TOP_dsll32, r, r1, Gen_Literal_TN(twop - 32, 4));
      }
      OPS_Prepend_Op(&ops_r, op);
      op = Mk_OP(is64bit ? TOP_dsubu : TOP_subu, r1, Zero_TN, x);
      OPS_Prepend_Op(&ops_r, op);
      break;
    }

    TN *r2 = Build_TN_Like(r);

    if ((val & 0x3) == 0x3) {
      val = val + 1;
      op = Mk_OP(is64bit ? TOP_dsubu : TOP_subu, r, r1, r2);
    } else if (val > 0) {
      val = val - 1;
      op = Mk_OP(is64bit ? TOP_daddu : TOP_addu, r, r1, r2);
    } else {
      val = 1 - val;
      op = Mk_OP(is64bit ? TOP_dsubu : TOP_subu, r, r2, r1);
    }

    OPS_Prepend_Op(&ops_r, op);
    if (twop < 32) {
      op = Mk_OP(is64bit ? TOP_dsll : TOP_sll, r2, x, Gen_Literal_TN(twop, 4));
    } else {
      op = Mk_OP(TOP_dsll32, r2, x, Gen_Literal_TN(twop - 32, 4));
    }
    OPS_Prepend_Op(&ops_r, op);
    r = r1;
  }

  OPS_Append_Ops(ops, &ops_r);  // Append ops_r onto ops
}


// Count the number of left shifts, adds, and subtract OPs that would be
// generated by Expand_Multiply_Into_Shifts
static INT
Count_Multiply_Into_Shifts(INT64 val)
{
  INT count = 0;
  if (val & 0x1) {
    count += 1;
    if ((val & 0x3) == 0x3) val = val + 1;
    else if (val > 0) val = val - 1;
    else val = 1 - val;
  }

  while (true) {
    do val >>= 1; while (! (val & 0x1));
    if (val == 1) return count + 1;
    if (val == -1) return count + 2;
    count += 2;
    if ((val & 0x3) == 0x3) val = val + 1;
    else if (val > 0) val = val - 1;
    else val = 1 - val;
  }
}


/*
 *  Try to expand a multiply into a sequence of less expensive operations.
 */
#define NUM_FAST_MPYS 10
static INT fast_mpys[NUM_FAST_MPYS] = {65,63,33,31,17,15,9,7,5,3};

static BOOL
Expand_Constant_Multiply (TN *result, TN *var_tn, TARG_INT constant,
			  INT16 limit, TYPE_ID mtype, OPS *ops)
{
  INT64 c = constant; // I don't want to depend on TARG_INT

  // fast special cases
  if (c == 0) {
    Expand_Copy(result, Zero_TN, mtype, ops);
    return TRUE;
  } else if (c == 1) {
    Expand_Copy(result, var_tn, mtype, ops);
    return TRUE;
  } else if (c == -1) {
    Expand_Neg(result, var_tn, mtype, ops);
    return TRUE;
  }

  // By default, we will use Expand_Multiply_Into_Shifts
  INT i, count_ops = Count_Multiply_Into_Shifts(c);

  // Look for more efficent approaches
  if (count_ops > 5 && 4 <= limit) {
    for (i = 0; i < NUM_FAST_MPYS; ++i) {
      INT mpy = fast_mpys[i];

      // Can we factor c == (2^e +/- 1) * k ?
      if (c % mpy == 0) {
	INT64 cq = c / mpy;
	INT cnt = Count_Multiply_Into_Shifts(cq);
	if (cnt + 2 <= limit && cnt + 2 < count_ops) {
	  TN *r1 = Build_TN_Like(result);
	  if (Expand_Constant_Multiply(r1, var_tn, cq, limit-2, mtype, ops)) {
	    Expand_Multiply_Into_Shifts(result, r1, mpy, mtype, ops);
	    return TRUE;
	  }
	}
      }
    }
  }

  if (count_ops > 6 && 5 <= limit) {
    for (i = 0; i < NUM_FAST_MPYS; ++i) {
      INT mpy = fast_mpys[i];

      // Can we factor c == (2^e +/- 1) * k + 1 ?
      if ((c - 1) % mpy == 0) {
	INT64 cq = (c - 1) / mpy;
	INT cnt = Count_Multiply_Into_Shifts(cq);
	if (cnt + 3 <= limit && cnt + 3 < count_ops) {
	  TN *r1 = Build_TN_Like(result);
	  TN *r2 = Build_TN_Like(result);
	  if (Expand_Constant_Multiply(r1, var_tn, cq, limit-3, mtype, ops)) {
	    Expand_Multiply_Into_Shifts(r2, r1, mpy, mtype, ops);
	    Expand_Add(result, r2, var_tn, mtype, ops);
	    return TRUE;
	  }
	}
      }

      // Can we factor c == (2^e +/- 1) * k - 1 ?
      if ((c + 1) % mpy == 0) {
	INT64 cq = (c + 1) / mpy;
	INT cnt = Count_Multiply_Into_Shifts(cq);
	if (cnt + 3 <= limit && cnt + 3 < count_ops) {
	  TN *r1 = Build_TN_Like(result);
	  TN *r2 = Build_TN_Like(result);
	  if (Expand_Constant_Multiply(r1, var_tn, cq, limit-3, mtype, ops)) {
	    Expand_Multiply_Into_Shifts(r2, r1, mpy, mtype, ops);
	    Expand_Sub(result, r2, var_tn, mtype, ops);
	    return TRUE;
	  }
	}
      }
    }
  }

  // Could also try c == ((2^e +/- 1) * k +/- 1) * 2^f, as in 172 and 212,
  // but this is reaching diminishing returns (using 6 ops).

  // Use default approach
  if (count_ops > limit) return FALSE;
  Expand_Multiply_Into_Shifts(result, var_tn, c, mtype, ops);
  return TRUE;
}


// Latency of multiplication plus expanding constant into register,
// plus mflow to copy answer into register
#define MULTIPLICATION_LATENCY_32_BIT 5  /* 1 + 3 + 1 */
#define MULTIPLICATION_LATENCY_64_BIT 7  /* 2 + 4 + 1 */


void
Expand_Multiply (TN *result, TN *src1, TN *src2, TYPE_ID mtype, OPS *ops)
{
  TOP top;
  INT64 constant;
  //
  // Check for two constants
  // 
  // TODO: check this portion of Exapnd_Multiply once divrem is retargeted.
  if ((TN_has_value(src1) || TN_is_rematerializable(src1)) &&
      (TN_has_value(src2) || TN_is_rematerializable(src2))) {
    // Two constants can sometimes occur because of DIVREM production in 
    TN *val_tn;
    constant = TN_has_value(src1) ? TN_value(src1) : WN_const_val(TN_home(src1));
    constant *= TN_has_value(src2) ? TN_value(src2) : WN_const_val(TN_home(src2));
    // Need to get the constant of the right length
    constant = Targ_To_Host(Host_To_Targ(mtype,constant));
    val_tn = Gen_Literal_TN(constant, 8);
    Exp_Immediate(result,val_tn,MTYPE_is_signed(mtype),ops);
    return;
  }

  if (CGEXP_cvrt_int_mult_to_add_shift &&
      (TN_has_value(src1) || TN_has_value(src2) ||
       TN_is_rematerializable(src1) || TN_is_rematerializable(src2))) {
    TN *var_tn;
    if ( TN_has_value(src1) || TN_is_rematerializable(src1) ) {
      constant = TN_has_value(src1) ? TN_value(src1) : WN_const_val(TN_home(src1));
      var_tn = src2;
    }
    else {
      constant = TN_has_value(src2) ? TN_value(src2) : WN_const_val(TN_home(src2));
      var_tn = src1;
    }

    // Try to convert multiply into shifts/adds/subs if profitable
    // Instead of calling Can_Do_Fast_Multiply, Expand_Constant_Multiply
    // will check for latency limit.
    INT16 limit = (MTYPE_is_size_double(mtype) ? MULTIPLICATION_LATENCY_32_BIT
		   : MULTIPLICATION_LATENCY_64_BIT);
    if (Expand_Constant_Multiply (result, var_tn, constant,
				  limit, mtype, ops)) return;
  }
  if (TN_has_value(src2)) {
    src2 = Expand_Immediate_Into_Register (src2, MTYPE_is_size_double(mtype), ops);
  }

  FmtAssert(!TN_is_constant(src1),("Expand_Multiply: unexpected constant operand"));
  if (! MTYPE_is_size_double(mtype)) {
    if (MTYPE_signed(mtype)) {
      Build_OP(TOP_mul, result, Hilo_TN(), src1, src2, ops);
      return;
    }
    top = TOP_multu;
  } else top = MTYPE_signed(mtype) ? TOP_dmult : TOP_dmultu;
  if (TN_is_constant(src2))
    src2 = Expand_Immediate_Into_Register(src2, MTYPE_is_size_double(mtype), ops);
  Build_OP(top, Hilo_TN(), src1, src2, ops);
  Build_OP(TOP_mflo, result, Hilo_TN(), ops);
}

/* return high part of multiply result */
void
Expand_High_Multiply (TN *result, TN *src1, TN *src2, TYPE_ID mtype, OPS *ops)
{
  TOP top;
  FmtAssert(!TN_is_constant(src1),("Expand_High_Multiply: unexpected constant operand"));
  if (!MTYPE_is_size_double(mtype))
    top = MTYPE_signed(mtype) ? TOP_mult : TOP_multu;
  else top = MTYPE_signed(mtype) ? TOP_dmult : TOP_dmultu;
  if (TN_is_constant(src2))
    src2 = Expand_Immediate_Into_Register(src2, MTYPE_is_size_double(mtype), ops);
  Build_OP(top, Hilo_TN(), src1, src2, ops);
  Build_OP(TOP_mfhi, result, Hilo_TN(), ops);
}


void
Expand_Logical_Not (TN *dest, TN *src, VARIANT variant, OPS *ops)
{
  /* dest = (src == 0) ? 1 : 0 */
  Build_OP (TOP_xori, dest, src, Gen_Literal_TN(1, 4), ops);
}

void
Expand_Logical_And (TN *dest, TN *src1, TN *src2, VARIANT variant, OPS *ops)
{
  Build_OP (TOP_and, dest, src1, src2, ops);
}

void
Expand_Logical_Or (TN *dest, TN *src1, TN *src2, VARIANT variant, OPS *ops)
{
  Build_OP (TOP_or, dest, src1, src2, ops);
}


void
Expand_Binary_Complement (TN *dest, TN *src, TYPE_ID /* mtype */, OPS *ops)
{
  Build_OP(TOP_nor, dest, src, Zero_TN, ops);
}

void
Expand_Binary_And (TN *dest, TN *src1, TN *src2, TYPE_ID mtype, OPS *ops)
{
  FmtAssert ((MTYPE_bit_size(mtype) == 32 || MTYPE_bit_size(mtype) == 64),
	               ("Expand_Binary_And: illegal result size\n"));
  if (TN_is_constant(src1)) {
    INT64 val;
    if (TN_has_value(src1)) {
	    val = TN_value(src1);
	    if (val == -1) {
		    Expand_Copy (dest, src2, mtype, ops);
		    return;
	    }
    } 
    else FmtAssert(FALSE,("unexpected constant in Expand_Binary_And"));

    TOP new_opcode;
    if (ISA_LC_Value_In_Class ( val, LC_uimm16)) 
      new_opcode = TOP_andi;
    else {
      src1 = Expand_Immediate_Into_Register(src1, MTYPE_bit_size(mtype) == 64,
					    ops);
      new_opcode = TOP_and;
    }
    Build_OP (new_opcode, dest, src2, src1, ops);
  }
  else if (TN_is_constant(src2)) {
    // switch order of src so immediate is first
    Expand_Binary_And (dest, src2, src1, mtype, ops);
  } 
  else {
    Build_OP (TOP_and, dest, src1, src2, ops);
  }
}

void
Expand_Binary_Or (TN *dest, TN *src1, TN *src2, TYPE_ID mtype, OPS *ops)
{
  FmtAssert ((MTYPE_bit_size(mtype) == 32 || MTYPE_bit_size(mtype) == 64),
	               ("Expand_Binary_Or: illegal dest size\n"));
  if (TN_is_constant(src1)) {
    INT64 val;
    if (TN_has_value(src1)) {
	    val = TN_value(src1);
	    if (val == 0) {
		    Expand_Copy (dest, src2, mtype, ops);
		    return;
	    }
    } 
    else FmtAssert(FALSE,("unexpected constant in Expand_Binary_Or"));

    TOP new_opcode;
    if (ISA_LC_Value_In_Class ( val, LC_uimm16)) 
      new_opcode = TOP_ori;
    else {
      src1 = Expand_Immediate_Into_Register(src1, MTYPE_bit_size(mtype) == 64,
					    ops);
      new_opcode = TOP_or;
    }
    Build_OP (new_opcode, dest, src2, src1, ops);
  }
  else if (TN_is_constant(src2)) {
    // switch order of src so immediate is first
    Expand_Binary_Or (dest, src2, src1, mtype, ops);
  } 
  else {
    Build_OP (TOP_or, dest, src1, src2, ops);
  } 
}

void
Expand_Binary_Xor (TN *dest, TN *src1, TN *src2, TYPE_ID mtype, OPS *ops)
{
  FmtAssert ((MTYPE_bit_size(mtype) == 32 || MTYPE_bit_size(mtype) == 64),
	               ("Expand_Binary_Xor: illegal dest size\n"));
  if (TN_is_constant(src1)) {
    INT64 val;
    if (TN_has_value(src1)) 
      val = TN_value(src1);
    else FmtAssert(FALSE,("unexpected constant in Expand_Binary_Xor"));
    if (val == 0 && src1 == dest)
      return;

    TOP new_opcode;
    if (ISA_LC_Value_In_Class ( val, LC_uimm16)) 
      new_opcode = TOP_xori;
    else {
      src1 = Expand_Immediate_Into_Register(src1, MTYPE_bit_size(mtype) == 64,
					    ops);
      new_opcode = TOP_xor;
    }
    Build_OP (new_opcode, dest, src2, src1, ops);
  }
  else if (TN_is_constant(src2)) {
    // switch order of src so immediate is first
    Expand_Binary_Xor (dest, src2, src1, mtype, ops);
  } 
  else {
    Build_OP (TOP_xor, dest, src1, src2, ops);
  }
}

void
Expand_Binary_Nor (TN *dest, TN *src1, TN *src2, TYPE_ID mtype, OPS *ops)
{
  Build_OP (TOP_nor, dest, src1, src2, ops);
}

void
Expand_Int_Less (TN *dest, TN *src1, TN *src2, TYPE_ID mtype, OPS *ops)
{
  FmtAssert ((MTYPE_bit_size(mtype) == 32 || MTYPE_bit_size(mtype) == 64),
	               ("Expand_Int_Less: illegal dest size\n"));

  // Deal with constant src1
  if (TN_has_value(src1) && ! TN_has_value(src2)) {
    INT64 val = TN_value(src1);
    if ((mtype == MTYPE_U4 && (UINT32) val >= UINT32_MAX) ||
	(mtype == MTYPE_I4 &&          val >=  INT32_MAX) ||
	(mtype == MTYPE_U8 && (UINT64) val == UINT64_MAX) ||
	(mtype == MTYPE_I8 &&          val ==  INT64_MAX)) {
      // Bug 13393: Unsigned -1 < j ==> false
      // More generally, MAXINTVAL < j ==> false
      Expand_Copy(dest, Zero_TN, mtype, ops);
      return;
    }
    if (ISA_LC_Value_In_Class(val + 1, LC_simm16)) {
      //  N < j  =>  !(j <= N)  ==>  !(j < N+1)
      TN *tmp = Build_TN_Of_Mtype(mtype);
      src1 = Gen_Literal_TN(val + 1, MTYPE_byte_size(mtype));
      Expand_Int_Less(tmp, src2, src1, mtype, ops);
      Build_OP(TOP_xori, dest, tmp, Gen_Literal_TN(1, 4), ops);
      return;
    }
  }
  if (TN_is_constant(src1)) {
    src1 = Expand_Immediate_Into_Register(src1,
					  MTYPE_bit_size(mtype) == 64, ops);
  }

  TOP opcode = MTYPE_is_signed(mtype) ? TOP_slt : TOP_sltu;
  if (TN_is_constant(src2)) {
    INT64 val;
    if (TN_has_value(src2)) {
      val = TN_value(src2);
    } else if ( TN_is_symbol(src2) ) {
      /* symbolic constant, gp-relative or sp-relative */
      ST *base;
      Base_Symbol_And_Offset_For_Addressing (TN_var(src2), TN_offset(src2),
					     &base, &val);
    }
    else FmtAssert(FALSE,("unexpected constant in Expand_Int_Less"));

    if (ISA_LC_Value_In_Class(val, LC_simm16)) {
      opcode = MTYPE_is_signed(mtype) ? TOP_slti : TOP_sltiu;
    } else {
      src2 = Expand_Immediate_Into_Register(src2, MTYPE_bit_size(mtype) == 64,
					    ops);
    }
  }
  Build_OP(opcode, dest, src1, src2, ops);
}

void
Expand_Int_Less_Equal (TN *dest, TN *src1, TN *src2, TYPE_ID mtype, OPS *ops)
{
  FmtAssert ((MTYPE_bit_size(mtype) == 32 || MTYPE_bit_size(mtype) == 64),
	               ("Expand_Int_Less_Equal: illegal dest size\n"));
  if (TN_has_value(src2)) {
    INT64 val = TN_value(src2);
    if ((mtype == MTYPE_U4 && (UINT32) val >= UINT32_MAX) ||
	(mtype == MTYPE_I4 &&          val >=  INT32_MAX) ||
	(mtype == MTYPE_U8 && (UINT64) val == UINT64_MAX) ||
	(mtype == MTYPE_I8 &&          val ==  INT64_MAX)) {
      //  i <= MAXINTVAL  =>  true
      Expand_Immediate(dest, Gen_Literal_TN(1, 4), FALSE, ops);
    } else {
      //  i <= N  =>  i < N+1
      src2 = Gen_Literal_TN(val + 1, MTYPE_byte_size(mtype));
      Expand_Int_Less(dest, src1, src2, mtype, ops);
    }
  } else {
    //  i <= j  =>  !(j < i)
    TN *tmp = Build_TN_Of_Mtype(mtype);
    Expand_Int_Less(tmp, src2, src1, mtype, ops);
    Build_OP(TOP_xori, dest, tmp, Gen_Literal_TN(1, 4), ops);
  }
}

void
Expand_Int_Greater (TN *dest, TN *src1, TN *src2, TYPE_ID mtype, OPS *ops)
{
  Expand_Int_Less(dest, src2, src1, mtype, ops);
}

void
Expand_Int_Greater_Equal (TN *dest, TN *src1, TN *src2, TYPE_ID mtype, OPS *ops)
{
  Expand_Int_Less_Equal(dest, src2, src1, mtype, ops);
}

void
Expand_Int_Equal (TN *dest, TN *src1, TN *src2, TYPE_ID mtype, OPS *ops)
{
  TN *tmp_tn;
  if ((TN_size(dest) == MTYPE_byte_size(mtype)) &&
      !TN_is_dedicated(dest))
    tmp_tn = dest;
  else tmp_tn  = Gen_Typed_Register_TN(mtype, MTYPE_byte_size(mtype));
  Expand_Binary_Xor(tmp_tn, src1, src2, mtype, ops);
  Build_OP (TOP_sltiu, dest, tmp_tn, Gen_Literal_TN(1, 4), ops);
}

void
Expand_Int_Not_Equal (TN *dest, TN *src1, TN *src2, TYPE_ID mtype, OPS *ops)
{
  TN *tmp_tn;
  if ((TN_size(dest) == MTYPE_byte_size(mtype)) && 
      !TN_is_dedicated(dest))
    tmp_tn = dest;
  else tmp_tn  = Gen_Typed_Register_TN(mtype, MTYPE_byte_size(mtype));
  Expand_Binary_Xor(tmp_tn, src1, src2, mtype, ops);
  Build_OP (TOP_sltu, dest, Zero_TN, tmp_tn, ops);
}

static void
Expand_Bool_Comparison (BOOL equals, TN *dest, TN *src1, TN *src2, OPS *ops)
{ FmtAssert(FALSE,("Unimplemented")); }

void
Expand_Bool_Equal (TN *dest, TN *src1, TN *src2, OPS *ops)
{ FmtAssert(FALSE,("Unimplemented")); }

void
Expand_Bool_Not_Equal (TN *dest, TN *src1, TN *src2, OPS *ops)
{ FmtAssert(FALSE,("Unimplemented")); }

void
Expand_Bool_To_Int (TN *dest, TN *src, TYPE_ID rtype, OPS *ops)
{ FmtAssert(FALSE,("Unimplemented")); }

typedef enum {
  ROUND_USER,
  ROUND_NEAREST,
  ROUND_CHOP,
  ROUND_NEG_INF,
  ROUND_PLUS_INF
} ROUND_MODE;

// TODO how do you trap on float val too big for [u]int32?
static void
Expand_Float_To_Int (ROUND_MODE rm, TN *dest, TN *src, TYPE_ID imtype, TYPE_ID fmtype, OPS *ops)
{
  TOP top;
  BOOL int_64bit = MTYPE_byte_size(imtype) == 8;
  if (fmtype == MTYPE_F4) {
    if (int_64bit || !(MTYPE_is_signed(imtype))) {
      switch (rm) {
      case ROUND_USER: top = TOP_cvt_l_s; break;
      case ROUND_NEAREST: top = TOP_round_l_s; break;
      case ROUND_CHOP: top = TOP_trunc_l_s; break;
      case ROUND_NEG_INF: top = TOP_floor_l_s; break;
      case ROUND_PLUS_INF: top = TOP_ceil_l_s; break;
      default: FmtAssert(FALSE,("Unimplemented rounding mode"));
      }
    }
    else {
      switch (rm) {
      case ROUND_USER: top = TOP_cvt_w_s; break;
      case ROUND_NEAREST: top = TOP_round_w_s; break;
      case ROUND_CHOP: top = TOP_trunc_w_s; break;
      case ROUND_NEG_INF: top = TOP_floor_w_s; break;
      case ROUND_PLUS_INF: top = TOP_ceil_w_s; break;
      default: FmtAssert(FALSE,("Unimplemented rounding mode"));
      }
    }
  }
  else if (fmtype == MTYPE_F8) {
    if (int_64bit || !(MTYPE_is_signed(imtype))) {
      switch (rm) {
      case ROUND_USER: top = TOP_cvt_l_d; break;
      case ROUND_NEAREST: top = TOP_round_l_d; break;
      case ROUND_CHOP: top = TOP_trunc_l_d; break;
      case ROUND_NEG_INF: top = TOP_floor_l_d; break;
      case ROUND_PLUS_INF: top = TOP_ceil_l_d; break;
      default: FmtAssert(FALSE,("Unimplemented rounding mode"));
      }
    }
    else {
      switch (rm) {
      case ROUND_USER: top = TOP_cvt_w_d; break;
      case ROUND_NEAREST: top = TOP_round_w_d; break;
      case ROUND_CHOP: top = TOP_trunc_w_d; break;
      case ROUND_NEG_INF: top = TOP_floor_w_d; break;
      case ROUND_PLUS_INF: top = TOP_ceil_w_d; break;
      default: FmtAssert(FALSE,("Unimplemented rounding mode"));
      }
    }
  }
  else FmtAssert(FALSE,("unsupported float size in Expand_Float_To_Int"));
  TN *tmp = Build_TN_Of_Mtype (fmtype);
  Build_OP(top, tmp, src, ops);
  Build_OP(int_64bit ? TOP_dmfc1 : TOP_mfc1, dest, tmp, ops);
}

void
Expand_Float_To_Int_Cvt (TN *dest, TN *src, TYPE_ID imtype, TYPE_ID fmtype, OPS *ops)
{
        Expand_Float_To_Int (ROUND_USER, dest, src, imtype, fmtype, ops);
}

void
Expand_Float_To_Int_Round (TN *dest, TN *src, TYPE_ID imtype, TYPE_ID fmtype, OPS *ops)
{
        Expand_Float_To_Int (ROUND_NEAREST, dest, src, imtype, fmtype, ops);
}

void
Expand_Float_To_Int_Trunc (TN *dest, TN *src, TYPE_ID imtype, TYPE_ID fmtype, OPS *ops)
{
        Expand_Float_To_Int (ROUND_CHOP, dest, src, imtype, fmtype, ops);
}

void
Expand_Float_To_Int_Floor (TN *dest, TN *src, TYPE_ID imtype, TYPE_ID fmtype, OPS *ops)
{
        Expand_Float_To_Int (ROUND_NEG_INF, dest, src, imtype, fmtype, ops);
}

void
Expand_Float_To_Int_Ceil (TN *dest, TN *src, TYPE_ID imtype, TYPE_ID fmtype, OPS *ops)
{
        Expand_Float_To_Int (ROUND_PLUS_INF, dest, src, imtype, fmtype, ops);
}

void
Expand_Float_To_Float_Floor (TN *dest, TN *src, TYPE_ID rtype, TYPE_ID desc, OPS *ops)
{
  // For float rtype, we currently only generate F4F4FLOOR and F8F8FLOOR.
  FmtAssert (!MTYPE_is_quad (rtype) && !MTYPE_is_quad (desc),
             ("Expand_Float_To_Float_Floor: NYI"));

  TOP top = TOP_UNDEFINED;
  if (rtype == MTYPE_F8)
  {
    if (desc == MTYPE_F8)
      top = TOP_floor_l_d;
    else if (desc == MTYPE_F4)
      top = TOP_floor_l_s;
  }
  else if (rtype == MTYPE_F4)
  {
    if (desc == MTYPE_F8)
      top = TOP_floor_w_d;
    else if (desc == MTYPE_F4)
      top = TOP_floor_w_s;
  }
  Is_True (top != TOP_UNDEFINED, ("TOP has undefined value"));
  Build_OP (top, dest, src, ops);
}

void
Expand_Float_To_Float (TN *dest, TN *src, TYPE_ID mtype, OPS *ops)
{
  TOP top = (mtype == MTYPE_F8) ? TOP_cvt_d_s : TOP_cvt_s_d;
  Build_OP(top, dest, src, ops);
}


void
Expand_Int_To_Float (TN *dest, TN *src, TYPE_ID imtype, TYPE_ID fmtype, OPS *ops)
{
  // 15067: Handle MTYPE_U4 input by zero extending to MTYPE_U8
  if (imtype == MTYPE_U4) {
    DevWarn("Expand_Int_To_Float is zero extending MTYPE_U4 input");
    TN *src_orig = src;
    src = Build_TN_Of_Mtype(MTYPE_U8);
    Exp_COPY_Ext(TOP_lwu, src, src_orig, ops);
    imtype = MTYPE_U8;
  }

  TOP top;
  BOOL int_64bit = MTYPE_byte_size(imtype) == 8;
  TN *tmp = Build_TN_Of_Mtype(int_64bit ? MTYPE_F8 : MTYPE_F4);  // 15067
  Build_OP(int_64bit ? TOP_dmtc1 : TOP_mtc1, tmp, src, ops);
  // See example gcc.c-torture/execute/conversion.c
  if (fmtype == MTYPE_F4) {
    top = (int_64bit ? TOP_cvt_s_l: TOP_cvt_s_w);
  }
  else if (fmtype == MTYPE_F8) {
    top = (int_64bit ? TOP_cvt_d_l: TOP_cvt_d_w);
  }
  else FmtAssert(FALSE,("unsupported float size in Expand_Int_To_Float"));
  Build_OP(top, dest, tmp, ops);
}


static BOOL
Optimize_Select (
	TOP cmp,
  	TN *cond1, 
  	TN *cond2, 
  	TN *dest, 
  	TN *dest2,
  	TN *src1, 
  	TN *src2, 
	BOOL is_float,
	OPS *ops)
{
  return FALSE;
}


static void
Expand_Compare_And_Select (
	TOP cmp,
  	TN *cond1, 
  	TN *cond2, 
  	TN *dest, 
  	TN *opposite_dest, 
  	TN *true_tn, 
  	TN *false_tn, 
	BOOL is_float,
	OPS *ops)
{ FmtAssert(FALSE,("Unimplemented")); }

void
Expand_Select (
  TN *dest_tn, 
  TN *cond_tn, 
  TN *true_tn, 
  TN *false_tn, 
  TYPE_ID mtype, 
  BOOL float_cond,
  OPS *ops)
{
  const BOOL is_float = MTYPE_is_float(mtype);
  if (TN_register_class(cond_tn) == ISA_REGISTER_CLASS_fcc) {
    if (is_float) {
      if (true_tn != dest_tn) {
	Build_OP(MTYPE_is_size_double(mtype) ? TOP_movt_d : TOP_movt_s, 
		 dest_tn, true_tn, cond_tn, dest_tn, ops);
	Set_OP_cond_def_kind(OPS_last(ops), OP_ALWAYS_COND_DEF);
      }
      if (false_tn != dest_tn) {
	Build_OP(MTYPE_is_size_double(mtype) ? TOP_movf_d : TOP_movf_s, 
		 dest_tn, false_tn, cond_tn, dest_tn, ops);
	Set_OP_cond_def_kind(OPS_last(ops), OP_ALWAYS_COND_DEF);
      }
    } else {
      if (true_tn != dest_tn) {
	Build_OP(TOP_movt, dest_tn, true_tn, cond_tn, dest_tn, ops);
	Set_OP_cond_def_kind(OPS_last(ops), OP_ALWAYS_COND_DEF);
      }
      if (false_tn != dest_tn) {
	Build_OP(TOP_movf, dest_tn, false_tn, cond_tn, dest_tn, ops);
	Set_OP_cond_def_kind(OPS_last(ops), OP_ALWAYS_COND_DEF);
      }
    }
  } else if (TN_register_class(cond_tn) == ISA_REGISTER_CLASS_integer) {
    if (is_float) {
      if (true_tn != dest_tn) {
	Build_OP(MTYPE_is_size_double(mtype) ? TOP_movn_d : TOP_movn_s, 
		 dest_tn, true_tn, cond_tn, dest_tn, ops);
	Set_OP_cond_def_kind(OPS_last(ops), OP_ALWAYS_COND_DEF);
      }
      if (false_tn != dest_tn) {
	Build_OP(MTYPE_is_size_double(mtype) ? TOP_movz_d : TOP_movz_s, 
		 dest_tn, false_tn, cond_tn, dest_tn, ops);
	Set_OP_cond_def_kind(OPS_last(ops), OP_ALWAYS_COND_DEF);
      }      
    } else {
      if (true_tn != dest_tn) {
	Build_OP(TOP_movn, dest_tn, true_tn, cond_tn, dest_tn, ops);
	Set_OP_cond_def_kind(OPS_last(ops), OP_ALWAYS_COND_DEF);
      }
      if (false_tn != dest_tn) {
	Build_OP(TOP_movz, dest_tn, false_tn, cond_tn, dest_tn, ops);
	Set_OP_cond_def_kind(OPS_last(ops), OP_ALWAYS_COND_DEF);
      }
    }
  } else if (TN_register_class(cond_tn) == ISA_REGISTER_CLASS_float) {
    TN *tmp_tn = Gen_Typed_Register_TN(MTYPE_I4, 4);
    Build_OP(TOP_mfc1, tmp_tn, cond_tn, ops);
    if (is_float) {
      if (true_tn != dest_tn) {
	Build_OP(MTYPE_is_size_double(mtype) ? TOP_movn_d : TOP_movn_s, 
		 dest_tn, true_tn, tmp_tn, dest_tn, ops);
	Set_OP_cond_def_kind(OPS_last(ops), OP_ALWAYS_COND_DEF);
      }
      if (false_tn != dest_tn) {
	Build_OP(MTYPE_is_size_double(mtype) ? TOP_movz_d : TOP_movz_s, 
		 dest_tn, false_tn, tmp_tn, dest_tn, ops);
	Set_OP_cond_def_kind(OPS_last(ops), OP_ALWAYS_COND_DEF);      
      }
    } else {
      if (true_tn != dest_tn) {
	Build_OP(TOP_movn, dest_tn, true_tn, tmp_tn, dest_tn, ops);
	Set_OP_cond_def_kind(OPS_last(ops), OP_ALWAYS_COND_DEF);
      }
      if (false_tn != dest_tn) {
	Build_OP(TOP_movz, dest_tn, false_tn, tmp_tn, dest_tn, ops);
	Set_OP_cond_def_kind(OPS_last(ops), OP_ALWAYS_COND_DEF);
      }
    }
  } else {
    FmtAssert(FALSE, ("UNIMPLEMENTED"));
  }
}

void
Expand_Min (TN *dest, TN *src1, TN *src2, TYPE_ID mtype, OPS *ops)
{
  BOOL is_signed = MTYPE_is_signed(mtype);
  if (!MTYPE_is_float(mtype)) {
    TN *tmp_tn = Gen_Typed_Register_TN(MTYPE_I4, 4);
    if (src1 == dest) {
      Build_OP(is_signed?TOP_slt:TOP_sltu, tmp_tn, src1, src2, ops);
      Build_OP(TOP_movz, dest, src2, tmp_tn, dest, ops); 
    }
    else if (src2 == dest) {
      Build_OP(is_signed?TOP_slt:TOP_sltu, tmp_tn, src2, src1, ops);
      Build_OP(TOP_movz, dest, src1, tmp_tn, dest, ops); 
    }
    else {
      Build_OP(TOP_or, dest, src1, Zero_TN, ops);
      Build_OP(is_signed?TOP_slt:TOP_sltu, tmp_tn, src1, src2, ops);
      Build_OP(TOP_movz, dest, src2, tmp_tn, dest, ops); 
    }
    Set_OP_cond_def_kind(OPS_last(ops), OP_ALWAYS_COND_DEF);
  } else {
    TN *tmp_fcc = Gen_Register_TN(ISA_REGISTER_CLASS_fcc, 1);
    if (src1 == dest) {
      Build_OP(MTYPE_is_size_double(mtype)? TOP_c_lt_d: TOP_c_lt_s,
	       tmp_fcc, src2, src1, ops);
      Build_OP(MTYPE_is_size_double(mtype)? TOP_movt_d: TOP_movt_s,
	       dest, src2, tmp_fcc, dest, ops);
    }
    else if (src2 == dest) {
      Build_OP(MTYPE_is_size_double(mtype)? TOP_c_lt_d: TOP_c_lt_s,
	       tmp_fcc, src1, src2, ops);
      Build_OP(MTYPE_is_size_double(mtype)? TOP_movt_d: TOP_movt_s,
	       dest, src1, tmp_fcc, dest, ops);
    }
    else {
      Build_OP(MTYPE_is_size_double(mtype)? TOP_mov_d: TOP_mov_s,
	       dest, src2, ops);
      Build_OP(MTYPE_is_size_double(mtype)? TOP_c_lt_d: TOP_c_lt_s,
	       tmp_fcc, src1, src2, ops);
      Build_OP(MTYPE_is_size_double(mtype)? TOP_movt_d: TOP_movt_s,
	       dest, src1, tmp_fcc, dest, ops);
    }
    Set_OP_cond_def_kind(OPS_last(ops), OP_ALWAYS_COND_DEF);
  }
}

void
Expand_Max (TN *dest, TN *src1, TN *src2, TYPE_ID mtype, OPS *ops)
{     
  BOOL is_signed = MTYPE_is_signed(mtype);
  if (!MTYPE_is_float(mtype)) {
    TN *tmp_tn = Gen_Typed_Register_TN(MTYPE_I4, 4);
    if (src1 == dest) {
      Build_OP(is_signed?TOP_slt:TOP_sltu, tmp_tn, src2, src1, ops);
      Build_OP(TOP_movz, dest, src2, tmp_tn, dest, ops); 
    }
    else if (src2 == dest) {
      Build_OP(is_signed?TOP_slt:TOP_sltu, tmp_tn, src1, src2, ops);
      Build_OP(TOP_movz, dest, src1, tmp_tn, dest, ops); 
    }
    else {
      Build_OP(TOP_or, dest, src2, Zero_TN, ops);
      Build_OP(is_signed?TOP_slt:TOP_sltu, tmp_tn, src1, src2, ops);
      Build_OP(TOP_movz, dest, src1, tmp_tn, dest, ops); 
    }
    Set_OP_cond_def_kind(OPS_last(ops), OP_ALWAYS_COND_DEF);
  } else {
    TN *tmp_fcc = Gen_Register_TN(ISA_REGISTER_CLASS_fcc, 1);
    if (src1 == dest) {
      Build_OP(MTYPE_is_size_double(mtype)? TOP_c_lt_d: TOP_c_lt_s,
	       tmp_fcc, src1, src2, ops);
      Build_OP(MTYPE_is_size_double(mtype)? TOP_movt_d: TOP_movt_s,
	       dest, src2, tmp_fcc, dest, ops);
    }
    else if (src2 == dest) {
      Build_OP(MTYPE_is_size_double(mtype)? TOP_c_lt_d: TOP_c_lt_s,
	       tmp_fcc, src2, src1, ops);
      Build_OP(MTYPE_is_size_double(mtype)? TOP_movt_d: TOP_movt_s,
	       dest, src1, tmp_fcc, dest, ops);
    }
    else {
      Build_OP(MTYPE_is_size_double(mtype)? TOP_mov_d: TOP_mov_s,
	       dest, src1, ops);
      Build_OP(MTYPE_is_size_double(mtype)? TOP_c_lt_d: TOP_c_lt_s,
	       tmp_fcc, src1, src2, ops);
      Build_OP(MTYPE_is_size_double(mtype)? TOP_movt_d: TOP_movt_s,
	       dest, src2, tmp_fcc, dest, ops);
    }
    Set_OP_cond_def_kind(OPS_last(ops), OP_ALWAYS_COND_DEF);
  }
}

void
Expand_MinMax (TN *dest, TN *dest2, TN *src1, TN *src2, TYPE_ID mtype, OPS *ops)
{ 
  BOOL is_signed = MTYPE_is_signed(mtype);
  FmtAssert(dest != src1 && dest2 != src1 && dest != src2 && dest2 != src2,
	    ("Expand_MinMax: dest TN cannot be also src TN"));
  if (!MTYPE_is_float(mtype)) {
    TN *tmp_tn = Gen_Typed_Register_TN(MTYPE_I4, 4);
    Build_OP(TOP_or, dest, src1, Zero_TN, ops);
    Build_OP(TOP_or, dest2, src2, Zero_TN, ops);
    Build_OP(is_signed?TOP_slt:TOP_sltu, tmp_tn, src1, src2, ops);
    Build_OP(TOP_movz, dest, src2, tmp_tn, dest, ops); 
    Set_OP_cond_def_kind(OPS_last(ops), OP_ALWAYS_COND_DEF);
    Build_OP(TOP_movz, dest2, src1, tmp_tn, dest2, ops); 
    Set_OP_cond_def_kind(OPS_last(ops), OP_ALWAYS_COND_DEF);
  } else {
    TN *tmp_fcc = Gen_Register_TN(ISA_REGISTER_CLASS_fcc, 1);
    Build_OP(MTYPE_is_size_double(mtype)? TOP_mov_d: TOP_mov_s,
	     dest, src1, ops);
    Build_OP(MTYPE_is_size_double(mtype)? TOP_mov_d: TOP_mov_s,
	     dest2, src2, ops);
    Build_OP(MTYPE_is_size_double(mtype)? TOP_c_lt_d: TOP_c_lt_s,
	     tmp_fcc, src1, src2, ops);
    Build_OP(MTYPE_is_size_double(mtype)? TOP_movf_d: TOP_movf_s,
	     dest, src2, tmp_fcc, dest, ops);
    Set_OP_cond_def_kind(OPS_last(ops), OP_ALWAYS_COND_DEF);
    Build_OP(MTYPE_is_size_double(mtype)? TOP_movf_d: TOP_movf_s,
	     dest2, src1, tmp_fcc, dest2, ops);
    Set_OP_cond_def_kind(OPS_last(ops), OP_ALWAYS_COND_DEF);
  }
}

/* check whether to eval condition before select */
extern BOOL
Check_Select_Expansion (OPCODE compare)
{
  // in order to get optimal code,
  // don't evaluate the condition first,
  // but pass the condition and kids to exp_select,
  // which will do the compare and use the predicate results.
  return FALSE;
}

extern void 
Exp_Select_And_Condition (
        OPCODE select, TN *result, TN *true_tn, TN *false_tn,
        OPCODE compare, TN *cmp_kid1, TN *cmp_kid2, VARIANT variant, OPS *ops)
{
  TOP cmp1, cmp2;

  if (Trace_Exp) {
    fprintf(TFile, "expand %s: ", OPCODE_name(select));
    if (result) Print_TN(result,FALSE);
    fprintf(TFile, " :- (");
    if (cmp_kid1) Print_TN(cmp_kid1,FALSE);
    fprintf(TFile, " ");
    fprintf(TFile, OPCODE_name(compare));
    fprintf(TFile, " ");
    if (cmp_kid2) Print_TN(cmp_kid2,FALSE);
    fprintf(TFile, ") ? ");
    if (true_tn) Print_TN(true_tn,FALSE);
    fprintf(TFile, " : ");
    if (false_tn) Print_TN(false_tn,FALSE);
    fprintf(TFile, " ");
    if (variant) fprintf(TFile, "(0x%" SCNx64 ")", (INT64)variant);
    fprintf(TFile, "\n");
  }

  TYPE_ID desc = OPCODE_desc(compare);
  OPERATOR compare_opr = OPCODE_operator(compare);
  TN *p;
  OPS new_ops;
  OPS_Init(&new_ops);
  if (MTYPE_is_float(desc))
    p = Gen_Register_TN(ISA_REGISTER_CLASS_fcc, 1);
  else p = Gen_Register_TN(ISA_REGISTER_CLASS_integer, 4);

  if (compare_opr == OPR_NE) { // c_neq_[sd] not supported by assembler
    TN *tmp = true_tn;
    true_tn = false_tn;
    false_tn = tmp;
    compare_opr = OPR_EQ;
  }
  
  switch(compare_opr) {
  case OPR_LT:
    if (MTYPE_is_float(desc))
	Expand_Float_Less (p, cmp_kid1, cmp_kid2, variant, desc, &new_ops);
    else
	Expand_Int_Less (p, cmp_kid1, cmp_kid2, desc, &new_ops);
    break;
  case OPR_LE:
    if (MTYPE_is_float(desc))
	Expand_Float_Less_Equal (p, cmp_kid1, cmp_kid2, variant, desc, &new_ops);
    else {
      // 14840: Invoking GT/LT instead of LE/GE often saves one xori OP
      // Was: Expand_Int_Less_Equal (p, cmp_kid1, cmp_kid2, desc, &new_ops);
      TN *tmp = true_tn;
      true_tn = false_tn;
      false_tn = tmp;
      compare_opr = OPR_GT;
      Expand_Int_Greater (p, cmp_kid1, cmp_kid2, desc, &new_ops);
    }
    break;
  case OPR_EQ:
    if (MTYPE_is_float(desc))
	Expand_Float_Equal (p, cmp_kid1, cmp_kid2, variant, desc, &new_ops);
    else if (desc == MTYPE_B)
	Expand_Bool_Equal (p, cmp_kid1, cmp_kid2, &new_ops);
    else
	Expand_Int_Equal (p, cmp_kid1, cmp_kid2, desc, &new_ops);
    break;
#if 0 // this case avoided by above transformation
  case OPR_NE:
    if (MTYPE_is_float(desc))
	Expand_Float_Not_Equal (p, cmp_kid1, cmp_kid2, variant, desc, &new_ops);
    else if (desc == MTYPE_B)
	Expand_Bool_Not_Equal (p, cmp_kid1, cmp_kid2, &new_ops);
    else
	Expand_Int_Not_Equal (p, cmp_kid1, cmp_kid2, desc, &new_ops);
    break;
#endif
  case OPR_GE:
    if (MTYPE_is_float(desc))
	Expand_Float_Greater_Equal (p, cmp_kid1, cmp_kid2, variant, desc, &new_ops);
    else {
      // 14840: Invoking GT/LT instead of LE/GE often saves one xori OP
      // Was: Expand_Int_Greater_Equal (p, cmp_kid1, cmp_kid2, desc, &new_ops);
      TN *tmp = true_tn;
      true_tn = false_tn;
      false_tn = tmp;
      compare_opr = OPR_LT;
      Expand_Int_Less (p, cmp_kid1, cmp_kid2, desc, &new_ops);
    }
    break;
  case OPR_GT:
    if (MTYPE_is_float(desc))
	Expand_Float_Greater (p, cmp_kid1, cmp_kid2, variant, desc, &new_ops);
    else
	Expand_Int_Greater (p, cmp_kid1, cmp_kid2, desc, &new_ops);
    break;
  default:
    FmtAssert(FALSE, ("Unknown opcode"));
  }

  if (result != true_tn && result != false_tn) {
    if (MTYPE_is_float(OPCODE_rtype(select))) {
      Build_OP (MTYPE_is_size_double(OPCODE_rtype(select))?TOP_mov_d:TOP_mov_s, 
	        result, true_tn, &new_ops);
    } else {
      Build_OP (TOP_or, result, Zero_TN, true_tn, &new_ops);
    }
  }

  if (MTYPE_is_float(OPCODE_rtype(select))) {
    if (result != false_tn) {
      if (MTYPE_is_float(desc))
	Build_OP (MTYPE_is_size_double(OPCODE_rtype(select))?TOP_movf_d:TOP_movf_s, 
		  result, false_tn, p, result, &new_ops);
      else Build_OP (MTYPE_is_size_double(OPCODE_rtype(select))?TOP_movz_d:TOP_movz_s,
		     result, false_tn, p, result, &new_ops);
    } else {
      if (MTYPE_is_float(desc))
	Build_OP (MTYPE_is_size_double(OPCODE_rtype(select))?TOP_movt_d:TOP_movt_s, 
		  result, true_tn, p, result, &new_ops);
      else Build_OP (MTYPE_is_size_double(OPCODE_rtype(select))?TOP_movn_d:TOP_movn_s,
		     result, true_tn, p, result, &new_ops);
    }
  } else {
    if (result != false_tn) {
      if (MTYPE_is_float(desc))
	Build_OP (TOP_movf, result, false_tn, p, result, &new_ops);
      else Build_OP (TOP_movz, result, false_tn, p, result, &new_ops);
    } else {
      if (MTYPE_is_float(desc))
	Build_OP (TOP_movt, result, true_tn, p, result, &new_ops);
      else Build_OP (TOP_movn, result, true_tn, p, result, &new_ops);
    }
  }
  Set_OP_cond_def_kind(OPS_last(&new_ops), OP_ALWAYS_COND_DEF); 
  if (Trace_Exp) {
    OP *op;
    FOR_ALL_OPS_OPs (&new_ops, op) {
	    fprintf(TFile, " into "); Print_OP (op);
    }
  }
  OPS_Append_Ops(ops, &new_ops);
}


#define RESET_COND_DEF_LAST(ops) Set_OP_cond_def_kind(OPS_last(ops),OP_ALWAYS_UNC_DEF)

static void
Expand_SGI_Sqrt (TN *result, TN *src, TYPE_ID mtype, OPS *ops)
{
  /*	(p0) frsqrta.s0 f6,p2=src	# y2 = ~1/sqrt(x)
   *
   *	(p2) ldfd	f4=half		# f4 = 0.5 (0x3fe0000000000000)
   *	(p2) ldfd	f7=ah		# f7 = 0x3fe0000000000001
   *
   *	(p2) fmpy.d.s1	f3=src,f6	# g = x*y2
   *	(p2) fmpy.d.s1	f2=f4,f6	# y = 0.5*y2
   *
   *	(p2) fnma.d.s1	f5=f3,f3,src	# d = x - g*g
   *
   *	(p2) fma.d.s1	f3=f2,f5,f3	# g = g + y*d # 16 bit approximation
   *
   *	(p2) fnma.d.s1	f8=f2,f3,f7	# e = ah - y*g
   *	(p2) fnma.d.s1	f5=f3,f3,src    # d = x - g*g
   *	(p2) fma.d.s1	f2=f8,f6,f2	# y = y + e*y2
   *
   *	(p2) fma.d.s1   f3=f2,f5,f3     # g = g + y*d # 32 bit approximation
   *	(p2) fadd.d.s1  f6=f3,f3        # y2 = y + y
   *
   *	(p2) fnma.d.s1	f8=f2,f3,f7	# e = ah - y*g
   *	(p2) fnma.d.s1	f5=f3,f3,src    # d = x - g*g
   *	(p2) fma.d.s1	f2=f8,f6,f2	# y = y + e*y2
   *
   *	(p2) fma.d.s1   f3=f2,f5,f3     # g = g + y*d # 64 bit approximation before rounding
   *	(p2) fadd.d.s1  f6=f3,f3        # y2 = y + y
   *
   *	(p2) fnma.d.s1	f8=f2,f3,f7	# e = ah - y*g
   *	(p2) fnma.d.s1	f5=f3,f3,src    # d = x - g*g
   *	(p2) fma.d.s1	f2=f8,f6,f2	# y = y + e*y2
   *
   *	(p2) fma.d.s0   f6=f2,f5,f3	# result = g + y*d
   */
  // 3-mar-00/ken: this doesn't work for MTYPE_F10!!!!
}

static void
Expand_Intel_F10_Sqrt(TN *result, TN *src, OPS *ops)
{ FmtAssert(FALSE,("Unimplemented")); }


static void
Expand_Intel_Max_Thr_F8_Sqrt(TN *result, TN *src, OPS *ops)
{ FmtAssert(FALSE,("Unimplemented")); }


static void
Expand_Intel_Max_Thr_F4_Sqrt(TN *result, TN *src, OPS *ops)
{ FmtAssert(FALSE,("Unimplemented")); }


static void
Expand_Intel_Min_Lat_F8_Sqrt(TN *result, TN *src, OPS *ops)
{ FmtAssert(FALSE,("Unimplemented")); }


static void
Expand_Intel_Min_Lat_F4_Sqrt(TN *result, TN *src, OPS *ops)
{ FmtAssert(FALSE,("Unimplemented")); }


static void
Expand_Intel_Max_Thr_Sqrt (TN *result, TN *src, TYPE_ID mtype, OPS *ops)
{
  switch (mtype) {
  case MTYPE_F4:
    Expand_Intel_Max_Thr_F4_Sqrt(result, src, ops);
    break;
  case MTYPE_F8:
    Expand_Intel_Max_Thr_F8_Sqrt(result, src, ops);
    break;
  case MTYPE_F10:
    Expand_Intel_F10_Sqrt(result, src, ops);
    break;
  default:
    FmtAssert(FALSE, ("Bad type in Expand_Intel_Max_Thr_Sqrt"));
    /*NOTREACHED*/
  }
}


static void
Expand_Intel_Min_Lat_Sqrt (TN *result, TN *src, TYPE_ID mtype, OPS *ops)
{
  switch (mtype) {
  case MTYPE_F4:
    Expand_Intel_Min_Lat_F4_Sqrt(result, src, ops);
    break;
  case MTYPE_F8:
    Expand_Intel_Min_Lat_F8_Sqrt(result, src, ops);
    break;
  case MTYPE_F10:
    Expand_Intel_F10_Sqrt(result, src, ops);
    break;
  default:
    FmtAssert(FALSE, ("Bad type in Expand_Intel_Min_Lat_Sqrt"));
    /*NOTREACHED*/
  }
}


void
Expand_Sqrt (TN *result, TN *src, TYPE_ID mtype, OPS *ops)
{
  FmtAssert(TN_register_class(result) == ISA_REGISTER_CLASS_float && 
	    TN_register_class(src) == ISA_REGISTER_CLASS_float, 
	    ("Unimplemented sqrt for integer src/dest"));  
  Build_OP(MTYPE_is_size_double(mtype)?TOP_sqrt_d:TOP_sqrt_s, 
	   result, src, ops);
}


static void
Expand_Float_Compares(TOP cmp_opcode, TN *dest, TN *src1, TN *src2, TYPE_ID mtype, OPS *ops)
{
  if (TN_register_class(dest) == ISA_REGISTER_CLASS_fcc) {
    FmtAssert(cmp_opcode != TOP_c_neq_d && cmp_opcode != TOP_c_neq_s,
	      ("Expand_Float_Compares: should not use c_neq_[sd] which is not supported by assembler"));  
    Build_OP(cmp_opcode, dest, src1, src2, ops);   
  } else {
    FmtAssert(TN_register_class(dest) == ISA_REGISTER_CLASS_integer,
	      ("Expand_Float_Compares: result must be fcc or integer"));  
    TN *tmp_fcc = Gen_Register_TN(ISA_REGISTER_CLASS_fcc, 1);
    Build_OP(MTYPE_is_size_double(mtype)?TOP_daddiu:TOP_addiu, 
	     dest, Zero_TN, Gen_Literal_TN(1, 4), ops);
    if (cmp_opcode == TOP_c_neq_d || cmp_opcode == TOP_c_neq_s) {
      Build_OP(cmp_opcode == TOP_c_neq_d ? TOP_c_eq_d : TOP_c_eq_s, 
	       tmp_fcc, src1, src2, ops);     
      Build_OP(TOP_movt, dest, Zero_TN, tmp_fcc, dest, ops);
    }
    else {
      Build_OP(cmp_opcode, tmp_fcc, src1, src2, ops);     
      Build_OP(TOP_movf, dest, Zero_TN, tmp_fcc, dest, ops);
    }
    Set_OP_cond_def_kind(OPS_last(ops), OP_ALWAYS_COND_DEF);
  }
}

void
Expand_Float_Less (TN *dest, TN *src1, TN *src2, VARIANT variant, TYPE_ID mtype, OPS *ops)
{
  Expand_Float_Compares(MTYPE_is_size_double(mtype)?TOP_c_lt_d:TOP_c_lt_s, 
			dest, src1, src2, mtype, ops);
}

void
Expand_Float_Greater (TN *dest, TN *src1, TN *src2, VARIANT variant, TYPE_ID mtype, OPS *ops)
{
  Expand_Float_Compares(MTYPE_is_size_double(mtype)?TOP_c_lt_d:TOP_c_lt_s, 
			dest, src2, src1, mtype, ops);
}

void
Expand_Float_Less_Equal (TN *dest, TN *src1, TN *src2, VARIANT variant, TYPE_ID mtype, OPS *ops)
{
  Expand_Float_Compares(MTYPE_is_size_double(mtype)?TOP_c_le_d:TOP_c_le_s, 
			dest, src1, src2, mtype, ops);
}

void
Expand_Float_Greater_Equal (TN *dest, TN *src1, TN *src2, VARIANT variant, TYPE_ID mtype, OPS *ops)
{
  Expand_Float_Compares(MTYPE_is_size_double(mtype)?TOP_c_le_d:TOP_c_le_s, 
			dest, src2, src1, mtype, ops);
}

void
Expand_Float_Equal (TN *dest, TN *src1, TN *src2, VARIANT variant, TYPE_ID mtype, OPS *ops)
{
  Expand_Float_Compares(MTYPE_is_size_double(mtype)?TOP_c_eq_d:TOP_c_eq_s, 
			dest, src1, src2, mtype, ops);
}

void
Expand_Float_Not_Equal (TN *dest, TN *src1, TN *src2, VARIANT variant, TYPE_ID mtype, OPS *ops)
{
  Expand_Float_Compares(MTYPE_is_size_double(mtype)?TOP_c_neq_d:TOP_c_neq_s, 
			dest, src1, src2, mtype, ops);
}

void
Expand_Recip_Sqrt (TN *result, TN *src, TYPE_ID mtype, OPS *ops)
{
  /*	(p0) frsqrta.s0 f2,p2=src	# y = ~1/sqrt(x)
   *
   *	(p2) ldfd	f4=half		# f4 = 0.5
   *	(p2) fmpy.d.s1	f5=f4,src	# hx = 0.5*x
   *
   *	(p2) fmpy.d.s1	f3=f2,f2	# y2 = y*y
   *	(p2) fnma.d.s1	f6=f5,f3,f4	# z = 0.5 - 0.5*x*y*y
   *	(p2) fma.d.s1   f2=f2,f6,f2	# y = y + y*z
   *
   *	(p2) fmpy.d.s1	f3=f2,f2	# y2 = y*y
   *	(p2) fnma.d.s1	f6=f5,f3,f4	# z = 0.5 - 0.5*x*y*y
   *	(p2) fma.d.s1   f2=f2,f6,f2	# y = y + y*z
   *
   *	(p2) fmpy.d.s1	f3=f2,f2	# y2 = y*y
   *	(p2) fnma.d.s1	f6=f5,f3,f4	# z = 0.5 - 0.5*x*y*y
   *	(p2) fma.d.s0   f2=f2,f6,f2	# result = y + y*z
   */
}


// 14790: Don't use TOP_recip_s, which gives non-accurate result,
// unless -OPT:recip=ON is explicitly invoked.
static void Expand_Recip( TN* result, TN* src, TYPE_ID mtype, OPS* ops )
{
  if ( Recip_Allowed ) {
    TOP opc = MTYPE_is_size_double(mtype) ? TOP_recip_d : TOP_recip_s;
    Build_OP(opc, result, src, ops);
  } else {
    TCON tcon = Host_To_Targ_Float( mtype, 1.0 );
    ST *sym = New_Const_Sym( Enter_tcon(tcon), Be_Type_Tbl( TCON_ty(tcon) ) );
    Allocate_Object( sym );
#if 0
    // This code was copied from the x8664/expand.cxx, but it doesn't work!
    ST* base_sym = NULL;
    INT64 base_ofst = 0;
    Base_Symbol_And_Offset_For_Addressing( sym, 0, &base_sym, &base_ofst );
    TN *tn_1 = Gen_Symbol_TN( base_sym, base_ofst, TN_RELOC_NONE );
#else
    TN *tn_1 = Gen_Symbol_TN( sym, 0, 0 );
#endif
    TN *src_1 = Build_TN_Like( src );
    Expand_Const( src_1, tn_1, mtype, ops );
    TOP opc = ( MTYPE_is_size_double(mtype) ? TOP_div_d : TOP_div_s );
    Build_OP( opc, result, src_1, src, ops );
  }
}


void
Expand_Flop (OPCODE opcode, TN *result, TN *src1, TN *src2, TN *src3, OPS *ops)
{
  TOP opc;

  switch (opcode) {
  case OPC_F4ADD:     opc = TOP_add_s;    break;
  case OPC_F8ADD:     opc = TOP_add_d;    break;
  case OPC_V8F4ADD:   opc = TOP_add_ps;   break;
  case OPC_F4SUB:     opc = TOP_sub_s;    break;
  case OPC_F8SUB:     opc = TOP_sub_d;    break;
  case OPC_V8F4SUB:   opc = TOP_sub_ps;   break;
  case OPC_F4MPY:     opc = TOP_mul_s;    break;
  case OPC_F8MPY:     opc = TOP_mul_d;    break;
  case OPC_V8F4MPY:   opc = TOP_mul_ps;   break;
  case OPC_F4MADD:    opc = TOP_madd_s;   break;  // (src2 * src3) + src1
  case OPC_F8MADD:    opc = TOP_madd_d;   break;
  case OPC_V8F4MADD:  opc = TOP_madd_ps;  break;
  case OPC_F4MSUB:    opc = TOP_msub_s;   break;  // (src2 * src3) - src1
  case OPC_F8MSUB:    opc = TOP_msub_d;   break;
  case OPC_V8F4MSUB:  opc = TOP_msub_ps;  break;
  case OPC_F4NMADD:   opc = TOP_nmadd_s;  break;  // -((src2 * src3) + src1)
  case OPC_F8NMADD:   opc = TOP_nmadd_d;  break;
  case OPC_V8F4NMADD: opc = TOP_nmadd_ps; break;
  case OPC_F4NMSUB:   opc = TOP_nmsub_s;  break;  // -((src2 * src3) - src1)
  case OPC_F8NMSUB:   opc = TOP_nmsub_d;  break;
  case OPC_V8F4NMSUB: opc = TOP_nmsub_ps; break;
  case OPC_F4DIV:     opc = TOP_div_s;    break;
  case OPC_F8DIV:     opc = TOP_div_d;    break;
  // case OPC_F4RECIP:   opc = TOP_recip_s;  break;
  // case OPC_F8RECIP:   opc = TOP_recip_d;  break;
  case OPC_F4RECIP:
  case OPC_F8RECIP:
    Expand_Recip( result, src1, OPCODE_rtype(opcode), ops );
    return;
  case OPC_F4RSQRT:   opc = TOP_rsqrt_s;  break;
  case OPC_F8RSQRT:   opc = TOP_rsqrt_d;  break;
  default:
    #pragma mips_frequency_hint NEVER
    FmtAssert(FALSE, ("unexpected opcode %s", OPCODE_name(opcode)));
    /*NOTREACHED*/
  }
  if (TOP_is_madd(opc)) {
    Build_OP(opc, result, src1, src2, src3, ops);
  } else {
    Build_OP(opc, result, src1, src2, ops);
  }
}


void
Expand_Replicate (OPCODE opcode, TN *result, TN *op1, OPS *ops)
{
  switch (opcode) {
  case OPC_V8F4F4REPLICA:
    Build_OP(TOP_cvt_ps_s, result, op1, op1, ops);
    break;
  default:
    FmtAssert(FALSE, ("Expand_Replicate: Unsupported opcode (%s)",
		      OPCODE_name(opcode)));
    break;
  }
  return;
}


void
Expand_Reduce_Add (OPCODE opcode, TN *result, TN *op1, OPS *ops)
{
  switch (opcode) {
  case OPC_F4V8F4REDUCE_ADD:
  {
    TN* tmp1 = Build_TN_Like(result);
    TN* tmp2 = Build_TN_Like(result);
    Build_OP(TOP_cvt_s_pu, tmp1, op1, ops);
    Build_OP(TOP_cvt_s_pl, tmp2, op1, ops);
    Build_OP(TOP_add_s, result, tmp1, tmp2, ops);
    break;
  }
  default:
    FmtAssert(FALSE, ("Expand_Reduce_Add: Unsupported opcode (%s)",
		      OPCODE_name(opcode)));
  }
  return;
}


void
Expand_Reduce_Mpy (OPCODE opcode, TN *result, TN *op1, OPS *ops)
{
  switch (opcode) {
  case OPC_F4V8F4REDUCE_MPY:
  {
    TN* tmp1 = Build_TN_Like(result);
    TN* tmp2 = Build_TN_Like(result);
    Build_OP(TOP_cvt_s_pu, tmp1, op1, ops);
    Build_OP(TOP_cvt_s_pl, tmp2, op1, ops);
    Build_OP(TOP_mul_s, result, tmp1, tmp2, ops);
    break;
  }
  default:
    FmtAssert(FALSE, ("Expand_Reduce_Mpy: Unsupported opcode (%s)",
		      OPCODE_name(opcode)));
  }
  return;
}


void
Expand_Reduce_Max (OPCODE opcode, TN *result, TN *op1, OPS *ops)
{
  switch (opcode) {
  case OPC_F4V8F4REDUCE_MAX:
  {
    TN* tmp1 = Build_TN_Like(result);
    TN* tmp2 = Build_TN_Like(result);
    Build_OP(TOP_cvt_s_pu, tmp1, op1, ops);
    Build_OP(TOP_cvt_s_pl, tmp2, op1, ops);
    Expand_Max (result, tmp1, tmp2, MTYPE_F4, ops);
    break;
  }
  default:
    FmtAssert(FALSE, ("Expand_Reduce_Max: Unsupported opcode (%s)",
		      OPCODE_name(opcode)));
  }
  return;
}


void
Expand_Reduce_Min (OPCODE opcode, TN *result, TN *op1, OPS *ops)
{
  switch (opcode) {
  case OPC_F4V8F4REDUCE_MIN:
  {
    TN* tmp1 = Build_TN_Like(result);
    TN* tmp2 = Build_TN_Like(result);
    Build_OP(TOP_cvt_s_pu, tmp1, op1, ops);
    Build_OP(TOP_cvt_s_pl, tmp2, op1, ops);
    Expand_Min (result, tmp1, tmp2, MTYPE_F4, ops);
    break;
  }
  default:
    FmtAssert(FALSE, ("Expand_Reduce_Min: Unsupported opcode (%s)",
		      OPCODE_name(opcode)));
  }
  return;
}


void
Expand_Shuffle (OPCODE opcode, TN* result, TN* op1, VARIANT variant, OPS *ops)
{
  FmtAssert(variant == V_SHUFFLE_REVERSE, ("Expand_Shuffle: variant NYI"));
  switch (opcode) {
  case OPC_V8F4V8F4SHUFFLE:
    Build_OP(TOP_pul_ps, result, op1, op1, ops);
    break;
  default:
    FmtAssert(FALSE, ("Expand_Shuffle: Unsupported opcode (%s)",
		      OPCODE_name(opcode)));
  }
  return;
}


extern void
Init_CG_Expand (void)
{
  static BOOL Initialized = FALSE;

  // per PU:
  Trace_Exp = Get_Trace (TP_CGEXP, 1);
  /* whirl2ops uses -ttexp:2 */
  Trace_Exp2 = Get_Trace (TP_CGEXP, 4);
  
  if (Initialized) return;
  Initialized = TRUE;
  // once per file:
  Initialize_Branch_Variants();
}


/* ======================================================================
 * Exp_COPY_Ext
 *
 * Generate a register transfer copy from 'src_tn' to 'tgt_tn' with
 * sign/zero extension appropriate to model the given opcode.
 * ======================================================================*/
void
Exp_COPY_Ext (TOP opcode, TN *tgt_tn, TN *src_tn, OPS *ops)
{
  switch (opcode) {
  case TOP_lb:
    Build_OP(TOP_dsll32, tgt_tn, src_tn, Gen_Literal_TN(24, 4), ops);
    Build_OP(TOP_dsra32, tgt_tn, tgt_tn, Gen_Literal_TN(24, 4), ops);
    break;
  case TOP_lbu:
    Build_OP(TOP_andi, tgt_tn, src_tn, Gen_Literal_TN(0xff, 4), ops);
    break;
  case TOP_lh:
    Build_OP(TOP_dsll32, tgt_tn, src_tn, Gen_Literal_TN(16, 4), ops);
    Build_OP(TOP_dsra32, tgt_tn, tgt_tn, Gen_Literal_TN(16, 4), ops);
    break;
  case TOP_lhu:
    Build_OP(TOP_andi, tgt_tn, src_tn, Gen_Literal_TN(0xffff, 4), ops);
    break;
  case TOP_lw:
    if (Sext_Allowed) {
      // Bug 13497: sll tgt_tn, src_tn, 0x0 safely sign extends
      // Build_OP(TOP_sll, tgt_tn, src_tn, Gen_Literal_TN(0, 4), ops);
      Build_OP(TOP_sext, tgt_tn, src_tn, ops);
    } else {
      Build_OP(TOP_dsll32, tgt_tn, src_tn, Gen_Literal_TN(0, 4), ops);
      Build_OP(TOP_dsra32, tgt_tn, tgt_tn, Gen_Literal_TN(0, 4), ops);
    }
    break;
  case TOP_lwu:
    Build_OP(TOP_dsll32, tgt_tn, src_tn, Gen_Literal_TN(0, 4), ops);
    Build_OP(TOP_dsrl32, tgt_tn, tgt_tn, Gen_Literal_TN(0, 4), ops);
    break;
  case TOP_ld:
    Build_OP(TOP_or, tgt_tn, src_tn, Zero_TN, ops);
    break;
  default:
    FmtAssert(FALSE, ("Exp_COPY_Ext: Unsupported opcode (%s)",
		      TOP_Name(opcode)));
  }
}


/* ======================================================================
 * Exp_COPY
 * 
 * Generate a register transfer copy from 'src_tn' to 'tgt_tn'. 
 * ======================================================================*/
void 
Exp_COPY (TN *tgt_tn, TN *src_tn, OPS *ops)
{
  // Warning: Don't return NOP even if src_tn == tgt_tn.  EBO expects a real
  // move OP in order to track the usage info of src_tn.

  if( TN_is_constant(src_tn) )
  {
    FmtAssert (TN_has_value(src_tn), ("Exp_COPY: illegal source tn"));
    /* expansion for INTCONST doesn't depend on size */
    Exp_OP1 (OPC_I4INTCONST, tgt_tn, src_tn, ops);
  }
  else
  {
    ISA_REGISTER_CLASS tgt_rc = TN_register_class(tgt_tn);
    ISA_REGISTER_CLASS src_rc = TN_register_class(src_tn);

    if (tgt_rc == src_rc && tgt_rc == ISA_REGISTER_CLASS_integer) {
      Build_OP( TOP_or, tgt_tn, src_tn, Zero_TN, ops );
      Set_OP_copy (OPS_last(ops));
    }
    else if (tgt_rc == src_rc && tgt_rc == ISA_REGISTER_CLASS_float) {
      /* dedicated TNs always have size 8, so need to check both TNs */
      BOOL is_double = (TN_size(tgt_tn) == 8 && TN_size(src_tn) == 8);
      TOP top = is_double ? TOP_mov_d : TOP_mov_s;
      // Check for paired single
      BOOL is_vector = TN_is_vector(tgt_tn) || TN_is_vector(src_tn);
      if (is_vector) top = TOP_mov_ps;
      FmtAssert(! is_vector ||
		((TN_is_vector(tgt_tn) || TN_is_dedicated(tgt_tn)) &&
		 (TN_is_vector(src_tn) || TN_is_dedicated(src_tn))),
		("Exp_COPY: is_vector mismatch"));  // LATER: Is_True
      Build_OP(top, tgt_tn, src_tn, ops);
      Set_OP_copy (OPS_last(ops));
    }
    else if (src_rc == ISA_REGISTER_CLASS_integer &&
	     tgt_rc == ISA_REGISTER_CLASS_float) {
      /* dedicated TNs always have size 8, so need to check both TNs */
      BOOL is_double = (TN_size(tgt_tn) == 8 && TN_size(src_tn) == 8);
      Build_OP(is_double ? TOP_dmtc1 : TOP_mtc1, tgt_tn, src_tn, ops);
    }
    else if (src_rc == ISA_REGISTER_CLASS_float &&
	     tgt_rc == ISA_REGISTER_CLASS_integer) {
      /* dedicated TNs always have size 8, so need to check both TNs */
      BOOL is_double = (TN_size(tgt_tn) == 8 && TN_size(src_tn) == 8);
      Build_OP(is_double ? TOP_dmfc1 : TOP_mfc1, tgt_tn, src_tn, ops);
    }
    else if (src_rc == ISA_REGISTER_CLASS_hilo &&
	     src_tn == tgt_tn) {
      /* MIPS has one hilo register and there is no move instruction to copy
	 from hilo to hilo.  However, to perform CSE, EBO needs to insert a
	 copy to properly model the liveness of the CSE result.  To fix this,
	 generate a simulated hilo-hilo move which will be deleted at the end
	 of EBO. */
      Build_OP(TOP_mov_hilo, tgt_tn, src_tn, ops);
      Set_OP_copy(OPS_last(ops));
    }
    else if (   src_rc == ISA_REGISTER_CLASS_fcc 
             && tgt_rc == ISA_REGISTER_CLASS_fcc) 
    {
      Build_OP (TOP_mov_fcc, tgt_tn, src_tn, ops);
      Set_TN_is_gra_cannot_split (tgt_tn);
      Set_TN_is_gra_cannot_split (src_tn);
      Set_OP_copy (OPS_last (ops));
    }
    else {
      FmtAssert(FALSE, ("Exp_COPY: unimplemented copy"));
    }
  }
}

static ST *tmp_apply_arg = NULL;
void
Generate_Temp_Apply_Arg ( )
{
  TY_IDX tyi;
  TY& ty = New_TY(tyi);
  TY_Init(ty, 144, KIND_STRUCT, MTYPE_M,
          Save_Str("__apply_arg"));
  Set_TY_align(tyi, 8);
  tmp_apply_arg = New_ST(CURRENT_SYMTAB);
  ST_Init(tmp_apply_arg, TY_name_idx(ty),
          CLASS_VAR, SCLASS_AUTO, EXPORT_LOCAL, tyi);
  Set_ST_is_temp_var(tmp_apply_arg);
  Allocate_Object(tmp_apply_arg);
}


static void
Expand_Count_Leading_Zeros (TN *result, TN *op, TYPE_ID mtype, OPS *ops)
{
  if ( mtype != MTYPE_I1 && mtype != MTYPE_U1 &&
       mtype != MTYPE_I2 && mtype != MTYPE_U2 &&
       mtype != MTYPE_I4 && mtype != MTYPE_U4 &&
       mtype != MTYPE_I8 && mtype != MTYPE_U8 )
    Fail_FmtAssertion("Expand_Count_Leading_Zeros: unexpected mtype");
  BOOL is_small = ( MTYPE_bit_size(mtype) < 32 );
  BOOL is_double = ( MTYPE_bit_size(mtype) == 64 );
  TN *tmp1 = is_small ? Build_TN_Like( result ) : result;
  Build_OP( is_double ? TOP_dclz : TOP_clz, tmp1, op, ops );
  if ( is_small ) {
    // Account for zero/sign extension
    Expand_Sub( result, tmp1,
		Gen_Literal_TN(32 - MTYPE_bit_size(mtype), 4),
		MTYPE_I4, ops );
    // Followed by a conditional move to cover op == -1.
    Build_OP( TOP_movz, result, Zero_TN, tmp1, result, ops );
    Set_OP_cond_def_kind( OPS_last(ops), OP_ALWAYS_COND_DEF );
  }
}


static void
Expand_Count_Trailing_Zeros (TN *result, TN *op, TYPE_ID mtype, OPS *ops)
{
  if ( mtype != MTYPE_I4 && mtype != MTYPE_U4 &&
       mtype != MTYPE_I8 && mtype != MTYPE_U8 )
    Fail_FmtAssertion("Expand_Count_Trailing_Zeros: unexpected mtype");
  BOOL is_8 = ( mtype == MTYPE_I8 || mtype == MTYPE_U8 );
  // Implement ctz in terms of clo:
  //   ctz(x) = 64 - clz( ~x & (x - 1) )  [6 ops]
  //   ctz(x) = 64 - clo( x | -x )        [5 ops]
  // Source: p. 84 and p. 11 of Hacker's Delight by Henry Warren
  TN *tmp1 = Build_TN_Of_Mtype( mtype );
  TN *tmp2 = Build_TN_Of_Mtype( mtype );
  TN *tmp3 = Build_TN_Like( result );
  Expand_Neg( tmp1, op, mtype, ops );
  Build_OP( TOP_or, tmp2, op, tmp1, ops );
  Build_OP( is_8 ? TOP_dclo : TOP_clo, tmp3, tmp2, ops );
  TN *tmp4 = Gen_Literal_TN( is_8 ? 64 : 32, 4 );
  Expand_Sub( result, tmp4, tmp3, MTYPE_I4, ops );
  return;
}


static void
Expand_Least_Significant_Bit (TN *result, TN *op, TYPE_ID mtype, OPS *ops)
{
  if ( mtype != MTYPE_I4 && mtype != MTYPE_U4 &&
       mtype != MTYPE_I8 && mtype != MTYPE_U8 )
    Fail_FmtAssertion("Expand_Least Significant_Bit: unexpected mtype");
  BOOL is_double = ( mtype == MTYPE_I8 || mtype == MTYPE_U8 );
  // Implement ffs (for x nonzero) in terms of clo:
  //   ffs(x) = 65 - clo( x | -x )        [5 ops]
  // Source: p. 84 and p. 11 of Hacker's Delight by Henry Warren
  TN *tmp1 = Build_TN_Of_Mtype( mtype );
  TN *tmp2 = Build_TN_Of_Mtype( mtype );
  TN *tmp3 = Build_TN_Like( result );
  Expand_Neg( tmp1, op, mtype, ops );
  Build_OP( TOP_or, tmp2, op, tmp1, ops );
  Build_OP( is_double ? TOP_dclo : TOP_clo, tmp3, tmp2, ops );
  TN *tmp4 = Gen_Literal_TN( is_double ? 65 : 33, 4 );
  Expand_Sub( result, tmp4, tmp3, MTYPE_I4, ops );
  // Followed by a conditional move to cover ffs(0) == 0.
  Build_OP( TOP_movz, result, Zero_TN, op, result, ops );
  Set_OP_cond_def_kind( OPS_last(ops), OP_ALWAYS_COND_DEF );
  return;
}


void
Exp_Intrinsic_Op (INTRINSIC id, TN *result, TN *op0, TN *op1, TYPE_ID mtype, OPS *ops)
{
  const BOOL is_double = MTYPE_is_size_double(mtype);
  const BOOL is_quad = MTYPE_is_quad(mtype);
  FmtAssert(! is_quad, ("Exp_Intrinsic_Op: quad unsupported"));
  TN *tmp_fcc = NULL;
  if (id >= INTRN_ISGREATER && id <= INTRN_ISUNORDERED) {
    tmp_fcc = Gen_Register_TN(ISA_REGISTER_CLASS_fcc, 1);
    Build_OP( TN_size(result) == 4 ? TOP_addiu : TOP_daddiu,
	      result, Zero_TN, Gen_Literal_TN(1, 4), ops );
  }

  switch (id) {
    // Note: Frontend generates INTRN_CLZ/INTRN_CTZ64 for library calls,
    // and INTRN_CLZ32/INTRN_CTZ for clz/dclz/ctz/dctz instruction.
    // (Reasons related to x86.)  mtype is the rtype from op0.
  case INTRN_I1LEADZ:
  case INTRN_I2LEADZ:
  case INTRN_I4LEADZ:
  case INTRN_I8LEADZ:
  case INTRN_CLZ32:
    if ( id == INTRN_I1LEADZ ) mtype = MTYPE_I1;
    if ( id == INTRN_I2LEADZ ) mtype = MTYPE_I2;
    Expand_Count_Leading_Zeros( result, op0, mtype, ops );
    break;
  case INTRN_CTZ:
    Expand_Count_Trailing_Zeros( result, op0, mtype, ops );
    break;
  case INTRN_I4FFS:
    Expand_Least_Significant_Bit( result, op0, mtype, ops );
    break;
  case INTRN_ISGREATER:
    // 14757: Gnu4 compatable TOPs used below.  For gnu3, substitite
    //        ult --> olt, ule --> ole
    Build_OP( is_double ? TOP_c_ult_d : TOP_c_ult_s, tmp_fcc, op1, op0, ops );
    break;
  case INTRN_ISGREATEREQUAL:
    Build_OP( is_double ? TOP_c_ule_d : TOP_c_ule_s, tmp_fcc, op1, op0, ops );
    break;
  case INTRN_ISLESS:
    Build_OP( is_double ? TOP_c_ult_d : TOP_c_ult_s, tmp_fcc, op0, op1, ops );
    break;
  case INTRN_ISLESSEQUAL:
    Build_OP( is_double ? TOP_c_ule_d : TOP_c_ule_s, tmp_fcc, op0, op1, ops );
    break;
  case INTRN_ISLESSGREATER:
    // Bug 12441: GCC MIPS assembler doesn't support c.ogl.d, so use negation
    Build_OP( is_double ? TOP_c_ueq_d : TOP_c_ueq_s, tmp_fcc, op0, op1, ops );
    break;
  case INTRN_ISORDERED:
    // Bug 14757: GCC MIPS assembler doesn't support c.or.d, so fall through
    // to ISUNORDERED, then use negation below
  case INTRN_ISUNORDERED:
    Build_OP( is_double ? TOP_c_un_d : TOP_c_un_s, tmp_fcc, op0, op1, ops );
    break;
  case INTRN_SUBSU2:
    {
      TN * tmp1 = Build_TN_Like (result);
      TN * tmp2 = Build_TN_Like (result);
      TN * tmp_fcc = Gen_Register_TN (ISA_REGISTER_CLASS_fcc, 1);
      Build_OP (TOP_sub, tmp1, op0, op1, ops);
      Exp_Immediate (result, Gen_Literal_TN (0, 4), FALSE, ops);
      Build_OP (TOP_slti, tmp2, tmp1, Gen_Literal_TN (0, 4), ops);
      Build_OP (TOP_movz, result, tmp1, tmp2, result, ops);
      Set_OP_cond_def_kind(OPS_last(ops), OP_ALWAYS_COND_DEF);
      break;
    }
  default: FmtAssert(FALSE, ("Exp_Intrinsic_Op: unsupported intrinsic (%s)",
			     INTRN_rt_name (id)));
  }

  if (tmp_fcc != NULL) {
    Build_OP( id == INTRN_ISLESSGREATER || id == INTRN_ISORDERED
	      ? TOP_movt : TOP_movf,  // Bug 12441
	      result, Zero_TN, tmp_fcc, result, ops );
    Set_OP_cond_def_kind( OPS_last(ops), OP_ALWAYS_COND_DEF );
  }
  return;
}

/* ======================================================================
 * Expand_TOP_intrncall
 * 
 * Given a TOP_intrncall <op>, expand it into the sequence of instructions 
 * that must be generated. If <get_sequence_length> is TRUE, return only
 * the number of instructions in the sequence and don't actually do the 
 * expansion.  (Bug 12454)
 * ======================================================================*/

static BOOL
Intrinsic_Returns_New_Value (INTRINSIC id)
{
  switch (id) {
  case INTRN_ADD_AND_FETCH_I4:
  case INTRN_SUB_AND_FETCH_I4:
  case INTRN_OR_AND_FETCH_I4:
  case INTRN_XOR_AND_FETCH_I4:
  case INTRN_AND_AND_FETCH_I4:
  case INTRN_NAND_AND_FETCH_I4:
  case INTRN_ADD_AND_FETCH_I8:
  case INTRN_SUB_AND_FETCH_I8:
  case INTRN_OR_AND_FETCH_I8:
  case INTRN_XOR_AND_FETCH_I8:
  case INTRN_AND_AND_FETCH_I8:
  case INTRN_NAND_AND_FETCH_I8:
	return TRUE;
  default:
	return FALSE;
  }
}


static TYPE_ID
Get_Intrinsic_Size_Mtype (INTRINSIC id)
{
  switch (id) {
  case INTRN_COMPARE_AND_SWAP_I4:
  case INTRN_LOCK_TEST_AND_SET_I4:
  case INTRN_LOCK_RELEASE_I4:
  case INTRN_FETCH_AND_ADD_I4:
  case INTRN_ADD_AND_FETCH_I4:
  case INTRN_SUB_AND_FETCH_I4:
  case INTRN_OR_AND_FETCH_I4:
  case INTRN_XOR_AND_FETCH_I4:
  case INTRN_AND_AND_FETCH_I4:
  case INTRN_NAND_AND_FETCH_I4:
  case INTRN_FETCH_AND_SUB_I4:
  case INTRN_FETCH_AND_OR_I4:
  case INTRN_FETCH_AND_XOR_I4:
  case INTRN_FETCH_AND_AND_I4:
  case INTRN_FETCH_AND_NAND_I4:
	return MTYPE_I4;
  case INTRN_COMPARE_AND_SWAP_I8:
  case INTRN_LOCK_TEST_AND_SET_I8:
  case INTRN_LOCK_RELEASE_I8:
  case INTRN_FETCH_AND_ADD_I8:
  case INTRN_ADD_AND_FETCH_I8:
  case INTRN_SUB_AND_FETCH_I8:
  case INTRN_OR_AND_FETCH_I8:
  case INTRN_XOR_AND_FETCH_I8:
  case INTRN_AND_AND_FETCH_I8:
  case INTRN_NAND_AND_FETCH_I8:
  case INTRN_FETCH_AND_SUB_I8:
  case INTRN_FETCH_AND_OR_I8:
  case INTRN_FETCH_AND_XOR_I8:
  case INTRN_FETCH_AND_AND_I8:
  case INTRN_FETCH_AND_NAND_I8:
  case INTRN_SYNCHRONIZE:
	return MTYPE_I8;
  default:
  	FmtAssert(FALSE, ("Unexpected intrinsic %d", id));
	return MTYPE_UNKNOWN;
  }
}


static INT
Expand_TOP_intrncall ( const OP *op, OPS *ops, 
		       BOOL get_sequence_length,
		       INT pc_value )
{
  INTRINSIC id = (INTRINSIC) TN_value(OP_opnd(op, 0));

  INT num_ops = 5;
  if ( id == INTRN_COMPARE_AND_SWAP_I4 ||
       id == INTRN_COMPARE_AND_SWAP_I8 ) num_ops = 7;
  else if ( Intrinsic_Returns_New_Value(id) ) num_ops = 6;

  if (get_sequence_length) return num_ops;

  // Grab the Label from this BB
  BB *bb = OP_bb(op);
  LABEL_IDX lab = Gen_Label_For_BB(bb);
  INT pc_offset = pc_value - Get_Label_Offset(lab);

  const TYPE_ID mtype = Get_Intrinsic_Size_Mtype(id);
  TN *tn_addr = OP_opnd(op, 1);
  TN *tn_val = OP_opnd(op, 2);
  TN *tn_new = OP_opnd(op, 3);
  TN *tn_res = OP_result(op, 0);

  const BOOL is_64bit = MTYPE_is_size_double(mtype);
  Build_OP( is_64bit ? TOP_lld : TOP_ll,
	    tn_res, tn_addr, Gen_Literal_TN(0, 4),  ops );

  switch (id) {
  case INTRN_FETCH_AND_ADD_I4:
  case INTRN_FETCH_AND_ADD_I8:
  case INTRN_ADD_AND_FETCH_I4:
  case INTRN_ADD_AND_FETCH_I8:
    Expand_Add( tn_new, tn_res, tn_val, mtype, ops );
    break;
  case INTRN_FETCH_AND_AND_I4:
  case INTRN_FETCH_AND_AND_I8:
  case INTRN_AND_AND_FETCH_I4:
  case INTRN_AND_AND_FETCH_I8:
    Expand_Binary_And( tn_new, tn_res, tn_val, mtype, ops );
    break;
  case INTRN_FETCH_AND_OR_I4:
  case INTRN_FETCH_AND_OR_I8:
  case INTRN_OR_AND_FETCH_I4:
  case INTRN_OR_AND_FETCH_I8:
    Expand_Binary_Or( tn_new, tn_res, tn_val, mtype, ops );
    break;
  case INTRN_FETCH_AND_XOR_I4:
  case INTRN_FETCH_AND_XOR_I8:
  case INTRN_XOR_AND_FETCH_I4:
  case INTRN_XOR_AND_FETCH_I8:
    Expand_Binary_Xor( tn_new, tn_res, tn_val, mtype, ops );
    break;
  case INTRN_FETCH_AND_SUB_I4:
  case INTRN_FETCH_AND_SUB_I8:
  case INTRN_SUB_AND_FETCH_I4:
  case INTRN_SUB_AND_FETCH_I8:
    Expand_Sub( tn_new, tn_res, tn_val, mtype, ops );
    break;

  case INTRN_COMPARE_AND_SWAP_I4:
  case INTRN_COMPARE_AND_SWAP_I8:
    {
      Build_OP( TOP_bne, tn_res, tn_val,
		Gen_Label_TN( lab, pc_offset + 4 * num_ops ), ops );
      Expand_Copy( tn_res, Zero_TN, mtype, ops );
      Expand_Copy( tn_res, tn_new, mtype, ops );
      tn_new = tn_res;
      break;
    }

  default:
    FmtAssert(FALSE, ("Expand_TOP_intrncall: Unexpected intrinsic %d", id));
    return 0;
  }

  if ( Intrinsic_Returns_New_Value(id) ) {
    Expand_Copy( tn_res, tn_new, mtype, ops );
  }

  Build_OP( is_64bit ? TOP_scd : TOP_sc, tn_new, tn_addr, Gen_Literal_TN(0, 4),
	    ops );
  Build_OP( TOP_beq, tn_new, Zero_TN, Gen_Label_TN( lab, pc_offset ), ops );
  Exp_Noop( ops );
  Is_True( num_ops == OPS_length(ops),
	   ( "Expand_TOP_intrncall: num_ops %d should be %d",
	     num_ops, OPS_length(ops) ) );
  return num_ops;
}


// initial expansion of intrinsic call (may not be complete lowering).
// return result TN (if set).
// If the intrinsic requires a label and loop (2 bb's)
// then ops is for first bb and ops2 is for bb after the label.
// Otherwise only ops is filled in.
TN *
Exp_Intrinsic_Call (INTRINSIC id, TN *op0, TN *op1, TN *op2, OPS *ops, 
		    LABEL_IDX *label, OPS *loop_ops)
{
  switch (id) {
  case INTRN_APPLY_ARGS: 
    {
      static ST *last_PU = NULL;
      static TN *return_tn;
      ST *current_pu_st = Get_Current_PU_ST();
      if (last_PU == current_pu_st)
	return return_tn;
      else {
	Generate_Temp_Apply_Arg();
	last_PU = current_pu_st;
	INT par;
	INT ofst = 16;
	TN *ded_tn;

	// We always store the varargs immediately after FP
	// Store the address of vararg_temp_0 into new struct
	TN *vararg_ptr_tn = Build_TN_Of_Mtype(MTYPE_I8);
	Build_OP(TOP_addiu, vararg_ptr_tn, FP_TN, Gen_Literal_TN(-56, 4), ops);
	Exp_Store (MTYPE_I4, vararg_ptr_tn, tmp_apply_arg, 8, ops, 0);

	// Now, store all parameters into the new structure
	for (par = 0; par < MAX_NUMBER_OF_REGISTER_PARAMETERS; par ++) {
	  ded_tn = Build_Dedicated_TN(ISA_REGISTER_CLASS_integer,
				      par+5,
				      8 /* assume 64 bit registers */);
	  Exp_Store (MTYPE_I8, ded_tn, tmp_apply_arg, ofst, ops, 0);
	  ofst+= 8;
	}
	for (par = 0; par < MAX_NUMBER_OF_REGISTER_PARAMETERS; par ++) {
	  ded_tn = Build_Dedicated_TN(ISA_REGISTER_CLASS_float,
				      par+13,
				      8 /* assume 64 bit registers */);
	  Exp_Store (MTYPE_F8, ded_tn, tmp_apply_arg, ofst, ops, 0);
	  ofst+= 8;
	}

	// return the pointer to the new structure
	return_tn = Build_TN_Of_Mtype(MTYPE_I8);
	TN *base_tn, *ofst_tn;
	base_tn = FP_TN;
	ofst_tn = Gen_Symbol_TN (tmp_apply_arg, 8, 0);
	Exp_OP2( Pointer_Size == 4 ? OPC_I4ADD : OPC_I8ADD,
		 return_tn, base_tn, ofst_tn, ops );

	// store the first argument (function); can not see how this is 
	// useful. builtin_apply always has the name of the function as the
	// first argument, so why store it again here?
	ded_tn = Build_Dedicated_TN(ISA_REGISTER_CLASS_integer, 5,
				    8 /* assume 64 bit registers */);
	Exp_Store (MTYPE_I4, ded_tn, tmp_apply_arg, 0, ops, 0);
	return return_tn;
      }
    }
  case INTRN_APPLY:  
    {
      if (CG_opt_level > 0) {
	char asm_string[256];
	sprintf(asm_string, "__asm_builtin_apply_load");
	TN *opnd[1];
	opnd[0] = op1;
	OP *asm_op = Mk_VarOP(TOP_asm, 0, 1, NULL, opnd);
	Set_OP_volatile(asm_op);
	ASM_OP_ANNOT* asm_info = TYPE_PU_ALLOC(ASM_OP_ANNOT);
	bzero(asm_info, sizeof(ASM_OP_ANNOT));
	WN *asm_wn = WN_CreateAsm_Stmt (0, asm_string);
	ASM_OP_wn(asm_info) = asm_wn;
	OP_MAP_Set(OP_Asm_Map, asm_op, asm_info);
	OPS_Append_Op(ops, asm_op);
      } else {
	INT par;
	INT ofst = 8;
	TN *ded_tn;
	for (par = 0; par < MAX_NUMBER_OF_REGISTER_PARAMETERS; par ++) {
	  ded_tn = Build_Dedicated_TN(ISA_REGISTER_CLASS_integer,
				      par+5,
				      8 /* assume 64 bit registers */);
	  Build_OP(TOP_ld, ded_tn, op1, Gen_Literal_TN(ofst, 4), ops);
	  ofst+= 8;
	}
	for (par = 0; par < MAX_NUMBER_OF_REGISTER_PARAMETERS; par ++) {
	  ded_tn = Build_Dedicated_TN(ISA_REGISTER_CLASS_float,
				      par+13,
				      8 /* assume 64 bit registers */);
	  Build_OP(TOP_ldc1, ded_tn, op1, Gen_Literal_TN(ofst, 4), ops);
	  ofst+= 8;
	}      
      }
      return NULL;
    }
  case INTRN_RETURN:
    {
      TN *ded_tn;
      ded_tn = Build_Dedicated_TN(ISA_REGISTER_CLASS_integer,
		      		  3, 8);
      Build_OP(TOP_ld, ded_tn, op0, Gen_Literal_TN(0, 4), ops);
      ded_tn = Build_Dedicated_TN(ISA_REGISTER_CLASS_float,
		      		  1, 8);
      Build_OP(TOP_ldc1, ded_tn, op0, Gen_Literal_TN(8, 4), ops);
      return NULL;
    }

  case INTRN_SYNCHRONIZE:
    {
      Build_OP(TOP_sync, ops);
      return NULL;
    }

  case INTRN_FETCH_AND_ADD_I4:
  case INTRN_FETCH_AND_ADD_I8:
  case INTRN_FETCH_AND_AND_I4:
  case INTRN_FETCH_AND_AND_I8:
  case INTRN_FETCH_AND_OR_I4:
  case INTRN_FETCH_AND_OR_I8:
  case INTRN_FETCH_AND_XOR_I4:
  case INTRN_FETCH_AND_XOR_I8:
  case INTRN_FETCH_AND_SUB_I4:
  case INTRN_FETCH_AND_SUB_I8:
  case INTRN_ADD_AND_FETCH_I4:
  case INTRN_ADD_AND_FETCH_I8:
  case INTRN_AND_AND_FETCH_I4:
  case INTRN_AND_AND_FETCH_I8:
  case INTRN_OR_AND_FETCH_I4:
  case INTRN_OR_AND_FETCH_I8:
  case INTRN_XOR_AND_FETCH_I4:
  case INTRN_XOR_AND_FETCH_I8:
  case INTRN_SUB_AND_FETCH_I4:
  case INTRN_SUB_AND_FETCH_I8:
    {
      // Generate a new TN placeholder to be used inside intrncall expansion.
      const TYPE_ID mtype = Get_Intrinsic_Size_Mtype(id);
      op2 = Build_TN_Of_Mtype(mtype);
      Build_OP(TOP_ifixup, op2, ops);
      // Fall through to COMPARE_AND_SWAP case
    }
  case INTRN_COMPARE_AND_SWAP_I4:
  case INTRN_COMPARE_AND_SWAP_I8:
    {
      const TYPE_ID mtype = Get_Intrinsic_Size_Mtype(id);
      TN *tn_result = Build_TN_Of_Mtype(mtype);
      Build_OP(TOP_intrncall, tn_result, Gen_Literal_TN((INT) id, 4),
	       op0, op1, op2, ops);
      return tn_result;
    }

  default:
    FmtAssert(FALSE, ("Exp_Intrinsic_Call NYI"));
    return NULL; // if you can
  }
}


/* ======================================================================
 * Exp_Simulated_Op
 *
 * Given a simulated <op>, expand it into the sequence of instructions
 * supported by the target.
 * ======================================================================*/
void Exp_Simulated_Op(const OP *op, OPS *ops, INT pc_value)
{
  TOP top = OP_code(op);

  switch (top)
  {
  case TOP_intrncall:
    Expand_TOP_intrncall(op, ops, FALSE, pc_value);
    break;
  case TOP_sext:  // Bug 13497:  sext r2, r1  -->  sll r2, r1, 0x0
    Build_OP(TOP_sll, OP_result(op, 0), OP_opnd(op, 0),
	     Gen_Literal_TN(0, 4), ops);
    break;
  case TOP_mov_fcc:
  case TOP_mov_hilo:
    /* mov_fcc is used only for glue copies by SWP. Since this is a
     * simulated op, the result and the operand register must always
     * be the same, because there is no move instruction for fcc 
     * registers.
     * EBO normaly removes this OP. If we ever should reach this point 
     * here, we expand this OP to a noop.
     * 14780: mov_hilo can also be generated.  If mov_hilo or mov_fcc
     * appears inside a SWP kernel block, ebo won't remove them.
     */
    {
      TN *tgt_tn = OP_result (op, 0);
      TN *src_tn = OP_opnd (op, 0);
      if (!TNs_Are_Equivalent (tgt_tn, src_tn)) {
        Print_OP (op, stderr);
        Fail_FmtAssertion ("Result and operand register for "
			   "mov_fcc/mov_hilo are not equal");
      }
    }
    break;
  default:
    FmtAssert(FALSE, ("simulated OP %s not handled", TOP_Name(top)));
  }
}


/* ======================================================================
 * Simulated_Op_Real_Ops
 *
 * Return the number of instructions that will be generated by Exp_Simulated_Op
 * ======================================================================*/
INT
Simulated_Op_Real_Ops(const OP *op)
{
  switch (OP_code(op)) {
  case TOP_intrncall:
    return Expand_TOP_intrncall (op, NULL, TRUE, 0);
  case TOP_spadjust:
  case TOP_sext:
    return 1;
  default:

    /* Anything other than the above is presumed to be removed by
     * emit time, therefore we just say the expansion generates 0 ops.
     * (we used to assert, but that isn't a good solution -- see pv 652898).
     */
    return 0;
  }
}


/* ======================================================================
 * Simulated_Op_Real_Inst_Words
 *
 * Return the number of instruction words that will ultimately be emitted
 * for the expansion generated by Exp_Simulated_Op
 * ======================================================================*/
INT
Simulated_Op_Real_Inst_Words(const OP *op)
{
  INT num_bytes = 0;
  TOP top = OP_code(op);

  switch (top)
  {
  case TOP_spadjust:
  case TOP_sext:
    num_bytes = 1;
    break;

  case TOP_asm:
    /* We don't know how many instructions are "within" the asm, so we
       just assume 3 bytes. */
    num_bytes = 3;
    break;

  case TOP_intrncall:
    num_bytes = Expand_TOP_intrncall(op, NULL, TRUE, 0);
    break;

  case TOP_mov_fcc:
  case TOP_mov_hilo:
    num_bytes = 0;
    break;

  default:
    FmtAssert(FALSE, ("simulated OP %s not handled", TOP_Name(OP_code(op))));
  }

  return num_bytes;
}


/* ======================================================================
 * Exp_Is_Large_Stack_Sym
 *
 * determine if a given symbol is a stack relative reference that will
 * require multiple instructions to load or store.
 * ======================================================================*/
BOOL
Exp_Is_Large_Stack_Sym(ST* sym,  INT64 ofst)
{
  ST *base_sym;
  INT64 base_ofst;
  
  if (sym == NULL)
    return FALSE;

  Allocate_Object(sym);
  Base_Symbol_And_Offset_For_Addressing (sym, ofst, &base_sym, &base_ofst);

  /* We can assume that 'sym' is a spill location for an integer
     register, so we can check for l32i/s32i range. */
  
  if ((base_sym == SP_Sym || base_sym == FP_Sym) &&
      !ISA_LC_Value_In_Class(base_ofst, LC_simm16)) {
    return TRUE;
  }

  return FALSE;
}

void
Exp_Noop (OPS *ops)
{
  Build_OP (CGTARG_Noop_Top(), ops);
}

void Expand_Const (TN *dest, TN *src, TYPE_ID mtype, OPS *ops)
{
  FmtAssert( TN_is_symbol(src), ("Expand_Const: src not a symbol TN"));

  TCON tcon = STC_val(TN_var(src));
  // Bugs 9734, 12398: Must use Targ_Is_Zero to check for positive zero.
  if (TCON_ty(tcon) == MTYPE_F4 && Targ_Is_Zero(tcon)) {
    FmtAssert(TCON_ty(tcon) == mtype || mtype == MTYPE_V8F4,
	      ("Expand_Const: inconsistent mtypes"));
    Build_OP(TOP_mtc1, dest, Zero_TN, ops);
    return;
  }
  if (TCON_ty(tcon) == MTYPE_F8 && Targ_Is_Zero(tcon)) {
    FmtAssert(TCON_ty(tcon) == mtype, ("Expand_Const: inconsistent mtypes"));
    Build_OP(TOP_dmtc1, dest, Zero_TN, ops);
    return;
  }
  Exp_Load(mtype, mtype, dest, TN_var(src), 0, ops, 0);
}

static BB* last_bb = NULL;
static TN *last_true_tn = NULL, *last_false_tn = NULL;
void
HB_Reinit_Pred ()
{
  last_true_tn = NULL;
  last_false_tn = NULL;
  last_bb = NULL;
}

void
Exp_True_False_Preds_For_Block(BB *bb, TN* &true_tn, TN * &false_tn)
{ 
  if (last_bb != bb)
    last_bb = bb;
  else {
    true_tn = last_true_tn;
    false_tn = last_false_tn;
    return;
  }
  OP* br_op = BB_branch_op(bb);
  if (!br_op)
    return;
  if (OP_code(br_op) == TOP_beq || OP_code(br_op) == TOP_bne) {
    OPS new_ops;
    OPS_Init(&new_ops);
    true_tn = Gen_Register_TN(ISA_REGISTER_CLASS_integer, 4);
    false_tn = Gen_Register_TN(ISA_REGISTER_CLASS_integer, 4);
    TN *tmp_tn = Gen_Register_TN(ISA_REGISTER_CLASS_integer, 4);
    TN *tmp1_tn;
    Build_OP(TOP_xor, tmp_tn, OP_opnd(br_op, 0), OP_opnd(br_op, 1), 
	     &new_ops);
    Build_OP(TOP_sltiu, false_tn, tmp_tn, Gen_Literal_TN(1, 4), &new_ops);
    Build_OP(TOP_sltu, true_tn, Zero_TN, tmp_tn, &new_ops);
    if (OP_code(br_op) == TOP_bne) {
      tmp1_tn = true_tn;
      true_tn = false_tn;
      false_tn = tmp1_tn;
    }
    BB_Insert_Ops_Before(bb, br_op, &new_ops);
    last_true_tn = true_tn;
    last_false_tn = false_tn;
    return;       
  } else if (OP_code(br_op) == TOP_bc1f || OP_code(br_op) == TOP_bc1t) {
    OPS new_ops;
    OPS_Init(&new_ops);
    true_tn = OP_opnd(br_op, 0);
    false_tn = Gen_Register_TN(ISA_REGISTER_CLASS_fcc, 4);
    // search backwards for the instruction that sets the conditon bit;
    // The false TN would be obtained by inverting that condition.
    OP *op;
    BOOL found_cond_set = FALSE;
    FOR_ALL_BB_OPs_REV(bb,op) {
      if (OP_results(op) && 
	  (OP_result(op, 0) == OP_opnd(br_op, 0))) {
	found_cond_set = TRUE;
	break;
      }
    }
    FmtAssert((found_cond_set==TRUE), 
	       ("Did not find instruction setting condition bit in BB"));    
    switch (OP_code(op)) {
    case TOP_c_le_d:
      Build_OP(TOP_c_lt_d, false_tn, OP_opnd(op, 1), OP_opnd(op, 0), &new_ops);
      break;
    case TOP_c_le_s:
      Build_OP(TOP_c_lt_s, false_tn, OP_opnd(op, 1), OP_opnd(op, 0), &new_ops);
      break;
    case TOP_c_lt_d:
      Build_OP(TOP_c_le_d, false_tn, OP_opnd(op, 1), OP_opnd(op, 0), &new_ops);
      break;
    case TOP_c_lt_s:
      Build_OP(TOP_c_le_s, false_tn, OP_opnd(op, 1), OP_opnd(op, 0), &new_ops);
      break;
    case TOP_c_eq_d:
      Build_OP(TOP_c_neq_d, false_tn, OP_opnd(op, 1), OP_opnd(op, 0), 
	       &new_ops);
      break;
    case TOP_c_eq_s:
      Build_OP(TOP_c_neq_s, false_tn, OP_opnd(op, 1), OP_opnd(op, 0), 
	       &new_ops);
      break;      
    default:
      FmtAssert(FALSE, ("Handle this case"));
      break;
    }
    TN *tmp_tn = Gen_Register_TN(ISA_REGISTER_CLASS_fcc, 4);
    if (OP_code(br_op) == TOP_bc1t) {
      tmp_tn = true_tn;
      true_tn = false_tn;
      false_tn = tmp_tn;
    }
    BB_Insert_Ops_Before(bb, br_op, &new_ops);
    last_true_tn = true_tn;
    last_false_tn = false_tn;
    return;           
  } else if (OP_code(br_op) == TOP_bgez || OP_code(br_op) == TOP_bltz) {
    OPS new_ops;
    OPS_Init(&new_ops);
    true_tn = Gen_Register_TN(ISA_REGISTER_CLASS_integer, 4);
    false_tn = Gen_Register_TN(ISA_REGISTER_CLASS_integer, 4);
    TN *tmp_tn = Gen_Register_TN(ISA_REGISTER_CLASS_integer, 4);
    TN *tmp1_tn;
    Build_OP(TOP_slt, tmp_tn, OP_opnd(br_op, 0), Zero_TN,
	     &new_ops);
    Build_OP(TOP_xori, true_tn, tmp_tn, Gen_Literal_TN(1, 4), &new_ops);
    Build_OP(TOP_slti, false_tn, OP_opnd(br_op, 0), Gen_Literal_TN(0, 4), 
	     &new_ops);
    if (OP_code(br_op) == TOP_bgez) {
      tmp1_tn = true_tn;
      true_tn = false_tn;
      false_tn = tmp1_tn;
    }
    BB_Insert_Ops_Before(bb, br_op, &new_ops);
    last_true_tn = true_tn;
    last_false_tn = false_tn;
    return;       
  }
  FmtAssert(FALSE, ("HANDLE THIS CASE"));
  true_tn = false_tn = Zero_TN;
}

BOOL
Target_Has_Immediate_Operand (WN *parent, WN *expr)
{
  OPERATOR opr = WN_operator(parent);
  return opr == OPR_ADD || opr == OPR_SUB || 
         opr == OPR_BAND || opr == OPR_BIOR || opr == OPR_BXOR ||
         opr == OPR_LT || opr == OPR_LE || opr == OPR_GT || opr == OPR_GE ||
         opr == OPR_LSHR || opr == OPR_ASHR || opr == OPR_SHL;
}

void 
Exp_Spadjust (TN *dest, TN *size, VARIANT variant, OPS *ops)
{
  // A temp register is needed if the adjustment size can't fit in the
  // immediate field of an add instruction.  spadjust is converted into add
  // after register allocation, at which time there are no more registers.  So
  // create a temp TN now.  Bug 8734.
  if (TN_is_constant(size) &&
      TN_has_value(size) &&
      !CGTARG_Can_Fit_Immediate_In_Add_Instruction(TN_value(size))) {
    TN *temp = Build_TN_Of_Mtype(MTYPE_I8);
    Exp_Immediate(temp, size, TRUE, ops);
    size = temp;
  }

  Build_OP (TOP_spadjust, dest, SP_TN, size, ops);
  OP_variant(OPS_last(ops)) = variant;
}

/* Return a unique name for a symbol representing a literal. */
char *
Exp_Unique_Literal_Name (void)
{
  static int unique;
  static char name[32];

  sprintf(name, ".LC%d", unique);
  unique++;
  return name;
}

void
Exp_Generic_Pred_Calc(TN* result1, TN *result2, COMPARE_TYPE ctype,
                      TN *qual_pred, OPS* ops)
{ FmtAssert(FALSE,("Unimplemented")); }
  
  
void
Exp_Pred_Calc(TN* result, OP* cmp_op, COMPARE_TYPE ctype, BOOL false_result,
	      OPS* ops)
{ FmtAssert(FALSE,("Unimplemented")); }
