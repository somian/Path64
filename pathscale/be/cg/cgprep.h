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


/* =======================================================================
 * =======================================================================
 *
 *  Module: cgprep.h
 *  $Revision$
 *  $Date$
 *  $Author$
 *  $Source$
 *
 *  Revision comments:
 *
 *  4-Apr-1995 - Initial version
 *
 *  Description:
 *  ============
 *
 *  Utilities for loop and dep-graph code.
 *
 *  Interface:
 *
 *	void CGPREP_Init_Op(OP *op)
 *	  Initialize CGPREP data structures for newly-created <op>.
 *
 *	INT16 CGPREP_Same_Res_Opnd(OP *op)
 *	  Requires: OPER_same_res(Operator(OP_code(op))) 
 *		    (currently means <op> is select or unaligned ld)
 *	  Return the operand number for the operand of <op> that can
 *	  be the same as the result, or -1 if no operand qualifies.
 *
 *	TN *CGPREP_Dup_TN(TN *old_tn)
 *	  Return a TN equivalent to <old_tn>, except that it has a new
 *	  TN_number.  This works even for dedicated TNs, and differs from
 *	  Dup_TN_Even_If_Dedicated because it doesn't reset the flag
 *	  for TN_is_dedicated or the register assignment for dedicated TNs.
 *
 *	void CGPREP_Copy_TN(TN *dest, TN *src, OP *point, UINT8 omega,
 *			    BOOL before)
 *	  Insert a copy from <src>[<omega>] to <dest>, either just
 *	  before <point> if <before> is TRUE, or just after <point>
 *	  otherwise.
 *
 *	void CGPREP_Copy_TN_Into_BB(TN *dest, TN *src, BB *bb, OP *point,
 *				    UINT8 omega, BOOL before)
 *	  Insert a copy from <src>[<omega>] to <dest>.
 *        Arguments are the same as in CGPREP_Copy_TN with the addition of 
 *        BB *bb. This can be used in cases where there may be no OPs
 *        in the BB yet.
 *
 * =======================================================================
 * =======================================================================
 */

#ifndef CGPREP_INCLUDED
#define CGPREP_INCLUDED

#include "tn.h"
#include "op.h"

void CGPREP_Copy_TN(TN *dest, TN *src, OP *point, UINT8 omega, BOOL before);
void CGPREP_Copy_TN_Into_BB(TN *dest, TN *src, BB *bb, OP *point, UINT8 omega, BOOL before);

INT16 CGPREP_Same_Res_Opnd(OP *op);

void CGPREP_Init_Op(OP *op);

TN * CGPREP_Dup_TN(TN *old_tn);

#endif /* CGPREP_INCLUDED */
