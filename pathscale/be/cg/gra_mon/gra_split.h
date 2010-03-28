/*
 * Copyright 2006.  QLogic Corporation.  All Rights Reserved.
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

//  Live range splitting
/////////////////////////////////////
//  
//  Description:
//
//      When the coloring process comes to a live range that it cannot color,
//      it tries to split it into a higher priority part that it can color and
//      a bunch of other stuff that is not guaranteed to be colorable.  The
//      second part may be split again when it is considered for coloring.
//      The second part must be placed in the correct position on the coloring
//      list so that it will be considered before any Briggs points of lower
//      priority that intefere with it.
//
//  Exported function:
//
//      BOOL LRANGE_Split( LRANGE* lrange, LRANGE_CLIST_ITER* iter,
//                                         LRANGE**           alloc_lrange )
//          If possible, split <lrange> into a colorable part and a deferred
//          part.  Return TRUE to indicate success, with the new colorable
//          LRANGE returned by reference in <alloc_lrange>.
//
//          In fact, only one new LRANGE is created, the one returned by
//          reference in <alloc_lrange> which is guaranteed to have an
//          avialable register before any further allocations.  The original
//          <lrange> is modified to reflect the split and added back into the
//          coloring list at the appropriate point.  <iter> is the coloring
//          list iterator, <lrange> must be the _Current element of <iter>.
//
/////////////////////////////////////


//  $Revision$
//  $Date$
//  $Author$
//  $Source$


#ifndef GRA_SPLIT_INCLUDED
#define GRA_SPLIT_INCLUDED

#ifndef GRA_SPLIT_RCS_ID
#define GRA_SPLIT_RCS_ID
#ifdef _KEEP_RCS_ID
static char *gra_split_rcs_id = "$Source$ $Revision$";
#endif
#endif

extern BOOL
LRANGE_Split( LRANGE* lrange, LRANGE_CLIST_ITER* iter,
                              LRANGE**           alloc_lrange
#ifdef KEY
                              , BOOL reclaim = FALSE
#endif
	      );

#ifdef KEY
extern void LRANGE_Split_Reclaimed_BBs (LRANGE *lrange, REGISTER reg);
extern BOOL Has_Live_In_Successor (GRA_BB *gbb, LRANGE *lrange);
#endif

#ifdef TARG_X8664
extern BOOL
LRANGE_Split_Mixed_x87_MMX(LRANGE* lrange, LRANGE_CLIST_ITER* iter,
			   LRANGE** alloc_lrange);
#endif

#endif
