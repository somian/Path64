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


// -*-C++-*-
// ====================================================================
// ====================================================================
//
// Module: array_lower.cxx
// $Revision$
// $Date$
// $Author$
// $Source$
//
// Revision history:
//  dd-mmm-95 - Original Version
//
// Description:
// 
//  Lower all accesses to reshaped array variables.
//
// 
// Implementation Notes:
//
//  During array lowering we need to generate divfloor, divceil and 
//  modulus operations.  Since all arrays always start at 0, the array
//  index functions are always positive.  Also, the number of processors
//  and the block sizes are always positive.  We use this information to
//  perform the following optimizations:
// 
//  divfloor(a,b)  ==>  a / b
//  divceil(a,b)   ==>  (a + b - 1) / b
//  mod(a,b)       ==>  rem(a,b)
// 
//  We can use either mod or rem for the modulus operation since they have
//  the same value when both operands are the same sign.
//  Since the global optimizer can optimize div and rem operations
//  on the same operands and only perform a single divide for both, we
//  generate rem operations.
//
// ====================================================================
// ====================================================================
//
// TODO:
//
// o Try to form valid array expressions out of invalid ones by de-linearizing
// o Can turn I/O statements IO_ITEMs of kind IOL_ARRAY into implied dos
// 
// ====================================================================

#ifdef USE_PCH
#include "lno_pch.h"
#endif // USE_PCH
#pragma hdrstop

static const char *source_file = __FILE__;

#ifdef _KEEP_RCS_ID
static char *rcs_id = "$Source$ $Revision$";
#endif /* _KEEP_RCS_ID */

#include <sys/types.h>
#include <ctype.h>
#include <limits.h>
#include <alloca.h>

#include "pu_info.h"
#include "opt_du.h" 
#include "wn.h"
#include "lego.h"
#include "array_lower.h"
#include "al_loop.h"

/* ========================================================================
   Public Function Declarations
   ======================================================================== */

extern void Lower_Array_Accesses(WN* func_nd);

/* ========================================================================
   Public Function Implementations
   ======================================================================== */

void
Lower_Array_Accesses(WN *func_nd)
{
  if (WN_operator(func_nd) != OPR_FUNC_ENTRY) {
    DevWarn ("Lower_Array_Accesses called with non-func node (opcode=%d)\n",
             WN_opcode(func_nd));
  }

  ARRAY_LOWER_LOOP *root_node = 
    CXX_NEW(ARRAY_LOWER_LOOP(NULL, func_nd, -1), LEGO_pool);

  root_node->Build_Loop(WN_func_body(func_nd));
  root_node->Process_Loop();

  CXX_DELETE(root_node, LEGO_pool);
}
