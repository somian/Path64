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


#ifndef ipa_cost_util_INCLUDED
#define ipa_cost_util_INCLUDED "ipa_cost_util.h"

// Constant declaration.

const INT DEFAULT_TRIP_COUNT = 100;
const INT64 DEFAULT_CALL_COST = 100; 
const INT MAX_VALUE_COUNT = 100;
const INT MAX_EXPR_COUNT = 100;
const INT MAX_CALL_EXPR_COUNT = 15;

extern INT IPL_EX_New_Constant(DYN_ARRAY<SUMMARY_VALUE>* sv,
                               INT64 constant_value);

extern INT IPL_EX_New_Value_Expr(DYN_ARRAY<SUMMARY_EXPR>* sx,
                                 INT sv_index);

extern void IPL_EX_Eliminate_Value(DYN_ARRAY<SUMMARY_VALUE>* sv,
                                   DYN_ARRAY<SUMMARY_EXPR>* sx,
                                   INT value_index);

extern void IPL_EX_Add_Value_Offsets(DYN_ARRAY<SUMMARY_VALUE>* sv,
                                     INT formal_offset,
                                     INT global_offset);

extern void IPL_EX_Add_Expr_Offsets(DYN_ARRAY<SUMMARY_EXPR>* sx,
                                    INT value_offset,
                                    INT expr_offset);

extern void Print_Exprs(FILE* fp,
			DYN_ARRAY<SUMMARY_VALUE>* sv,
                        DYN_ARRAY<SUMMARY_EXPR>* sx);

extern INT Check_Exprs(DYN_ARRAY<SUMMARY_VALUE>* sv,       
                       DYN_ARRAY<SUMMARY_EXPR>* sx,
                       FILE* fp);

extern void IPL_EX_Collapse_Trip_Counts(DYN_ARRAY<SUMMARY_VALUE>* sv,
                                        DYN_ARRAY<SUMMARY_EXPR>* sx);

extern BOOL IPL_EXS_Too_Complicated(DYN_ARRAY<SUMMARY_VALUE>* sv,
                                    DYN_ARRAY<SUMMARY_EXPR>* sx,
				    INT multiplier);

extern INT IPL_EXS_Chop_Down_Estimate(DYN_ARRAY<SUMMARY_VALUE>* sv,
                                      DYN_ARRAY<SUMMARY_EXPR>* sx);

extern void IPL_EX_Simplify(DYN_ARRAY<SUMMARY_VALUE>* sv,
                            DYN_ARRAY<SUMMARY_EXPR>* sx);

#endif /* ipa_cost_util_INCLUDED */

