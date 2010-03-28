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
//                     Array Dead-Store and Partial Dead-Store Elimination
//                     --------------------------------------------------
//
// Description:
//
// 	In loops given
//	do i
//	  a[i] = ...
//	      ...
//	  a[i] = ...
//
//      dead store eliminate the first store
//
//      also, convert
//
//	do i
//	   a[i] = ...
//	   ...
//	   if (...)
//           a[i] = ...
//
//	into
//
//	do i
//	  t = ...
//	  ...
//	  if (...)
//	    a[i] = ...
//	  else 
//	     a[i] = t
// Exported functions:
//
// void Dead_Store_Eliminate_Arrays(ARRAY_DIRECTED_GRAPH16 *dep_graph)
//
//
//


/**
*** $Source$
**/

#ifndef DEAD_RCS_ID
#define DEAD_RCS_ID
#ifdef _KEEP_RCS_ID
static char *dead = "$Source$ $Revision$";
#endif /* _KEEP_RCS_ID */
#endif

#ifndef DEAD_DECLARE

#define DEAD_DECLARE

void Dead_Store_Eliminate_Arrays(ARRAY_DIRECTED_GRAPH16 *dep_graph);
void Process_Store(WN *, VINDEX16 , ARRAY_DIRECTED_GRAPH16 *);

#endif  // DEAD_DECLARE

