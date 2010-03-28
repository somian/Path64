/*
 * Copyright 2005, 2006 PathScale, Inc.  All Rights Reserved.
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
 * Module: cwh_block.h
 * $Revision: 1.4 $
 * $Date: 05/09/22 10:54:46-07:00 $
 * $Author: gautam@jacinth.keyresearch $
 * $Source: crayf90/sgi/SCCS/s.cwh_block.h $
 *
 * Revision history:
 *  dd-mmm-95 - Original Version
 *
 * Description: Entry points into cwh_block.cxx
 *
 * ====================================================================
 * ====================================================================
 */

#ifndef CWH_BLOCK_INCLUDED
#define CWH_BLOCK_INCLUDED

#ifdef _KEEP_RCS_ID
static char *rcs_id = "$Source: crayf90/sgi/SCCS/s.cwh_block.h $ $Revision: 1.4 $";
#endif /* _KEEP_RCS_ID */

/* block stack routines */

extern void cwh_block_push_region(WN *region);
extern WN * cwh_block_pop_region(void);

extern WN * cwh_block_set_region_pragmas(BOOL assert = TRUE);
extern void cwh_block_push_block(WN *deferred, WN *append, BOOL is_top_pdo) ;
extern void cwh_block_pop_block(void) ;
extern void cwh_block_add_to_enclosing_regions(WN_PRAGMA_ID id, ST * st);


/* block maintenance routines */

extern WN * cwh_block_current(void);
extern void cwh_block_set_current(WN * blk );
extern WN * cwh_block_new_and_current(void) ;
extern WN * cwh_block_exchange_current(WN * blk );

extern void cwh_block_init_pu() ;
extern BOOL cwh_block_toggle_debug(BOOL debug) ;


/* adding items to blocks */

extern void cwh_block_append(WN *wn) ; 
extern void cwh_block_append_given_block(WN *wn, WN* block); 
extern void cwh_block_insert_after(WN *wn, WN*in) ;


/* predefined blocks of code, to be added at convenient times */
/* via  enum to describe possible blocks                      */

enum block_id{
  Defer_Block,              /* deferred after a statement */
  Preamble_Block,           /* Last part of preamble      */
  First_Block,              /* First block after preamble */
  Top_of_Loop_Block         /* 1st block in loop          */
}; 

extern void cwh_block_append_given_id(WN *wn, enum block_id block,BOOL first) ;
extern void cwh_block_append_given(enum block_id id) ;



#endif /* CWH_BLOCK_INCLUDED */

