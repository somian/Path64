/*
 *  Copyright (C) 2007, 2008 PathScale, LLC.  All Rights Reserved.
 */

/*
 * Copyright (C) 2007. QLogic Corporation. All Rights Reserved.
 */

/*
 * Copyright 2003, 2004, 2005 PathScale, Inc.  All Rights Reserved.
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

/*
 * MIPS scheduling information for mips5kf on SiCortex ice9.
 */

#include "si_gen.h"
#include "targ_isa_subset.h"
#include "topcode.h"

// CG should fine-tune the latency of instructions going to the MDU.

static RESOURCE res_Issue, res_EX, res_MD, res_FP, res_Prefetch;

int
main (int argc, char *argv[])
{
  Machine("mips5kf", ISA_SUBSET_MIPS4, argc, argv);

  res_Issue = RESOURCE_Create ("Issue", 2);
  res_EX = RESOURCE_Create ("Execution", 1);		// integer pipeline
  res_MD = RESOURCE_Create ("MultiplyDivide", 1);	// integer mult/div
  res_FP = RESOURCE_Create ("Float", 1);		// fp pipeline

  // Schedule prefetches 16 cycles apart.
  res_Prefetch = RESOURCE_Create ("Prefetch", 1);

  /* instruction categories and units */

  // Instruction Summary
  //	TOP_abs_d         	Pipef__Rep01__Lat04
  //	TOP_abs_s         	Pipef__Rep01__Lat04
  //	TOP_add           	Pipee__Rep01__Lat01
  //	TOP_add_d         	Pipef__Rep01__Lat04
  //	TOP_add_s         	Pipef__Rep01__Lat04
  //	TOP_addi          	Pipee__Rep01__Lat01
  //	TOP_addiu         	Pipee__Rep01__Lat01
  //	TOP_addu          	Pipee__Rep01__Lat01
  //	TOP_and           	Pipee__Rep01__Lat01
  //	TOP_andi          	Pipee__Rep01__Lat01
  //	TOP_bc1f          	Pipee__Rep01__Lat01_branch
  //	TOP_bc1t          	Pipee__Rep01__Lat01_branch
  //	TOP_beq           	Pipee__Rep01__Lat01_branch
  //	TOP_bgez          	Pipee__Rep01__Lat01_branch
  //	TOP_bgezal        	Pipee__Rep01__Lat01_branch
  //	TOP_bgtz          	Pipee__Rep01__Lat01_branch
  //	TOP_blez          	Pipee__Rep01__Lat01_branch
  //	TOP_bltz          	Pipee__Rep01__Lat01_branch
  //	TOP_bltzal        	Pipee__Rep01__Lat01_branch
  //	TOP_bne           	Pipee__Rep01__Lat01_branch
  //	TOP_break         	Pipee__Rep01__Lat01
  //	TOP_c_eq_d        	Pipef__Rep01__Lat02
  //	TOP_c_eq_s        	Pipef__Rep01__Lat02
  //	TOP_c_f_d         	Pipef__Rep01__Lat02
  //	TOP_c_f_s         	Pipef__Rep01__Lat02
  //	TOP_c_ge_d        	Pipef__Rep01__Lat02
  //	TOP_c_ge_s        	Pipef__Rep01__Lat02
  //	TOP_c_gl_d        	Pipef__Rep01__Lat02
  //	TOP_c_gl_s        	Pipef__Rep01__Lat02
  //	TOP_c_gle_d       	Pipef__Rep01__Lat02
  //	TOP_c_gle_s       	Pipef__Rep01__Lat02
  //	TOP_c_gt_d        	Pipef__Rep01__Lat02
  //	TOP_c_gt_s        	Pipef__Rep01__Lat02
  //	TOP_c_le_d        	Pipef__Rep01__Lat02
  //	TOP_c_le_s        	Pipef__Rep01__Lat02
  //	TOP_c_lt_d        	Pipef__Rep01__Lat02
  //	TOP_c_lt_s        	Pipef__Rep01__Lat02
  //	TOP_c_neq_d       	Pipef__Rep01__Lat02
  //	TOP_c_neq_s       	Pipef__Rep01__Lat02
  //	TOP_c_nge_d       	Pipef__Rep01__Lat02
  //	TOP_c_nge_s       	Pipef__Rep01__Lat02
  //	TOP_c_ngl_d       	Pipef__Rep01__Lat02
  //	TOP_c_ngl_s       	Pipef__Rep01__Lat02
  //	TOP_c_ngle_d      	Pipef__Rep01__Lat02
  //	TOP_c_ngle_s      	Pipef__Rep01__Lat02
  //	TOP_c_ngt_d       	Pipef__Rep01__Lat02
  //	TOP_c_ngt_s       	Pipef__Rep01__Lat02
  //	TOP_c_nle_d       	Pipef__Rep01__Lat02
  //	TOP_c_nle_s       	Pipef__Rep01__Lat02
  //	TOP_c_nlt_d       	Pipef__Rep01__Lat02
  //	TOP_c_nlt_s       	Pipef__Rep01__Lat02
  //	TOP_c_oge_d       	Pipef__Rep01__Lat02
  //	TOP_c_oge_s       	Pipef__Rep01__Lat02
  //	TOP_c_ogl_d       	Pipef__Rep01__Lat02
  //	TOP_c_ogl_s       	Pipef__Rep01__Lat02
  //	TOP_c_ogt_d       	Pipef__Rep01__Lat02
  //	TOP_c_ogt_s       	Pipef__Rep01__Lat02
  //	TOP_c_ole_d       	Pipef__Rep01__Lat02
  //	TOP_c_ole_s       	Pipef__Rep01__Lat02
  //	TOP_c_olt_d       	Pipef__Rep01__Lat02
  //	TOP_c_olt_s       	Pipef__Rep01__Lat02
  //	TOP_c_or_d        	Pipef__Rep01__Lat02
  //	TOP_c_or_s        	Pipef__Rep01__Lat02
  //	TOP_c_seq_d       	Pipef__Rep01__Lat02
  //	TOP_c_seq_s       	Pipef__Rep01__Lat02
  //	TOP_c_sf_d        	Pipef__Rep01__Lat02
  //	TOP_c_sf_s        	Pipef__Rep01__Lat02
  //	TOP_c_sne_d       	Pipef__Rep01__Lat02
  //	TOP_c_sne_s       	Pipef__Rep01__Lat02
  //	TOP_c_st_d        	Pipef__Rep01__Lat02
  //	TOP_c_st_s        	Pipef__Rep01__Lat02
  //	TOP_c_t_d         	Pipef__Rep01__Lat02
  //	TOP_c_t_s         	Pipef__Rep01__Lat02
  //	TOP_c_ueq_d       	Pipef__Rep01__Lat02
  //	TOP_c_ueq_s       	Pipef__Rep01__Lat02
  //	TOP_c_uge_d       	Pipef__Rep01__Lat02
  //	TOP_c_uge_s       	Pipef__Rep01__Lat02
  //	TOP_c_ugt_d       	Pipef__Rep01__Lat02
  //	TOP_c_ugt_s       	Pipef__Rep01__Lat02
  //	TOP_c_ule_d       	Pipef__Rep01__Lat02
  //	TOP_c_ule_s       	Pipef__Rep01__Lat02
  //	TOP_c_ult_d       	Pipef__Rep01__Lat02
  //	TOP_c_ult_s       	Pipef__Rep01__Lat02
  //	TOP_c_un_d        	Pipef__Rep01__Lat02
  //	TOP_c_un_s        	Pipef__Rep01__Lat02
  //	TOP_ceil_l_d      	Pipef__Rep01__Lat05
  //	TOP_ceil_l_s      	Pipef__Rep01__Lat05
  //	TOP_ceil_w_d      	Pipef__Rep01__Lat05
  //	TOP_ceil_w_s      	Pipef__Rep01__Lat05
  //	TOP_clo          	Pipee__Rep01__Lat01
  //	TOP_clz          	Pipee__Rep01__Lat01
  //	TOP_cfc1          	Pipeef__Rep01__Lat04
  //	TOP_ctc1          	Pipeef__Rep01__Lat04
  //	TOP_cvt_d_l       	Pipef__Rep01__Lat04
  //	TOP_cvt_d_s       	Pipef__Rep01__Lat04
  //	TOP_cvt_d_w       	Pipef__Rep01__Lat04
  //	TOP_cvt_l_d       	Pipef__Rep01__Lat05
  //	TOP_cvt_l_s       	Pipef__Rep01__Lat05
  //	TOP_cvt_s_d       	Pipef__Rep01__Lat06
  //	TOP_cvt_s_l       	Pipef__Rep01__Lat04
  //	TOP_cvt_s_w       	Pipef__Rep01__Lat04
  //	TOP_cvt_w_d       	Pipef__Rep01__Lat05
  //	TOP_cvt_w_s       	Pipef__Rep01__Lat05
  //	TOP_dadd          	Pipee__Rep01__Lat01
  //	TOP_daddi         	Pipee__Rep01__Lat01
  //	TOP_daddiu        	Pipee__Rep01__Lat01
  //	TOP_daddu         	Pipee__Rep01__Lat01
  //	TOP_dclo          	Pipee__Rep01__Lat01
  //	TOP_dclz          	Pipee__Rep01__Lat01
  //	TOP_ddiv          	Piped__Rep67__Lat69
  //	TOP_ddivu         	Piped__Rep67__Lat69
  //	TOP_div           	Piped__Rep36__Lat37
  //	TOP_div_d         	Pipef__Rep29__Lat32
  //	TOP_div_s         	Pipef__Rep14__Lat17
  //	TOP_divu          	Piped__Rep36__Lat37
  //	TOP_dmfc1         	Pipeef__Rep01__Lat02
  //	TOP_dmtc1         	Pipeef__Rep01__Lat02
  //	TOP_dmult         	Piped__Rep09__Lat11
  //	TOP_dmultu        	Piped__Rep09__Lat11
  //	TOP_dsll          	Pipee__Rep01__Lat01
  //	TOP_dsll32        	Pipee__Rep01__Lat01
  //	TOP_dsllv         	Pipee__Rep01__Lat01
  //	TOP_dsra          	Pipee__Rep01__Lat01
  //	TOP_dsra32        	Pipee__Rep01__Lat01
  //	TOP_dsrav         	Pipee__Rep01__Lat01
  //	TOP_dsrl          	Pipee__Rep01__Lat01
  //	TOP_dsrl32        	Pipee__Rep01__Lat01
  //	TOP_dsrlv         	Pipee__Rep01__Lat01
  //	TOP_dsub          	Pipee__Rep01__Lat01
  //	TOP_dsubu         	Pipee__Rep01__Lat01
  //	TOP_floor_l_d     	Pipef__Rep01__Lat05
  //	TOP_floor_l_s     	Pipef__Rep01__Lat05
  //	TOP_floor_w_d     	Pipef__Rep01__Lat05
  //	TOP_floor_w_s     	Pipef__Rep01__Lat05
  //	TOP_j             	Pipee__Rep01__Lat01_branch
  //	TOP_jal           	Pipee__Rep01__Lat01_branch
  //	TOP_jalr          	Pipee__Rep01__Lat01_branch
  //	TOP_jr            	Pipee__Rep01__Lat01_branch
  //	TOP_lb            	Pipeem__Rep01__Lat02
  //	TOP_lbu           	Pipeem__Rep01__Lat02
  //	TOP_ld            	Pipeem__Rep01__Lat02
  //	TOP_ldc1          	Pipeem__Rep01__Lat03
  //	TOP_ldl           	Pipeem__Rep01__Lat02
  //	TOP_ldr           	Pipeem__Rep01__Lat02
  //	TOP_ldxc1         	Pipeem__Rep01__Lat03
  //	TOP_lh            	Pipeem__Rep01__Lat02
  //	TOP_lhu           	Pipeem__Rep01__Lat02
  //	TOP_ll            	Pipeem__Rep01__Lat02
  //	TOP_lld           	Pipeem__Rep01__Lat02
  //	TOP_lui           	Pipee__Rep01__Lat01
  //	TOP_lw            	Pipeem__Rep01__Lat02
  //	TOP_lwc1          	Pipeem__Rep01__Lat03
  //	TOP_lwl           	Pipeem__Rep01__Lat02
  //	TOP_lwr           	Pipeem__Rep01__Lat02
  //	TOP_lwu           	Pipeem__Rep01__Lat02
  //	TOP_lwxc1         	Pipeem__Rep01__Lat03
  //	TOP_madd_d        	Pipef__Rep01__Lat04
  //	TOP_madd_s        	Pipef__Rep01__Lat04
  //	TOP_mfc1          	Pipeef__Rep01__Lat02
  //	TOP_mfhi          	Pipee__Rep01__Lat01
  //	TOP_mflo          	Pipee__Rep01__Lat01
  //	TOP_mov_d         	Pipef__Rep01__Lat04
  //	TOP_mov_s         	Pipef__Rep01__Lat04
  //	TOP_movf          	Pipeef__Rep01__Lat01
  //	TOP_movf_d        	Pipef__Rep01__Lat04
  //	TOP_movf_s        	Pipef__Rep01__Lat04
  //	TOP_movn          	Pipee__Rep01__Lat01
  //	TOP_movn_d        	Pipeef__Rep01__Lat04
  //	TOP_movn_s        	Pipeef__Rep01__Lat04
  //	TOP_movt          	Pipeef__Rep01__Lat01
  //	TOP_movt_d        	Pipef__Rep01__Lat04
  //	TOP_movt_s        	Pipef__Rep01__Lat04
  //	TOP_movz          	Pipee__Rep01__Lat01
  //	TOP_movz_d        	Pipeef__Rep01__Lat04
  //	TOP_movz_s        	Pipeef__Rep01__Lat04
  //	TOP_msub_d        	Pipef__Rep01__Lat04
  //	TOP_msub_s        	Pipef__Rep01__Lat04
  //	TOP_mtc1          	Pipeef__Rep01__Lat02
  //	TOP_mthi          	Pipee__Rep01__Lat01
  //	TOP_mtlo          	Pipee__Rep01__Lat01
  //	TOP_mul_d         	Pipef__Rep01__Lat04
  //	TOP_mul_s         	Pipef__Rep01__Lat04
  //	TOP_mul          	Piped__Rep02__Lat04
  //	TOP_mult          	Piped__Rep02__Lat04
  //	TOP_multu         	Piped__Rep02__Lat04
  //	TOP_neg_d         	Pipef__Rep01__Lat04
  //	TOP_neg_s         	Pipef__Rep01__Lat04
  //	TOP_nmadd_d       	Pipef__Rep01__Lat04
  //	TOP_nmadd_s       	Pipef__Rep01__Lat04
  //	TOP_nmsub_d       	Pipef__Rep01__Lat04
  //	TOP_nmsub_s       	Pipef__Rep01__Lat04
  //	TOP_nor           	Pipee__Rep01__Lat01
  //	TOP_or            	Pipee__Rep01__Lat01
  //	TOP_ori           	Pipee__Rep01__Lat01
  //	TOP_pref          	Pipeem__Rep01__Lat01
  //	TOP_prefx         	Pipeem__Rep01__Lat01
  //	TOP_recip_d       	Pipef__Rep18__Lat23
  //	TOP_recip_s       	Pipef__Rep10__Lat15
  //	TOP_round_l_d     	Pipef__Rep01__Lat05
  //	TOP_round_l_s     	Pipef__Rep01__Lat05
  //	TOP_round_w_d     	Pipef__Rep01__Lat05
  //	TOP_round_w_s     	Pipef__Rep01__Lat05
  //	TOP_rsqrt_d       	Pipef__Rep26__Lat31
  //	TOP_rsqrt_s       	Pipef__Rep14__Lat19
  //	TOP_sb            	Pipeem__Rep01__Lat01
  //	TOP_sc            	Pipeem__Rep01__Lat01
  //	TOP_scd           	Pipeem__Rep01__Lat01
  //	TOP_sd            	Pipeem__Rep01__Lat01
  //	TOP_sdc1          	Pipeem__Rep01__Lat01
  //	TOP_sdl           	Pipeem__Rep01__Lat01
  //	TOP_sdr           	Pipeem__Rep01__Lat01
  //	TOP_sdxc1         	Pipeem__Rep01__Lat01
  //	TOP_sext          	Pipee__Rep01__Lat01
  //	TOP_sh            	Pipeem__Rep01__Lat01
  //	TOP_sll           	Pipee__Rep01__Lat01
  //	TOP_sllv          	Pipee__Rep01__Lat01
  //	TOP_slt           	Pipee__Rep01__Lat01
  //	TOP_slti          	Pipee__Rep01__Lat01
  //	TOP_sltiu         	Pipee__Rep01__Lat01
  //	TOP_sltu          	Pipee__Rep01__Lat01
  //	TOP_sqrt_d        	Pipef__Rep29__Lat32
  //	TOP_sqrt_s        	Pipef__Rep14__Lat17
  //	TOP_sra           	Pipee__Rep01__Lat01
  //	TOP_srav          	Pipee__Rep01__Lat01
  //	TOP_srl           	Pipee__Rep01__Lat01
  //	TOP_srlv          	Pipee__Rep01__Lat01
  //	TOP_sub           	Pipee__Rep01__Lat01
  //	TOP_sub_d         	Pipef__Rep01__Lat04
  //	TOP_sub_s         	Pipef__Rep01__Lat04
  //	TOP_subu          	Pipee__Rep01__Lat01
  //	TOP_sw            	Pipeem__Rep01__Lat01
  //	TOP_swc1          	Pipeem__Rep01__Lat01
  //	TOP_swl           	Pipeem__Rep01__Lat01
  //	TOP_swr           	Pipeem__Rep01__Lat01
  //	TOP_swxc1         	Pipeem__Rep01__Lat01
  //	TOP_sync          	Pipeem__Rep01__Lat01
  //	TOP_syscall       	Pipee__Rep01__Lat01
  //	TOP_teq           	Pipee__Rep01__Lat01
  //	TOP_teqi          	Pipee__Rep01__Lat01
  //	TOP_tge           	Pipee__Rep01__Lat01
  //	TOP_tgei          	Pipee__Rep01__Lat01
  //	TOP_tgeiu         	Pipee__Rep01__Lat01
  //	TOP_tgeu          	Pipee__Rep01__Lat01
  //	TOP_tlt           	Pipee__Rep01__Lat01
  //	TOP_tlti          	Pipee__Rep01__Lat01
  //	TOP_tltiu         	Pipee__Rep01__Lat01
  //	TOP_tltu          	Pipee__Rep01__Lat01
  //	TOP_tne           	Pipee__Rep01__Lat01
  //	TOP_tnei          	Pipee__Rep01__Lat01
  //	TOP_trunc_l_d     	Pipef__Rep01__Lat05
  //	TOP_trunc_l_s     	Pipef__Rep01__Lat05
  //	TOP_trunc_w_d     	Pipef__Rep01__Lat05
  //	TOP_trunc_w_s     	Pipef__Rep01__Lat05
  //	TOP_xor           	Pipee__Rep01__Lat01
  //	TOP_xori          	Pipee__Rep01__Lat01

  Instruction_Group ("Piped__Rep02__Lat04",
			  TOP_mul,
			  TOP_mult,
			  TOP_multu,
			  TOP_UNDEFINED);
  Any_Operand_Access_Time (0);
  Any_Result_Available_Time (4);
  Resource_Requirement (res_Issue, 0);
  Resource_Requirement (res_EX, 0);
  Resource_Requirement (res_MD, 0);
  Resource_Requirement (res_MD, 1);

  Instruction_Group ("Piped__Rep09__Lat11",
			  TOP_dmult,
			  TOP_dmultu,
			  TOP_UNDEFINED);
  Any_Operand_Access_Time (0);
  Any_Result_Available_Time (11);
  Resource_Requirement (res_Issue, 0);
  Resource_Requirement (res_EX, 0);
  Resource_Requirement (res_MD, 0);
  Resource_Requirement (res_MD, 1);
  Resource_Requirement (res_MD, 2);
  Resource_Requirement (res_MD, 3);
  Resource_Requirement (res_MD, 4);
  Resource_Requirement (res_MD, 5);
  Resource_Requirement (res_MD, 6);
  Resource_Requirement (res_MD, 7);
  Resource_Requirement (res_MD, 8);

  Instruction_Group ("Piped__Rep36__Lat37",
			  TOP_div,
			  TOP_divu,
			  TOP_UNDEFINED);
  Any_Operand_Access_Time (0);
  Any_Result_Available_Time (37);
  Resource_Requirement (res_Issue, 0);
  Resource_Requirement (res_EX, 0);
  Resource_Requirement (res_MD, 0);
  Resource_Requirement (res_MD, 1);
  Resource_Requirement (res_MD, 2);
  Resource_Requirement (res_MD, 3);
  Resource_Requirement (res_MD, 4);
  Resource_Requirement (res_MD, 5);
  Resource_Requirement (res_MD, 6);
  Resource_Requirement (res_MD, 7);
  Resource_Requirement (res_MD, 8);
  Resource_Requirement (res_MD, 9);
  Resource_Requirement (res_MD, 10);
  Resource_Requirement (res_MD, 11);
  Resource_Requirement (res_MD, 12);
  Resource_Requirement (res_MD, 13);
  Resource_Requirement (res_MD, 14);
  Resource_Requirement (res_MD, 15);
  Resource_Requirement (res_MD, 16);
  Resource_Requirement (res_MD, 17);
  Resource_Requirement (res_MD, 18);
  Resource_Requirement (res_MD, 19);
  Resource_Requirement (res_MD, 20);
  Resource_Requirement (res_MD, 21);
  Resource_Requirement (res_MD, 22);
  Resource_Requirement (res_MD, 23);
  Resource_Requirement (res_MD, 24);
  Resource_Requirement (res_MD, 25);
  Resource_Requirement (res_MD, 26);
  Resource_Requirement (res_MD, 27);
  Resource_Requirement (res_MD, 28);
  Resource_Requirement (res_MD, 29);
  Resource_Requirement (res_MD, 30);
  Resource_Requirement (res_MD, 31);
  Resource_Requirement (res_MD, 32);
  Resource_Requirement (res_MD, 33);
  Resource_Requirement (res_MD, 34);
  Resource_Requirement (res_MD, 35);

  Instruction_Group ("Piped__Rep67__Lat69",
			  TOP_ddiv,
			  TOP_ddivu,
			  TOP_UNDEFINED);
  Any_Operand_Access_Time (0);
  Any_Result_Available_Time (69);
  Resource_Requirement (res_Issue, 0);
  Resource_Requirement (res_EX, 0);
  Resource_Requirement (res_MD, 0);
  Resource_Requirement (res_MD, 1);
  Resource_Requirement (res_MD, 2);
  Resource_Requirement (res_MD, 3);
  Resource_Requirement (res_MD, 4);
  Resource_Requirement (res_MD, 5);
  Resource_Requirement (res_MD, 6);
  Resource_Requirement (res_MD, 7);
  Resource_Requirement (res_MD, 8);
  Resource_Requirement (res_MD, 9);
  Resource_Requirement (res_MD, 10);
  Resource_Requirement (res_MD, 11);
  Resource_Requirement (res_MD, 12);
  Resource_Requirement (res_MD, 13);
  Resource_Requirement (res_MD, 14);
  Resource_Requirement (res_MD, 15);
  Resource_Requirement (res_MD, 16);
  Resource_Requirement (res_MD, 17);
  Resource_Requirement (res_MD, 18);
  Resource_Requirement (res_MD, 19);
  Resource_Requirement (res_MD, 20);
  Resource_Requirement (res_MD, 21);
  Resource_Requirement (res_MD, 22);
  Resource_Requirement (res_MD, 23);
  Resource_Requirement (res_MD, 24);
  Resource_Requirement (res_MD, 25);
  Resource_Requirement (res_MD, 26);
  Resource_Requirement (res_MD, 27);
  Resource_Requirement (res_MD, 28);
  Resource_Requirement (res_MD, 29);
  Resource_Requirement (res_MD, 30);
  Resource_Requirement (res_MD, 31);
  Resource_Requirement (res_MD, 32);
  Resource_Requirement (res_MD, 33);
  Resource_Requirement (res_MD, 34);
  Resource_Requirement (res_MD, 35);
  Resource_Requirement (res_MD, 36);
  Resource_Requirement (res_MD, 37);
  Resource_Requirement (res_MD, 38);
  Resource_Requirement (res_MD, 39);
  Resource_Requirement (res_MD, 40);
  Resource_Requirement (res_MD, 41);
  Resource_Requirement (res_MD, 42);
  Resource_Requirement (res_MD, 43);
  Resource_Requirement (res_MD, 44);
  Resource_Requirement (res_MD, 45);
  Resource_Requirement (res_MD, 46);
  Resource_Requirement (res_MD, 47);
  Resource_Requirement (res_MD, 48);
  Resource_Requirement (res_MD, 49);
  Resource_Requirement (res_MD, 50);
  Resource_Requirement (res_MD, 51);
  Resource_Requirement (res_MD, 52);
  Resource_Requirement (res_MD, 53);
  Resource_Requirement (res_MD, 54);
  Resource_Requirement (res_MD, 55);
  Resource_Requirement (res_MD, 56);
  Resource_Requirement (res_MD, 57);
  Resource_Requirement (res_MD, 58);
  Resource_Requirement (res_MD, 59);
  Resource_Requirement (res_MD, 60);
  Resource_Requirement (res_MD, 61);
  Resource_Requirement (res_MD, 62);
  Resource_Requirement (res_MD, 63);
  Resource_Requirement (res_MD, 64);
  Resource_Requirement (res_MD, 65);
  Resource_Requirement (res_MD, 66);

  Instruction_Group ("Pipee__Rep01__Lat01",
			  TOP_add,
			  TOP_addi,
			  TOP_addiu,
			  TOP_addu,
			  TOP_and,
			  TOP_andi,
			  TOP_break,
			  TOP_clo,
			  TOP_clz,
			  TOP_dadd,
			  TOP_daddi,
			  TOP_daddiu,
			  TOP_daddu,
			  TOP_dclo,
			  TOP_dclz,
			  TOP_dsll,
			  TOP_dsll32,
			  TOP_dsllv,
			  TOP_dsra,
			  TOP_dsra32,
			  TOP_dsrav,
			  TOP_dsrl,
			  TOP_dsrl32,
			  TOP_dsrlv,
			  TOP_dsub,
			  TOP_dsubu,
			  TOP_lui,
			  TOP_mfhi,
			  TOP_mflo,
			  TOP_movn,
			  TOP_movz,
			  TOP_mthi,
			  TOP_mtlo,
			  TOP_nor,
			  TOP_or,
			  TOP_ori,
			  TOP_sext,
			  TOP_sll,
			  TOP_sllv,
			  TOP_slt,
			  TOP_slti,
			  TOP_sltiu,
			  TOP_sltu,
			  TOP_sra,
			  TOP_srav,
			  TOP_srl,
			  TOP_srlv,
			  TOP_sub,
			  TOP_subu,
			  TOP_syscall,
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
			  TOP_xor,
			  TOP_xori,
			  TOP_UNDEFINED);
  Any_Operand_Access_Time (0);
  Any_Result_Available_Time (1);
  Resource_Requirement (res_Issue, 0);
  Resource_Requirement (res_EX, 0);

  Instruction_Group ("Pipee__Rep01__Lat01_branch",
			  TOP_bc1f,
			  TOP_bc1t,
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
			  TOP_UNDEFINED);
  Any_Operand_Access_Time (0);
  Any_Result_Available_Time (1);
  Resource_Requirement (res_Issue, 0);
  Resource_Requirement (res_Issue, 0);
  Resource_Requirement (res_Issue, 1);
  Resource_Requirement (res_EX, 0);

  Instruction_Group ("Pipeef__Rep01__Lat01",
			  TOP_movf,
			  TOP_movt,
			  TOP_UNDEFINED);
  Any_Operand_Access_Time (0);
  Any_Result_Available_Time (1);
  Resource_Requirement (res_Issue, 0);
  Resource_Requirement (res_EX, 0);
  Resource_Requirement (res_FP, 0);

  Instruction_Group ("Pipeef__Rep01__Lat02",
			  TOP_dmfc1,
			  TOP_dmtc1,
			  TOP_mfc1,
			  TOP_mtc1,
			  TOP_UNDEFINED);
  Any_Operand_Access_Time (0);
  Any_Result_Available_Time (2);
  Resource_Requirement (res_Issue, 0);
  Resource_Requirement (res_EX, 0);
  Resource_Requirement (res_FP, 0);

  Instruction_Group ("Pipeef__Rep01__Lat04",
			  TOP_cfc1,
			  TOP_ctc1,
			  TOP_movn_d,
			  TOP_movn_s,
			  TOP_movz_d,
			  TOP_movz_s,
			  TOP_UNDEFINED);
  Any_Operand_Access_Time (0);
  Any_Result_Available_Time (4);
  Resource_Requirement (res_Issue, 0);
  Resource_Requirement (res_EX, 0);
  Resource_Requirement (res_FP, 0);

  Instruction_Group ("Pipeem__Rep01__Lat01",
			  TOP_sb,
			  TOP_sc,
			  TOP_scd,
			  TOP_sd,
			  TOP_sdc1,
			  TOP_sdl,
			  TOP_sdr,
			  TOP_sdxc1,
			  TOP_sh,
			  TOP_sw,
			  TOP_swc1,
			  TOP_swl,
			  TOP_swr,
			  TOP_swxc1,
			  TOP_sync,
			  TOP_UNDEFINED);
  Any_Operand_Access_Time (0);
  Any_Result_Available_Time (1);
  Resource_Requirement (res_Issue, 0);
  Resource_Requirement (res_EX, 0);

  Instruction_Group ("Pipeem__Rep01__Lat02",
			  TOP_lb,
			  TOP_lbu,
			  TOP_ld,
			  TOP_ldl,
			  TOP_ldr,
			  TOP_lh,
			  TOP_lhu,
			  TOP_ll,
			  TOP_lld,
			  TOP_lw,
			  TOP_lwl,
			  TOP_lwr,
			  TOP_lwu,
			  TOP_UNDEFINED);
  Any_Operand_Access_Time (0);
  Any_Result_Available_Time (2);
  Resource_Requirement (res_Issue, 0);
  Resource_Requirement (res_EX, 0);

  Instruction_Group ("Pipeem__Rep01__Lat03",
			  TOP_ldc1,
			  TOP_ldxc1,
			  TOP_lwc1,
			  TOP_lwxc1,
			  TOP_UNDEFINED);
  Any_Operand_Access_Time (0);
  Any_Result_Available_Time (3);
  Resource_Requirement (res_Issue, 0);
  Resource_Requirement (res_EX, 0);

  Instruction_Group ("Pipef__Rep01__Lat02",
			  TOP_c_eq_d,
			  TOP_c_eq_s,
			  TOP_c_f_d,
			  TOP_c_f_s,
			  TOP_c_ge_d,
			  TOP_c_ge_s,
			  TOP_c_gl_d,
			  TOP_c_gl_s,
			  TOP_c_gle_d,
			  TOP_c_gle_s,
			  TOP_c_gt_d,
			  TOP_c_gt_s,
			  TOP_c_le_d,
			  TOP_c_le_s,
			  TOP_c_lt_d,
			  TOP_c_lt_s,
			  TOP_c_neq_d,
			  TOP_c_neq_s,
			  TOP_c_nge_d,
			  TOP_c_nge_s,
			  TOP_c_ngl_d,
			  TOP_c_ngl_s,
			  TOP_c_ngle_d,
			  TOP_c_ngle_s,
			  TOP_c_ngt_d,
			  TOP_c_ngt_s,
			  TOP_c_nle_d,
			  TOP_c_nle_s,
			  TOP_c_nlt_d,
			  TOP_c_nlt_s,
			  TOP_c_oge_d,
			  TOP_c_oge_s,
			  TOP_c_ogl_d,
			  TOP_c_ogl_s,
			  TOP_c_ogt_d,
			  TOP_c_ogt_s,
			  TOP_c_ole_d,
			  TOP_c_ole_s,
			  TOP_c_olt_d,
			  TOP_c_olt_s,
			  TOP_c_or_d,
			  TOP_c_or_s,
			  TOP_c_seq_d,
			  TOP_c_seq_s,
			  TOP_c_sf_d,
			  TOP_c_sf_s,
			  TOP_c_sne_d,
			  TOP_c_sne_s,
			  TOP_c_st_d,
			  TOP_c_st_s,
			  TOP_c_t_d,
			  TOP_c_t_s,
			  TOP_c_ueq_d,
			  TOP_c_ueq_s,
			  TOP_c_uge_d,
			  TOP_c_uge_s,
			  TOP_c_ugt_d,
			  TOP_c_ugt_s,
			  TOP_c_ule_d,
			  TOP_c_ule_s,
			  TOP_c_ult_d,
			  TOP_c_ult_s,
			  TOP_c_un_d,
			  TOP_c_un_s,
			  TOP_UNDEFINED);
  Any_Operand_Access_Time (0);
  Any_Result_Available_Time (2);
  Resource_Requirement (res_Issue, 0);
  Resource_Requirement (res_FP, 0);

  Instruction_Group ("Pipef__Rep01__Lat04",
			  TOP_abs_d,
			  TOP_abs_s,
			  TOP_add_d,
			  TOP_add_s,
			  TOP_cvt_d_l,
			  TOP_cvt_d_s,
			  TOP_cvt_d_w,
			  TOP_cvt_s_l,
			  TOP_cvt_s_w,
			  TOP_madd_d,
			  TOP_madd_s,
			  TOP_mov_d,
			  TOP_mov_s,
			  TOP_movf_d,
			  TOP_movf_s,
			  TOP_movt_d,
			  TOP_movt_s,
			  TOP_msub_d,
			  TOP_msub_s,
			  TOP_mul_d,
			  TOP_mul_s,
			  TOP_neg_d,
			  TOP_neg_s,
			  TOP_nmadd_d,
			  TOP_nmadd_s,
			  TOP_nmsub_d,
			  TOP_nmsub_s,
			  TOP_sub_d,
			  TOP_sub_s,
			  TOP_UNDEFINED);
  Any_Operand_Access_Time (0);
  Any_Result_Available_Time (4);
  Resource_Requirement (res_Issue, 0);
  Resource_Requirement (res_FP, 0);

  Instruction_Group ("Pipef__Rep01__Lat05",
			  TOP_ceil_l_d,
			  TOP_ceil_l_s,
			  TOP_ceil_w_d,
			  TOP_ceil_w_s,
			  TOP_cvt_l_d,
			  TOP_cvt_l_s,
			  TOP_cvt_w_d,
			  TOP_cvt_w_s,
			  TOP_floor_l_d,
			  TOP_floor_l_s,
			  TOP_floor_w_d,
			  TOP_floor_w_s,
			  TOP_round_l_d,
			  TOP_round_l_s,
			  TOP_round_w_d,
			  TOP_round_w_s,
			  TOP_trunc_l_d,
			  TOP_trunc_l_s,
			  TOP_trunc_w_d,
			  TOP_trunc_w_s,
			  TOP_UNDEFINED);
  Any_Operand_Access_Time (0);
  Any_Result_Available_Time (5);
  Resource_Requirement (res_Issue, 0);
  Resource_Requirement (res_FP, 0);

  Instruction_Group ("Pipef__Rep01__Lat06",
			  TOP_cvt_s_d,
			  TOP_UNDEFINED);
  Any_Operand_Access_Time (0);
  Any_Result_Available_Time (6);
  Resource_Requirement (res_Issue, 0);
  Resource_Requirement (res_FP, 0);

  Instruction_Group ("Pipef__Rep10__Lat15",
			  TOP_recip_s,
			  TOP_UNDEFINED);
  Any_Operand_Access_Time (0);
  Any_Result_Available_Time (15);
  Resource_Requirement (res_Issue, 0);
  Resource_Requirement (res_FP, 0);
  Resource_Requirement (res_FP, 1);
  Resource_Requirement (res_FP, 2);
  Resource_Requirement (res_FP, 3);
  Resource_Requirement (res_FP, 4);
  Resource_Requirement (res_FP, 5);
  Resource_Requirement (res_FP, 6);
  Resource_Requirement (res_FP, 7);
  Resource_Requirement (res_FP, 8);
  Resource_Requirement (res_FP, 9);

  Instruction_Group ("Pipef__Rep14__Lat17",
			  TOP_div_s,
			  TOP_sqrt_s,
			  TOP_UNDEFINED);
  Any_Operand_Access_Time (0);
  Any_Result_Available_Time (17);
  Resource_Requirement (res_Issue, 0);
  Resource_Requirement (res_FP, 0);
  Resource_Requirement (res_FP, 1);
  Resource_Requirement (res_FP, 2);
  Resource_Requirement (res_FP, 3);
  Resource_Requirement (res_FP, 4);
  Resource_Requirement (res_FP, 5);
  Resource_Requirement (res_FP, 6);
  Resource_Requirement (res_FP, 7);
  Resource_Requirement (res_FP, 8);
  Resource_Requirement (res_FP, 9);
  Resource_Requirement (res_FP, 10);
  Resource_Requirement (res_FP, 11);
  Resource_Requirement (res_FP, 12);
  Resource_Requirement (res_FP, 13);

  Instruction_Group ("Pipef__Rep14__Lat19",
			  TOP_rsqrt_s,
			  TOP_UNDEFINED);
  Any_Operand_Access_Time (0);
  Any_Result_Available_Time (19);
  Resource_Requirement (res_Issue, 0);
  Resource_Requirement (res_FP, 0);
  Resource_Requirement (res_FP, 1);
  Resource_Requirement (res_FP, 2);
  Resource_Requirement (res_FP, 3);
  Resource_Requirement (res_FP, 4);
  Resource_Requirement (res_FP, 5);
  Resource_Requirement (res_FP, 6);
  Resource_Requirement (res_FP, 7);
  Resource_Requirement (res_FP, 8);
  Resource_Requirement (res_FP, 9);
  Resource_Requirement (res_FP, 10);
  Resource_Requirement (res_FP, 11);
  Resource_Requirement (res_FP, 12);
  Resource_Requirement (res_FP, 13);

  Instruction_Group ("Pipef__Rep18__Lat23",
			  TOP_recip_d,
			  TOP_UNDEFINED);
  Any_Operand_Access_Time (0);
  Any_Result_Available_Time (23);
  Resource_Requirement (res_Issue, 0);
  Resource_Requirement (res_FP, 0);
  Resource_Requirement (res_FP, 1);
  Resource_Requirement (res_FP, 2);
  Resource_Requirement (res_FP, 3);
  Resource_Requirement (res_FP, 4);
  Resource_Requirement (res_FP, 5);
  Resource_Requirement (res_FP, 6);
  Resource_Requirement (res_FP, 7);
  Resource_Requirement (res_FP, 8);
  Resource_Requirement (res_FP, 9);
  Resource_Requirement (res_FP, 10);
  Resource_Requirement (res_FP, 11);
  Resource_Requirement (res_FP, 12);
  Resource_Requirement (res_FP, 13);
  Resource_Requirement (res_FP, 14);
  Resource_Requirement (res_FP, 15);
  Resource_Requirement (res_FP, 16);
  Resource_Requirement (res_FP, 17);

  Instruction_Group ("Pipef__Rep26__Lat31",
			  TOP_rsqrt_d,
			  TOP_UNDEFINED);
  Any_Operand_Access_Time (0);
  Any_Result_Available_Time (31);
  Resource_Requirement (res_Issue, 0);
  Resource_Requirement (res_FP, 0);
  Resource_Requirement (res_FP, 1);
  Resource_Requirement (res_FP, 2);
  Resource_Requirement (res_FP, 3);
  Resource_Requirement (res_FP, 4);
  Resource_Requirement (res_FP, 5);
  Resource_Requirement (res_FP, 6);
  Resource_Requirement (res_FP, 7);
  Resource_Requirement (res_FP, 8);
  Resource_Requirement (res_FP, 9);
  Resource_Requirement (res_FP, 10);
  Resource_Requirement (res_FP, 11);
  Resource_Requirement (res_FP, 12);
  Resource_Requirement (res_FP, 13);
  Resource_Requirement (res_FP, 14);
  Resource_Requirement (res_FP, 15);
  Resource_Requirement (res_FP, 16);
  Resource_Requirement (res_FP, 17);
  Resource_Requirement (res_FP, 18);
  Resource_Requirement (res_FP, 19);
  Resource_Requirement (res_FP, 20);
  Resource_Requirement (res_FP, 21);
  Resource_Requirement (res_FP, 22);
  Resource_Requirement (res_FP, 23);
  Resource_Requirement (res_FP, 24);
  Resource_Requirement (res_FP, 25);

  Instruction_Group ("Pipef__Rep29__Lat32",
			  TOP_div_d,
			  TOP_sqrt_d,
			  TOP_UNDEFINED);
  Any_Operand_Access_Time (0);
  Any_Result_Available_Time (32);
  Resource_Requirement (res_Issue, 0);
  Resource_Requirement (res_FP, 0);
  Resource_Requirement (res_FP, 1);
  Resource_Requirement (res_FP, 2);
  Resource_Requirement (res_FP, 3);
  Resource_Requirement (res_FP, 4);
  Resource_Requirement (res_FP, 5);
  Resource_Requirement (res_FP, 6);
  Resource_Requirement (res_FP, 7);
  Resource_Requirement (res_FP, 8);
  Resource_Requirement (res_FP, 9);
  Resource_Requirement (res_FP, 10);
  Resource_Requirement (res_FP, 11);
  Resource_Requirement (res_FP, 12);
  Resource_Requirement (res_FP, 13);
  Resource_Requirement (res_FP, 14);
  Resource_Requirement (res_FP, 15);
  Resource_Requirement (res_FP, 16);
  Resource_Requirement (res_FP, 17);
  Resource_Requirement (res_FP, 18);
  Resource_Requirement (res_FP, 19);
  Resource_Requirement (res_FP, 20);
  Resource_Requirement (res_FP, 21);
  Resource_Requirement (res_FP, 22);
  Resource_Requirement (res_FP, 23);
  Resource_Requirement (res_FP, 24);
  Resource_Requirement (res_FP, 25);
  Resource_Requirement (res_FP, 26);
  Resource_Requirement (res_FP, 27);
  Resource_Requirement (res_FP, 28);

    /* Dummy instructions */

  Instruction_Group ("Dummy",
			  TOP_asm,
			  TOP_intrncall,
			  TOP_spadjust,
			  TOP_nop,
			  TOP_UNDEFINED);
  Any_Operand_Access_Time(0);
  Any_Result_Available_Time(1);
  Resource_Requirement(res_Issue, 0);

    /* Fictitious instructions requiring no resource and zero time to execute.
     */

  Instruction_Group ("Dummy_no_res",
			  TOP_mov_hilo,
			  TOP_mov_fcc,
                          TOP_swp_start,
                          TOP_swp_stop,
			  TOP_UNDEFINED);
  Any_Operand_Access_Time(0);
  Any_Result_Available_Time(0);

    /* Non-exist instructions.  Not available on mips5kf (Ice9). */
  
  Instruction_Group ("Non-exist",
			TOP_dsbh,
			TOP_dshd,
			TOP_seb,
			TOP_seh,
			TOP_wsbh,
			TOP_dext,
			TOP_dextm,
			TOP_dextu,
			TOP_dins,
			TOP_dinsm,
			TOP_dinsu,
			TOP_drotr,
			TOP_drotr32,
			TOP_drotrv,
			TOP_ext,
			TOP_ins,
			TOP_rotr,
			TOP_rotrv,
			TOP_alnv_ps,
			TOP_movn_ps,
			TOP_movz_ps,
			TOP_abs_ps,
			TOP_c_eq_ps,
			TOP_c_f_ps,
			TOP_c_ge_ps,
			TOP_c_gl_ps,
			TOP_c_gle_ps,
			TOP_c_gt_ps,
			TOP_c_le_ps,
			TOP_c_lt_ps,
			TOP_c_neq_ps,
			TOP_c_nge_ps,
			TOP_c_ngl_ps,
			TOP_c_ngle_ps,
			TOP_c_ngt_ps,
			TOP_c_nle_ps,
			TOP_c_nlt_ps,
			TOP_c_oge_ps,
			TOP_c_ogl_ps,
			TOP_c_ogt_ps,
			TOP_c_ole_ps,
			TOP_c_olt_ps,
			TOP_c_or_ps,
			TOP_c_seq_ps,
			TOP_c_sf_ps,
			TOP_c_sne_ps,
			TOP_c_st_ps,
			TOP_c_t_ps,
			TOP_c_ueq_ps,
			TOP_c_uge_ps,
			TOP_c_ugt_ps,
			TOP_c_ule_ps,
			TOP_c_ult_ps,
			TOP_c_un_ps,
			TOP_cvt_ps_s,
			TOP_cvt_s_pl,
			TOP_cvt_s_pu,
			TOP_mov_ps,
			TOP_movf_ps,
			TOP_movt_ps,
			TOP_neg_ps,
			TOP_pll_ps,
			TOP_plu_ps,
			TOP_pul_ps,
			TOP_puu_ps,
			TOP_add_ps,
			TOP_madd_ps,
			TOP_msub_ps,
			TOP_mul_ps,
			TOP_nmadd_ps,
			TOP_nmsub_ps,
			TOP_sub_ps,
			TOP_UNDEFINED);
  Any_Operand_Access_Time (0);		/* bogus timing */
  Any_Result_Available_Time (1);
  Resource_Requirement (res_Issue, 0);
  Resource_Requirement (res_EX, 0);

  Instruction_Group ("Pipeem__Rep01__Lat01_pref",
			  TOP_pref,
			  TOP_prefx,
			  TOP_UNDEFINED);
  Any_Operand_Access_Time (0);
  Any_Result_Available_Time (16);
  Resource_Requirement (res_Issue, 0);
  Resource_Requirement (res_EX, 0);
  Resource_Requirement (res_Prefetch, 0);
  Resource_Requirement (res_Prefetch, 1);
  Resource_Requirement (res_Prefetch, 2);
  Resource_Requirement (res_Prefetch, 3);
  Resource_Requirement (res_Prefetch, 4);
  Resource_Requirement (res_Prefetch, 5);
  Resource_Requirement (res_Prefetch, 6);
  Resource_Requirement (res_Prefetch, 7);
  Resource_Requirement (res_Prefetch, 8);
  Resource_Requirement (res_Prefetch, 9);
  Resource_Requirement (res_Prefetch, 10);
  Resource_Requirement (res_Prefetch, 11);
  Resource_Requirement (res_Prefetch, 12);
  Resource_Requirement (res_Prefetch, 13);
  Resource_Requirement (res_Prefetch, 14);
  Resource_Requirement (res_Prefetch, 15);

  Machine_Done("mips5kf.c");
}
