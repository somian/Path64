/*
 * Copyright 2009 SiCortex, Inc.
 */

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


/* CGEXP routines for loads and stores */
#include "elf_stuff.h"
#include "defs.h"
#include "em_elf.h"
#include "erglob.h"
#include "ercg.h"
#include "tracing.h"
#include "config.h"
#include "config_targ_opt.h"
#include "config_debug.h"
#include "xstats.h"
#include "topcode.h"
#include "tn.h"
#include "cg_flags.h"
#include "targ_isa_lits.h"
#include "op.h"
#include "stblock.h"
#include "data_layout.h"
#include "strtab.h"
#include "symtab.h"
#include "cg.h"
#include "cgexp.h"
#include "cgexp_internals.h"
#include "cgemit.h"	// for CG_emit_non_gas_syntax

#define OFFSET_HI(offset) ((((offset) + 0x8000) >> 16) << 16)
#define OFFSET_LO(offset) ((((offset) + 0x8000) & 0xffff) - 0x8000)

void
Expand_Lda (TN *dest, TN *src, OPS *ops)
{
  FmtAssert(FALSE, ("NYI: Expand_Lda"));
}

static TOP
Pick_Load_Instruction (TYPE_ID rtype, TYPE_ID desc)
{
  switch (desc) {
  case MTYPE_I1: return TOP_lb;
  case MTYPE_U1: return TOP_lbu;
  case MTYPE_I2: return TOP_lh;
  case MTYPE_U2: return TOP_lhu;
  case MTYPE_I4: return TOP_lw;
  case MTYPE_U4: return MTYPE_is_size_double(rtype) ? TOP_lwu : TOP_lw;
  case MTYPE_I8:  case MTYPE_U8:	return TOP_ld;
  case MTYPE_F4:			return TOP_lwc1;
  case MTYPE_F8:  case MTYPE_V8F4:	return TOP_ldc1;

  case MTYPE_V:
    if (rtype != MTYPE_V)
      // use rtype to pick load (e.g. if lda)
      return Pick_Load_Instruction(rtype,rtype);
    // else fallthru
  default:  
    FmtAssert(FALSE, ("NYI: Pick_Load_Instruction mtype"));
    return TOP_UNDEFINED;
  }
}

void
Expand_Load (OPCODE opcode, TN *result, TN *base, TN *ofst, OPS *ops)
{
  TYPE_ID mtype = OPCODE_desc(opcode);
  TOP top = Pick_Load_Instruction (OPCODE_rtype(opcode), mtype);
  if (OPCODE_operator(opcode) == OPR_ILOADX)
    top = (top == TOP_lwc1) ? TOP_lwxc1 : TOP_ldxc1;
  else Is_True (TN_is_constant(ofst), ("Expand_Load: Illegal offset TN"));
  // Handle very large offsets (that consume greater than 16 bits).
  // Try and see if the base and offset can be merged.
  // An example is gcc.c-torture/compile/990617-1.c
  if (TN_is_rematerializable(base)) {
    WN *home = TN_home(base);
    if (WN_operator(home) == OPR_INTCONST) {
      INT64 val = WN_const_val(home);
      BOOL is_double = TN_size(base) == 8;
      TN *tmp_base = Gen_Literal_TN (val + TN_value(ofst), TN_size(base));
      tmp_base = Expand_Immediate_Into_Register(tmp_base, is_double, ops);
      Build_OP (top, result, tmp_base, Gen_Literal_TN (0, 2), ops);
      return;
    }
  }
  if (!TN_has_value(ofst) || ISA_LC_Value_In_Class(TN_value(ofst), LC_simm16))
    Build_OP (top, result, base, ofst, ops);
  else {
    // Bug 12475: Handle large offsets
    TN *tmp_tn = Build_TN_Of_Mtype(Pointer_Mtype);
    INT64 offset = TN_value(ofst);
    INT64 low16 = (offset << 48) >> 48;
    INT64 high16 = ((offset - low16) >> 16) & 0xffff;
    Build_OP(TOP_lui, tmp_tn, Gen_Literal_TN(high16, 4), ops);
    Build_OP(Use_32_Bit_Pointers ? TOP_addu : TOP_daddu, tmp_tn,
	     tmp_tn, base, ops);
    Build_OP(top, result, tmp_tn, Gen_Literal_TN(low16, 4), ops);
  }
}

static TOP
Pick_Store_Instruction (TYPE_ID mtype)
{
  switch (mtype) {
  case MTYPE_I1:  case MTYPE_U1:	return TOP_sb;
  case MTYPE_I2:  case MTYPE_U2:	return TOP_sh;
  case MTYPE_I4:  case MTYPE_U4:	return TOP_sw;
  case MTYPE_I8:  case MTYPE_U8:	return TOP_sd;
  case MTYPE_F4: 			return TOP_swc1;
  case MTYPE_F8:  case MTYPE_V8F4:	return TOP_sdc1;
  default:  FmtAssert(FALSE, ("NYI: Pick_Store_Instruction mtype"));
    return TOP_UNDEFINED;
  }
}

void
Expand_Store (TYPE_ID mtype, TN *src, TN *base, TN *ofst, OPS *ops)
{
  TOP top = Pick_Store_Instruction (mtype);
  if (! TN_is_constant(ofst))
    top = (top == TOP_swc1) ? TOP_swxc1 : TOP_sdxc1;
  if (!TN_has_value(ofst) || ISA_LC_Value_In_Class(TN_value(ofst), LC_simm16))
    Build_OP (top, src, base, ofst, ops);
  else {
    // Bug 12475: Handle large offsets
    TN *tmp_tn = Build_TN_Of_Mtype(Pointer_Mtype);
    INT64 offset = TN_value(ofst);
    INT64 low16 = (offset << 48) >> 48;
    INT64 high16 = ((offset - low16) >> 16) & 0xffff;
    Build_OP(TOP_lui, tmp_tn, Gen_Literal_TN(high16, 4), ops);
    Build_OP(Use_32_Bit_Pointers ? TOP_addu : TOP_daddu, tmp_tn,
	     tmp_tn, base, ops);
    Build_OP(top, src, tmp_tn, Gen_Literal_TN(low16, 4), ops);
  }
}

static OPCODE 
OPCODE_make_signed_op(OPERATOR op, TYPE_ID rtype, TYPE_ID desc, BOOL is_signed)
{
  if (MTYPE_is_signed(rtype) != is_signed)
	rtype = MTYPE_complement(rtype);
  if (MTYPE_is_signed(desc) != is_signed)
	desc =	MTYPE_complement(desc);

  return OPCODE_make_op(op, rtype, desc);
}

/* ====================================================================
 *
 * Adjust_Addr_TNs
 *
 * We have a memory reference operation, with a base and displacement,
 * where the displacement is literal.  We want to create another memop
 * with the displacement modified by a small amount.
 *
 * WARNING:  If an add operation is required, it will be expanded here.
 *
 * ====================================================================
 */

static void
Adjust_Addr_TNs (
  TOP	opcode,		/* The new memory operation */
  TN	**base_tn,	/* The base address -- may be modified */
  TN	**disp_tn,	/* The displacement -- may be modified */
  INT16	disp,		/* A displacement to add */
  OPS *ops)
{

  if ( Potential_Immediate_TN_Expr (opcode, *disp_tn, disp) )
  {
    if ( TN_has_value(*disp_tn) ) {
      *disp_tn = Gen_Literal_TN ( TN_value(*disp_tn) + disp, 4 );
    } else {
      *disp_tn = Gen_Symbol_TN ( TN_var(*disp_tn),
				 TN_offset(*disp_tn) + disp, 0);
    }
  } else {
    TN *tmp = Build_TN_Of_Mtype (Pointer_Mtype);
    // because disp may be symbolic reloc on base,
    // want to still add it with base and create new base and disp.
    Expand_Add (tmp, *disp_tn, *base_tn, Pointer_Mtype, ops);
    *base_tn = tmp;
    *disp_tn = Gen_Literal_TN (disp, 4);
  }
}

static void
Expand_Composed_Load ( OPCODE op, TN *result, TN *base, TN *disp, VARIANT variant, OPS *ops)
{
  TYPE_ID rtype= OPCODE_rtype(op);
  TYPE_ID desc = OPCODE_desc(op);
  TN *tmp1, *tmp2, *tmp3;

  if (MTYPE_is_float(rtype)) {
    TN     *load;

    if (rtype == MTYPE_F4) {
      load = Build_TN_Of_Mtype(MTYPE_I4);
      Expand_Composed_Load ( OPC_I4I4LDID, load, base, disp, variant, ops);
      // mtc1 moves bits into fp reg.
      Build_OP ( TOP_mtc1, result, load, ops );
    } else {
      load = Build_TN_Of_Mtype(MTYPE_I8);
      Expand_Composed_Load ( OPC_I8I8LDID, load, base, disp, variant, ops);
      Build_OP ( TOP_dmtc1, result, load, ops );
    }
    Reset_TN_is_fpu_int(result);
    return;
  }

  // Construct RESULT using a sequence of instructions.  Be careful not to use
  // RESULT in the sequence (unless we mark it as OP_ALWAYS_COND_DEF), so that
  // when RESULT is a dedicated TN (parameter register for example), localize
  // (Localize_or_Replace_Dedicated_TNs) will not complain about the dedicated
  // TN being used but BB does not follow a call BB.  Bug 12712.
  switch (desc) {
    case MTYPE_I8:
    case MTYPE_U8:
      if (result == base) { // bug 13127
	TN *tmp = Build_TN_Like(base);
	Expand_Copy(tmp, base, desc, ops);
	base = tmp;
      }
      Build_OP (Target_Byte_Sex == BIG_ENDIAN ? TOP_ldl : TOP_ldr,
  		result, base, disp, Zero_TN, ops);
      // The highest byte is at effective address + 7.
      Adjust_Addr_TNs (Target_Byte_Sex == BIG_ENDIAN ? TOP_ldr : TOP_ldl,
  		      &base, &disp, 7, ops);
      Build_OP (Target_Byte_Sex == BIG_ENDIAN ? TOP_ldr : TOP_ldl,
  		result, base, disp, result, ops);
      Set_OP_cond_def_kind(OPS_last(ops), OP_ALWAYS_COND_DEF);
      return;
    case MTYPE_I4:
    case MTYPE_U4:
      // The only difference is that the first case requires clearing the top
      // 32 bits.
      if ((rtype == MTYPE_U8) && (desc == MTYPE_U4)) {
        tmp1 = Build_TN_Of_Mtype(rtype);  // See comment above for using tmps.
	tmp2 = Build_TN_Of_Mtype(rtype);
	Build_OP(Target_Byte_Sex == BIG_ENDIAN ? TOP_lwl : TOP_lwr,
  		 tmp1, base, disp, Zero_TN, ops);
	// The highest byte is at effective address + 3.
	Adjust_Addr_TNs(Target_Byte_Sex == BIG_ENDIAN ? TOP_lwr : TOP_lwl,
			&base, &disp, 3, ops);
	Build_OP(Target_Byte_Sex == BIG_ENDIAN ? TOP_lwr : TOP_lwl,
		 tmp1, base, disp, tmp1, ops);
	Set_OP_cond_def_kind(OPS_last(ops), OP_ALWAYS_COND_DEF);
	// Clear the top 32 bits for U8U4 load. (Bug 13237)
	Build_OP (TOP_dsll32, tmp2, tmp1, Gen_Literal_TN(0, 4), ops);
	Build_OP (TOP_dsrl32, result, tmp2, Gen_Literal_TN(0, 4), ops);
      } else {
	if (result == base) { // bug 13127
	  TN *tmp = Build_TN_Like(base);
	  Expand_Copy(tmp, base, desc, ops);
	  base = tmp;
	}
	Build_OP(Target_Byte_Sex == BIG_ENDIAN ? TOP_lwl : TOP_lwr,
		 result, base, disp, Zero_TN, ops);
	// The highest byte is at effective address + 3.
	Adjust_Addr_TNs(Target_Byte_Sex == BIG_ENDIAN ? TOP_lwr : TOP_lwl,
			&base, &disp, 3, ops);
	Build_OP(Target_Byte_Sex == BIG_ENDIAN ? TOP_lwr : TOP_lwl,
		 result, base, disp, result, ops);
	Set_OP_cond_def_kind(OPS_last(ops), OP_ALWAYS_COND_DEF);
      }
      return;
    case MTYPE_I2:
    case MTYPE_U2:
      if (Target_Byte_Sex == BIG_ENDIAN) {
        tmp1 = Build_TN_Of_Mtype(rtype);  // See comment above for using tmps.
        tmp2 = Build_TN_Of_Mtype(rtype);
        tmp3 = Build_TN_Of_Mtype(rtype);

        Build_OP (desc == MTYPE_I2 ? TOP_lb : TOP_lbu,
		  tmp1, base, disp, ops);
	Build_OP (TOP_sll, tmp2, tmp1, Gen_Literal_TN(8, 4), ops);
        // The LSB is at effective address + 1.
        Adjust_Addr_TNs (TOP_lb, &base, &disp, 1, ops);
        Build_OP (TOP_lbu, tmp3, base, disp, ops);
        Build_OP (TOP_or, result, tmp2, tmp3, ops);
      } else {
        tmp1 = Build_TN_Of_Mtype(rtype);  // See comment above for using tmps.
        tmp2 = Build_TN_Of_Mtype(rtype);
        tmp3 = Build_TN_Of_Mtype(rtype);

        Build_OP (TOP_lbu, tmp1, base, disp, ops);
        // The MSB is at effective address + 1.
        Adjust_Addr_TNs (TOP_lb, &base, &disp, 1, ops);
        Build_OP (desc == MTYPE_I2 ? TOP_lb : TOP_lbu,
		  tmp2, base, disp, ops);
	Build_OP (TOP_sll, tmp3, tmp2, Gen_Literal_TN(8, 4), ops);
        Build_OP (TOP_or, result, tmp1, tmp3, ops);
      }
      return;
    default:
      FmtAssert (FALSE, ("Expand_Composed_Load: unexpected operand size\n"));
  }
}

void
Expand_Misaligned_Load ( OPCODE op, TN *result, TN *base, TN *disp, VARIANT variant, OPS *ops)
{
  Expand_Composed_Load ( op, result, base, disp, variant, ops);
}


static void
Expand_Composed_Store (TYPE_ID mtype, TN *obj, TN *base, TN *disp, VARIANT variant, OPS *ops)
{
  if (MTYPE_is_float(mtype))
  {
	TN     *tmp;

	if (mtype == MTYPE_F4)
	{
		tmp = Build_TN_Of_Mtype(MTYPE_I4);
		Build_OP ( TOP_mfc1, tmp, obj, ops );
		Expand_Composed_Store (MTYPE_I4, tmp, base, disp, variant, ops);
	}
	else
	{
		tmp = Build_TN_Of_Mtype(MTYPE_I8);
		Build_OP ( TOP_dmfc1, tmp, obj, ops );
		Expand_Composed_Store (MTYPE_I8, tmp, base, disp, variant, ops);
	}
	return;
  }

  TN *tmp;
  switch (mtype) {
    case MTYPE_I8:
    case MTYPE_U8:
      Build_OP (Target_Byte_Sex == BIG_ENDIAN ? TOP_sdl : TOP_sdr,
		obj, base, disp, ops);
      // The highest byte is at effective address + 7.
      Adjust_Addr_TNs (Target_Byte_Sex == BIG_ENDIAN ? TOP_sdr : TOP_sdl,
		      &base, &disp, 7, ops);
      Build_OP (Target_Byte_Sex == BIG_ENDIAN ? TOP_sdr : TOP_sdl,
		obj, base, disp, ops);
      return;
    case MTYPE_I4:
    case MTYPE_U4:
      Build_OP (Target_Byte_Sex == BIG_ENDIAN ? TOP_swl : TOP_swr,
		obj, base, disp, ops);
      // The highest byte is at effective address + 3.
      Adjust_Addr_TNs (Target_Byte_Sex == BIG_ENDIAN ? TOP_swr : TOP_swl,
		      &base, &disp, 3, ops);
      Build_OP (Target_Byte_Sex == BIG_ENDIAN ? TOP_swr : TOP_swl,
		obj, base, disp, ops);
      return;
    case MTYPE_I2:
    case MTYPE_U2:
      if (Target_Byte_Sex == BIG_ENDIAN) {
        tmp = Build_TN_Of_Mtype(mtype);
	Build_OP (TOP_srl, tmp, obj, Gen_Literal_TN(8, 4), ops);
        Build_OP (TOP_sb, tmp, base, disp, ops);
        // The LSB is at effective address + 1.
        Adjust_Addr_TNs (TOP_lb, &base, &disp, 1, ops);
        Build_OP (TOP_sb, obj, base, disp, ops);
      } else {
        Build_OP (TOP_sb, obj, base, disp, ops);
        // The MSB is at effective address + 1.
        Adjust_Addr_TNs (TOP_lb, &base, &disp, 1, ops);
        tmp = Build_TN_Of_Mtype(mtype);
	Build_OP (TOP_srl, tmp, obj, Gen_Literal_TN(8, 4), ops);
        Build_OP (TOP_sb, tmp, base, disp, ops);
      }
      return;
    default:
      FmtAssert (FALSE, ("Expand_Composed_Store: unexpected operand size\n"));
  }
}

void
Expand_Misaligned_Store (TYPE_ID mtype, TN *obj_tn, TN *base_tn, TN *disp_tn, VARIANT variant, OPS *ops)
{
  Expand_Composed_Store (mtype, obj_tn, base_tn, disp_tn, variant, ops);
}

static void
Exp_Ldst (
  OPCODE opcode,
  TN *tn,
  ST *sym,
  INT64 ofst,
  BOOL indirect_call,
  BOOL is_store,
  BOOL is_load,
  OPS *ops,
  VARIANT variant,
  OPERATOR stmt_opr = OPERATOR_UNKNOWN)
{
  ST *base_sym;
  INT64 base_ofst;
  TN *base_tn;
  TN *ofst_tn;
  TN *tmp_tn;
  BOOL is_lda = (!is_load && !is_store);
  OPS newops;
  OP *op;
  OPS_Init(&newops);

  if (Trace_Exp2) {
    fprintf(TFile, "exp_ldst %s: ", OPCODE_name(opcode));
    if (tn) Print_TN(tn,FALSE);
    if (is_store) fprintf(TFile, " -> ");
    else fprintf(TFile, " <- ");
    if (ST_class(sym) != CLASS_CONST)
      fprintf(TFile, "%" SCNd64 " (%s)\n", ofst, ST_name(sym));
    else
      fprintf(TFile, "%" SCNd64 " ()\n", ofst);
  }
  
  Allocate_Object(sym);         /* make sure sym is allocated */
  
  Base_Symbol_And_Offset_For_Addressing (sym, ofst, &base_sym, &base_ofst);

  if (base_sym == SP_Sym || base_sym == FP_Sym) {
    base_tn = (base_sym == SP_Sym) ? SP_TN : FP_TN;
    if (sym == base_sym) {
      // can have direct reference to SP or FP,
      // e.g. if actual stored to stack.
      if (ISA_LC_Value_In_Class(base_ofst, LC_simm16))
	ofst_tn = Gen_Literal_TN (base_ofst, 4);
      else {
	tmp_tn = Build_TN_Of_Mtype(Pointer_Mtype);
	// Bug 12475, 10978: Handle large offsets
	INT64 low16 = (base_ofst << 48) >> 48;
	INT64 high16 = ((base_ofst - low16) >> 16) & 0xffff;
	Build_OP(TOP_lui, tmp_tn, Gen_Literal_TN(high16, 4), &newops);
	Build_OP(Use_32_Bit_Pointers ? TOP_addu : TOP_daddu, tmp_tn, 
		 tmp_tn, base_tn, &newops);
	base_tn = tmp_tn;
	ofst_tn = Gen_Literal_TN (low16, 4);
      }
    }
    else {
      /* Because we'd like to see symbol name in .s file, 
       * still reference the symbol rather than the sp/fp base.  
       * Do put in the offset from the symbol.  
       * We put the symbol in the TN and then
       * let cgemit replace symbol with the final offset.
       * We generate a SW reg, <sym>, <SP> rather than SW reg,<sym>
       * because cgemit and others expect a separate tn for the
       * offset and base. 
       */
      if (ISA_LC_Value_In_Class(base_ofst, LC_simm16))
	ofst_tn = Gen_Symbol_TN (sym, ofst, 0);
      else if ((base_ofst << 32 >> 32) == base_ofst) { // fit in signed 32-bit
	tmp_tn = Build_TN_Of_Mtype(Pointer_Mtype);
	Build_OP(TOP_lui, tmp_tn, 
		 Gen_Symbol_TN(sym, ofst, TN_RELOC_HIGH16), &newops);
	Build_OP(Use_32_Bit_Pointers ? TOP_addu : TOP_daddu, tmp_tn, 
		 tmp_tn, base_tn, &newops);
	base_tn = tmp_tn;
	ofst_tn = Gen_Symbol_TN(sym, ofst, TN_RELOC_LOW16);
      }
      else { // bug 12701
	ofst_tn = Gen_Literal_TN (base_ofst, 8);
	ofst_tn = Expand_Immediate_Into_Register(ofst_tn, TRUE, &newops);
	tmp_tn = Build_TN_Of_Mtype(Pointer_Mtype);
	Build_OP(TOP_daddu, tmp_tn, base_tn, ofst_tn, &newops);
	base_tn = tmp_tn;
	ofst_tn = Gen_Literal_TN(0, 8);
      }
    }
  }
  else if ((ST_class(base_sym) == CLASS_BLOCK || ST_class(base_sym)==CLASS_VAR)
	   && ST_gprel(base_sym)) 
    {
      // gp-relative reference
      PU_References_GP = TRUE;
      if (ISA_LC_Value_In_Class(base_ofst, LC_simm16)) {
	base_tn = GP_TN;
	ofst_tn = Gen_Symbol_TN (sym, ofst, TN_RELOC_GPREL16);
      } 
      else {
	FmtAssert(FALSE, ("gp-relative offset doesn't fit in 16 bits"));
      }
    }
  else if (! Gen_PIC_Shared) {
    base_tn = Build_TN_Of_Mtype(Pointer_Mtype);
    Build_OP(TOP_lui, base_tn, 
	     Gen_Symbol_TN(base_sym, base_ofst, TN_RELOC_HIGH16), &newops);
    ofst_tn = Gen_Symbol_TN(base_sym, base_ofst, TN_RELOC_LOW16);
  }
  else if (Guaranteed_Small_GOT) {
    TN *tmp2;
    if (! ST_is_export_local(base_sym) &&
	! ISA_LC_Value_In_Class(base_ofst, LC_simm16)) { // use %got_page and %got_offset
      tmp2 = Build_TN_Of_Mtype (Pointer_Mtype); //to store loaded sym addr 
      Expand_Load (
	    // load is of address, not of result type
	    OPCODE_make_signed_op(OPR_LDID, Pointer_Mtype, Pointer_Mtype, FALSE),
	    tmp2, GP_TN, Gen_Symbol_TN(base_sym, base_ofst, TN_RELOC_GOT_PAGE), 
	    &newops);
      // got address should not alias
      Set_OP_no_alias(OPS_last(&newops));
      base_tn = tmp2;
      // add offset to address
      ofst_tn = Gen_Symbol_TN(base_sym, base_ofst, TN_RELOC_GOT_OFST);
    }
    else {
      // bug 14982, sicortex 10068: generate %call16 for external function
      // calls, unless -OPT:got_call, which enables using %got_disp.
      mUINT8 relocs = (stmt_opr != OPERATOR_UNKNOWN &&
                       OPERATOR_is_call(stmt_opr) && 
                       ST_class(base_sym) == CLASS_FUNC &&
                       !ST_is_export_local(base_sym)) ? TN_RELOC_CALL16 :
                                                        TN_RELOC_GOT_DISP;

      if (Enable_GOT_Call_Conversion)
        relocs = TN_RELOC_GOT_DISP;
      if (is_lda && OFFSET_LO(base_ofst) == 0) {
        // want to stop at address
        tmp2 = tn;
        is_lda = FALSE;	// to save the additional add
      }
      else tmp2 = Build_TN_Of_Mtype (Pointer_Mtype); //to store loaded sym addr 
      Expand_Load (
	    // load is of address, not of result type
	    OPCODE_make_signed_op(OPR_LDID, Pointer_Mtype, Pointer_Mtype, FALSE),
	    tmp2, GP_TN, 
	    Gen_Symbol_TN(base_sym, OFFSET_HI(base_ofst), relocs), 
	    &newops);
      // got address should not alias
      Set_OP_no_alias(OPS_last(&newops));
      base_tn = tmp2;
      // add offset to address
      ofst_tn = Gen_Literal_TN(OFFSET_LO(base_ofst), 4);
    }
  }
  else {
    FmtAssert(FALSE, ("NYI: Exp_Ldst"));
  }
  
  if (is_store) {
    if (variant == 0)
      Expand_Store (OPCODE_desc(opcode), tn, base_tn, ofst_tn, &newops);
    else 
      Expand_Misaligned_Store (OPCODE_desc(opcode), tn, 
			       base_tn, ofst_tn, variant, &newops);
  }
  else if (is_load) {
    if (variant == 0)
      Expand_Load (opcode, tn, base_tn, ofst_tn, &newops);
    else 
      Expand_Misaligned_Load (opcode, tn, 
			      base_tn, ofst_tn, variant, &newops);
  }
  else if (is_lda) {
    if (TN_is_symbol(ofst_tn)) 
      Build_OP(Use_32_Bit_Pointers?TOP_addiu:TOP_daddiu, 
		      tn, base_tn, ofst_tn, &newops);
    else Expand_Add (tn, ofst_tn, base_tn, OPCODE_rtype(opcode), &newops);
  }
  
  FOR_ALL_OPS_OPs (&newops, op) {
    if (is_load && ST_is_constant(sym) && OP_load(op)) {
      // If we expanded a load of a constant, 
      // nothing else can alias with the loads 
      // we have generated.
      Set_OP_no_alias(op);
    }
    if (Trace_Exp2) {
      fprintf(TFile, "exp_ldst into "); Print_OP (op);
    }
  }
  /* Add the new OPs to the end of the list passed in */
  OPS_Append_Ops(ops, &newops);
}

void Exp_Lda ( 
  TYPE_ID mtype, 
  TN *tgt_tn, 
  ST *sym, 
  INT64 ofst, 
  OPERATOR call_opr,
  OPS *ops)
{
  OPCODE opcode = OPCODE_make_op(OPR_LDA, mtype, MTYPE_V);
  Exp_Ldst (opcode, tgt_tn, sym, ofst, 
	(call_opr == OPR_ICALL),
	FALSE, FALSE, ops, 0, call_opr);
}

void
Exp_Load (
  TYPE_ID rtype, 
  TYPE_ID desc, 
  TN *tgt_tn, 
  ST *sym, 
  INT64 ofst, 
  OPS *ops, 
  VARIANT variant)
{
  OPCODE opcode = OPCODE_make_op (OPR_LDID, rtype, desc);
  Exp_Ldst (opcode, tgt_tn, sym, ofst, FALSE, FALSE, TRUE, ops, variant);
}

void
Exp_Store (
  TYPE_ID mtype, 
  TN *src_tn, 
  ST *sym, 
  INT64 ofst, 
  OPS *ops, 
  VARIANT variant)
{
  OPCODE opcode = OPCODE_make_op(OPR_STID, MTYPE_V, mtype);
  Exp_Ldst (opcode, src_tn, sym, ofst, FALSE, TRUE, FALSE, ops, variant);
}

static ISA_ENUM_CLASS_VALUE
Pick_Prefetch_Hint (VARIANT variant)
{
  UINT32 pf_flags = V_pf_flags(variant);
  if (Is_Target_twc9a()) {
    if (PF_GET_READ(pf_flags)) {
      if (PF_GET_NON_TEMPORAL(pf_flags))
        return ECV_pfhint_load_streamed;
      else
        return ECV_pfhint_load;
    }
    else {
      if (PF_GET_NON_TEMPORAL(pf_flags))
        return ECV_pfhint_store_streamed;
      else
        return ECV_pfhint_store;
    }
  } else {
    if (PF_GET_READ(pf_flags)) {
      return ECV_pfhint_load_streamed;
    }
    else {
      return ECV_pfhint_store_streamed;
    }
  }
}

void Exp_Prefetch (TOP opc, TN* src1, TN* src2, VARIANT variant, OPS* ops)
{
  ISA_ENUM_CLASS_VALUE pfhint;
  FmtAssert(opc == TOP_UNDEFINED,
            ("Prefetch opcode should be selected in Exp_Prefetch"));
  pfhint = Pick_Prefetch_Hint(variant);
  Build_OP(TOP_pref, Gen_Enum_TN(pfhint), src1, src2, ops);
}

/* ======================================================================
 * Exp_Extract_Bits
 * ======================================================================*/
void
Exp_Extract_Bits (TYPE_ID rtype, TYPE_ID desc, UINT bit_offset, UINT bit_size,
		  TN *tgt_tn, TN *src_tn, OPS *ops)
{
  TN *tmp1_tn = Build_TN_Like (tgt_tn);
  UINT pos = (Target_Byte_Sex == BIG_ENDIAN || CG_emit_non_gas_syntax)
	     ? MTYPE_bit_size(desc)-bit_offset-bit_size : bit_offset;
  if (pos == 0 && bit_size <= 16 && ! MTYPE_signed(rtype)) {
    Build_OP(TOP_andi, tgt_tn, src_tn, 
	     Gen_Literal_TN((1 << bit_size)-1, 4), ops);
    return;
  }

  TOP left_shift_op = TOP_sll;
  INT left_shift_amt = MTYPE_bit_size(rtype) - pos - bit_size;
  if (MTYPE_is_size_double(rtype)) {
    left_shift_op = TOP_dsll;
    if (left_shift_amt > 31) {
      left_shift_op = TOP_dsll32;
      left_shift_amt -= 32;
    }
  }
  Build_OP(left_shift_op, tmp1_tn, src_tn, Gen_Literal_TN(left_shift_amt, 4),
	   ops);
  TOP right_shift_op = TOP_sra;
  INT right_shift_amt = MTYPE_bit_size(rtype) - bit_size;
  if (MTYPE_is_size_double(rtype)) {
    right_shift_op = TOP_dsra;
    if (right_shift_amt > 31) {
      right_shift_op = TOP_dsra32;
      right_shift_amt -= 32;
    }
  }
  if (! MTYPE_signed(rtype)) {
    if (right_shift_op == TOP_sra) 
      right_shift_op = TOP_srl;
    else if (right_shift_op == TOP_dsra)
      right_shift_op = TOP_dsrl;
    else right_shift_op = TOP_dsrl32;
  }
  Build_OP(right_shift_op, tgt_tn, tmp1_tn, Gen_Literal_TN(right_shift_amt, 4), 
	   ops);
}

/* ======================================================================
 * Exp_Deposit_Bits - deposit src2_tn into a field of src1_tn returning
 * the result in tgt_tn.
 * ======================================================================*/
void
Exp_Deposit_Bits (TYPE_ID rtype, TYPE_ID desc, UINT bit_offset, UINT bit_size,
		  TN *tgt_tn, TN *src1_tn, TN *src2_tn, OPS *ops)
{
  FmtAssert(bit_size != 0, ("size of bit field cannot be 0"));

  UINT targ_bit_offset = bit_offset;
  if (Target_Byte_Sex == BIG_ENDIAN) {
    targ_bit_offset = MTYPE_bit_size(desc) - bit_offset - bit_size;
  }
  TN *tmp1_tn = Build_TN_Like (src1_tn);
  TOP shift_op = TOP_srl;
  INT shift_amt = targ_bit_offset;
  if (MTYPE_is_size_double(rtype)) {
    shift_op = TOP_dsrl;
    if (shift_amt > 31) {
      shift_op = TOP_dsrl32;
      shift_amt -= 32;
    }
  }
  Build_OP(shift_op, tmp1_tn, src1_tn, Gen_Literal_TN(shift_amt,4), ops);
  Build_OP(TOP_xor, tmp1_tn, tmp1_tn, src2_tn, ops);
  shift_op = TOP_sll;
  shift_amt = MTYPE_bit_size(rtype) - bit_size;
  if (MTYPE_is_size_double(rtype)) {
    shift_op = TOP_dsll;
    if (shift_amt > 31) {
      shift_op = TOP_dsll32;
      shift_amt -= 32;
    }
  }
  Build_OP(shift_op, tmp1_tn, tmp1_tn, Gen_Literal_TN(shift_amt, 4), ops);
  shift_op = TOP_srl;
  shift_amt = MTYPE_bit_size(rtype) - bit_size - targ_bit_offset;
  if (MTYPE_is_size_double(rtype)) {
    shift_op = TOP_dsrl;
    if (shift_amt > 31) {
      shift_op = TOP_dsrl32;
      shift_amt -= 32;
    }
  }
  Build_OP(shift_op, tmp1_tn, tmp1_tn, Gen_Literal_TN(shift_amt, 4), ops);
  Build_OP(TOP_xor, tgt_tn, src1_tn, tmp1_tn, ops);
}

void 
Expand_Lda_Label (TN *dest, TN *lab, OPS *ops)
{
  Set_TN_is_reloc_got_disp(lab);
  if (Use_32_Bit_Pointers)
    Build_OP(TOP_lw, dest, GP_TN, lab, ops);
  else
    Build_OP(TOP_ld, dest, GP_TN, lab, ops);
}
