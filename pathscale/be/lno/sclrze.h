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


//-*-c++-*-
//                     Array Scalarization
//                     -------------------
//
// Description:
//
// 	In loops, convert things that look like
//	do i
//	  a[i] = ...
//	  ... = a[i]
//
//	into
//
//	do i
//	  t = ...
//	  a[i] = t
//	  ... = t
//
//	This is useful because 
//	  1) It gets rid of loads
//	  2) If it gets rid of all the loads to a local array then
//	     the array equivalencing algorithm will get rid of the array
//
//	Because SWP will do 1 as well as we do, we'll only apply this
//	algorithm to local arrays (Although it's trivial to change this).
//
// Exported functions:
//
// void Scalarize_Arrays(ARRAY_DIRECTED_GRAPH16 *dep_graph,
//	BOOL do_variants, BOOL do_invariants)
//
//
//
//


/**
*** $Source$
**/

#ifndef SCLRZE_RCS_ID
#define SCLRZE_RCS_ID
#ifdef _KEEP_RCS_ID
static char *sclrze = "$Source$ $Revision$";
#endif /* _KEEP_RCS_ID */
#endif

#ifndef SCLRZE_DECLARE

void Scalarize_Arrays(ARRAY_DIRECTED_GRAPH16 *dep_graph,
	BOOL do_variants, BOOL do_invariants, REDUCTION_MANAGER *red_manager);

#endif  // SCLRZE_DECLARE

