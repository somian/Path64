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


#ifndef erirb_INCLUDED
#define erirb_INCLUDED
#ifdef __cplusplus
extern "C" {
#endif
/* ====================================================================
 * ====================================================================
 *
 * Module: erirb.h
 * $Revision$
 * $Date$
 * $Author$
 * $Source$
 *
 * Revision history:
 *  23-Jun-89 - Original Version
 *  12-Jun-91 - Integrated from Josie
 *
 * Description:
 *
 * IR Builder error code definitions.
 *
 * ====================================================================
 * ====================================================================
 */


#ifdef _KEEP_RCS_ID
static char *erirb_rcs_id = "$Source$ $Revision$";
#endif /* _KEEP_RCS_ID */

/* Include errors.h for the benefit of any users: */
#include "errors.h"

/* Define the initial error code to use: */
#define EC_IRB_Start	(EP_IR_BUILDER)*1000

/* General irb error codes: */
#define EC_Irb_Internal	EC_IRB_Start		/* str */

/* irbmain error codes: */
#define EC_No_ASSIGN	EC_IRB_Start+1		/* str */

/* irbmem error codes: */
#define EC_Array_OOB	EC_IRB_Start+11		/* sym */
#define EC_Bad_Formal	EC_IRB_Start+12		/* sym */
#define EC_Addr_Formal	EC_IRB_Start+13		/* sym */
#define EC_Null_Base	EC_IRB_Start+14		/* tree, tree */
#define EC_Invalid_Addr	EC_IRB_Start+15		/* str */
#define EC_Var_TN	EC_IRB_Start+16		/* tn, str */
#define EC_Bad_Const	EC_IRB_Start+17		/* int, str */
#define EC_Mult_Defer	EC_IRB_Start+18		/* sym, tree */
#define EC_Abs_Addr	EC_IRB_Start+19		/* tree, tree, str */
#define EC_Load_Opnds	EC_IRB_Start+20		/* int, str, str */

/* irbcall/callutil error codes: */
#define EC_Return_Style	EC_IRB_Start+31		/* int, str */
#define EC_Need_Value	EC_IRB_Start+32		/* str */
#define EC_Inv_Actual	EC_IRB_Start+33		/* node, int */
#define EC_Mem_Actual	EC_IRB_Start+34		/* int, int */
#define EC_No_Einfo	EC_IRB_Start+35		/* tree */
#define EC_Not_Entry	EC_IRB_Start+37		/* sym, sym */
#define EC_Flt_Varargs1	EC_IRB_Start+38		/* str */
#define EC_Flt_Varargs2	EC_IRB_Start+39		/* str */

/* irbexec error codes: */
#define EC_Agt_Uninit	EC_IRB_Start+41		/* sym */

/* irbdo error codes: */
#define EC_Inv_GOTO	EC_IRB_Start+51		/* int, sym */

/* irbexpr error codes: */
#define EC_Inv_Field_At	EC_IRB_Start+61		/* int, int, int */
#define EC_Inv_Alloca_Size  EC_IRB_Start+62	/* int64 */
#define EC_Zero_Alloca_Size EC_IRB_Start+63	/* none */
#define EC_Inv_TAS_Size	EC_IRB_Start+64		/* nd, int, nd, int, str */
#define EC_TAS_Nonload	EC_IRB_Start+65		/* nd, nd, tn */

/* Memory model errors: */
#define EC_Ill_TDT_Seg	EC_IRB_Start+80		/* int, stab */
#define EC_Large_Temp	EC_IRB_Start+81		/* int, stab */
#define EC_Ill_Stack_Base EC_IRB_Start+82	/* stab, stab */
#define EC_Huge_Frame	EC_IRB_Start+83		/* int, int */
#define EC_Huge_Frame2	EC_IRB_Start+84		/* none */
#define EC_Not_Sorted	EC_IRB_Start+85		/* str */
#define EC_Pop_Scope	EC_IRB_Start+86		/* none */
#define EC_Ill_Frame_Seg EC_IRB_Start+87	/* int, str */
#define EC_Ill_Stack_Model EC_IRB_Start+88	/* int, str */
#define EC_Sym_Removal	EC_IRB_Start+89		/* stab, str */
#define EC_Gnum_Range	EC_IRB_Start+90		/* str */

#ifdef __cplusplus
}
#endif
#endif /* erirb_INCLUDED */
