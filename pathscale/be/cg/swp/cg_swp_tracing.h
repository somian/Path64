/* 
 *  Copyright (C) 2008  PathScale, LLC. All Rights Reserved.
 *
   Path64 is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by
   the Free Software Foundation version 3

   Path64 is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
   or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
   License for more details.

   You should have received a copy of the GNU General Public License
   along with Path64; see the file COPYING.  If not, write to the Free
   Software Foundation, 51 Franklin Street, Fifth Floor, Boston, MA
   02110-1301, USA.
 */

#ifndef cg_swp_tracing_INCLUDED
#define cg_swp_tracing_INCLUDED "cg_swp_tracing.h"

enum SWP_TRACING {
  DETAILED_FLAG  = 0x00000001,
  SUMMARY_FLAG   = 0x00000002,
  DDG_FLAG       = 0x00000004,
  MII_FLAG       = 0x00000008,
  SCHEDULER_FLAG = 0x00000010,
  MINDIST_FLAG   = 0x00000020,
  MINLT_FLAG     = 0x00000040,
  SLACK_FLAG     = 0x00000080,
  HEURISTIC_FLAG = 0x00000100,
  MRT_FLAG       = 0x00000200,
  LIFETIME_FLAG  = 0x00000400,
  LIFENESS_FLAG  = 0x00000800,
  MVE_FLAG       = 0x00001000,
  REGISTER_FLAG  = 0x00002000,
  CODE_GEN_FLAG  = 0x00004000,
  UNROLL_FLAG    = 0x00008000,
  GRA_FLAG       = 0x00010000
};

#endif /* cg_swp_tracing_INCLUDED */

