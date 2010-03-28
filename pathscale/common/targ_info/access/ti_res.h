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
 *  Module: ti_res.h
 *  $Revision$
 *  $Date$
 *  $Author$
 *  $Source$
 *
 *  Synopsis:
 *
 *	Miscellaneous resource related functions.
 *
 *  Interface Description:
 *
 *      Exported functions:
 *
 *	    INT TI_RES_Cycle_Count(
 *		TOP     opcode
 *	    )
 *
 *              How many consecutive cycles are affected by 'opcode's
 *              resources?
 *
 *	    BOOL TI_RES_Can_Dual_Issue(
 *		TOP     opcode1
 *		TOP     opcode2
 *	    )
 *
 *		Can the two specified instructions issue in the
 *		same cycle? (ignoring register constraints)
 *
 * ====================================================================
 * ====================================================================
 */

#ifndef ti_res_INCLUDED
#define ti_res_INCLUDED

#include "topcode.h"
#include "ti_si.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _KEEP_RCS_ID
static const char ti_res_rcs_id[] = "$Source$ $Revision$";
#endif /* _KEEP_RCS_ID */

inline INT TI_RES_Cycle_Count(TOP opcode)
{
  return SI_RR_Length(TSI_Resource_Requirement(opcode));
}

extern BOOL TI_RES_Can_Dual_Issue(TOP opcode1, TOP opcode2);

#ifdef __cplusplus
}
#endif
#endif /* ti_res_INCLUDED */
