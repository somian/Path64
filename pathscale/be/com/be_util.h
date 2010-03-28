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
 * Module: be_util.h
 * $Revision$
 * $Date$
 * $Author$
 * $Source$
 *
 * Revision history:
 *  06-Dec-95 - Original Version
 *
 * Description:
 *    Utilities for all backend components.
 *
 * ====================================================================
 * ====================================================================
 */
#ifndef be_util_INCLUDED
#define be_util_INCLUDED

#ifdef _KEEP_RCS_ID
static char *be_utilrcs_id = "$Source$ $Revision$";
#endif /* _KEEP_RCS_ID */

#ifndef wn_INCLUDED
#include "wn.h"
#endif 

extern void  Reset_Current_PU_Count(void);
extern void  Advance_Current_PU_Count(void);
extern INT32 Current_PU_Count(void);
extern BOOL St_Idx_Is_Intconst(ST_IDX st_idx, INT64 *val);
extern BOOL Wn_Is_Intconst(WN *ldid, INT64 *val);

#endif /* be_util_INCLUDED */
