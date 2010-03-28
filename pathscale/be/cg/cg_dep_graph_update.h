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
 *  Module: cg_dep_graph_update.h
 *  $Revision$
 *  $Date$
 *  $Author$
 *  $Source$
 *
 *  Revision comments:
 *
 *  3-Aug-1995 - Initial version
 *
 *  Description:
 *  ============
 *
 *  Some semi-private routines called only from the low-level BB and OP
 *  manipulation routines to keep an active dependence graph up-to-date.
 *  These functions are implemented in "cg_dep_graph.cxx".
 *
 *  See "cg_dep_graph.h" for the main CG dep graph interface.
 *
 *
 *  Keeping the dependence graph up to date requires calling the following
 *  "update" functions at the appropriate time by the client.
 *
 *  IMPORTANT: When adding/changing/deleting multiple OPs, these update
 *  routines must be called as each *individual* OP is added/changed/deleted.
 *  They may not work correctly (one symptom is "OP has no CG_DEP info" error)
 *  if a bunch of ops are added/changed/deleted before any of the update
 *  routines are called.
 *
 * =======================================================================
 * =======================================================================
 */

#ifndef CG_DEP_GRAPH_UPDATE_INCLUDED
#define CG_DEP_GRAPH_UPDATE_INCLUDED

typedef struct bb BB;

extern BB * _cg_dep_bb;

inline BOOL CG_DEP_Has_Graph(BB *bb)
{
  return _cg_dep_bb == bb;
}

#endif /* CG_DEP_GRAPH_UPDATE_INCLUDED */
