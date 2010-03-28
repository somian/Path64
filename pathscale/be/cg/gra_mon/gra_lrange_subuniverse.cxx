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

/*
 *  $Revision$
 *  $Date$
 *  $Author$
 *  $Source$
 */

#ifdef USE_PCH
#include "cg_pch.h"
#endif // USE_PCH
#pragma hdrstop

#ifdef _KEEP_RCS_ID
static char *rcs_id = "$Source$ $Revision$";
#endif

#include "defs.h"
#include "mempool.h"
#include "gra_bb.h"
#include "gra_trace.h"
#include "gra_lrange.h"
#include "gra_lrange_subuniverse.h"

LRANGE_SUB_MGR lrange_sub_mgr;

LRANGE*
INT_LRANGE_Sub(INT i, LRANGE_SET_SUBUNIVERSE *sub)
{
  return sub->Nth_Lrange(i);
}

///////////////////////////////////////
// Used by lrange set code to map between integers and LRANGEs.
// Returns the unique LRANGE* associated with 'i' by the given
// subuniverse, 'sub'.  As a special case, returns
// LRANGE_SET_CHOOSE_FAILURE for -1.
LRANGE*
LRANGE_SET_SUBUNIVERSE::Nth_Lrange(INT i)
{
  DevAssert(i !=-1,("Choose failure passed to LRANGE_SET_SUBUNIVERSE::Nth_Lrange"));
  DevAssert(i < Count(), ("LRANGE_SET_SUBUNIVERSE index out of range %d vs %d",
            i, Count()));
  return Lranges()[i];
}

///////////////////////////////////////
// Initialize this package.  No subuniverses may be created before the 
// package is initialized. 
void
LRANGE_SUB_MGR::Initialize(void)
{
  if ( ! pool_initialized ) {
    MEM_POOL_Initialize(&pool,"GRA lrange universe pool",FALSE);
    MEM_POOL_Push(&pool);
    pool_initialized = TRUE;
  }
}

///////////////////////////////////////
// Finalize this package. All subuniverses are no longer valid after the 
// package is finalized.  All the memory allocated for LRANGE_SET_SUBUNIVERSEs 
// is returned when the package is finalized.
void
LRANGE_SUB_MGR::Finalize(void)
{
  if (pool_initialized) {
    MEM_POOL_Pop(&pool);
    MEM_POOL_Delete(&pool);
    pool_initialized = FALSE;
  }
}

///////////////////////////////////////
// Create and return an LRANGE_SET_SUBUNIVERSE with space initially
// allocated for 'initial_size' elements.
LRANGE_SUBUNIVERSE*
LRANGE_SUB_MGR::Create(INT32 initial_size)
{
  LRANGE_SUBUNIVERSE* result = TYPE_MEM_POOL_ALLOC(LRANGE_SUBUNIVERSE, &pool);
  result->count = 0;
  result->alloc_size = initial_size;
  result->lranges = TYPE_MEM_POOL_ALLOC_N(LRANGE*,&pool,initial_size);
  return result;
}

///////////////////////////////////////
// Add 'lrange' to 'subuniverse'.  It must not be a part of any
// other subuniverse.
void
LRANGE_SET_SUBUNIVERSE::Add(LRANGE* lrange)
{
  INT32 id = count++;

  if (count >= alloc_size) {
    // Too small, reallocate it to be twice as large.  That ought to fix it.
    INT32 new_alloc_size = alloc_size * 2;
    GRA_Trace_Memory_Realloc("LRANGE_SET_SUBUNIVERSE::Add()");

    lranges = TYPE_MEM_POOL_REALLOC_N(LRANGE*, &lrange_sub_mgr.pool, lranges, 
				      alloc_size, new_alloc_size);
    alloc_size = new_alloc_size;
  }
  lrange->Id_Set(id);
  lranges[id] = lrange;
}
