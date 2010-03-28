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
 *  Module: ti_latency.h
 *  $Revision$
 *  $Date$
 *  $Author$
 *  $Source$
 *
 *  Synopsis:
 *
 *	Instruction latency information, i.e. the cycles at which 
 *	key events occur.
 *
 *  Interface Description:
 *
 *      Exported functions:
 *
 *	    INT TI_LATENCY_Load_Cycle(
 *		TOP opcode
 *	    )
 *
 *		The cycle in which an operand is loaded from memory.
 *
 *	    INT TI_LATENCY_Store_Cycle(
 *		TOP opcode
 *	    )
 *
 *		The cycle in which a value is stored to memory.
 *
 *	    INT TI_LATENCY_Last_Issue_Cycle(
 *		TOP opcode
 *	    )
 *
 *               The last issue cycle for the given operation.
 *
 *	    INT TI_LATENCY_Commit_Cycle(
 *		TOP opcode
 *	    )
 *
 *		The cycle in which the operation is fully committed,
 *		i.e. no machine exceptions or other events will
 *		prevent its completion. For branches, this is interpreted
 *		as the last cycle before instructions from the branch
 *		target, instead of the shadow, begin being issued.
 *
 *	    INT TI_LATENCY_Operand_Access_Cycle(
 *		TOP   opcode
 *		INT   opnd
 *	    )
 *
 *		The cycle in which operand number 'opnd' is read from
 *		a register.
 *
 *	    INT TI_LATENCY_Result_Available_Cycle(
 *		TOP opcode
 *		INT result
 *	    )
 *
 *		The cycle in which result number 'result' is written to 
 *		a register.
 *
 *	Instruction latencies, i.e. hazards, are things to avoid.  
 *	Unfortunately, the MIPS architecture did not take this advice 
 *	to heart, so we need to be able to keep certain ops away from 
 *	one another.  To do so, we use:
 *
 *	    INT TI_LATENCY_Operand_Hazard(
 *		TOP    opcode
 *		INT   *opnd
 *		INT   *error
 *	    )
 *
 *		If 'opcode' has an operand hazard, the operand number
 *		is returned by reference through 'opnd' and the function
 *		return value indicates the size (in instructions)
 *		and direction of the hazard. The absolute value
 *		of the return value indicates how many instructions
 * 		must precede (negative return value) or follow
 *		(positive return value) before the operand can be
 *		written by another instruction.
 *
 *		A return value of 0 indicates there is no operand
 *		hazard for 'opcode'.
 *		If an error occurs, 'error' will be set to TI_RC_ERROR.
 *
 *	    INT TI_LATENCY_Result_Hazard(
 *		TOP  opcode
 *		INT *result
 *		INT *error
 *	    )
 *
 *		If 'opcode' has a result hazard, the result number
 *		is returned by reference through 'result' and the function 
 *		return 	value indicates how many instructions must follow
 *		before the result can be referenced.
 *		
 *		A return value of 0 indicates there is no result
 *		hazard for 'opcode'.
 *		If an error occurs, 'error' will be set to TI_RC_ERROR.
 *
 *	    INT TI_LATENCY_Errata_Hazard(
 *		TOP  opcode
 *		INT *number
 *		INT *error
 *	    )
 *
 *		Errata hazards identify particular bugs in the hardware 
 *		that must be worked around. The return value indicates if 
 *		'opcode' has an errata hazard and represents the size of 
 *		the hazard (in instructions). The particular errata number
 *		of the hazard is returned through the out parameter 
 *		'number'.
 *
 *		A return value of 0 indicates there is no errata
 *		hazard for 'opcode'.
 *		If an error occurs, 'error' will be set to TI_RC_ERROR.
 *
 * ====================================================================
 * ====================================================================
 */

#ifndef ti_latency_INCLUDED
#define ti_latency_INCLUDED

#include "topcode.h"
#include "ti_si.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _KEEP_RCS_ID
static const char ti_latency_rcs_id[] = "$Source$ $Revision$";
#endif /* _KEEP_RCS_ID */

inline INT TI_LATENCY_Load_Cycle(TOP opcode)
{
  return TSI_Load_Access_Time(opcode);
}

inline INT TI_LATENCY_Store_Cycle(TOP opcode)
{
  return TSI_Store_Available_Time(opcode);
}

inline INT TI_LATENCY_Operand_Access_Cycle(TOP opcode, INT opnd)
{
  return TSI_Operand_Access_Time(opcode, opnd);
}

inline INT TI_LATENCY_Result_Available_Cycle(TOP opcode, INT result)
{
  return TSI_Result_Available_Time(opcode, result);
}

inline INT TI_LATENCY_Last_Issue_Cycle(TOP opcode)
{
  return TSI_Last_Issue_Cycle(opcode);
}

extern INT TI_LATENCY_Commit_Cycle(TOP opcode);
extern INT TI_LATENCY_Operand_Hazard(TOP opcode, INT *opnd, INT *error);
extern INT TI_LATENCY_Result_Hazard(TOP opcode, INT *result, INT *error);
extern INT TI_LATENCY_Errata_Hazard(TOP opcode, INT *number, INT *error);

#ifdef __cplusplus
}
#endif
#endif /* ti_latency_INCLUDED */
