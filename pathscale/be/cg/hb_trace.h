/*
 * Copyright 2004, 2005, 2006 PathScale, Inc.  All Rights Reserved.
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


/////////////////////////////////////
//
//  Defines and such for tracing.
//
//  Exported functions:
//
//	void HB_Trace_Init(void)
//	  Initialize trace state.
//
//	BOOL HB_Trace(INT trace_flag)
//	  Returns true if that trace is enabled
//
//	void HB_PATH_Trace(HB_PATH* path)
//	  Print debugging trace for <path>.
//
//	void HB_Trace_If_Convert_Blocks(HB* hb)
//	  Print bbset being if-converted for <hb>.
/////////////////////////////////////
#ifndef HB_TRACE_H_INCLUDED
#define HB_TRACE_H_INCLUDED

#include <stdio.h>
#include <list>

#include "tracing.h"
#include "hb_path.h"
#include "hb_id_candidates.h"

/////////////////////////////////////
// 
// Trace flags.
//
/////////////////////////////////////
#define HB_TRACE_ID		0x00000001
#define HB_TRACE_SELECT		0x00000002
#define HB_TRACE_DUP		0x00000004
#define HB_TRACE_CONVERT	0x00000008
#define HB_TRACE_CDEP	        0x00000010
#define HB_TRACE_HAMMOCK        0x00000020
#define HB_TRACE_BEFORE	        0x00000080
#define HB_TRACE_DRAWFLOW1      0x00000100
#define HB_TRACE_DRAWFLOW2      0x00000200
#define HB_TRACE_DRAWFLOW3      0x00000400
#define HB_TRACE_TO_STDOUT	0x80000000

/////////////////////////////////////
inline BOOL
HB_Trace(INT trace_flag)
/////////////////////////////////////
//  See interface description.
/////////////////////////////////////
{
  return Get_Trace(TP_HBF, trace_flag);
}

extern void HB_PATH_Trace(HB_PATH* path);
extern void HB_Trace_Init(void);
extern void HB_Trace_If_Convert_Blocks(HB* hb);
extern void HB_Trace_HB_List();
extern void HB_Trace_Candidates(const char* tstring,
				std::list<HB_CAND_TREE*>& cands);
extern void HB_Trace_Print_Cand_Tree(HB_CAND_TREE* cand,INT indent=0);

extern FILE *HB_TFile;

#endif



