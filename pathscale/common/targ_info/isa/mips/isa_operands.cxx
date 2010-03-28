/*
 * Copyright (C) 2007, 2008, 2009 PathScale, LLC.  All Rights Reserved.
 */

/*
 *  Copyright (C) 2007. QLogic Corporation. All Rights Reserved.
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

//
// Generate an ISA containing and group TOPS with similar operands/results 
// format.
/////////////////////////////////////////////////////////
// The instructions are listed below in the order as shown below. 
//
//   1. Real and pseudo instructions
//   2. Simulated instructions.
//
// Within each category, the instructions are arranged roughly in order
// of increasing numbers of operands.
/////////////////////////////////////
//
//  $Revision: 1.8 $
//  $Date: 05/03/02 15:27:55-08:00 $
//  $Author: bos@eng-24.internal.keyresearch.com $
//  $Source: common/targ_info/isa/mips/SCCS/s.isa_operands.cxx $

#include "topcode.h"
#include "isa_operands_gen.h"

main()
{
  ISA_Operands_Begin("MIPS");

  /* Literals... */

  OPERAND_VALUE_TYPE simm16, uimm16, uimm5, pcrel16, pc26, len5;

  simm16 = ISA_Lit_Opnd_Type_Create("simm16", 16, SIGNED, LC_simm16);
  uimm16 = ISA_Lit_Opnd_Type_Create("uimm16", 16, UNSIGNED, LC_uimm16);
  uimm5 = ISA_Lit_Opnd_Type_Create("uimm5", 5, UNSIGNED, LC_uimm5);
  pcrel16 = ISA_Lit_Opnd_Type_Create("pcrel16", 16, SIGNED, LC_pcrel16);
  pc26 = ISA_Lit_Opnd_Type_Create("pc26", 26, UNSIGNED, LC_pc26);
  len5 = ISA_Lit_Opnd_Type_Create("len5", 6, UNSIGNED, LC_len5);

  /* Registers... */

  OPERAND_VALUE_TYPE int64, fp64, fcc, hilo;

  int64 = ISA_Reg_Opnd_Type_Create("int64", ISA_REGISTER_CLASS_integer,
				ISA_REGISTER_SUBCLASS_UNDEFINED,
				64, SIGNED, INVALID);
  fp64 = ISA_Reg_Opnd_Type_Create("fp64", ISA_REGISTER_CLASS_float,
				ISA_REGISTER_SUBCLASS_UNDEFINED,
				64, SIGNED, INVALID);
  fcc = ISA_Reg_Opnd_Type_Create("fcc", ISA_REGISTER_CLASS_fcc,
				ISA_REGISTER_SUBCLASS_UNDEFINED,
				1, UNSIGNED, INVALID);
  hilo = ISA_Reg_Opnd_Type_Create("hilo", ISA_REGISTER_CLASS_hilo,
				ISA_REGISTER_SUBCLASS_UNDEFINED,
				64, SIGNED, INVALID);

  /* Enums */
  OPERAND_VALUE_TYPE pfhint;
  pfhint = ISA_Enum_Opnd_Type_Create("pfhint", 8, UNSIGNED, EC_pfhint);

  /* Operand uses... */

  OPERAND_USE_TYPE
	  predicate,	// a qualifying predicate
	  base,		// a base address (for memory insts)
	  index,	// an index address (for memory insts)
	  offset,	// an offset added to a base (implies immed val)
	  target,	// the target of a branch
	  storeval,	// value to be stored
	  opnd1,	// first/left operand of an alu operator
	  opnd2,	// second/right operand of an alu operator
	  opnd3,	// third operand of an alu operator
	  maddend,	// addend/subtrahend operand of a madd
          same_res;     // operand has same register as result

  predicate  = Create_Operand_Use("predicate");
  base       = Create_Operand_Use("base");
  index      = Create_Operand_Use("index");
  offset     = Create_Operand_Use("offset");
  target     = Create_Operand_Use("target");
  storeval   = Create_Operand_Use("storeval");
  opnd1      = Create_Operand_Use("opnd1");
  opnd2      = Create_Operand_Use("opnd2");
  opnd3      = Create_Operand_Use("opnd3");
  maddend    = Create_Operand_Use("maddend");
  same_res   = Create_Operand_Use("same_res");

  Instruction_Group("no operand",
		    TOP_nop,
		    TOP_noop,
		    TOP_sync,
		    TOP_break,
		    TOP_syscall,
		    TOP_asm,
		    TOP_bwd_bar,
		    TOP_fwd_bar,
                    TOP_swp_start,
                    TOP_swp_stop,
		    TOP_UNDEFINED);

  Instruction_Group("load imm",
		    TOP_lui,
		    TOP_UNDEFINED);
  Result(0, int64);
  Operand(0, uimm16, opnd2);

  Instruction_Group("sign extend and count bits",
		    TOP_sext,
		    TOP_clo,
		    TOP_clz,
		    TOP_dclo,
		    TOP_dclz,
		    TOP_UNDEFINED);
  Result(0, int64);
  Operand(0, int64, opnd1);

  Instruction_Group("int arithmetic",
		    TOP_add,
		    TOP_addu,
		    TOP_dadd,
		    TOP_daddu,
		    TOP_slt,
		    TOP_sltu,
		    TOP_sub,
		    TOP_subu,
		    TOP_dsub,
		    TOP_dsubu,
		    TOP_and,
		    TOP_nor,
		    TOP_or,
		    TOP_xor,
		    TOP_UNDEFINED);
  Result(0, int64);
  Operand(0, int64, opnd1);
  Operand(1, int64, opnd2);

  Instruction_Group("int arithmetic with simm",
		    TOP_addi,
		    TOP_daddi,
		    TOP_addiu,
		    TOP_daddiu,
		    TOP_slti,
		    TOP_sltiu,
		    TOP_UNDEFINED);
  Result(0, int64);
  Operand(0, int64, opnd1);
  Operand(1, simm16, opnd2);

  Instruction_Group("int arithmetic with uimm",
		    TOP_andi,
		    TOP_ori,
		    TOP_xori,
		    TOP_UNDEFINED);
  Result(0, int64);
  Operand(0, int64, opnd1);
  Operand(1, uimm16, opnd2);

  Instruction_Group("int load",
		    TOP_lb,
		    TOP_lbu,
		    TOP_lh,
		    TOP_lhu,
		    TOP_lw,
		    TOP_ll,
		    TOP_lwu,
		    TOP_ld,
		    TOP_lld,
		    TOP_UNDEFINED);
  Result(0, int64);
  Operand(0, int64, base);
  Operand(1, simm16, offset);
  
  Instruction_Group("int unaligned load",
		    TOP_lwl,
		    TOP_lwr,
		    TOP_ldl,
		    TOP_ldr,
		    TOP_UNDEFINED);
  Result(0, int64);
  Operand(0, int64, base);
  Operand(1, simm16, offset);
  Operand(2, int64, same_res);

  Instruction_Group("int store",
		    TOP_sb,
		    TOP_sh,
		    TOP_sw,
		    TOP_swl,
		    TOP_swr,
		    TOP_sc,
		    TOP_sd,
		    TOP_sdl,
		    TOP_sdr,
		    TOP_scd,
		    TOP_UNDEFINED);
  Operand(0, int64, storeval);
  Operand(1, int64, base);
  Operand(2, simm16, offset);

  Instruction_Group("prefetch",
		    TOP_pref,
		    TOP_UNDEFINED);
  Operand(0, pfhint);
  Operand(1, int64, base);
  Operand(2, simm16, offset);

  Instruction_Group("prefetch indexed",
		    TOP_prefx,
		    TOP_UNDEFINED);
  Operand(0, pfhint);
  Operand(1, int64, base);
  Operand(2, int64, index);

  Instruction_Group("int mul",
                    TOP_mul,
                    TOP_UNDEFINED);
  Result(0, int64);
  Result(1, hilo);
  Operand(0, int64, opnd1);
  Operand(1, int64, opnd2);

  Instruction_Group("int mult/div",
		    TOP_div,
		    TOP_divu,
		    TOP_mult,
		    TOP_multu,
		    TOP_ddiv,
		    TOP_ddivu,
		    TOP_dmult,
		    TOP_dmultu,
		    TOP_UNDEFINED);
  Result(0, hilo);
  Operand(0, int64, opnd1);
  Operand(1, int64, opnd2);

  Instruction_Group("move from hi/lo",
		    TOP_mfhi,
		    TOP_mflo,
		    TOP_UNDEFINED);
  Result(0, int64);
  Operand(0, hilo);

  Instruction_Group("move to hi/lo",
		    TOP_mthi,
		    TOP_mtlo,
		    TOP_UNDEFINED);
  Operand(0, int64, opnd1);
  Result(0, hilo);

  Instruction_Group("float compare",
		    TOP_c_f_s,
		    TOP_c_f_d,
		    TOP_c_f_ps,
		    TOP_c_t_s,
		    TOP_c_t_d,
		    TOP_c_t_ps,
		    TOP_c_un_s,
		    TOP_c_un_d,
		    TOP_c_un_ps,
		    TOP_c_or_s,
		    TOP_c_or_d,
		    TOP_c_or_ps,
		    TOP_c_eq_s,
		    TOP_c_eq_d,
		    TOP_c_eq_ps,
		    TOP_c_neq_s,
		    TOP_c_neq_d,
		    TOP_c_neq_ps,
		    TOP_c_ueq_s,
		    TOP_c_ueq_d,
		    TOP_c_ueq_ps,
		    TOP_c_ogl_s,
		    TOP_c_ogl_d,
		    TOP_c_ogl_ps,
		    TOP_c_olt_s,
		    TOP_c_olt_d,
		    TOP_c_olt_ps,
		    TOP_c_uge_s,
		    TOP_c_uge_d,
		    TOP_c_uge_ps,
		    TOP_c_ult_s,
		    TOP_c_ult_d,
		    TOP_c_ult_ps,
		    TOP_c_oge_s,
		    TOP_c_oge_d,
		    TOP_c_oge_ps,
		    TOP_c_ole_s,
		    TOP_c_ole_d,
		    TOP_c_ole_ps,
		    TOP_c_ugt_s,
		    TOP_c_ugt_d,
		    TOP_c_ugt_ps,
		    TOP_c_ule_s,
		    TOP_c_ule_d,
		    TOP_c_ule_ps,
		    TOP_c_ogt_s,
		    TOP_c_ogt_d,
		    TOP_c_ogt_ps,
		    TOP_c_sf_s,
		    TOP_c_sf_d,
		    TOP_c_sf_ps,
		    TOP_c_st_s,
		    TOP_c_st_d,
		    TOP_c_st_ps,
		    TOP_c_ngle_s,
		    TOP_c_ngle_d,
		    TOP_c_ngle_ps,
		    TOP_c_gle_s,
		    TOP_c_gle_d,
		    TOP_c_gle_ps,
		    TOP_c_seq_s,
		    TOP_c_seq_d,
		    TOP_c_seq_ps,
		    TOP_c_sne_s,
		    TOP_c_sne_d,
		    TOP_c_sne_ps,
		    TOP_c_ngl_s,
		    TOP_c_ngl_d,
		    TOP_c_ngl_ps,
		    TOP_c_gl_s,
		    TOP_c_gl_d,
		    TOP_c_gl_ps,
		    TOP_c_lt_s,
		    TOP_c_lt_d,
		    TOP_c_lt_ps,
		    TOP_c_nlt_s,
		    TOP_c_nlt_d,
		    TOP_c_nlt_ps,
		    TOP_c_nge_s,
		    TOP_c_nge_d,
		    TOP_c_nge_ps,
		    TOP_c_ge_s,
		    TOP_c_ge_d,
		    TOP_c_ge_ps,
		    TOP_c_le_s,
		    TOP_c_le_d,
		    TOP_c_le_ps,
		    TOP_c_nle_s,
		    TOP_c_nle_d,
		    TOP_c_nle_ps,
		    TOP_c_ngt_s,
		    TOP_c_ngt_d,
		    TOP_c_ngt_ps,
		    TOP_c_gt_s,
		    TOP_c_gt_d,
		    TOP_c_gt_ps,
		    TOP_UNDEFINED);
  Result(0,  fcc);
  Operand(0, fp64, opnd1);
  Operand(1, fp64, opnd2);

  Instruction_Group("int movf/movt",
		    TOP_movf,
		    TOP_movt,
		    TOP_UNDEFINED);
  Result(0,  int64);
  Operand(0, int64, opnd1);
  Operand(1, fcc,   opnd2);
  Operand(2, int64, same_res);

  Instruction_Group("int movn/movz",
		    TOP_movn,
		    TOP_movz,
		    TOP_UNDEFINED);
  Result(0, int64);
  Operand(0, int64, opnd1);
  Operand(1, int64, opnd2);
  Operand(2, int64, same_res);

  Instruction_Group("float moves",
		    TOP_mov_s,
		    TOP_mov_d,
		    TOP_mov_ps,
		    TOP_UNDEFINED);
  Result(0, fp64);
  Operand(0, fp64, opnd1);

  Instruction_Group("float movf/movt",
		    TOP_movf_s,
		    TOP_movf_d,
		    TOP_movf_ps,
		    TOP_movt_s,
		    TOP_movt_d,
		    TOP_movt_ps,
		    TOP_UNDEFINED);
  Result(0, fp64);
  Operand(0, fp64, opnd1);
  Operand(1, fcc,  opnd2);
  Operand(2, fp64, same_res);

  Instruction_Group("float movn/movz",
		    TOP_movn_s,
		    TOP_movn_d,
		    TOP_movn_ps,
		    TOP_movz_s,
		    TOP_movz_d,
		    TOP_movz_ps,
		    TOP_UNDEFINED);
  Result(0, fp64);
  Operand(0, fp64,  opnd1);
  Operand(1, int64, opnd2);
  Operand(2, fp64,  same_res);

  Instruction_Group("shifts",
		    TOP_sll,
		    TOP_dsll,
		    TOP_dsll32,
		    TOP_sra,
		    TOP_dsra,
		    TOP_dsra32,
		    TOP_srl,
		    TOP_dsrl,
		    TOP_dsrl32,
		    TOP_UNDEFINED);
  Result(0, int64);
  Operand(0, int64, opnd1);
  Operand(1, uimm5, opnd2);

  Instruction_Group("variable shifts",
		    TOP_sllv,
		    TOP_srav,
		    TOP_srlv,
		    TOP_dsllv,
		    TOP_dsrav,
		    TOP_dsrlv,
		    TOP_UNDEFINED);
  Result(0, int64);
  Operand(0, int64, opnd1);
  Operand(1, int64, opnd2);

  Instruction_Group("float load",
		    TOP_lwc1,
		    TOP_ldc1,
		    TOP_UNDEFINED);
  Result(0, fp64);
  Operand(0, int64, base);
  Operand(1, simm16, offset);

  Instruction_Group("float store",
		    TOP_swc1,
		    TOP_sdc1,
		    TOP_UNDEFINED);
  Operand(0, fp64, storeval);
  Operand(1, int64, base);
  Operand(2, simm16, offset);

  Instruction_Group("float load indexed",
		    TOP_lwxc1,
		    TOP_ldxc1,
		    TOP_UNDEFINED);
  Result(0, fp64);
  Operand(0, int64, base);
  Operand(1, int64, index);

  Instruction_Group("float store indexed",
		    TOP_swxc1,
		    TOP_sdxc1,
		    TOP_UNDEFINED);
  Operand(0, fp64, storeval);
  Operand(1, int64, base);
  Operand(2, int64, index);

  Instruction_Group("float arithmetic",
		    TOP_add_s,
		    TOP_add_d,
		    TOP_add_ps,
		    TOP_div_s,
		    TOP_div_d,
		    TOP_mul_s,
		    TOP_mul_d,
		    TOP_mul_ps,
		    TOP_sub_s,
		    TOP_sub_d,
		    TOP_sub_ps,
		    TOP_UNDEFINED);
  Result(0, fp64);
  Operand(0, fp64, opnd1);
  Operand(1, fp64, opnd2);

  Instruction_Group("float unary arithmetic",
		    TOP_abs_s,
		    TOP_abs_d,
		    TOP_abs_ps,
		    TOP_neg_s,
		    TOP_neg_d,
		    TOP_neg_ps,
		    TOP_sqrt_s,
		    TOP_sqrt_d,
		    TOP_recip_s,
		    TOP_recip_d,
		    TOP_rsqrt_s,
		    TOP_rsqrt_d,
		    TOP_UNDEFINED);
  Result(0, fp64);
  Operand(0, fp64, opnd1);

  Instruction_Group("float madd",
		    TOP_madd_s,
		    TOP_madd_d,
		    TOP_madd_ps,
		    TOP_msub_s,
		    TOP_msub_d,
		    TOP_msub_ps,
		    TOP_nmadd_s,
		    TOP_nmadd_d,
		    TOP_nmadd_ps,
		    TOP_nmsub_s,
		    TOP_nmsub_d,
		    TOP_nmsub_ps,
		    TOP_UNDEFINED);
  Result(0, fp64);
  Operand(0, fp64, maddend);
  Operand(1, fp64, opnd1);
  Operand(2, fp64, opnd2);

  Instruction_Group("beq/bne",
		    TOP_beq,
		    TOP_bne,
		    TOP_UNDEFINED);
  Operand(0, int64, opnd1);
  Operand(1, int64, opnd2);
  Operand(2, pcrel16, target);

  Instruction_Group("branch compared with zero",
		    TOP_bgez,
		    TOP_bgtz,
		    TOP_blez,
		    TOP_bltz,
		    TOP_bgezal,
		    TOP_bltzal,
		    TOP_UNDEFINED);
  Operand(0, int64, opnd1);
  Operand(1, pcrel16, target);

  Instruction_Group("branch fcc",
		    TOP_bc1f,
		    TOP_bc1t,
		    TOP_UNDEFINED);
  Operand(0, fcc, opnd1);
  Operand(1, pcrel16, target);

  Instruction_Group("jump",
		    TOP_j,
		    TOP_UNDEFINED);
  Operand(0, pc26, target);

  Instruction_Group("jump register",
		    TOP_jr,
		    TOP_UNDEFINED);
  Operand(0, int64, target);

  Instruction_Group("jump-and-link",
		    TOP_jal,
		    TOP_UNDEFINED);
  Result(0, int64);
  Operand(0, pc26, target);

  Instruction_Group("jump-and-link register",
		    TOP_jalr,
		    TOP_UNDEFINED);
  Result(0, int64);
  Operand(0, int64, target);

  Instruction_Group("trap",
		    TOP_teq,
		    TOP_tge,
		    TOP_tgeu,
		    TOP_tlt,
		    TOP_tltu,
		    TOP_tne,
		    TOP_UNDEFINED);
  Operand(0, int64, opnd1);
  Operand(1, int64, opnd2);
  Operand(2, uimm16);

  Instruction_Group("trap immediate",
		    TOP_teqi,
		    TOP_tgei,
		    TOP_tgeiu,
		    TOP_tlti,
		    TOP_tltiu,
		    TOP_tnei,
		    TOP_UNDEFINED);
  Operand(0, int64, opnd1);
  Operand(1, simm16, opnd2);
  Operand(2, uimm16);

  Instruction_Group("mfc1",
		    TOP_cfc1,
		    TOP_mfc1,
		    TOP_dmfc1,
		    TOP_UNDEFINED);
  Result(0, int64);
  Operand(0, fp64, opnd1);

  Instruction_Group("mtc1",
		    TOP_ctc1,
		    TOP_mtc1,
		    TOP_dmtc1,
		    TOP_UNDEFINED);
  Result(0, fp64);
  Operand(0, int64, opnd1);

  Instruction_Group("cvt in float registers",
		    TOP_cvt_l_s,
		    TOP_cvt_l_d,
		    TOP_cvt_w_s,
		    TOP_cvt_w_d,
		    TOP_ceil_w_s,
		    TOP_ceil_w_d,
		    TOP_ceil_l_s,
		    TOP_ceil_l_d,
		    TOP_floor_w_s,
		    TOP_floor_w_d,
		    TOP_floor_l_s,
		    TOP_floor_l_d,
		    TOP_round_w_s,
		    TOP_round_w_d,
		    TOP_round_l_s,
		    TOP_round_l_d,
		    TOP_trunc_w_s,
		    TOP_trunc_w_d,
		    TOP_trunc_l_s,
		    TOP_trunc_l_d,
		    TOP_cvt_d_s,
		    TOP_cvt_s_d,
		    TOP_cvt_d_w,
		    TOP_cvt_d_l,
		    TOP_cvt_s_w,
		    TOP_cvt_s_l,
		    TOP_UNDEFINED);
  Result(0, fp64);
  Operand(0, fp64, opnd1);

  Instruction_Group("label",
		    TOP_label,
		    TOP_UNDEFINED);
  Operand(0, pcrel16);
  Relocatable(0);

  Instruction_Group("spadjust",
		    TOP_spadjust,
		    TOP_UNDEFINED);
  Result(0, int64);
  Operand(0, int64, opnd1);
  Operand(1, simm16, opnd2);

  Instruction_Group("mov_hilo",
		    TOP_mov_hilo,
		    TOP_UNDEFINED);
  Result(0, hilo);
  Operand(0, hilo, opnd1);

  Instruction_Group("mov_fcc",
                    TOP_mov_fcc,
                    TOP_UNDEFINED);
  Result(0, fcc);
  Operand(0, fcc, opnd1);

  Instruction_Group("intrncall",
		    TOP_intrncall,
		    TOP_UNDEFINED);
  Result(0, int64);
  Operand(0, uimm16);	// the intrinsic ID
  Operand(1, int64);
  Operand(2, int64);
  Operand(3, int64);

  Instruction_Group("O_noop_ifixup",
		    TOP_ifixup,
		    TOP_UNDEFINED);
  Result(0, int64);

  Instruction_Group("O_noop_ffixup",
		    TOP_ffixup,
		    TOP_dfixup,
		    TOP_UNDEFINED);
  Result(0, fp64);

  Instruction_Group("pregtn tuple",
		    TOP_begin_pregtn,
		    TOP_end_pregtn,
		    TOP_UNDEFINED);
  Result(0, int64);
  Operand(0, uimm16);

  // From here on, the convention is "O_res__opnd0_opnd1...", where __
  // separates opnds from result.

  Instruction_Group("O_int64__int64",
		    TOP_dsbh,
		    TOP_dshd,
		    TOP_seb,
		    TOP_seh,
		    TOP_wsbh,
		    TOP_UNDEFINED);
  Result(0, int64);
  Operand(0, int64, opnd1);

  Instruction_Group("O_int64__int64_int64",
		    TOP_drotrv,
		    TOP_rotrv,
		    TOP_UNDEFINED);
  Result(0, int64);
  Operand(0, int64, opnd1);
  Operand(1, int64, opnd2);

  Instruction_Group("O_int64__int64_uimm5",
		    TOP_drotr,
		    TOP_drotr32,
		    TOP_rotr,
		    TOP_UNDEFINED);
  Result(0, int64);
  Operand(0, int64, opnd1);
  Operand(1, uimm5, opnd2);

  Instruction_Group("O_int64__int64_uimm5_len5",
		    TOP_dext,
		    TOP_dextm,
		    TOP_dextu,
		    TOP_ext,
		    TOP_UNDEFINED);
  Result(0, int64);
  Operand(0, int64, opnd1);
  Operand(1, uimm5, opnd2);
  Operand(2, len5, opnd3);

  Instruction_Group("O_int64__int64_uimm5_len5_int64",
		    TOP_dins,
		    TOP_dinsm,
		    TOP_dinsu,
		    TOP_ins,
		    TOP_UNDEFINED);
  Result(0, int64);
  Operand(0, int64, opnd1);
  Operand(1, uimm5, opnd2);
  Operand(2, len5, opnd3);
  Operand(3, int64, same_res);

  Instruction_Group("O_fp64__fp64",
		    TOP_cvt_s_pl,
		    TOP_cvt_s_pu,
		    TOP_UNDEFINED);
  Result(0, fp64);
  Operand(0, fp64, opnd1);

  Instruction_Group("O_fp64__fp64_fp64",
		    TOP_cvt_ps_s,
		    TOP_pll_ps,
		    TOP_plu_ps,
		    TOP_pul_ps,
		    TOP_puu_ps,
		    TOP_UNDEFINED);
  Result(0, fp64);
  Operand(0, fp64, opnd1);
  Operand(1, fp64, opnd2);

  Instruction_Group("O_fp64__fp64_fp64_int64",
		    TOP_alnv_ps,
		    TOP_UNDEFINED);
  Result(0, fp64);
  Operand(0, fp64, opnd1);
  Operand(1, fp64, opnd2);
  Operand(2, int64, opnd3);

  ISA_Operands_End();
  return 0;
}
