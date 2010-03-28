/*
 * Copyright 2004, 2005, 2006, 2007 PathScale Inc.  All Rights Reserved.
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

#ifndef _GCCFE_WGEN_OMP_DIRECIVES_H_
#define _GCCFE_WGEN_OMP_DIRECIVES_H_

struct ST_list {
    ST *st;
    ST_list *next;
} ;

struct WN_list {
    WN *wn;
    WN_list *next;
} ;

extern void 
WGEN_expand_start_parallel (gs_t stmt);

extern void 
WGEN_expand_end_parallel ();

extern void 
WGEN_expand_start_for (gs_t stmt);

extern void 
WGEN_expand_end_for(void);

extern void 
WGEN_expand_start_sections (gs_t statement);

extern void 
WGEN_expand_end_sections(void);

extern void 
WGEN_expand_start_section (void);

extern void 
WGEN_expand_end_section (void);

extern void 
WGEN_expand_start_single (gs_t stmt);

extern void 
WGEN_expand_end_single(void);

extern void WGEN_expand_start_parallel_for (gs_t stmt);

extern void WGEN_expand_end_parallel_for (void);

extern void WGEN_expand_start_parallel_sections (gs_t stmt);

extern void WGEN_expand_end_parallel_sections (void);

extern void WGEN_expand_start_master (void);

extern void WGEN_expand_end_master (void);

extern void  WGEN_expand_start_critical (ST *region_phrase,char* name);

extern void  WGEN_expand_end_critical ( );

extern void  WGEN_expand_start_atomic (void);

extern void WGEN_expand_end_atomic (void);

extern void  WGEN_expand_start_ordered (void);

extern void  WGEN_expand_end_ordered (void);

extern void  WGEN_expand_barrier ( );

extern void  WGEN_expand_flush (WN_list *flush_variables);

extern void WGEN_expand_start_do_loop (WN *, WN *, WN *, WN *);

extern void WGEN_expand_end_do_loop (void);

extern BOOL Trace_Omp;
#endif

