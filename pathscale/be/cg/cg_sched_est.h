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




#ifndef cg_sched_est_INCLUDED
#define cg_sched_est_INCLUDED

#include "mempool.h"
#include "ti_res_count.h"
#include "bb.h"
#include "bb_set.h"
#include "bb_map.h"


/* Define the different conditions under which sched_est interface is
   invoked */

#define SCHED_EST_FOR_SWP               0x001
#define SCHED_EST_FOR_UNROLL            0x002
#define SCHED_EST_FOR_IF_CONV           0x004
#define SCHED_EST_FOR_CLONING           0x008
#define SCHED_EST_FOR_GCM               0x010
#define SCHED_EST_FOR_HB		0x020

#define SCHED_EST_USE_DEP_GRAPH         0x020
#define SCHED_EST_IGNORE_BRANCH         0x040
#define SCHED_EST_IGNORE_PREFETCH       0x080
#define SCHED_EST_IGNORE_INT_OPS        0x100
#define SCHED_EST_IGNORE_LOH_OPS        0x200
#define SCHED_EST_IGNORE_COPY           0x400

typedef UINT16 SCHED_EST_TYPE;

typedef UINT32 (*COST_FUNCTION)(const UINT32, const UINT32, const UINT32);

inline UINT32
umax(const UINT32 value1, const UINT32 value2)
{
  return value1 > value2 ? value1 : value2;
}

typedef struct cg_sched_est {
  BB_SET *contents;
  TI_RES_COUNT *res_count;
  BB_MAP latency_to_map;
  BB_MAP order;
  mUINT32 cached_crit_path_len;
  mUINT32 cached_resource_cycles;
  mINT16 sched_cycles;
  mBOOL use_dep_graph;
  mBOOL latency_to_map_dirty;
} CG_SCHED_EST;

extern UINT32 CG_SCHED_EST_Cycles(CG_SCHED_EST *se);

extern UINT32 CG_SCHED_EST_Critical_Length(CG_SCHED_EST *se);

extern UINT32 CG_SCHED_EST_Resource_Cycles(CG_SCHED_EST *se);

extern CG_SCHED_EST *CG_SCHED_EST_Create(BB *bb, MEM_POOL *pool,
					 SCHED_EST_TYPE type);

extern CG_SCHED_EST *CG_SCHED_EST_Create_Empty(MEM_POOL *pool,
					       SCHED_EST_TYPE type);

extern CG_SCHED_EST *CG_SCHED_EST_Path_Create(BB_SET *path,
					      BB_MAP bb_ests,
					      MEM_POOL *pool,
					      SCHED_EST_TYPE type);

extern CG_SCHED_EST *CG_SCHED_EST_Clone(CG_SCHED_EST *se, MEM_POOL *pool);
					
extern float CG_SCHED_EST_Avg_Cycles_Thru(BB_SET *region, BB *entry, 
					  BB_MAP ests, SCHED_EST_TYPE type);

extern UINT32 CG_SCHED_EST_BB_Cycles(BB *bb, SCHED_EST_TYPE type);

extern float CG_SCHED_EST_Region_Cycles(BB_SET *region, BB *entry, BOOL merged,
					SCHED_EST_TYPE type);
extern void CG_SCHED_EST_Ignore_Op(CG_SCHED_EST *se, OP *op);

extern void CG_SCHED_EST_Append_Scheds(CG_SCHED_EST *se,
				       CG_SCHED_EST *other_se);

extern BOOL CG_SCHED_EST_Is_Region(BB_SET *region, BB *entry);

extern void CG_SCHED_EST_Print(FILE *fp, CG_SCHED_EST *se);
#pragma mips_frequency_hint NEVER CG_SCHED_EST_Print

extern void CG_SCHED_EST_Delete(CG_SCHED_EST *se);

inline void CG_SCHED_EST_Add_Op_Resources(CG_SCHED_EST *se, TOP top) {
  se->cached_resource_cycles = 0;
  se->cached_crit_path_len = 0;
  TI_RES_COUNT_Add_Op_Resources(se->res_count, top);
}
     
inline void CG_SCHED_EST_Subtract_Op_Resources(CG_SCHED_EST *se, TOP top) {
  se->cached_resource_cycles = 0;
  se->cached_crit_path_len = 0;
  TI_RES_COUNT_Subtract_Op_Resources(se->res_count, top);
}

inline double CG_SCHED_EST_Resources_Min_Cycles(CG_SCHED_EST *se) {
  return TI_RES_COUNT_Min_Cycles(se->res_count);
}

extern BOOL CG_SCHED_EST_calc_dep_graph;
extern BOOL CG_SCHED_EST_use_locs;
extern BOOL CG_SCHED_EST_call_cost;


#endif /* cg_sched_est_INCLUDED */
