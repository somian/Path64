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
 * Module: be_util.c
 * $Revision$
 * $Date$
 * $Author$
 * $Source$
 *
 * Revision history:
 *  06-Dec -95 - Original Version
 *
 * Description:
 *    Utilities for all backend components.
 *
 * ====================================================================
 * ====================================================================
 */

#include "defs.h"
#include "wn.h"
#include "be_util.h"

static INT32 current_pu_count;

void
Reset_Current_PU_Count(void)
{
  current_pu_count = 0;
}

void
Advance_Current_PU_Count(void)
{
  current_pu_count++;
}

INT32
Current_PU_Count(void)
{
  return current_pu_count;
}

//-----------------------------------------------------------------------
// NAME: St_Idx_Is_Intconst
// FUNCTION: Returns TRUE if 'st_idx' is an integer constant and, when this
//   is the case, sets the value in 'val'.  Otherwise, returns FALSE.
//-----------------------------------------------------------------------

extern BOOL St_Idx_Is_Intconst(ST_IDX st_idx, INT64 *val)
{
  ST* st = &St_Table[st_idx]; 
  if (ST_class(st)==CLASS_CONST) {
    TCON t = STC_val(st);
    switch(TCON_ty(t)) {
      case MTYPE_I1: case MTYPE_I2 : case MTYPE_I4: case MTYPE_I8 :
        *val = t.vals.i0;
        return TRUE;
      case MTYPE_U1: case MTYPE_U2 : case MTYPE_U4: case MTYPE_U8 :
        *val = t.vals.k0;
        return TRUE;
      default:
        return FALSE;
    }
  } else {
    return FALSE;
  }
}

//-----------------------------------------------------------------------
// NAME: Wn_Is_Intconst
// FUNCTION: Returns TRUE if 'ldid' is an integer constant and, when this
//   is the case, sets the value in 'val'.  Otherwise, returns FALSE.
//-----------------------------------------------------------------------

extern BOOL Wn_Is_Intconst(WN *ldid, INT64 *val)
{
  return St_Idx_Is_Intconst(ST_st_idx(WN_st(ldid)), val);
} 

// ----------------------------------------------------------------------
// symbols defined in be.so but used in cg.so
/* official builds will link with identfile that defines _Release_ID */
extern const char *__Release_ID;
const char *Default_Release_ID = "none";
#if defined(BUILD_OS_DARWIN)
#pragma weak __Release_ID
const char *__Release_ID = "none";
#else /* defined(BUILD_OS_DARWIN) */
#pragma weak __Release_ID = Default_Release_ID
#endif /* defined(BUILD_OS_DARWIN) */
