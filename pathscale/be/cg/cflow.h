/*
 *  Copyright (C) 2007. QLogic Corporation. All Rights Reserved.
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


/* ====================================================================
 * ====================================================================
 *
 * Module: cflow.h
 * $Revision$
 * $Date$
 * $Author$
 * $Source$
 *
 * Revision history:
 *  23-Jan-92 - Original Version
 *
 * Description:
 *
 *   External interface to the control flow transformations. Note
 *   that the interaces described in dominate.h and freq.h are considered
 *   to fall under the "cflow" umbrella.
 *
 * Variables:
 *
 *   BOOL CFLOW_Trace
 *	Enable all cflow tracing.
 *
 *   BOOL CFLOW_Trace_Detail
 *	Enable verbose tracing.
 *
 *   BOOL CFLOW_Trace_Unreach
 *	Enable tracing of unreachable basic block removal.
 *
 *   BOOL CFLOW_Trace_Branch
 *	Enable tracing of redundant branch removal.
 *
 *   BOOL CFLOW_Trace_Merge
 *	Enable tracing of basic block merging.
 *
 *   BOOL CFLOW_Trace_Reorder
 *	Enable tracing of basic block reordering.
 *
 *   BOOL CFLOW_Trace_Freq_Order
 *	Enable tracing of basic block frequency-guided ordering.
 *
 *   BOOL CFLOW_Trace_Freq
 *	Enable tracing of frequency estimates.
 *
 *   BOOL CFLOW_Trace_Clone
 *	Enable tracing of BB cloning.
 *
 *   BOOL CFLOW_Trace_Dom
 *	Enable tracing of BB dominator set calculation.
 *
 * Constants:
 *
 *   INT CFLOW_UNREACHABLE
 *   INT CFLOW_BRANCH
 *   INT CFLOW_MERGE
 *   INT CFLOW_REORDER
 *   INT CFLOW_FREQ_ORDER
 *   INT CFLOW_CLONE
 *	Enables a particular optimization.
 *
 *   INT CFLOW_ALL_OPTS
 *	Perform all optimizations (an OR of the above constants).
 *
 *   INT CFLOW_OPT_ALL_BR_TO_BCOND
 *	When performing branch optimizations, always convert a branch
 *	to a conditional branch, even if it expands code size.
 *
 *   INT CFLOW_FILL_DELAY_SLOTS
 *	Implies that the input has delay slots filled, and that cflow
 *	should maintain this property.
 *
 * Utilities:
 *
 *   void CFLOW_Initialize(void)
 *	Perform one-time initialization.
 *
 *   void CFLOW_Optimize(INT32 flags, const char *phase_name)
 *	Perform control flow based optimizations according to the
 *	<flags> mask. <phase_name> is a string which identifies
 *	this cflow pass -- it is only used in messages.
 *
 * ====================================================================
 * ====================================================================
 */

#ifndef cflow_INCLUDED
#define cflow_INCLUDED

extern BOOL CFLOW_Trace;
extern BOOL CFLOW_Trace_Detail;
extern BOOL CFLOW_Trace_Unreach;
extern BOOL CFLOW_Trace_Branch;
extern BOOL CFLOW_Trace_Merge;
extern BOOL CFLOW_Trace_Reorder;
extern BOOL CFLOW_Trace_Clone;
extern BOOL CFLOW_Trace_Freq_Order;
extern BOOL CFLOW_Trace_Freq;
extern BOOL CFLOW_Trace_Dom;

/* "flags" for CFLOW_Optimize:
 */
#define CFLOW_UNREACHABLE		(0x00000001)
#define CFLOW_BRANCH			(0x00000002)
#define CFLOW_MERGE			(0x00000004)
#define CFLOW_REORDER			(0x00000008)
#define CFLOW_CLONE			(0x00000010)
#define CFLOW_FREQ_ORDER		(0x00000020)
#define CFLOW_OPT_ALL_BR_TO_BCOND	(0x00000040)
#define CFLOW_FILL_DELAY_SLOTS		(0x00000080)
#define CFLOW_IN_CGPREP			(0x00000100)
#define CFLOW_ALL_OPTS \
	(CFLOW_UNREACHABLE|CFLOW_BRANCH|CFLOW_MERGE|CFLOW_REORDER\
	|CFLOW_FREQ_ORDER|CFLOW_CLONE)

extern void CFLOW_Optimize(INT32 flags, const char *phase_name);
extern void CFLOW_Initialize(void);

#ifdef TARG_MIPS
extern void CFLOW_Fixup_Long_Branches(void);
#endif

#endif /* cflow_INCLUDED */
