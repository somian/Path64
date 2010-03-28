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
 * Module: cwh_expr.h
 * $Revision$
 * $Date$
 * $Author$
 * $Source$
 *
 * Revision history:
 *  dd-mmm-95 - Original Version
 *
 * Description:
 *
 * ====================================================================
 * ====================================================================
 */

#ifndef CWH_EXPR_INCLUDED
#define CWH_EXPR_INCLUDED

#ifdef _KEEP_RCS_ID
static char *rcs_id = "$Source$ $Revision$";
#endif /* _KEEP_RCS_ID */

typedef enum {
  f_NONE = 0,
  f_T_PASSED=1,
  f_T_SAVED=2
} FLAG;

extern WN * cwh_expr_operand(WN **arrexp) ;
extern WN * cwh_expr_address(FLAG flag) ;
extern WN * cwh_expr_bincalc(OPERATOR op, WN * wn1, WN * wn2) ;
extern WN  *cwh_get_typed_operand(TYPE_ID ty, WN **arrexp);
extern OPCODE cwh_make_typed_opcode(OPERATOR op, TYPE_ID ty1, TYPE_ID ty2);
extern WN * cwh_convert_to_ty(WN * wn, TYPE_ID ty);
extern WN * cwh_wrap_cvtl(WN * wn, TYPE_ID ty);
extern TYPE_ID cwh_get_highest_type(WN *lhs, WN *rhs);
extern WN * cwh_expr_temp(TY_IDX ty, WN *sz, FLAG flag);
extern void cwh_expr_compare(OPERATOR op,TY_IDX  ty);
extern void cwh_expr_str_operand(W_node expr[2]);
extern void cwh_expr_set_flags(ST *st, FLAG flag);
extern WN * cwh_generate_bitmask(WN *len, TYPE_ID ty);
extern WN * cwh_expr_extract_arrayexp(WN *node, WN **arrayexp);
extern WN * cwh_expr_restore_arrayexp(WN *node, WN *arrayexp);
extern void cwh_expr_temp_set_pragma(ST *st) ;
extern WN * cwh_expr_dispose_of_char(WN * wn);


/* This causes extract_arrayexp to delete all arrayexps it sees if passed
 * to the arrayexp arg.
 */
#define DELETE_ARRAYEXP_WN ((WN **) 1)

    
#endif /* CWH_EXPR_INCLUDED */

