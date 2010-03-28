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

// LRANGE_VSET implementation
/////////////////////////////////////

//  $Revision$
//  $Date$
//  $Author$
//  $Source$

#ifdef USE_PCH
#include "cg_pch.h"
#endif // USE_PCH
#pragma hdrstop

#ifdef _KEEP_RCS_ID
static char *rcs_id = "$Source$ $Revision$";
#endif

#include "defs.h"
#ifndef __GNUC__
#include "bstring.h"
#else
#include "string.h"
#endif
#include "gra_bb.h"
#include "gra_lrange.h"
#include "gra_lrange_vset.h"

/////////////////////////////////////
// Create and return a new LRANGE_VSET (in the GRA_pool).  Its
// elements are given in <vec>, a vector containing <count>
// distinct LRANGE pointers.
LRANGE_VSET*
LRANGE_VSET_Create( LRANGE** vec, size_t count, MEM_POOL *pool )
{
  LRANGE_VSET* result =
    (LRANGE_VSET*) MEM_POOL_Alloc(pool,  sizeof(LRANGE_VSET)
                                       + (count - 1) * sizeof(LRANGE*));
  //  To use bcopy or not?  I think that on banyon (due to improved shared
  //  library version), it will probably do better with long vectors than
  //  anything we can write in simple C without really understanding which
  //  compiler will be used.  *Sigh*
  bcopy((void*) vec,
        (void*) (result->vec),
        count * sizeof(LRANGE*));
  result->count = count;
  return result;
}

/////////////////////////////////////
// Deletes <lrange> from <vec>.  Raises an error if it doesn't
// find <lrange> in <vec>.
void
LRANGE_VSET::Delete_Element( LRANGE* lrange )
{
  size_t   i;

  for ( i = 0; i < count; ++i ) {
    if ( vec[i] == lrange ) {
      if ( i < count - 1 )
        vec[i] = vec[count - 1];
      --count;
      return;
    }
  }

  DevWarn("LRANGE_VSET::Delete_Element -- LRANGE not found");
}

/////////////////////////////////////
// <new> takes the place of <old>
void
LRANGE_VSET::Replace_Element( LRANGE* old_lr, LRANGE* new_lr )
{
  size_t   i;

  for ( i = 0; i < count; ++i ) {
    if ( vec[i] == old_lr ) {
      vec[i] = new_lr;
      return;
    }
  }

  DevWarn("LRANGE_VSET_Replace_Element -- old LRANGE not found");
}
