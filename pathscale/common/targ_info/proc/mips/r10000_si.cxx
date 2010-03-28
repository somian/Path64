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


//  MIPS processor scheduling information
/////////////////////////////////////
//  
//  Description:
//
//  Generate a scheduling description of a MIPS processor
//  via the si_gen interface.
//
/////////////////////////////////////


//  $Revision: 1.4 $
//  $Date: 05/03/02 15:28:01-08:00 $
//  $Author: bos@eng-24.internal.keyresearch.com $
//  $Source: common/targ_info/proc/mips/SCCS/s.r10000_si.cxx $


#include "si_gen.h"
#include "targ_isa_subset.h"
#include "topcode.h"

static RESOURCE res_issue,
                res_memory,	// Memory unit 
                res_float,	// Float unit
                res_integer,	// Integer unit
                res_branch;	// Branch unit

int
main (int argc, char *argv[])
{
  Machine("r10000", ISA_SUBSET_MIPS4, argc, argv);

  res_issue = RESOURCE_Create("issue", 4);
  res_memory = RESOURCE_Create("memory", 1);
  res_integer = RESOURCE_Create("integer", 2);
  res_float = RESOURCE_Create("float", 2);
  res_branch = RESOURCE_Create("branch", 1);

  Instruction_Group("default",
		    TOP_add,
		    TOP_addi,
		    TOP_addiu,
		    TOP_addu,
		    TOP_slt,
		    TOP_slti,
		    TOP_sltiu,
		    TOP_sltu,
		    TOP_sub,
		    TOP_subu,
		    TOP_dadd,
		    TOP_daddi,
		    TOP_daddiu,
		    TOP_daddu,
		    TOP_dsub,
		    TOP_dsubu,
		    TOP_and,
		    TOP_andi,
		    TOP_lui,
		    TOP_nor,
		    TOP_or,
		    TOP_ori,
		    TOP_xor,
		    TOP_xori,
		    TOP_mfhi,
		    TOP_mflo,
		    TOP_mthi,
		    TOP_mtlo,
		    TOP_movf,
		    TOP_movn,
		    TOP_movt,
		    TOP_movz,
		    TOP_sll,
		    TOP_sllv,
		    TOP_sra,
		    TOP_srav,
		    TOP_srl,
		    TOP_srlv,
		    TOP_dsll,
		    TOP_dsll32,
		    TOP_dsllv,
		    TOP_dsra,
		    TOP_dsra32,
		    TOP_dsrav,
		    TOP_dsrl,
		    TOP_dsrl32,
		    TOP_dsrlv,
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
  Any_Operand_Access_Time(0);
  Any_Result_Available_Time(1);
  Resource_Requirement(res_integer, 0);
  Resource_Requirement(res_issue, 0);

  Instruction_Group("imult",
		    TOP_mult,
		    TOP_multu,
		    TOP_dmult,
		    TOP_dmultu,
		    TOP_UNDEFINED);
  Any_Operand_Access_Time(0);
  Any_Result_Available_Time(7);
  Resource_Requirement(res_integer, 0);
  Resource_Requirement(res_issue, 0);

  Instruction_Group("idiv",
		    TOP_div,
		    TOP_divu,
		    TOP_ddiv,
		    TOP_ddivu,
		    TOP_UNDEFINED);
  Any_Operand_Access_Time(0);
  Any_Result_Available_Time(67);
  Resource_Requirement(res_integer, 0);
  Resource_Requirement(res_issue, 0);

  Instruction_Group("load",
		    TOP_lb,
		    TOP_lbu,
		    TOP_lh,
		    TOP_lhu,
		    TOP_lw,
		    TOP_lwl,
		    TOP_lwr,
		    TOP_lwu,
		    TOP_ld,
		    TOP_ldl,
		    TOP_ldr,
		    TOP_ll,
		    TOP_lld,
		    TOP_lwc1,
		    TOP_ldc1,
		    TOP_lwxc1,
		    TOP_ldxc1,
		    TOP_UNDEFINED);
  Any_Operand_Access_Time(0);
  Any_Result_Available_Time(2);
  Resource_Requirement(res_issue, 0);
  Resource_Requirement(res_memory, 0);
  Load_Access_Time(2);

  Instruction_Group("store",
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
  Any_Operand_Access_Time(0);
  Resource_Requirement(res_issue, 0);
  Resource_Requirement(res_memory, 0);
  Store_Available_Time(2);

  Instruction_Group("branch",
		    TOP_beq,
		    TOP_bgez,
		    TOP_bgezal,
		    TOP_bgtz,
		    TOP_blez,
		    TOP_bltz,
		    TOP_bltzal,
		    TOP_bne,
		    TOP_j,
		    TOP_jal,
		    TOP_jalr,
		    TOP_jr,
		    TOP_syscall,
		    TOP_UNDEFINED);
  Any_Operand_Access_Time(0);
  Any_Result_Available_Time(1);
  Resource_Requirement(res_issue, 0);
  Resource_Requirement(res_integer, 0);
  Resource_Requirement(res_branch, 0);

  Instruction_Group("coproc1",
		    TOP_c_f_s,
		    TOP_c_f_d,
		    TOP_c_t_s,
		    TOP_c_t_d,
		    TOP_c_un_s,
		    TOP_c_un_d,
		    TOP_c_or_s,
		    TOP_c_or_d,
		    TOP_c_eq_s,
		    TOP_c_eq_d,
		    TOP_c_neq_s,
		    TOP_c_neq_d,
		    TOP_c_ueq_s,
		    TOP_c_ueq_d,
		    TOP_c_ogl_s,
		    TOP_c_ogl_d,
		    TOP_c_olt_s,
		    TOP_c_olt_d,
		    TOP_c_uge_s,
		    TOP_c_uge_d,
		    TOP_c_ult_s,
		    TOP_c_ult_d,
		    TOP_c_oge_s,
		    TOP_c_oge_d,
		    TOP_c_ole_s,
		    TOP_c_ole_d,
		    TOP_c_ugt_s,
		    TOP_c_ugt_d,
		    TOP_c_ule_s,
		    TOP_c_ule_d,
		    TOP_c_ogt_s,
		    TOP_c_ogt_d,
		    TOP_c_sf_s,
		    TOP_c_sf_d,
		    TOP_c_st_s,
		    TOP_c_st_d,
		    TOP_c_ngle_s,
		    TOP_c_ngle_d,
		    TOP_c_gle_s,
		    TOP_c_gle_d,
		    TOP_c_seq_s,
		    TOP_c_seq_d,
		    TOP_c_sne_s,
		    TOP_c_sne_d,
		    TOP_c_ngl_s,
		    TOP_c_ngl_d,
		    TOP_c_gl_s,
		    TOP_c_gl_d,
		    TOP_c_lt_s,
		    TOP_c_lt_d,
		    TOP_c_nlt_s,
		    TOP_c_nlt_d,
		    TOP_c_nge_s,
		    TOP_c_nge_d,
		    TOP_c_ge_s,
		    TOP_c_ge_d,
		    TOP_c_le_s,
		    TOP_c_le_d,
		    TOP_c_nle_s,
		    TOP_c_nle_d,
		    TOP_c_ngt_s,
		    TOP_c_ngt_d,
		    TOP_c_gt_s,
		    TOP_c_gt_d,
		    TOP_abs_s,
		    TOP_abs_d,
		    TOP_add_s,
		    TOP_add_d,
		    TOP_neg_s,
		    TOP_neg_d,
		    TOP_sub_s,
		    TOP_sub_d,
		    TOP_cfc1,
		    TOP_ctc1,
		    TOP_mov_s,
		    TOP_mov_d,
		    TOP_movf_s,
		    TOP_movf_d,
		    TOP_movn_s,
		    TOP_movn_d,
		    TOP_movt_s,
		    TOP_movt_d,
		    TOP_movz_s,
		    TOP_movz_d,
		    TOP_bc1f,
		    TOP_bc1t,
		    TOP_cvt_d_s,
		    TOP_cvt_s_d,
		    TOP_UNDEFINED);
  Any_Operand_Access_Time(0);
  Any_Result_Available_Time(1);
  Resource_Requirement(res_issue, 0);
  Resource_Requirement(res_float, 0);

  Instruction_Group("coproc1-moves",
		    TOP_mfc1,
		    TOP_mtc1,
		    TOP_dmfc1,
		    TOP_dmtc1,
		    TOP_UNDEFINED);
  Any_Operand_Access_Time(0);
  Any_Result_Available_Time(3);
  Resource_Requirement(res_issue, 0);
  Resource_Requirement(res_float, 0);

  Instruction_Group("float-int-convert",
		    TOP_cvt_d_w,
		    TOP_cvt_d_l,
		    TOP_cvt_l_s,
		    TOP_cvt_l_d,
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
		    TOP_UNDEFINED);
  Any_Operand_Access_Time(0);
  Any_Result_Available_Time(4);
  Resource_Requirement(res_issue, 0);
  Resource_Requirement(res_float, 0);

  Instruction_Group("float-mult",
		    TOP_mul_s,
		    TOP_mul_d,
		    TOP_UNDEFINED);
  Any_Operand_Access_Time(0);
  Any_Result_Available_Time(2);
  Resource_Requirement(res_issue, 0);
  Resource_Requirement(res_float, 0);

  Instruction_Group("float-divide",
		    TOP_div_s,
		    TOP_div_d,
		    TOP_UNDEFINED);
  Any_Operand_Access_Time(0);
  Any_Result_Available_Time(12);
  Resource_Requirement(res_issue, 0);
  Resource_Requirement(res_float, 0);

  Instruction_Group("float-others",
		    TOP_sqrt_s,
		    TOP_sqrt_d,
		    TOP_madd_s,
		    TOP_madd_d,
		    TOP_msub_s,
		    TOP_msub_d,
		    TOP_nmadd_s,
		    TOP_nmadd_d,
		    TOP_nmsub_s,
		    TOP_nmsub_d,
		    TOP_recip_s,
		    TOP_recip_d,
		    TOP_rsqrt_s,
		    TOP_rsqrt_d,
		    TOP_UNDEFINED);
  Any_Operand_Access_Time(0);
  Any_Result_Available_Time(8);
  Resource_Requirement(res_issue, 0);
  Resource_Requirement(res_float, 0);

  Instruction_Group("sync",
		    TOP_sync,
		    TOP_UNDEFINED);
  Resource_Requirement(res_issue, 0);
  Resource_Requirement(res_memory, 0);

  Instruction_Group("prefetch",
		    TOP_pref,
		    TOP_prefx,
		    TOP_UNDEFINED);
  Any_Operand_Access_Time(0);
  Resource_Requirement(res_issue, 0);
  Resource_Requirement(res_memory, 0);

  Instruction_Group("nop",
		    TOP_nop,
		    TOP_break,
		    TOP_UNDEFINED);
  Resource_Requirement(res_issue, 0);
  Resource_Requirement(res_integer, 0);

  Instruction_Group("dummy",
		    TOP_asm,
		    TOP_intrncall,
		    TOP_spadjust,
		    TOP_UNDEFINED);
  Any_Operand_Access_Time(0);
  Any_Result_Available_Time(1);
  Resource_Requirement(res_issue, 0);
  Resource_Requirement(res_integer, 0);

  Machine_Done("r10000.c");
}
