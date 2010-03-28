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
 *  Module: gtn_tn_set.c
 *  $Revision$
 *  $Date$
 *  $Author$
 *  $Source$
 *
 *  Revision comments:
 *
 *  14-Oct-1994 - Initial version
 *
 *  Description:
 *  ============
 *
 *  Implements the translation from GTN sets to TN sets.
 *
 * =======================================================================
 * =======================================================================
 */

#ifdef USE_PCH
#include "cg_pch.h"
#endif // USE_PCH
#pragma hdrstop

#include "defs.h"
#include "errors.h"		    /* for definition of Is_True() */
#include "mempool.h"
#include "bitset.h"
#include "cgir.h"
#include "tn_set.h"
#include "tn_list.h"
#include "gtn_universe.h"
#include "gtn_set.h"
#include "bb_list.h"
#include "register.h"
#include "bbregs.h"
#include "gtn_tn_set.h"


MEM_POOL local_pool;
BOOL     local_pool_initialized = FALSE;


/* =======================================================================
 *
 *  Check_Local_Pool_Initialized
 *
 *  Check that the local pool is initialized and do so if required.
 *
 * =======================================================================
 */
inline void
Check_Local_Pool_Initialized(void)
{
  if ( ! local_pool_initialized ) {
    MEM_POOL_Initialize(&local_pool,"GTN_TN_SET_local_pool",FALSE);
    local_pool_initialized = TRUE;
  }
}


/* =======================================================================
 *
 *  GTN_TN_SET_Print
 *
 *  See interface description.
 *
 * =======================================================================
 */
void
GTN_TN_SET_Print(GTN_SET *gtn_set, FILE *file)
{
  Check_Local_Pool_Initialized();
  MEM_POOL_Push(&local_pool);

  TN_SET *tn_set = TN_SET_Create_Empty (Last_TN + 1,&local_pool);
  for ( TN *tn = GTN_SET_Choose(gtn_set);
	tn != GTN_SET_CHOOSE_FAILURE;
	tn = GTN_SET_Choose_Next(gtn_set,tn)) 
  {
    FmtAssert(TN_is_global_reg(tn),("TN%d is not global",TN_number(tn)));
    tn_set = TN_SET_Union1D(tn_set, tn, &local_pool);
  }

  TN_SET_Print(tn_set,file);

  MEM_POOL_Pop(&local_pool);
}
