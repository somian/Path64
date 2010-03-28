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


/* -*-Mode: c++;-*- (Tell emacs to use c++ mode) */
#ifndef cxx_ipl_bread_write_INCLUDED
#define cxx_ipl_bread_write_INCLUDED

#ifdef __cplusplus

#ifndef cxx_template_INCLUDED
#include "cxx_template.h"
#endif

extern "C" {
#endif /* __cplusplus */

extern void
IPA_irb_write_summary (struct output_file *fl);

/* Trace the summary information, given a section handle: */
extern void
IPA_Trace_Summary_Section(FILE *f, 
			  const void* section_base,
			  DYN_ARRAY<char*>* symbol_names,
			  DYN_ARRAY<char*>* function_names);

/* Trace the summary information: */
extern void
IPA_Trace_Summary_File (FILE *f, 
			struct output_file *fl, 
			BOOL verbose,
			DYN_ARRAY<char*>* symbol_names,
			DYN_ARRAY<char*>* function_names);

#ifdef __cplusplus
}
#endif /* __cplusplus */
    

#endif
