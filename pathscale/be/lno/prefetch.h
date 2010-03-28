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


/*      Main prefetching pass
 *
 * This file contains the driver routine for the prefetching phase, 
 * along with some support routines.
 *
 * Exported Functions:
 *
 *  void Prefetch (WN* func_nd)
 *
 *      Given the WN* for a PU, 
 *          - perform prefetching analysis
 *          - update func_nd to contain prefetch information
 */

#ifndef prefetch_INCLUDED
#define prefetch_INCLUDED

#include <sys/types.h>
#include "wn.h"
#include "dep_graph.h"

extern BOOL Debug_Prefetch;
extern BOOL Verbose_Prefetch;

extern MEM_POOL *PF_mpool;
extern void Prefetch_Driver (WN* func_nd,
                             ARRAY_DIRECTED_GRAPH16 *array_dep_graph);
extern void Init_Prefetch_Options (WN* func_nd);

void Initialize_Lvs ();
void Cleanup_Lvs ();

#endif // prefetch_INCLUDED
