/*
 * Copyright (C) 2007, 2008, 2009 PathScale, LLC.  All Rights Reserved.
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
// Group TOPS with similar Printing format together. 
/////////////////////////////////////////////////////////
// The instructions are listed by category. The different categories of
// instructions are:
//
//   1. Binary Operators
//   2. Jump, Branch and Trap operators
//   3. Multiply/Divide operators
//   4. Load/Store machine operators
//   5. FPU arithmetic/convert operators
//   6. FPU branch operators
//   7. FPU comparison operators
//   8. Coprocessor1 (FP) data movement operations
//   9. MIPS IV only arithmetic/cond_move FPU operations
//  10. Compiler IR and fictional operators
//
// Within each Print_Type instructions are listed in alphabetical order and
// as shown in the ISA manual
/////////////////////////////////////
//
//  $Revision: 1.5 $
//  $Date: 05/03/02 15:27:55-08:00 $
//  $Author: bos@eng-24.internal.keyresearch.com $
//  $Source: common/targ_info/isa/mips/SCCS/s.isa_print.cxx $

#include <stddef.h>
#include <string.h>
#include "topcode.h"
#include "isa_print_gen.h"

// Multiple topcodes map to the same assembly name. To disambiguate the 
// topcodes, we append a suffix to the basename. By convention, the 
// suffix starts with an underscore. To get the assembly name we strip off
// the suffix.

main()
{
  ISA_Print_Begin("MIPS");

  Define_Macro("END_GROUP", ";");		// end-of-group marker
  Define_Macro("PREDICATE", "(%s)");		// predicate operand format
  Define_Macro("BEGIN_BUNDLE", "");	// bundle introducer
  Define_Macro("END_BUNDLE", ";");		// bundle terminator

  Set_AsmName_Func(NULL);

  /* One result / two operands */
  ISA_PRINT_TYPE ropop =  ISA_Print_Type_Create("ropop", "%s %s,%s,%s");
  Name();
  Result(0);
  Operand(0);
  Operand(1);
  Instruction_Print_Group(ropop,
	  		  TOP_add,
			  TOP_addu,
			  TOP_sub,
			  TOP_subu,
			  TOP_dadd,
			  TOP_daddu,
			  TOP_dsub,
			  TOP_dsubu,
			  TOP_and,
			  TOP_mul,
			  TOP_nor,
			  TOP_or,
			  TOP_xor,
			  TOP_sllv,
			  TOP_srav,
			  TOP_srlv,
			  TOP_dsllv,
			  TOP_dsrav,
			  TOP_dsrlv,
			  TOP_slt,
			  TOP_sltu,
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
			  TOP_addi,
			  TOP_addiu,
			  TOP_slti,
			  TOP_sltiu,
			  TOP_daddi,
			  TOP_daddiu,
			  TOP_andi,
			  TOP_ori,
			  TOP_xori,
			  TOP_sll,
			  TOP_sra,
			  TOP_srl,
			  TOP_dsll,
			  TOP_dsll32,
			  TOP_dsra,
			  TOP_dsra32,
			  TOP_dsrl,
			  TOP_dsrl32,
			  TOP_movf,
			  TOP_movn,
			  TOP_movt,
			  TOP_movz,
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
			  TOP_movf_s,
			  TOP_movf_d,
			  TOP_movf_ps,
			  TOP_movn_s,
			  TOP_movn_d,
			  TOP_movn_ps,
			  TOP_movt_s,
			  TOP_movt_d,
			  TOP_movt_ps,
			  TOP_movz_s,
			  TOP_movz_d,
			  TOP_movz_ps,
			  TOP_drotr,
			  TOP_drotr32,
			  TOP_drotrv,
			  TOP_rotr,
			  TOP_rotrv,
			  TOP_cvt_ps_s,
			  TOP_pll_ps,
			  TOP_plu_ps,
			  TOP_pul_ps,
			  TOP_puu_ps,
			  TOP_UNDEFINED);

  /* No result / one operand */
  ISA_PRINT_TYPE op =  ISA_Print_Type_Create("op", "%s %s");
  Name();
  Operand(0);
  Instruction_Print_Group(op,
			  TOP_j,
			  TOP_jal,
			  TOP_jalr,
			  TOP_jr,
			  TOP_mthi,
			  TOP_mtlo,
			  TOP_UNDEFINED);

  /* No result / two operands */
  ISA_PRINT_TYPE opop =  ISA_Print_Type_Create("opop", "%s %s,%s");
  Name();
  Operand(0);
  Operand(1);
  Instruction_Print_Group(opop,
			  TOP_mult,
			  TOP_multu,
			  TOP_dmult,
			  TOP_dmultu,
			  TOP_bgez,
			  TOP_bgezal,
			  TOP_bgtz,
			  TOP_blez,
			  TOP_bltz,
			  TOP_bltzal,
			  TOP_bc1f,
			  TOP_bc1t,
			  TOP_UNDEFINED);

  /* No result / two operands extra $0 operand for divide */
  ISA_PRINT_TYPE divide =  ISA_Print_Type_Create("divide", "%s $0,%s,%s");
  Name();
  Operand(0);
  Operand(1);
  Instruction_Print_Group(divide,
			  TOP_div,
			  TOP_divu,
			  TOP_ddiv,
			  TOP_ddivu,
			  TOP_UNDEFINED);

  /* One result / no operand */
  ISA_PRINT_TYPE r =  ISA_Print_Type_Create("r", "%s %s");
  Name();
  Result(0);
  Instruction_Print_Group(r,
			  TOP_mfhi,
			  TOP_mflo,
			  TOP_UNDEFINED);

  /* One result / one operand */
  ISA_PRINT_TYPE rop =  ISA_Print_Type_Create("rop", "%s %s,%s");
  Name();
  Result(0);
  Operand(0);
  Instruction_Print_Group(rop,
			  TOP_lui,
			  TOP_clo,
			  TOP_clz,
			  TOP_dclo,
			  TOP_dclz,
			  TOP_neg_s,
			  TOP_neg_d,
			  TOP_neg_ps,
			  TOP_sqrt_s,
			  TOP_sqrt_d,
			  TOP_abs_s,
			  TOP_abs_d,
			  TOP_abs_ps,
			  TOP_recip_s,
			  TOP_recip_d,
			  TOP_rsqrt_s,
			  TOP_rsqrt_d,
			  TOP_mov_s,
			  TOP_mov_d,
			  TOP_mov_ps,
			  TOP_mfc1,
			  TOP_dmfc1,
			  TOP_cvt_d_s,
			  TOP_cvt_d_w,
			  TOP_cvt_d_l,
			  TOP_cvt_l_s,
			  TOP_cvt_l_d,
			  TOP_cvt_s_d,
			  TOP_cvt_s_w,
			  TOP_cvt_s_l,
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
			  TOP_dsbh,
			  TOP_dshd,
			  TOP_seb,
			  TOP_seh,
			  TOP_wsbh,
			  TOP_cvt_s_pl,
			  TOP_cvt_s_pu,
			  TOP_UNDEFINED);

  /* One operand / one result */
  ISA_PRINT_TYPE opr =  ISA_Print_Type_Create("opr", "%s %s,%s");
  Name();
  Operand(0);
  Result(0);
  Instruction_Print_Group(opr,
			  TOP_mtc1,
			  TOP_dmtc1,
			  TOP_UNDEFINED);

  /* One result / three operands */
  ISA_PRINT_TYPE ropopop =  ISA_Print_Type_Create("ropopop", "%s %s,%s,%s,%s");
  Name();
  Result(0);
  Operand(0);
  Operand(1);
  Operand(2);
  Instruction_Print_Group(ropopop,
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
			  TOP_dext,
			  TOP_dextm,
			  TOP_dextu,
			  TOP_dins,
			  TOP_dinsm,
			  TOP_dinsu,
			  TOP_ext,
			  TOP_ins,
			  TOP_alnv_ps,
			  TOP_UNDEFINED);

  /* No result / three operands */
  ISA_PRINT_TYPE opopop =  ISA_Print_Type_Create("opopop", "%s %s,%s,%s");
  Name();
  Operand(0);
  Operand(1);
  Operand(2);
  Instruction_Print_Group(opopop,
			  TOP_beq,
			  TOP_bne,
			  TOP_teq,
			  TOP_teqi,
			  TOP_tge,
			  TOP_tgei,
			  TOP_tgeiu,
			  TOP_tgeu,
			  TOP_tlt,
			  TOP_tlti,
			  TOP_tltiu,
			  TOP_tltu,
			  TOP_tne,
			  TOP_tnei,
			  TOP_UNDEFINED);

  /* regular load */
  ISA_PRINT_TYPE load =  ISA_Print_Type_Create("load", "%s %s,%s(%s)");
  Name();
  Result(0);
  Operand(1);
  Operand(0);
  Instruction_Print_Group(load,
			  TOP_lb,
			  TOP_lbu,
			  TOP_lh,
			  TOP_lhu,
			  TOP_lw,
			  TOP_lwl,
			  TOP_lwr,
			  TOP_ll,
			  TOP_lwu,
			  TOP_ld,
			  TOP_ldl,
			  TOP_ldr,
			  TOP_lld,
			  TOP_lwc1,
			  TOP_ldc1,
			  TOP_lwxc1,
			  TOP_ldxc1,
			  TOP_UNDEFINED);

  /* prefetch */
  ISA_PRINT_TYPE prefetch =  ISA_Print_Type_Create("prefetch", "%s %s,%s(%s)");
  Name();
  Operand(0);
  Operand(2);
  Operand(1);
  Instruction_Print_Group(prefetch,
			  TOP_pref,
			  TOP_prefx,
			  TOP_UNDEFINED);

  /* regular store */
  ISA_PRINT_TYPE store =  ISA_Print_Type_Create("store", "%s %s,%s(%s)");
  Name();
  Operand(0);
  Operand(2);
  Operand(1);
  Instruction_Print_Group(store,
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
			  TOP_swc1,
			  TOP_sdc1,
			  TOP_swxc1,
			  TOP_sdxc1,
			  TOP_UNDEFINED);

  /* No results / no operands TODO */
  ISA_PRINT_TYPE no_rop = ISA_Print_Type_Create("no_rop", "%s");
  Name();
  Instruction_Print_Group(no_rop,
	  		  TOP_nop,
			  TOP_sync,
			  TOP_break,
			  TOP_syscall,
			  TOP_cfc1,
			  TOP_ctc1,
			  TOP_UNDEFINED);

  ISA_Print_End();
}
