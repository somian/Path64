/*
 * Copyright 2003, 2004, 2005, 2006 PathScale, Inc.  All Rights Reserved.
 */

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

// LUNIT implemetation

//  $Revision: 1.5 $
//  $Date: 04/12/21 14:57:08-08:00 $
//  $Author: bos@eng-25.internal.keyresearch.com $
//  $Source: /home/bos/bk/kpro64-pending/be/cg/gra_mon/SCCS/s.gra_lunit.cxx $

#ifdef USE_PCH
#include "cg_pch.h"
#endif // USE_PCH
#pragma hdrstop

#ifdef _KEEP_RCS_ID
static char *rcs_id = "$Source: /home/bos/bk/kpro64-pending/be/cg/gra_mon/SCCS/s.gra_lunit.cxx $ $Revision: 1.5 $";
#endif

#include "defs.h"
#include "errors.h"
#include "mempool.h"
#include "register.h"
#include "bb.h"
#include "gra_bb.h"
#include "gra_lunit.h"
#include "gra_lrange.h"

#ifdef TARG_X8664
#include "targ_sim.h"   // For RAX, RCX and RDX
#endif


/////////////////////////////////////
// Create and return a new LUNIT associated with <lrange> and
// <gbb>.  The new LUNIT is also added to the collections of
// LUNITs associated with <lrange> and <gbb>.  See gra_bb.h for
// the definition of GRA_BB_RC_LUNIT_ITER and gra_lrange.h for
// the definition of LRANGE_LUNIT_ITER.
extern LUNIT*
LUNIT_Create( LRANGE* lrange, GRA_BB* gbb )
{
  LUNIT* result = TYPE_MEM_POOL_ALLOC(LUNIT,GRA_pool);

  result->lrange = lrange;
  result->gbb = gbb;
  result->pref_priority = 0.0;
  result->allowed_preferences = REGISTER_SET_EMPTY_SET;
  result->flags = 0;
  result->def_count = 0;
  result->last_def = -1;
  result->global_pref = NULL;
  gbb->Add_LUNIT(result);
  lrange->Add_LUNIT(result);
  return result;
}

/////////////////////////////////////
// Call once for each preferencing copy seen that copies into or
// out of <lunit>'s LRANGE in its BB.  <lrange> is the other side
// of the copy -- where we are copying lunit to or from.
void 
LUNIT::Preference_Copy(LRANGE *lr) 
{
  pref_priority += gbb->Freq();
  if (lr->Type() == LRANGE_TYPE_LOCAL && lr->Has_Wired_Register()) {
#ifdef TARG_X8664
    /* Relax me!!!
       The following condition is necessary when the curent lrange does not
       across an operation which uses RAX, RCX or RDX implicitly.
    */
    if( lr->Reg() == RAX ||
	lr->Reg() == RCX ||
	lr->Reg() == RDX )
      return;
#endif
    allowed_preferences = REGISTER_SET_Union1(allowed_preferences,lr->Reg());
  }
}

/////////////////////////////////////
// Is <lunit>'s LRANGE live-out of its BB?
BOOL 
LUNIT::Live_Out(void) 
{ 
  return GTN_SET_MemberP(BB_live_out(gbb->Bb()), lrange->Tn()); 
}

/////////////////////////////////////
// Is <lunit>'s LRANGE live-in to its BB?
BOOL 
LUNIT::Live_In(void) 
{ 
  return GTN_SET_MemberP(BB_live_in(gbb->Bb()), lrange->Tn()); 
}

/////////////////////////////////////
// Return the priority of the given <lunit>.
float 
LUNIT::Priority(void) {
  float result;
  result = Live_In() ? gbb->Freq() : 0.0;
  if (Live_Out() && Has_Def())
    result += gbb->Freq();
  return result;
}
