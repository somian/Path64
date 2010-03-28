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


#ifndef ipa_lno_read_INCLUDED
#define ipa_lno_read_INCLUDED "ipa_lno_read.h"

#ifndef _IPA_LNO_FILE
#include "ipa_lno_file.h"
#endif

extern TYPE_ID Formal_Machine_Type(WN* wn_call, 
				   INT formal_number,
				   IPA_LNO_READ_FILE* IPA_LNO_File);

extern BOOL Scalar_Expr(WN* wn_expr);

extern BOOL Linear_Expr(WN* wn_expr,
                        DYN_ARRAY<WN*>* wn_list,
                        DYN_ARRAY<INT>* int_list,
                        INT64* const_value);

extern void IPA_LNO_Map_Calls(WN* func_nd,
                              IPA_LNO_READ_FILE* IPA_LNO_File);

extern INT IPA_LNO_Procedure_Index(WN* wn_call,
                                   IPA_LNO_READ_FILE* IPA_LNO_File);

extern void Update_Loop_Stmt(WN* wn_use);

extern void Add_Scalars_In_Expr(WN* wn_expr,
                                SCALAR_STACK* st_scalar);

extern WN* Single_Definition_Temp(WN* wn_argument);

#endif /* ipa_lno_read_INCLUDED */
