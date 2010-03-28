/*
 * Copyright (C) 2007. PathScale, LLC. All Rights Reserved.
 */
/*
 * Copyright 2003, 2004, 2005, 2006 PathScale, Inc.  All Rights Reserved.
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
 * Module: cwh_defines.h
 * $Revision: 1.5 $
 * $Date: 04/12/21 14:57:32-08:00 $
 * $Author: bos@eng-25.internal.keyresearch.com $
 * $Source: /home/bos/bk/kpro64-pending/crayf90/sgi/SCCS/s.cwh_defines.h $
 *
 * Revision history:
 *  dd-mmm-95 - Original Version
 *
 * Description: Holds translation defintions used by all modules.
 *
 * ====================================================================
 * ====================================================================
 */

#ifndef CWH_DEFINES_INCLUDED
#define CWH_DEFINES_INCLUDED

#ifdef _KEEP_RCS_ID
static char *rcs_id = "$Source: /home/bos/bk/kpro64-pending/crayf90/sgi/SCCS/s.cwh_defines.h $ $Revision: 1.5 $";
#endif /* _KEEP_RCS_ID */


/* dubious casts and types for use with PDGCS */

typedef INT64 OFFSET_64 ; 
typedef unsigned long ULONG ;
typedef long SLONG ;
#define cast_to_TY(x) ((TY_IDX) (uintptr_t) (void *)x) 
#define cast_to_WN(x) ((WN *) (void *)x) 
#define cast_to_ST(x) ((ST *) (void *)x) 
#define cast_to_LB(x) ((LABEL_IDX) (uintptr_t) (void *)x) 
#define cast_to_uint(x) ((unsigned long) (uintptr_t) (void *)x)
#define cast_to_int(x) ((long ) (uintptr_t) (void *)x)
#define cast_to_void(x) ((void *)x)
#define cast_to_STB(x) ((STB_pkt *) (void *)x)
#define cast_to_FLD(x) ((FLD_IDX) (uintptr_t) (void *)x)
#define cast_to_LABEL(x) ((LABEL_IDX) (uintptr_t) (void *)x)
#define cast_to_ST_IDX(x) ((ST_IDX) (uintptr_t) (void *)x)


/* dump macros */

#define DUMP_TY(ty) dump_ty(ty)
#define DUMP_ST(st) dump_st(st)
#define DUMP_LB(lb) dump_label(lb)
#define DUMP_WN(wn) fdump_tree(stdout,wn);
#define DUMP_STK cwh_stk_dump()


extern void dump_ty (TY_IDX ty);
extern void dump_st (ST * st);
extern void dump_label(LABEL_IDX lb);

/* macros for operations on PDGCS arguments */

#define bit_to_byte(a) (a >> 3)
#define bit_to_byte_WN(a) (cwh_expr_bincalc(OPR_DIV,WN_COPY_Tree(a),WN_CreateIntconst (OPC_U4INTCONST,8)))
#define byte_to_bit(a) (a << 3)
#define test_flag(f,sh) ((INT32)((f >> sh) & 1))

/* Symbol table levels */

#define GLOBAL_LEVEL 1
#define HOST_LEVEL GLOBAL_LEVEL + 1
#define INTERNAL_LEVEL HOST_LEVEL + 1

/* Handy macros */

#define WNOPR(w) (WN_operator(w))
#define WNRTY(w) (WN_rtype(w))
#define IS_GLOBAL_SYMTAB(s) (SYMTAB_level(s) == GLOBAL_LEVEL)
#define IN_HOST_PU (CURRENT_SYMTAB == HOST_LEVEL)
#define IN_NESTED_PU (CURRENT_SYMTAB >= INTERNAL_LEVEL)

#define RESULT_SIZE 16
#define RESULT_ALIGN 8


/*
   A WN and TY pair. The  TY is that of the item the WN addresses because
   when a structure component is addressed,  the TY of the LDA/LDID is the
   structure's, so the component's TY is recorded here.
*/

typedef struct {
  WN * wn ;           
  TY_IDX  ty ;
} W_node ; 

#define W_wn(a) (a.wn)
#define W_ty(a) (a.ty)
#define iW_wn(a) (a->wn)
#define iW_ty(a) (a->ty)


/* 
    structures to associate a list of ST's with a parent
    ST, eg: the elements of a COMMON with the common ST,
    or alternate entry points of a procedure. 
    Just for DST information. 
*/

typedef struct blist {
		ST * element ;
		struct blist * nxt ;
} ITEM ;	

#define I_element(i)  ((i)->element)
#define I_next(i)     ((i)->nxt)

typedef struct clist {
		ITEM * first  ;
		ITEM * last   ;
	        INT32  nitems ;
} LIST ;	

#define L_first(l)	((l)->first)
#define L_last(l)	((l)->last)
#define L_num(l)	((l)->nitems)


/* local constants */

#define OPC_UNKNOWN 0
#define MAX_ARY_DIMS 7
#define NUM_LOG_KINDS 4

#define ANULL -1

/* Memory management */
extern MEM_POOL *FE_Mempool;


/* 
 * Calling convention for structure results. TODO: fix in FE.
 *
 * Would like to defer manipulation of ABI calling convention to BE. Unfortunately
 * both the MIPS and IA64 ABIs want to return small structure results in registers.
 * The BE handles this, but for structure results where the structure contains a 
 * dope vector, the FE doesn't know how to initialize the dope (with eg: n_dim,type,
 * flags), if the function result is not passed as the first dummy argument.
 * It's easier to fix this in the conversion to whirl for now, & the macros
 * below control this. Example...
 *
 *  TYPE VARYING_STRING
 *      CHARACTER,DIMENSION(:),POINTER :: chars
 *  ENDTYPE VARYING_STRING
 *
 *  FUNCTION s_concat_c(....
 *      type(VARYING_STRING)            :: s_concat_c
 *      ALLOCATE(s_concat_c%chars(1:la+lb))
 *      iia = SIZE(s_concat_c%chars)
 *
 * Note this isn't in globals.m, because the size of the dope could be smaller than
 * the struct-in-regs size. (eg: scalar pointer on IA64). In that case, the result
 * will have to be passed as the 1st dummy.
 */

#undef RESULT_SIZE 

#ifdef TARG_MIPS
#define RESULT_SIZE 16
#endif
#ifdef TARG_IA32
#if defined(BUILD_OS_DARWIN)
/* Darwin -m32 returns small structures via registers */
#define RESULT_SIZE 8
#else /* defined(BUILD_OS_DARWIN) */
#define RESULT_SIZE 0
#endif /* defined(BUILD_OS_DARWIN) */
#endif
#ifdef TARG_IA64 
#define RESULT_SIZE 64
#endif
#ifdef TARG_X8664
#define RESULT_SIZE 16
#endif

#define STRUCT_BY_VALUE(ty) ((TY_kind(ty) == KIND_STRUCT)      \
			     && (TY_size(ty) <= RESULT_SIZE)   \
                             && !cwh_types_contains_dope(ty)) 


#endif /* CWH_DEFINES_INCLUDED */

