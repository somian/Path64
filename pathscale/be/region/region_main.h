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


/*-*-c++-*-*/
/* ====================================================================
 *
 * Module: region_main.h
 * $Revision$
 * $Date$
 * $Author$
 * $Source$
 *
 * Revision history:
 *  8-NOV-95 dahl - Original Version
 *
 * Description:
 *	Definitions for back end driver. Should only be included by
 *	be/be/driver.c
 * ====================================================================*/

#ifndef	region_main_INCLUDED
#define	region_main_INCLUDED
#ifdef __cplusplus
extern "C" {
#endif

/* Create and push the REGION_mem_pool. Create the region_id map, 
   used to track region info after regions have been lowered. */
extern INT REGION_Initialize(WN *, BOOL);

/* Pop the REGION_mem_pool */
extern void REGION_Finalize(void);

/* Iterate over a REGION tree, finding those REGIONs with no "earlier"
   sub-REGION.  When First or Next returns, iter points to a REGION_CS_ITER
   with "kid" representing the location in the REGION tree which we have just
   finished visiting. This amounts to postorder traversal of the RID tree. */
extern void REGION_CS_ITER_init( REGION_CS_ITER *, WN * );
extern BOOL REGION_CS_NoEarlierSub_While( REGION_CS_ITER * );
extern void REGION_CS_NoEarlierSub_Next ( REGION_CS_ITER * );
extern void REGION_CS_NoEarlierSub_First( REGION_CS_ITER *, WN *, RID_TYPE);
extern void REGION_CS_print( REGION_CS_ITER * );

/* Remove a region from the PU and mark location for insertion later */
extern WN  *REGION_remove_and_mark(WN *, REGION_CS_ITER *);
/* Insert new region at mark left by REGION_remove_and_mark */
extern void REGION_replace_from_mark(WN *, REGION_CS_ITER *);

/* These two phases are called by preopt (wodriver.c) for :p (lno off)	*/
/* and by lno (lnodriver.c) for :l (lno on)				*/
extern WN *Rail(WN *);
extern void Region_Bound_Info(WN *, struct DU_MANAGER *, 
			      struct ALIAS_MANAGER *);

/* generate consistent region boundary sets regardless of optimization level
   code is in be/region/region_bounds.cxx */
extern void Generate_region_boundaries(WN *, struct ALIAS_MANAGER *);

#ifdef __cplusplus
}
#endif
#endif /* region_main_INCLUDED */
