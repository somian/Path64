/*
 *  Copyright (C) 2006. QLogic Corporation. All Rights Reserved.
 */

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


#include "basic.h"
#include "string_utils.h" 

extern boolean keep_flag;	/* keep intermediate files */

extern string_list_t *error_list;

extern string_list_t *count_files;

#ifdef KEY /* Bug 11265 */	
/* Accumulate -isystem directories for insertion in command line to Fortran
 * front end */
extern string_list_t *isystem_dirs;
#endif /* KEY Bug 11265 */

/* return object file that corresponds to source name */
extern char *get_object_file (char *src);

/* construct name for intermediate file with given suffix */
extern char *construct_name (char *src, char *suffix);
/* create name for temp file (similar to construct_name except
   it always gives a temp file even in the presence of -keep */
extern char *create_temp_file_name (char *suffix);
/* use given src name, but check if treated as a temp file or not */
extern char *construct_given_name (char *src, char *suffix, boolean keep);

/* For C++ single-source compilations, we still do multiple phases.
 * We need a special mechanism for getting rid of the .o file. */
extern void mark_saved_object_for_cleanup(void);

/* Create filename with the given extension, eg. create foo.anl from foo.f*/
extern char *construct_file_with_extension (char *src, char *ext);

/* init the maintenance of temp files */
extern void init_temp_files (void);

/* init the ability to report compiler crashes */
extern void init_crash_reporting (void);

/* init the maintenance of count files */
extern void init_count_files (void );

/* this will clean up temp files */
extern void cleanup (void);

/* mark file as a temp file to be cleaned up */
extern void mark_for_cleanup (char *file);

/* delete temp object files */
extern void cleanup_temp_objects ();

#ifdef KEY
/* return the report file name */
extern char *get_report_file_name();
#endif
