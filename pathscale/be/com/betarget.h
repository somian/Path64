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
 * Module: betarget.h
 * $Revision$
 * $Date$
 * $Author$
 * $Source$
 *
 * Description: Target Specific Miscellany
 *
 * Here are various miscellaneous functions to provide machine dependent
 * information.
 *
 * Exported functions:
 *
 *   INT Copy_Quantum_Ratio(void)
 *	???
 *
 *   TOP TAS_To_TOP(WN *tas_wn)
 *	Pick the opcode corresponding to the TAS, which will either
 *	be a float<->int move or a no-op.
 *
 * ====================================================================
 * ====================================================================
 */

#ifndef betarget_INCLUDED
#define betarget_INCLUDED

#ifdef _KEEP_RCS_ID
static const char betarget_rcs_id[] = "$Source$ $Revision$";
#endif /* _KEEP_RCS_ID */

#include "topcode.h"

#ifdef __cplusplus
extern "C" {
#endif

extern TOP TAS_To_TOP(WN *tas_wn);
extern INT Copy_Quantum_Ratio(void);

// largest offset possible in small-frame stack model
extern INT Max_Small_Frame_Offset;

extern BOOL Targ_Lower_Float_To_Unsigned;
extern BOOL Targ_Lower_Unsigned_To_Float;

#ifdef __cplusplus
}
#endif
#endif /* betarget_INCLUDED */
