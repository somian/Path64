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


//-*-c++-*-
/* ====================================================================
 * ====================================================================
 *
 * Module: opt_wn.h
 * $Revision$
 * $Date$
 * $Author$
 * $Source$
 *
 * Description:  interface for CVTL rules
 *
 * ====================================================================
 * ====================================================================
 */

#ifndef opt_cvtl_rule_INCLUDED
#define opt_cvtl_rule_INCLUDED "opt_cvtl_rule.h"

#include "defs.h"
#include "mtypes.h"
#include "opcode_core.h"

struct WN; // forward declaration

extern TYPE_ID Adjust_signed_type(TYPE_ID rtype, INT size, WN *wn);

// given a OPR_CVTL opcode and the number of bits it converting to,
// return the actual type.
extern TYPE_ID Actual_cvtl_type(OPCODE opc, INT bits);

extern TYPE_ID Actual_result_type(WN *wn);

extern TYPE_ID Actual_signed_type(TYPE_ID rtype, INT size, WN *wn);

// Check if TYPE_ID lhs_type is compatible with the RHS WN node
extern BOOL  Types_are_compatible(TYPE_ID t1, TYPE_ID t2);
extern BOOL  Types_are_compatible(TYPE_ID t1, WN *wn);

enum { NOT_AT_ALL, NEED_CVT, NEED_CVTL };

// return NOT_AT_ALL, NEED_CVT, or NEED_CVTL.  The reference parameter
// opc returns the opcode for the conversion, if it is either NEED_CVT
// or NEED_CVTL.
extern INT Need_type_conversion(TYPE_ID from_ty, TYPE_ID to_ty, OPCODE *opc);
extern INT Need_load_type_conversion(BOOL source_sign_extd, 
				     BOOL target_sign_extd, 
				     TYPE_ID to_ty, TYPE_ID dsc_ty, OPCODE *opc);

// Actual_result_type returns the data type of the tree with the
// actual number of bytes that we can determine at this point.  This
// function helps to determine a CVTL node is required or can be
// deleted by the optimizer emitter.
extern TYPE_ID Actual_result_type(WN *wn);
extern INT   Actual_data_size(WN *wn);

#endif
