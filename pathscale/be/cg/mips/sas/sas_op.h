/*
 * Copyright 2005, 2007 PathScale Inc.  All Rights Reserved.
 *

   Path64 is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by
   the Free Software Foundation version 3

   Path64 is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
   or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
   License for more details.

   You should have received a copy of the GNU General Public License
   along with Path64; see the file COPYING.  If not, write to the Free
   Software Foundation, 51 Franklin Street, Fifth Floor, Boston, MA
   02110-1301, USA.

 */

/* OPXs are defined inside mrt.cxx */
#ifdef NO_OPX
#define Load
#define Store
#define Add
#define Move
#define fMove
#define cvt
#define Mpy
#define Branch
#define Shift
#define Logic
#define Trap
#define fAdd
#define fBranch
#define fLoad
#define fStore
#define Ase
#define Ccond
#define IdxLS
#define Mvhl
#define Slt
#endif

#define LATENCY     -1
#define THROUGHPUT  1

#define LAT_mult    4
#define LAT_dmult   11
#define LAT_div     37
#define LAT_ddiv    69
#define LAT_lwc1    3
#define LAT_add     4
#define LAT_div_s   17
#define LAT_div_d   32
#define LAT_sqrt_s  17
#define LAT_sqrt_d  32
#define LAT_madd    4
#define LAT_recip_s 15
#define LAT_recip_d 23
#define LAT_rsqrt_s 19
#define LAT_rsqrt_d 31
#define LAT_cfc1    4
#define LAT_ctc1    4
#define LAT_mfc1    2
#define LAT_mtc1    2
#define LAT_mov     4
#define LAT_cvt4    4
#define LAT_cvt5    5
#define LAT_cvt6    6
#define LAT_ceil    5
#define LAT_bc1     2

#define TPUT_mult    2
#define TPUT_dmult   9
#define TPUT_div     36
#define TPUT_ddiv    68

#define TPUT_div_s   14
#define TPUT_div_d   29
#define TPUT_sqrt_s  14
#define TPUT_sqrt_d  29
#define TPUT_recip_s 10
#define TPUT_recip_d 21
#define TPUT_rsqrt_s 16
#define TPUT_rsqrt_d 28

#define ATTR_jump (ATTR_xfer | ATTR_ndual)

/* op( opcode, opcode_id, opcode_string, opx, special_latency, attr_flag, throughput ) */
op( STOP_lb,          0, "lb",        Load, LATENCY,  ATTR_load,  THROUGHPUT )
op( STOP_lbu,         1, "lbu",       Load, LATENCY,  ATTR_load,  THROUGHPUT )
op( STOP_lh,          2, "lh",        Load, LATENCY,  ATTR_load,  THROUGHPUT )
op( STOP_lhu,         3, "lhu",       Load, LATENCY,  ATTR_load,  THROUGHPUT )
op( STOP_lw,          4, "lw",        Load, LATENCY,  ATTR_load,  THROUGHPUT )
op( STOP_lwl,         5, "lwl",       Load, LATENCY,  ATTR_load,  THROUGHPUT )
op( STOP_lwr,         6, "lwr",       Load, LATENCY,  ATTR_load,  THROUGHPUT )
op( STOP_sb,          7, "sb",       Store, LATENCY,  ATTR_store, THROUGHPUT )
op( STOP_sh,          8, "sh",       Store, LATENCY,  ATTR_store, THROUGHPUT )
op( STOP_sw,          9, "sw",       Store, LATENCY,  ATTR_store, THROUGHPUT )
op( STOP_swl,        10, "swl",      Store, LATENCY,  ATTR_store, THROUGHPUT )
op( STOP_swr,        11, "swr",      Store, LATENCY,  ATTR_store, THROUGHPUT )
op( STOP_ll,         12, "ll",        Load, LATENCY,  ATTR_load,  THROUGHPUT )
op( STOP_sc,         13, "sc",       Store, LATENCY,  ATTR_store, THROUGHPUT )
op( STOP_sync,       14, "sync",      Trap, LATENCY,  ATTR,       THROUGHPUT )
op( STOP_lwu,        15, "lwu",       Load, LATENCY,  ATTR_load,  THROUGHPUT )
op( STOP_ld,         16, "ld",        Load, LATENCY,  ATTR_load,  THROUGHPUT )
op( STOP_ldl,        17, "ldl",       Load, LATENCY,  ATTR_load,  THROUGHPUT )
op( STOP_ldr,        18, "ldr",       Load, LATENCY,  ATTR_load,  THROUGHPUT )
op( STOP_lld,        19, "lld",       Load, LATENCY,  ATTR_load,  THROUGHPUT )
op( STOP_sd,         20, "sd",       Store, LATENCY,  ATTR_store, THROUGHPUT )
op( STOP_sdl,        21, "sdl",      Store, LATENCY,  ATTR_store, THROUGHPUT )
op( STOP_sdr,        22, "sdr",      Store, LATENCY,  ATTR_store, THROUGHPUT )
op( STOP_scd,        23, "scd",      Store, LATENCY,  ATTR_store, THROUGHPUT )
op( STOP_pref,       24, "pref",      Load, LATENCY,  ATTR_load,  THROUGHPUT )
op( STOP_prefx,      25, "prefx",     Load, LATENCY,  ATTR_load,  THROUGHPUT )
op( STOP_add,        26, "add",        Add, LATENCY,  ATTR,       THROUGHPUT )
op( STOP_addi,       27, "addi",       Add, LATENCY,  ATTR,       THROUGHPUT )
op( STOP_addiu,      28, "addiu",      Add, LATENCY,  ATTR,       THROUGHPUT )
op( STOP_addu,       29, "addu",       Add, LATENCY,  ATTR,       THROUGHPUT )
op( STOP_div,        30, "div",        Mpy, LAT_div,  ATTR_div,   TPUT_div )
op( STOP_divu,       31, "divu",       Mpy, LAT_div,  ATTR_div,   TPUT_div )
op( STOP_mult,       32, "mult",       Mpy, LAT_mult, ATTR_mult,  TPUT_mult )
op( STOP_multu,      33, "multu",      Mpy, LAT_mult, ATTR_mult,  TPUT_mult )
op( STOP_slt,        34, "slt",        Slt, LATENCY,  ATTR_cmp,   THROUGHPUT )
op( STOP_slti,       35, "slti",       Slt, LATENCY,  ATTR_cmp,   THROUGHPUT )
op( STOP_sltiu,      36, "sltiu",      Slt, LATENCY,  ATTR_cmp,   THROUGHPUT )
op( STOP_sltu,       37, "sltu",       Slt, LATENCY,  ATTR_cmp,   THROUGHPUT )
op( STOP_sub,        38, "sub",        Add, LATENCY,  ATTR,       THROUGHPUT )
op( STOP_subu,       39, "subu",       Add, LATENCY,  ATTR,       THROUGHPUT )
op( STOP_dadd,       40, "dadd",       Add, LATENCY,  ATTR_op64,  THROUGHPUT )
op( STOP_daddi,      41, "daddi",      Add, LATENCY,  ATTR_op64,  THROUGHPUT )
op( STOP_daddiu,     42, "daddiu",     Add, LATENCY,  ATTR_op64,  THROUGHPUT )
op( STOP_daddu,      43, "daddu",      Add, LATENCY,  ATTR_op64,  THROUGHPUT )
op( STOP_ddiv,       44, "ddiv",       Mpy, LAT_ddiv, ATTR_div|ATTR_op64, TPUT_ddiv )
op( STOP_ddivu,      45, "ddivu",      Mpy, LAT_ddiv, ATTR_div|ATTR_op64, TPUT_ddiv )
op( STOP_dmult,      46, "dmult",      Mpy, LAT_dmult, ATTR_mult|ATTR_op64, TPUT_dmult )
op( STOP_dmultu,     47, "dmultu",     Mpy, LAT_dmult, ATTR_mult|ATTR_op64, TPUT_dmult )
op( STOP_dsub,       48, "dsub",       Add, LATENCY,  ATTR_op64,  THROUGHPUT )
op( STOP_dsubu,      49, "dsubu",      Add, LATENCY,  ATTR_op64,  THROUGHPUT )
op( STOP_and,        50, "and",      Logic, LATENCY,  ATTR,       THROUGHPUT )
op( STOP_andi,       51, "andi",     Logic, LATENCY,  ATTR,       THROUGHPUT )
op( STOP_lui,        52, "lui",        Add, LATENCY,  ATTR,       THROUGHPUT )
op( STOP_nor,        53, "nor",      Logic, LATENCY,  ATTR,       THROUGHPUT )
op( STOP_or,         54, "or",       Logic, LATENCY,  ATTR,       THROUGHPUT )
op( STOP_ori,        55, "ori",      Logic, LATENCY,  ATTR,       THROUGHPUT )
op( STOP_xor,        56, "xor",      Logic, LATENCY,  ATTR,       THROUGHPUT )
op( STOP_xori,       57, "xori",     Logic, LATENCY,  ATTR,       THROUGHPUT )
op( STOP_mfhi,       58, "mfhi",      Mvhl, LATENCY,  ATTR_mf,    THROUGHPUT )
op( STOP_mflo,       59, "mflo",      Mvhl, LATENCY,  ATTR_mf,    THROUGHPUT )
op( STOP_mthi,       60, "mthi",      Mvhl, LATENCY,  ATTR_mt,    THROUGHPUT )
op( STOP_mtlo,       61, "mtlo",      Mvhl, LATENCY,  ATTR_mt,    THROUGHPUT )
op( STOP_movf,       62, "movf",      Move, LATENCY,  ATTR,       THROUGHPUT )
op( STOP_movn,       63, "movn",      Move, LATENCY,  ATTR,       THROUGHPUT )
op( STOP_movt,       64, "movt",      Move, LATENCY,  ATTR,       THROUGHPUT )
op( STOP_movz,       65, "movz",      Move, LATENCY,  ATTR,       THROUGHPUT )
op( STOP_sll,        66, "sll",      Shift, LATENCY,  ATTR,       THROUGHPUT )
op( STOP_sllv,       67, "sllv",     Shift, LATENCY,  ATTR,       THROUGHPUT )
op( STOP_sra,        68, "sra",      Shift, LATENCY,  ATTR,       THROUGHPUT )
op( STOP_srav,       69, "srav",     Shift, LATENCY,  ATTR,       THROUGHPUT )
op( STOP_srl,        70, "srl",      Shift, LATENCY,  ATTR,       THROUGHPUT )
op( STOP_srlv,       71, "srlv",     Shift, LATENCY,  ATTR,       THROUGHPUT )
op( STOP_dsll,       72, "dsll",     Shift, LATENCY,  ATTR_op64,  THROUGHPUT )
op( STOP_dsll32,     73, "dsll32",   Shift, LATENCY,  ATTR_op64,  THROUGHPUT )
op( STOP_dsllv,      74, "dsllv",    Shift, LATENCY,  ATTR_op64,  THROUGHPUT )
op( STOP_dsra,       75, "dsra",     Shift, LATENCY,  ATTR_op64,  THROUGHPUT )
op( STOP_dsra32,     76, "dsra32",   Shift, LATENCY,  ATTR_op64,  THROUGHPUT )
op( STOP_dsrav,      77, "dsrav",    Shift, LATENCY,  ATTR_op64,  THROUGHPUT )
op( STOP_dsrl,       78, "dsrl",     Shift, LATENCY,  ATTR_op64,  THROUGHPUT )
op( STOP_dsrl32,     79, "dsrl32",   Shift, LATENCY,  ATTR_op64,  THROUGHPUT )
op( STOP_dsrlv,      80, "dsrlv",    Shift, LATENCY,  ATTR_op64,  THROUGHPUT )
op( STOP_beq,        81, "beq",     Branch, LATENCY,  ATTR_jump,  THROUGHPUT )
op( STOP_bgez,       82, "bgez",    Branch, LATENCY,  ATTR_jump,  THROUGHPUT )
op( STOP_bgezal,     83, "bgezal",  Branch, LATENCY,  ATTR_jump,  THROUGHPUT )
op( STOP_bgtz,       84, "bgtz",    Branch, LATENCY,  ATTR_jump,  THROUGHPUT )
op( STOP_blez,       85, "blez",    Branch, LATENCY,  ATTR_jump,  THROUGHPUT )
op( STOP_bltz,       86, "bltz",    Branch, LATENCY,  ATTR_jump,  THROUGHPUT )
op( STOP_bltzal,     87, "bltzal",  Branch, LATENCY,  ATTR_jump,  THROUGHPUT )
op( STOP_bne,        88, "bne",     Branch, LATENCY,  ATTR_jump,  THROUGHPUT )
op( STOP_j,          89, "j",       Branch, LATENCY,  ATTR_jump,  THROUGHPUT )
op( STOP_jal,        90, "jal",     Branch, LATENCY,  ATTR_jump,  THROUGHPUT )
op( STOP_jalr,       91, "jalr",    Branch, LATENCY,  ATTR_jump,  THROUGHPUT )
op( STOP_jr,         92, "jr",      Branch, LATENCY,  ATTR_jump,  THROUGHPUT )
op( STOP_break,      93, "break",     Trap, LATENCY,  ATTR_jump,  THROUGHPUT )
op( STOP_syscall,    94, "syscall",   Trap, LATENCY,  ATTR,       THROUGHPUT )
op( STOP_teq,        95, "teq",       Trap, LATENCY,  ATTR,       THROUGHPUT )
op( STOP_teqi,       96, "teqi",      Trap, LATENCY,  ATTR,       THROUGHPUT )
op( STOP_tge,        97, "tge",       Trap, LATENCY,  ATTR,       THROUGHPUT )
op( STOP_tgei,       98, "tgei",      Trap, LATENCY,  ATTR,       THROUGHPUT )
op( STOP_tgeiu,      99, "tgeiu",     Trap, LATENCY,  ATTR,       THROUGHPUT )
op( STOP_tgeu,      100, "tgeu",      Trap, LATENCY,  ATTR,       THROUGHPUT )
op( STOP_tlt,       101, "tlt",       Trap, LATENCY,  ATTR,       THROUGHPUT )
op( STOP_tlti,      102, "tlti",      Trap, LATENCY,  ATTR,       THROUGHPUT )
op( STOP_tltiu,     103, "tltiu",     Trap, LATENCY,  ATTR,       THROUGHPUT )
op( STOP_tltu,      104, "tltu",      Trap, LATENCY,  ATTR,       THROUGHPUT )
op( STOP_tne,       105, "tne",       Trap, LATENCY,  ATTR,       THROUGHPUT )
op( STOP_tnei,      106, "tnei",      Trap, LATENCY,  ATTR,       THROUGHPUT )
op( STOP_lwc1,      107, "lwc1",     fLoad, LAT_lwc1, ATTR_load,  THROUGHPUT )
op( STOP_ldc1,      108, "ldc1",     fLoad, LAT_lwc1, ATTR_load,  THROUGHPUT )
op( STOP_lwxc1,     109, "lwxc1",    IdxLS, LAT_lwc1, ATTR_load,  THROUGHPUT )
op( STOP_ldxc1,     110, "ldxc1",    IdxLS, LAT_lwc1, ATTR_load,  THROUGHPUT )
op( STOP_swc1,      111, "swc1",    fStore, LATENCY,  ATTR_store, THROUGHPUT )
op( STOP_sdc1,      112, "sdc1",    fStore, LATENCY,  ATTR_store, THROUGHPUT )
op( STOP_swxc1,     113, "swxc1",    IdxLS, LATENCY,  ATTR_store, THROUGHPUT )
op( STOP_sdxc1,     114, "sdxc1",    IdxLS, LATENCY,  ATTR_store, THROUGHPUT )
op( STOP_abs_s,     115, "abs.s",     fAdd, LAT_add,  ATTR,       THROUGHPUT )
op( STOP_abs_d,     116, "abs.d",     fAdd, LAT_add,  ATTR,       THROUGHPUT )
op( STOP_add_s,     117, "add.s",     fAdd, LAT_add,  ATTR,       THROUGHPUT )
op( STOP_add_d,     118, "add.d",     fAdd, LAT_add,  ATTR,       THROUGHPUT )
op( STOP_c_f_s,     119, "c.f.s",    Ccond, LATENCY,  ATTR,       THROUGHPUT )
op( STOP_c_f_d,     120, "c.f.d",    Ccond, LATENCY,  ATTR,       THROUGHPUT )
op( STOP_c_t_s,     121, "c.t.s",    Ccond, LATENCY,  ATTR,       THROUGHPUT )
op( STOP_c_t_d,     122, "c.t.d",    Ccond, LATENCY,  ATTR,       THROUGHPUT )
op( STOP_c_un_s,    123, "c.un.s",   Ccond, LATENCY,  ATTR,       THROUGHPUT )
op( STOP_c_un_d,    124, "c.un.d",   Ccond, LATENCY,  ATTR,       THROUGHPUT )
op( STOP_c_or_s,    125, "c.or.s",   Ccond, LATENCY,  ATTR,       THROUGHPUT )
op( STOP_c_or_d,    126, "c.or.d",   Ccond, LATENCY,  ATTR,       THROUGHPUT )
op( STOP_c_eq_s,    127, "c.eq.s",   Ccond, LATENCY,  ATTR,       THROUGHPUT )
op( STOP_c_eq_d,    128, "c.eq.d",   Ccond, LATENCY,  ATTR,       THROUGHPUT )
op( STOP_c_neq_s,   129, "c.neq.s",  Ccond, LATENCY,  ATTR,       THROUGHPUT )
op( STOP_c_neq_d,   130, "c.neq.d",  Ccond, LATENCY,  ATTR,       THROUGHPUT )
op( STOP_c_ueq_s,   131, "c.ueq.s",  Ccond, LATENCY,  ATTR,       THROUGHPUT )
op( STOP_c_ueq_d,   132, "c.ueq.d",  Ccond, LATENCY,  ATTR,       THROUGHPUT )
op( STOP_c_olg_s,   133, "c.olg.s",  Ccond, LATENCY,  ATTR,       THROUGHPUT )
op( STOP_c_olg_d,   134, "c.olg.d",  Ccond, LATENCY,  ATTR,       THROUGHPUT )
op( STOP_c_olt_s,   135, "c.olt.s",  Ccond, LATENCY,  ATTR,       THROUGHPUT )
op( STOP_c_olt_d,   136, "c.olt.d",  Ccond, LATENCY,  ATTR,       THROUGHPUT )
op( STOP_c_uge_s,   137, "c.uge.s",  Ccond, LATENCY,  ATTR,       THROUGHPUT )
op( STOP_c_uge_d,   138, "c.uge.d",  Ccond, LATENCY,  ATTR,       THROUGHPUT )
op( STOP_c_ult_s,   139, "c.ult.s",  Ccond, LATENCY,  ATTR,       THROUGHPUT )
op( STOP_c_ult_d,   140, "c.ult.d",  Ccond, LATENCY,  ATTR,       THROUGHPUT )
op( STOP_c_oge_s,   141, "c.oge.s",  Ccond, LATENCY,  ATTR,       THROUGHPUT )
op( STOP_c_oge_d,   142, "c.oge.d",  Ccond, LATENCY,  ATTR,       THROUGHPUT )
op( STOP_c_ole_s,   143, "c.ole.s",  Ccond, LATENCY,  ATTR,       THROUGHPUT )
op( STOP_c_ole_d,   144, "c.ole.d",  Ccond, LATENCY,  ATTR,       THROUGHPUT )
op( STOP_c_ugt_s,   145, "c.ugt.s",  Ccond, LATENCY,  ATTR,       THROUGHPUT )
op( STOP_c_ugt_d,   146, "c.ugt.d",  Ccond, LATENCY,  ATTR,       THROUGHPUT )
op( STOP_c_ule_s,   147, "c.ule.s",  Ccond, LATENCY,  ATTR,       THROUGHPUT )
op( STOP_c_ule_d,   148, "c.ule.d",  Ccond, LATENCY,  ATTR,       THROUGHPUT )
op( STOP_c_ogt_s,   149, "c.ogt.s",  Ccond, LATENCY,  ATTR,       THROUGHPUT )
op( STOP_c_ogt_d,   150, "c.ogt.d",  Ccond, LATENCY,  ATTR,       THROUGHPUT )
op( STOP_c_sf_s,    151, "c.sf.s",   Ccond, LATENCY,  ATTR,       THROUGHPUT )
op( STOP_c_sf_d,    152, "c.sf.d",   Ccond, LATENCY,  ATTR,       THROUGHPUT )
op( STOP_c_st_s,    153, "c.st.s",   Ccond, LATENCY,  ATTR,       THROUGHPUT )
op( STOP_c_st_d,    154, "c.st.d",   Ccond, LATENCY,  ATTR,       THROUGHPUT )
op( STOP_c_ngle_s,  155, "c.ngle.s", Ccond, LATENCY,  ATTR,       THROUGHPUT )
op( STOP_c_ngle_d,  156, "c.ngle.d", Ccond, LATENCY,  ATTR,       THROUGHPUT )
op( STOP_c_gle_s,   157, "c.gle.s",  Ccond, LATENCY,  ATTR,       THROUGHPUT )
op( STOP_c_gle_d,   158, "c.gle.d",  Ccond, LATENCY,  ATTR,       THROUGHPUT )
op( STOP_c_seq_s,   159, "c.seq.s",  Ccond, LATENCY,  ATTR,       THROUGHPUT )
op( STOP_c_seq_d,   160, "c.seq.d",  Ccond, LATENCY,  ATTR,       THROUGHPUT )
op( STOP_c_sne_s,   161, "c.sne.s",  Ccond, LATENCY,  ATTR,       THROUGHPUT )
op( STOP_c_sne_d,   162, "c.sne.d",  Ccond, LATENCY,  ATTR,       THROUGHPUT )
op( STOP_c_ngl_s,   163, "c.ngl.s",  Ccond, LATENCY,  ATTR,       THROUGHPUT )
op( STOP_c_ngl_d,   164, "c.ngl.d",  Ccond, LATENCY,  ATTR,       THROUGHPUT )
op( STOP_c_gl_s,    165, "c.gl.s",   Ccond, LATENCY,  ATTR,       THROUGHPUT )
op( STOP_c_gl_d,    166, "c.gl.d",   Ccond, LATENCY,  ATTR,       THROUGHPUT )
op( STOP_c_lt_s,    167, "c.lt.s",   Ccond, LATENCY,  ATTR,       THROUGHPUT )
op( STOP_c_lt_d,    168, "c.lt.d",   Ccond, LATENCY,  ATTR,       THROUGHPUT )
op( STOP_c_nlt_s,   169, "c.nlt.s",  Ccond, LATENCY,  ATTR,       THROUGHPUT )
op( STOP_c_nlt_d,   170, "c.nlt.d",  Ccond, LATENCY,  ATTR,       THROUGHPUT )
op( STOP_c_nge_s,   171, "c.nge.s",  Ccond, LATENCY,  ATTR,       THROUGHPUT )
op( STOP_c_nge_d,   172, "c.nge.d",  Ccond, LATENCY,  ATTR,       THROUGHPUT )
op( STOP_c_ge_s,    173, "c.ge.s",   Ccond, LATENCY,  ATTR,       THROUGHPUT )
op( STOP_c_ge_d,    174, "c.ge.d",   Ccond, LATENCY,  ATTR,       THROUGHPUT )
op( STOP_c_le_s,    175, "c.le.s",   Ccond, LATENCY,  ATTR,       THROUGHPUT )
op( STOP_c_le_d,    176, "c.le.d",   Ccond, LATENCY,  ATTR,       THROUGHPUT )
op( STOP_c_nle_s,   177, "c.nle.s",  Ccond, LATENCY,  ATTR,       THROUGHPUT )
op( STOP_c_nle_d,   178, "c.nle.d",  Ccond, LATENCY,  ATTR,       THROUGHPUT )
op( STOP_c_ngt_s,   179, "c.ngt.s",  Ccond, LATENCY,  ATTR,       THROUGHPUT )
op( STOP_c_ngt_d,   180, "c.ngt.d",  Ccond, LATENCY,  ATTR,       THROUGHPUT )
op( STOP_c_gt_s,    181, "c.gt.s",   Ccond, LATENCY,  ATTR,       THROUGHPUT )
op( STOP_c_gt_d,    182, "c.gt.d",   Ccond, LATENCY,  ATTR,       THROUGHPUT )
op( STOP_div_s,     183, "div.s",     fAdd, LAT_div_s, ATTR_fdiv, TPUT_div_s )
op( STOP_div_d,     184, "div.d",     fAdd, LAT_div_d, ATTR_fdiv, TPUT_div_d )
op( STOP_mul_s,     185, "mul.s",     fAdd, LAT_madd, ATTR_fmult, THROUGHPUT )
op( STOP_mul_d,     186, "mul.d",     fAdd, LAT_madd, ATTR_fmult, THROUGHPUT )
op( STOP_neg_s,     187, "neg.s",     fAdd, LAT_add,  ATTR,       THROUGHPUT )
op( STOP_neg_d,     188, "neg.d",     fAdd, LAT_add,  ATTR,       THROUGHPUT )
op( STOP_sub_s,     189, "sub.s",     fAdd, LAT_add,  ATTR,       THROUGHPUT )
op( STOP_sub_d,     190, "sub.d",     fAdd, LAT_add,  ATTR,       THROUGHPUT )
op( STOP_sqrt_s,    191, "sqrt.s",    fAdd, LAT_sqrt_s, ATTR,    TPUT_sqrt_s )
op( STOP_sqrt_d,    192, "sqrt.d",    fAdd, LAT_sqrt_d, ATTR,    TPUT_sqrt_d )
op( STOP_madd_s,    193, "madd.s",    fAdd, LAT_madd, ATTR_madd,  THROUGHPUT )
op( STOP_madd_d,    194, "madd.d",    fAdd, LAT_madd, ATTR_madd,  THROUGHPUT )
op( STOP_msub_s,    195, "msub.s",    fAdd, LAT_madd, ATTR_madd,  THROUGHPUT )
op( STOP_msub_d,    196, "msub.d",    fAdd, LAT_madd, ATTR_madd,  THROUGHPUT )
op( STOP_nmadd_s,   197, "nmadd.s",   fAdd, LAT_madd, ATTR_madd,  THROUGHPUT )
op( STOP_nmadd_d,   198, "nmadd.d",   fAdd, LAT_madd, ATTR_madd,  THROUGHPUT )
op( STOP_nmsub_s,   199, "nmsub.s",   fAdd, LAT_madd, ATTR_madd,  THROUGHPUT )
op( STOP_nmsub_d,   200, "nmsub.d",   fAdd, LAT_madd, ATTR_madd,  THROUGHPUT )
op( STOP_recip_s,   201, "recip.s",   fAdd, LAT_recip_s, ATTR,  TPUT_recip_s )
op( STOP_recip_d,   202, "recip.d",   fAdd, LAT_recip_d, ATTR,  TPUT_recip_d )
op( STOP_rsqrt_s,   203, "rsqrt.s",   fAdd, LAT_rsqrt_s, ATTR,  TPUT_rsqrt_s )
op( STOP_rsqrt_d,   204, "rsqrt.d",   fAdd, LAT_rsqrt_d, ATTR,  TPUT_rsqrt_d )
op( STOP_cfc1,      205, "cfc1",     fMove, LAT_cfc1, ATTR_cfc1|ATTR_ndual, THROUGHPUT )
op( STOP_ctc1,      206, "ctc1",     fMove, LAT_ctc1, ATTR_ndual, THROUGHPUT )
op( STOP_mfc1,      207, "mfc1",     fMove, LAT_mfc1, ATTR,       THROUGHPUT )
op( STOP_mtc1,      208, "mtc1",     fMove, LAT_mtc1, ATTR_mtc1,  THROUGHPUT )
op( STOP_dmfc1,     209, "dmfc1",    fMove, LAT_mfc1, ATTR,       THROUGHPUT )
op( STOP_dmtc1,     210, "dmtc1",    fMove, LAT_mtc1, ATTR_mtc1,  THROUGHPUT )
op( STOP_mov_s,     211, "mov.s",    fMove, LAT_mov,  ATTR,       THROUGHPUT )
op( STOP_mov_d,     212, "mov.d",    fMove, LAT_mov,  ATTR,       THROUGHPUT )
op( STOP_movf_s,    213, "movf.s",   fMove, LAT_mov,  ATTR,       THROUGHPUT )
op( STOP_movf_d,    214, "movf.d",   fMove, LAT_mov,  ATTR,       THROUGHPUT )
op( STOP_movn_s,    215, "movn.s",   fMove, LAT_mov,  ATTR,       THROUGHPUT )
op( STOP_movn_d,    216, "movn.d",   fMove, LAT_mov,  ATTR,       THROUGHPUT )
op( STOP_movt_s,    217, "movt.s",   fMove, LAT_mov,  ATTR,       THROUGHPUT )
op( STOP_movt_d,    218, "movt.d",   fMove, LAT_mov,  ATTR,       THROUGHPUT )
op( STOP_movz_s,    219, "movz.s",   fMove, LAT_mov,  ATTR,       THROUGHPUT )
op( STOP_movz_d,    220, "movz.d",   fMove, LAT_mov,  ATTR,       THROUGHPUT )
op( STOP_cvt_d_s,   221, "cvt.d.s",    cvt, LAT_cvt4, ATTR,       THROUGHPUT )
op( STOP_cvt_d_w,   222, "cvt.d.w",    cvt, LAT_cvt4, ATTR,       THROUGHPUT )
op( STOP_cvt_d_l,   223, "cvt.d.l",    cvt, LAT_cvt4, ATTR,       THROUGHPUT )
op( STOP_cvt_l_s,   224, "cvt.l.s",    cvt, LAT_cvt5, ATTR,       THROUGHPUT )
op( STOP_cvt_l_d,   225, "cvt.l.d",    cvt, LAT_cvt5, ATTR,       THROUGHPUT )
op( STOP_cvt_s_d,   226, "cvt.s.d",    cvt, LAT_cvt6, ATTR,       THROUGHPUT )
op( STOP_cvt_s_w,   227, "cvt.s.w",    cvt, LAT_cvt4, ATTR,       THROUGHPUT )
op( STOP_cvt_s_l,   228, "cvt.s.l",    cvt, LAT_cvt4, ATTR,       THROUGHPUT )
op( STOP_cvt_w_s,   229, "cvt.w.s",    cvt, LAT_cvt5, ATTR,       THROUGHPUT )
op( STOP_cvt_w_d,   230, "cvt.w.d",    cvt, LAT_cvt5, ATTR,       THROUGHPUT )
op( STOP_ceil_w_s,  231, "ceil.w.s",   cvt, LAT_ceil, ATTR,       THROUGHPUT )
op( STOP_ceil_w_d,  232, "ceil.w.d",   cvt, LAT_ceil, ATTR,       THROUGHPUT )
op( STOP_ceil_l_s,  233, "ceil.l.s",   cvt, LAT_ceil, ATTR,       THROUGHPUT )
op( STOP_ceil_l_d,  234, "ceil.l.d",   cvt, LAT_ceil, ATTR,       THROUGHPUT )
op( STOP_floor_w_s, 235, "floor.w.s",  cvt, LAT_ceil, ATTR,       THROUGHPUT )
op( STOP_floor_w_d, 236, "floor.w.d",  cvt, LAT_ceil, ATTR,       THROUGHPUT )
op( STOP_floor_l_s, 237, "floor.l.s",  cvt, LAT_ceil, ATTR,       THROUGHPUT )
op( STOP_floor_l_d, 238, "floor.l.d",  cvt, LAT_ceil, ATTR,       THROUGHPUT )
op( STOP_round_w_s, 239, "round.w.s",  cvt, LAT_ceil, ATTR,       THROUGHPUT )
op( STOP_round_w_d, 240, "round.w.d",  cvt, LAT_ceil, ATTR,       THROUGHPUT )
op( STOP_round_l_s, 241, "round.l.s",  cvt, LAT_ceil, ATTR,       THROUGHPUT )
op( STOP_round_l_d, 242, "round.l.d",  cvt, LAT_ceil, ATTR,       THROUGHPUT )
op( STOP_trunc_w_s, 243, "trunc.w.s",  cvt, LAT_ceil, ATTR,       THROUGHPUT )
op( STOP_trunc_w_d, 244, "trunc.w.d",  cvt, LAT_ceil, ATTR,       THROUGHPUT )
op( STOP_trunc_l_s, 245, "trunc.l.s",  cvt, LAT_ceil, ATTR,       THROUGHPUT )
op( STOP_trunc_l_d, 246, "trunc.l.d",  cvt, LAT_ceil, ATTR,       THROUGHPUT )
op( STOP_bc1f,      247, "bc1f",   fBranch, LAT_bc1,  ATTR_jump,  THROUGHPUT )
op( STOP_bc1t,      248, "bc1t",   fBranch, LAT_bc1,  ATTR_jump,  THROUGHPUT )
op( STOP_sext,      249, "sext",     Shift, LATENCY,  ATTR,       THROUGHPUT )
op( STOP_asm,       250, "asm",       Trap, LATENCY,  ATTR,       THROUGHPUT )
op( STOP_intrncall, 251, "intrncall", Error, LATENCY, ATTR,       THROUGHPUT )
op( STOP_spadjust,  252, "spadjust", Error, LATENCY,  ATTR,       THROUGHPUT )
op( STOP_begin_pregtn, 253, "begin_pregtn", Error, LATENCY, ATTR, THROUGHPUT )
op( STOP_end_pregtn, 254, "end_pregtn", Error, LATENCY, ATTR,     THROUGHPUT )
op( STOP_bwd_bar,   255, "bwd_bar",  Error, LATENCY,  ATTR,       THROUGHPUT )
op( STOP_fwd_bar,   256, "fwd_bar",  Error, LATENCY,  ATTR,       THROUGHPUT )
op( STOP_dfixup,    257, "dfixup",   Error, LATENCY,  ATTR,       THROUGHPUT )
op( STOP_ffixup,    258, "ffixup",   Error, LATENCY,  ATTR,       THROUGHPUT )
op( STOP_ifixup,    259, "ifixup",   Error, LATENCY,  ATTR,       THROUGHPUT )
op( STOP_label,     260, "label",    Error, LATENCY,  ATTR,       THROUGHPUT )
op( STOP_nop,       261, "nop",        Add, LATENCY,  ATTR,       THROUGHPUT )
op( STOP_noop,      262, "noop",       Add, LATENCY,  ATTR,       THROUGHPUT )
op( STOP_unknown,   263, "unknown",  Error, LATENCY,  ATTR,       THROUGHPUT )


#undef op

#ifdef NO_OPX
#undef NO_OPX
#undef Load
#undef Store
#undef Add
#undef Move
#undef fMove
#undef cvt
#undef Mpy
#undef Branch
#undef Shift
#undef Logic
#undef Trap
#undef fAdd
#undef fBranch
#undef Ase
#undef fLoad
#undef fStore
#undef Ccond
#undef IdxLS
#undef Mvhl
#undef Slt
#endif
