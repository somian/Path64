/*
 * Copyright (C) 2007, 2008, 2009 PathScale, LLC.  All Rights Reserved.
 */
/*
 * Copyright (C) 2006. QLogic Corporation. All Rights Reserved.
 */

/* 
   Copyright 2003, 2004, 2005, 2006 PathScale, Inc.  All Rights Reserved.
   File modified June 20, 2003 by PathScale, Inc. to update Open64 C/C++ 
   front-ends to GNU 3.2.2 release.
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


/* translate gnu decl trees to whirl */

#ifndef wfe_util_INCLUDED
#define wfe_util_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

extern int  WGEN_Keep_Zero_Length_Structs;
#ifdef TARG_X8664
extern int Reg_Parm_Count;
extern BOOL SSE_Reg_Parm;
#endif

extern void WGEN_Init (INT argc, char **arrgv, char **envp);
extern void WGEN_Finish (void);
extern void WGEN_File_Init (INT argc, char **argv);
extern void WGEN_File_Finish (void);
extern void WGEN_Check_Errors (int *error_count, int *warning_count, BOOL *need_inliner);
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus

#include "wn.h"
#include "srcpos.h"

typedef enum {
  wgen_stmk_unknown,
  wgen_stmk_func_entry,
  wgen_stmk_func_pragmas,
  wgen_stmk_func_body,
  wgen_stmk_region_pragmas,
  wgen_stmk_region_body,
  wgen_stmk_region_exits,
  wgen_stmk_call_region_pragmas,	// not used currently
  wgen_stmk_call_region_body,
  wgen_stmk_call_region_exits,	// not used currently
  wgen_stmk_scope,
  wgen_stmk_if_cond,
  wgen_stmk_if_then,
  wgen_stmk_if_else,
  wgen_stmk_while_cond,
  wgen_stmk_while_body,
  wgen_stmk_dowhile_cond,
  wgen_stmk_dowhile_body,
  wgen_stmk_for_cond,
  wgen_stmk_for_body,
  wgen_stmk_switch,
  wgen_stmk_comma,
  wgen_stmk_rcomma,
  wgen_stmk_temp_cleanup,
  wgen_stmk_dummy,	// does not generate code
  wgen_stmk_last
} WGEN_STMT_KIND;

extern void WGEN_Stmt_Push (WN* wn, WGEN_STMT_KIND kind, SRCPOS srcpos);
extern WN*  WGEN_Stmt_Top (void);
#ifdef KEY
extern void WGEN_Stmt_Prepend_Last (WN* wn, SRCPOS srcpos);
extern void WGEN_Stmt_Append (WN* wn, SRCPOS srcpos, BOOL call_stmt = TRUE);
#else
extern void WGEN_Stmt_Append (WN* wn, SRCPOS srcpos);
#endif /* KEY */
extern WN*  WGEN_Stmt_Last (void);
extern WN*  WGEN_Stmt_Pop (WGEN_STMT_KIND kind);
extern void WGEN_Guard_Var_Push (void);
extern gs_t WGEN_Guard_Var_Pop (void);
extern gs_t WGEN_Get_Guard_Var (void);
extern bool Check_For_Call_Region (void);
extern bool Did_Not_Terminate_Region;
extern WN * WGEN_Find_Stmt_In_Stack (WGEN_STMT_KIND);
extern void Warning (const char *);

extern BOOL wgen_invoke_inliner;	/* from main.c */
extern char *asm_file_name;		/* from main.c */
extern BOOL TARGET_64BIT;		/* from main.c */
extern int lineno;			/* from main.c */
extern int key_exceptions;		/* from main.c */
extern BOOL opt_regions;		/* from main.c */
extern gs_t program;			/* from main.c */
extern BOOL lang_cplus;			/* from main.c */
#ifdef FE_GNU_4_2_0
#include "wn_util.h"
extern BOOL enable_cxx_openmp;		/* from main.c */
extern void WGEN_add_pragma_to_enclosing_regions (WN_PRAGMA_ID, ST *,
                                                  BOOL = FALSE);
void Add_Pragma_To_MP_Regions (WN_VECTOR *, WN_PRAGMA_ID, ST *, WN_OFFSET,
                               WN_MAP, BOOL, BOOL);
#endif
#if 0
extern char *Spin_File_Name;
extern FILE *Spin_File;
#endif
extern UINT current_file;		/* from wgen_dst.cxx */

// get the srcpos info from the global variable lineno
inline SRCPOS
Get_Srcpos (void)
{
  SRCPOS s;
  SRCPOS_clear(s);
  SRCPOS_filenum(s) = current_file;
  SRCPOS_linenum(s) = lineno; 
  return s;
}

#endif // __cplusplus

#endif
