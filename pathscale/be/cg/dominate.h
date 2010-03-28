/*
 * Copyright (C) 2008 Advanced Micro Devices, Inc.  All Rights Reserved.
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
 * Module: dominate.h
 * $Revision$
 * $Date$
 * $Author$
 * $Source$
 *
 * Description:
 *
 * Definitions for basic block (BB) dominator/post-dominator information
 * generation and maintenence.
 *
 * Constants:
 *
 * Utilities:
 *
 *   void Calculate_Dominators(void)
 *	Calculate_Dominators generates a dominator and post-dominator
 *	bit vector for each BB. Dynamic memory is allocated to hold these
 *	bit vectors and must be freed by calling Free_Dominators_Memory.
 *
 *	NOTE:  The performance of this routine will degrade if the BB list
 *	is not topologically sorted.
 *
 *   void BB_REGION_Calculate_Dominators(const BB_REGION& region)
 *	Generate dominator and post-dominators sets for a BB_REGION.  Assumes
 *	that the dominator/post-dominator information at the borders of the
 *	region is valid. Also, dominator information will not be correct outside the region after
 *      this is called.
 *
 *   void BB_SET_Calculate_Dominators(const BB_SET bbset, BOOL compute_dom, BOOL compute_pdom)
 *      Generate dominator info for the set of BB's in bbset. The dominator info will be correct
 *      and include only those BB's in bbset. If you use this, you'll need to do a global
 *      recomputation later. Compute_dom and compute_pdom allow the computation of only the 
 *      dominator and post dominator sets, if desired. 
 *
 *   void Free_Dominators_Memory(void)
 *	Frees dynamic resources allocated by Calculate_Dominators.
 *
 *   BS *BB_dom_set(BB *bb)
 *	Returns a bit vector indentifying the dominators of <bb>.
 *	Bit indicies correspond to BB_id's.
 *
 *   BS *BB_pdom_set(BB *bb)
 *	Returns a bit vector indentifying the post-dominators of <bb>.
 *	Bit indicies correspond to BB_id's.
 *
 *   BOOL BB_Has_Dominator_Info(BB *bb)
 *	Returns a boolean value to indicate if <bb> has dominator
 *	info available. This will be false for BBs created after
 *	a call to Calculate_Dominators or for all BBs once
 *	Free_Dominators_Memory is called.
 *
 * ====================================================================
 * ====================================================================
 */

#ifndef	DOMINATE_INCLUDED
#define	DOMINATE_INCLUDED

#include "bitset.h"

/* The following declarations are PRIVATE to the implementation:
 */
typedef struct dominators {
  BS **dom_set;
  BS **pdom_set;
  INT  size;
} DOMINATORS;

extern DOMINATORS bb_dom_map;

extern void Set_BB_dom_set(BB *bb, BS *bs);

extern void Set_BB_pdom_set(BB *bb, BS *bs);

/* Public declarations:
 */

#define     BB_dom_set(bb)	((BS *)(bb_dom_map.dom_set[BB_id(bb)]))
#define     BB_pdom_set(bb)	((BS *)(bb_dom_map.pdom_set[BB_id(bb)]))

#define BB_Has_Dominator_Info(bb) (BB_id(bb) < bb_dom_map.size)

extern BOOL Are_Dominators_Calculated(void);
extern void Calculate_Dominators(void);
extern void BB_REGION_Calculate_Dominators(const BB_REGION& region);
extern void BB_SET_Calculate_Dominators(BB_SET *bbset, BOOL compute_dom, BOOL compute_pdom);
extern void Free_Dominators_Memory(void);

#endif /* DOMINATE_INCLUDED */
