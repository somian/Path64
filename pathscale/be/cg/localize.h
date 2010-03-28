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
 *  Module: localize.h
 *  $Revision$
 *  $Date$
 *  $Author$
 *  $Source$
 *
 *  Description:
 *  ============
 *
 *  Localize any global TNs.
 *  The goal is to avoid the need for GRA by removing all global TNs.
 *  First we make a pass through the bb's to find any global TNs,
 *  then we insert save/restore code in each bb to turn those into local TNs.
 *
 * =======================================================================
 * =======================================================================
 */

#ifndef localize_INCLUDED
#define localize_INCLUDED

/* to get the definition of RID. */
#include "region_util.h"

/*
 * Change any global TNs into local TNs
 * within the current REGION or the whole PU if rid is NULL
 */
extern void Localize_Any_Global_TNs ( RID *rid );

/*
 * Check if a dedicated TN is global, and if so, make it local.
 * Our assumptions are that a use of a param reg should be in the
 * entry block, a def of a param reg should be in the call block,
 * the use of a return reg should be in the block following a call block,
 * and the def of a return reg should be in the exit block.
 */
extern void Check_If_Dedicated_TN_Is_Global (TN *tn, BB *current_bb, BOOL def);

/* When using GRA, still want to localize dedicated tns involved in 
 * calls that cross bb's, and replace dedicated TNs involved in 
 * REGION interface with the corresponding allocated TNs from 
 * previously compiled REGIONs.
 */
extern void Localize_or_Replace_Dedicated_TNs (void);

#endif /* localize_INCLUDED */
