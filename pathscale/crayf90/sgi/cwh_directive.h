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


/* ====================================================================
 * ====================================================================
 *
 * Module: cwh_directive.h
 * $Revision: 1.7 $
 * $Date: 05/09/22 10:54:47-07:00 $
 * $Author: gautam@jacinth.keyresearch $
 * $Source: crayf90/sgi/SCCS/s.cwh_directive.h $
 *
 * Description: Entry points into cwh_directive.c
 *
 * ====================================================================
 * ====================================================================
 */

#ifndef CWH_DIRECTIVE_INCLUDED
#define CWH_DIRECTIVE_INCLUDED

#ifdef _KEEP_RCS_ID
static char *rcs_id = "$Source: crayf90/sgi/SCCS/s.cwh_directive.h $";
#endif /* _KEEP_RCS_ID */

extern int parallel_do_count;
  /* marks the next n do loop as parallel loops */

extern WN *
cwh_mp_region(       WN_PRAGMA_ID wn_pragma_id,
                     int threadcount,
                     int datacount,
                     int ontocount,
                     int reductioncount,
                     int chunkcount,
		     int is_omp);


/* Adds all the deferred DO loop directives to the current block */
extern void
cwh_directive_insert_do_loop_directives(void); 

/* Adds a directive to the deferred DO loop list */
extern void
cwh_directive_add_do_loop_directive(WN *directive);

/* add fwd/back barriers, list of names and maybe insert WN between barriers*/

extern void
cwh_directive_barrier_insert(WN *ins, int  args) ;
// Bug 3836
#ifdef KEY
extern void 
cwh_directive_set_PU_flags(BOOL nested);
#endif

#endif /* CWH_DIRECTIVE_INCLUDED */

