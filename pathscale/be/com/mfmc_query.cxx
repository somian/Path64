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


#include "mfmc_defs.h"

/* Is node i on the source side of the fiducial minimum cut?
 */

BOOL
MFMC_Min_cut_lhs(MFMC_HANDLE handle, INT32 i)
{
  return (handle->nodes[i].rank >= handle->n);
}

/* In the fiducial maximum flow, how much commodity flows along the
 * queried arc?
 */
/* --- NOT IMPLEMENTED YET --- */

#if 0
INT64
MFMC_Max_flow_arc_flow(MFMC_HANDLE handle, MFMC_ARC_HANDLE arc_handle)
{
}
#endif

INT64
MFMC_Objective_value(MFMC_HANDLE handle)
{
  return handle->flow_value;
}

MFMC_STATS *
MFMC_Stats(MFMC_HANDLE handle)
{
  return &handle->stats;
}
