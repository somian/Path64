/*
 * Copyright (C) 2008 PathScale, LLC.  All Rights Reserved.
 */

/*
 * Copyright (C) 2007 QLogic Corporation.  All Rights Reserved.
 */

/*
 * Copyright 2003, 2004, 2005 PathScale, Inc.  All Rights Reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it would be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Further, this software is distributed without any warranty that it is
 * free of the rightful claim of any third person regarding infringement
 * or the like.  Any license provided herein, whether implied or
 * otherwise, applies only to this software file.  Patent licenses, if
 * any, provided herein do not apply to combinations of this program with
 * other software, or any other product whatsoever.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write the Free Software Foundation, Inc., 59
 * Temple Place - Suite 330, Boston MA 02111-1307, USA.
 *
 */

#include "defs.h"
#include "wn.h"
#include "wn_util.h"

// Returns function ST for 'name' with return type 'type'.
static ST * new_func_st (const char * name, TYPE_ID type = MTYPE_FQ)
{
  PU_IDX pu_idx;
  PU& new_pu = New_PU (pu_idx);
  PU_Init (new_pu, MTYPE_TO_TY_array[type] , GLOBAL_SYMTAB+1);
  ST * st = New_ST (GLOBAL_SYMTAB);
  ST_Init (st, Save_Str(name), CLASS_FUNC, SCLASS_EXTERN,
           EXPORT_PREEMPTIBLE, TY_IDX (pu_idx));

  return st;
}

// The em_quad_* functions below emulate quad precision for mips by
// calling appropriate library functions from libgcc.
//


// Handles CVT.
extern WN *em_quad_cvt(WN *block, WN *tree)
{
  WN * wn = NULL;
  const char * libfunc = NULL;

  Is_True (WN_operator (tree) == OPR_CVT,
           ("em_quad_cvt: cvt operator expected"));

  Is_True (MTYPE_is_quad(WN_rtype(tree)) ||
           MTYPE_is_quad(WN_desc(tree)),
           ("em_quad_cvt: quad cvt expected"));

  if (MTYPE_is_quad (WN_rtype (tree)))
    switch (WN_desc(tree)) // convert to quad
    {
      case MTYPE_U4:
      case MTYPE_I4:
        libfunc = "__floatsitf";
        break;
      case MTYPE_U8:
      case MTYPE_I8:
        libfunc = "__floatditf";
        break;
      case MTYPE_F4:
        libfunc = "__extendsftf2";
        break;
      case MTYPE_F8:
        libfunc = "__extenddftf2";
        break;
      default:
        Fail_FmtAssertion ("em_quad_cvt: CVT with quad rtype");
    }
  else
    switch (WN_rtype(tree)) // convert from quad
    {
      case MTYPE_F4:
        libfunc = "__trunctfsf2";
        break;
      case MTYPE_F8:
        libfunc = "__trunctfdf2";
        break;
      default:
        Fail_FmtAssertion ("em_quad_cvt: CVT with quad rtype");
    }

  Is_True (libfunc, ("em_quad_cvt: NULL library function"));

  ST * st = new_func_st (libfunc);
  WN * call = WN_Create (OPR_CALL, WN_rtype (tree), MTYPE_V, 1);
  WN_st_idx (call) = ST_st_idx (st);
  WN * opnd = WN_kid (tree, 0);
  // Set function argument
  WN_kid(call,0) = WN_CreateParm (WN_rtype (opnd),
                                  opnd,
                                  MTYPE_TO_TY_array[WN_rtype (opnd)],
                                  0);
  WN_INSERT_BlockLast (block, call);
  // Copy return value into non-dedicated register
  const TYPE_ID rtype = WN_rtype(call);
  const PREG_NUM result = Create_Preg( rtype, ".emulation_value" );
  WN *ldid = WN_Ldid(rtype, -1, Return_Val_Preg, MTYPE_TO_TY_array[rtype]);
  WN *stid = WN_StidIntoPreg(rtype, result, MTYPE_To_PREG(rtype), ldid);
  WN_INSERT_BlockLast(block, stid);
  wn = WN_LdidPreg(rtype, result);

  return wn;
}


// Handles arithmetic operators
extern WN *em_quad_arith(WN *block, WN *tree)
{
  WN * wn = NULL;
  const char * libfunc = NULL;

  Is_True (MTYPE_is_quad(WN_rtype(tree)),
	   ("em_quad_arith: quad rtype expected"));

  Is_True (WN_kid_count(tree) == 2,
	   ("em_quad_arith: expected binary operator"));

  WN * opnd0 = WN_kid0 (tree);
  WN * opnd1 = WN_kid1 (tree);

  Is_True (WN_rtype (opnd0) == WN_rtype (opnd1),
	   ("em_quad_arith: mixed types not supported yet"));

  switch (WN_operator (tree))
  {
    case OPR_ADD:
      libfunc = "__addtf3";
      break;
    case OPR_SUB:
      libfunc = "__subtf3";
      break;
    case OPR_DIV:
      libfunc = "__divtf3";
      break;
    case OPR_MPY:
      libfunc = "__multf3";
      break;
    default:
      Fail_FmtAssertion ("em_quad_arith: this quad rtype not handled");
  }

  Is_True (libfunc, ("em_quad_arith: NULL library function"));

  ST * st = new_func_st (libfunc);
  WN * call = WN_Create (OPR_CALL, WN_rtype (tree), MTYPE_V, 2);
  WN_st_idx (call) = ST_st_idx (st);
  // Operand 0
  WN_kid(call,0) = WN_CreateParm (WN_rtype (opnd0),
                                  opnd0,
                                  MTYPE_TO_TY_array [WN_rtype (opnd0)],
                                  0);
  // Operand 1
  WN_kid(call,1) = WN_CreateParm (WN_rtype (opnd1),
                                  opnd1,
                                  MTYPE_TO_TY_array [WN_rtype (opnd1)],
                                  0);
  WN_INSERT_BlockLast (block, call);

  // Copy return value into non-dedicated register
  const TYPE_ID rtype = WN_rtype(call);
  const PREG_NUM result = Create_Preg( rtype, ".emulation_value" );
  WN *ldid = WN_Ldid(rtype, -1, Return_Val_Preg, MTYPE_TO_TY_array[rtype]);
  WN *stid = WN_StidIntoPreg(rtype, result, MTYPE_To_PREG(rtype), ldid);
  WN_INSERT_BlockLast(block, stid);
  wn = WN_LdidPreg(rtype, result);

  return wn;
}


// Handles truncation operations
extern WN *em_quad_trunc(WN * block, WN * tree)
{
  WN * wn = NULL;
  const char * libfunc = NULL;

  Is_True (WN_operator (tree) == OPR_TRUNC,
           ("em_quad_cvt: cvt operator expected"));

  Is_True (MTYPE_is_quad(WN_desc(tree)),
           ("em_quad_cvt: quad trunc expected"));

  switch (WN_rtype(tree)) // convert from quad
  {
    case MTYPE_U4:
    case MTYPE_I4:
      libfunc = "__fixtfsi";
      break;
    case MTYPE_U8:
    case MTYPE_I8:
      libfunc = "__fixtfdi";
      break;
    default:
      Fail_FmtAssertion ("em_quad_cvt: TRUNC with quad rtype not supported");
  }

  Is_True (libfunc, ("em_quad_cvt: NULL library function"));

  ST * st = new_func_st (libfunc);
  WN * call = WN_Create (OPR_CALL, WN_rtype (tree), MTYPE_V, 1);
  WN_st_idx (call) = ST_st_idx (st);
  WN * opnd = WN_kid (tree, 0);
  // function operand
  WN_kid(call,0) = WN_CreateParm (WN_rtype (opnd),
                                  opnd,
                                  MTYPE_TO_TY_array[WN_rtype (opnd)],
                                  0);
  WN_INSERT_BlockLast (block, call);

  // Copy return value into non-dedicated register
  const TYPE_ID rtype = WN_rtype(call);
  const PREG_NUM result = Create_Preg( rtype, ".emulation_value" );
  WN *ldid = WN_Ldid(rtype, -1, Return_Val_Preg, MTYPE_TO_TY_array[rtype]);
  WN *stid = WN_StidIntoPreg(rtype, result, MTYPE_To_PREG(rtype), ldid);
  WN_INSERT_BlockLast(block, stid);
  wn = WN_LdidPreg(rtype, result);
 
  return wn;
}


// Handles comparison operations
extern WN *em_quad_cmp(WN *block, WN *tree)
{
  WN * wn = NULL;
  const char * libfunc = NULL;

  Is_True (OPERATOR_is_compare (WN_operator (tree)),
           ("em_quad_cmp: compare operator expected"));

  Is_True (MTYPE_is_quad(WN_desc(tree)),
           ("em_quad_cmp: quad compare expected"));

  WN * opnd0 = WN_kid0 (tree);
  WN * opnd1 = WN_kid1 (tree);

  Is_True (WN_rtype (opnd0) == WN_rtype (opnd1),
           ("em_quad_cmp: comparison between mixed types not supported yet"));

  switch (WN_operator (tree))
  {
    case OPR_NE:
      libfunc = "__netf2";
      break;
    case OPR_EQ:
      libfunc = "__eqtf2";
      break;
    case OPR_LE:
      libfunc = "__letf2";
      break;
    case OPR_LT:
      libfunc = "__lttf2";
      break;
    case OPR_GE:
      libfunc = "__getf2";
      break;
    case OPR_GT:
      libfunc = "__gttf2";
      break;
    default:
      Fail_FmtAssertion ("em_quad_cmp: unsupported relational operator");
  }

  Is_True (libfunc, ("em_quad_cmp: NULL library function"));

  // Bug 12914: Use unsigned integer type for intermediate
  TYPE_ID rtype = WN_rtype(tree);
  if (MTYPE_is_unsigned(rtype)) rtype = MTYPE_complement(rtype);

  ST * st = new_func_st (libfunc);
  WN * call = WN_Create (OPR_CALL, rtype, MTYPE_V, 2);
  WN_st_idx (call) = ST_st_idx (st);
  // Operand 0
  WN_kid(call,0) = WN_CreateParm (WN_rtype (opnd0),
                                  opnd0,
                                  MTYPE_TO_TY_array [WN_rtype (opnd0)],
                                  0);
  // Operand 1
  WN_kid(call,1) = WN_CreateParm (WN_rtype (opnd1),
                                  opnd1,
                                  MTYPE_TO_TY_array [WN_rtype (opnd1)],
                                  0);
  WN_INSERT_BlockLast (block, call);

  // Copy return value into non-dedicated register
  const PREG_NUM result = Create_Preg( rtype, ".emulation_value" );
  WN *ldid = WN_Ldid(rtype, -1, Return_Val_Preg, MTYPE_TO_TY_array[rtype]);
  WN *stid = WN_StidIntoPreg(rtype, result, MTYPE_To_PREG(rtype), ldid);
  WN_INSERT_BlockLast(block, stid);
  wn = WN_LdidPreg(rtype, result);

  // Bug 12866: These library functions return an integer that must be
  // compared to zero (using the matching comparison OP) to obtain the
  // boolean answer.
  wn = WN_Relational(WN_operator(tree), WN_rtype(wn),
		     wn, WN_Intconst(rtype, 0));
 
  return wn;
}


// Handles intrinsic comparison operations
extern WN *em_quad_intrn_cmp(WN *block, WN *tree)
{
  Is_True (WN_operator(tree) == OPR_INTRINSIC_OP,
	   ("em_quad_intrn_cmp: compare intrinsic operator expected"));

  WN *opnd0 = WN_kid0(tree);
  WN *opnd1 = WN_kid1(tree);
  Is_True(WN_rtype(opnd0) == WN_rtype(opnd1),
	  ("em_quad_intrn_cmp: compare between mixed types not supported yet"));
  Is_True(MTYPE_is_quad(WN_rtype(opnd0)),
	  ("em_quad_intrn_cmp: quad parameters expected"));

  const char *libfunc = NULL;
  OPERATOR opr;
  INTRINSIC id = (INTRINSIC) WN_intrinsic(tree);
  switch (id)
  {
    case INTRN_ISGREATER:      libfunc = "__gttf2";    opr = OPR_GT;  break;
    case INTRN_ISGREATEREQUAL: libfunc = "__getf2";    opr = OPR_GE;  break;
    case INTRN_ISLESS:         libfunc = "__lttf2";    opr = OPR_LT;  break;
    case INTRN_ISLESSEQUAL:    libfunc = "__letf2";    opr = OPR_LE;  break;
    case INTRN_ISLESSGREATER:  libfunc = "__netf2";    opr = OPR_NE;  break;
    case INTRN_ISORDERED:      libfunc = "__unordtf2"; opr = OPR_EQ;  break;
    case INTRN_ISUNORDERED:    libfunc = "__unordtf2"; opr = OPR_NE;  break;
    default:
      Fail_FmtAssertion("em_quad_intrn_cmp: unsupported intrinsic op");
  }
  Is_True(libfunc, ("em_quad_intrn_cmp: NULL library function"));

  TYPE_ID rtype = WN_rtype(tree);
  ST *st = new_func_st(libfunc);
  WN *call = WN_Create(OPR_CALL, rtype, MTYPE_V, 2);
  WN_st_idx(call) = ST_st_idx(st);
  WN_kid(call, 0) = opnd0;
  WN_kid(call, 1) = opnd1;
  WN_INSERT_BlockLast(block, call);

  // Copy return value into non-dedicated register
  const PREG_NUM result = Create_Preg(rtype, ".emulation_value");
  WN *ldid = WN_Ldid(rtype, -1, Return_Val_Preg, MTYPE_TO_TY_array[rtype]);
  WN *stid = WN_StidIntoPreg(rtype, result, MTYPE_To_PREG(rtype), ldid);
  WN_INSERT_BlockLast(block, stid);
  WN *wn = WN_LdidPreg(rtype, result);

  // Bug 12866: These library functions return an integer that must be
  // compared to zero (using the matching comparison OP) to obtain the
  // boolean answer.
  wn = WN_Relational(opr, WN_rtype(wn), wn, WN_Intconst(rtype, 0));
  return wn;
}


// Expands min and max
// We could call fminl or fmaxl, but we might not be linking to libm.
extern WN *em_quad_minmax(WN *block, WN *tree)
{
  WN *opnd0 = WN_kid0(tree);
  WN *opnd1 = WN_kid1(tree);
  const TYPE_ID rtype = WN_rtype(tree);
  const TYPE_ID rtype0 = WN_rtype(opnd0);
  const TYPE_ID rtype1 = WN_rtype(opnd1);
  Is_True (MTYPE_is_quad(rtype),
	   ("em_quad_minmax: quad rtype expected"));
  Is_True (WN_kid_count(tree) == 2,
	   ("em_quad_minmax: expected binary operator"));
  Is_True (rtype0 == rtype1,
	   ("em_quad_minmax: mixed types not supported yet"));

  // Copy arguments into pregs
  const PREG_NUM arg0 = Create_Preg( rtype0, ".emulation_argument" );
  const PREG_NUM arg1 = Create_Preg( rtype1, ".emulation_argument" );
  WN *wn = WN_StidIntoPreg(rtype0, arg0, MTYPE_To_PREG(rtype0), opnd0);
  WN_INSERT_BlockLast(block, wn);
  wn = WN_StidIntoPreg(rtype1, arg1, MTYPE_To_PREG(rtype1), opnd1);
  WN_INSERT_BlockLast(block, wn);

  // min(a,b)  ==>  (a < b) ? a : b
  // max(a,b)  ==>  (a < b) ? b : a
  WN *wn0 = WN_LdidPreg(rtype0, arg0);
  WN *wn1 = WN_LdidPreg(rtype1, arg1);
  wn = WN_LT(rtype0, wn0, wn1);
  wn = em_quad_cmp(block, wn);
  wn0 = WN_LdidPreg(rtype0, arg0);
  wn1 = WN_LdidPreg(rtype1, arg1);
  if (WN_operator(tree) == OPR_MAX) {
    WN *wnT = wn0;  wn0 = wn1;  wn1 = wnT;
  }
  wn = WN_Select(rtype, wn, wn0, wn1);
  return wn;
}
