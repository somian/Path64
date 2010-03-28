/*
 * Copyright (C) 2008, 2009 Pathscale, LLC.  All Rights Reserved.
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

extern void init_objects (void);

/* need to init crt paths if doing ipa link */
extern void init_crt_paths (void);
extern void init_stdc_plus_plus_path( void );

/*
 * Whether option is an object or not.
 * Our definition of "object" includes libraries and library path options,
 * because we need to keep the order of those correct. 
 */
extern boolean is_object_option (int flag);

/* add object to list of objects */
extern void add_object (int flag, char *arg);
/* add object to list of objects to pass to archive phase. */
extern void add_ar_objects (char *arg);

/* add library to list */
extern void add_library_dir (char *path);

/* get list of library dirs */
extern string_list_t *get_library_dirs (void);

/* append objects to end of list */
extern void append_objects_to_list (string_list_t *list);
extern void append_libraries_to_list (string_list_t *list);
extern void append_cxx_prelinker_objects_to_list (string_list_t *list);
extern void append_ar_objects_to_list (string_list_t *list);

extern void dump_objects (void);	/* for debugging */

/* extra hand-coded stuff for library options */
extern void add_library_options (void);

/* determine whether we have a profiling version of a library */
extern int prof_lib_exists (const char *lib);

/* add library to path */
extern void add_library (string_list_t *list, const char *lib);

/* search library_dirs for the crt file */
extern char *find_crt_path (char *crtname);

boolean is_maybe_linker_option (int flag);
void add_maybe_linker_option (int flag);
void finalize_maybe_linker_options (boolean is_linker);

#ifdef TARG_MIPS
extern char *sysroot_path_n32;
extern char *sysroot_path_64;
#else
extern char *sysroot_path;
#endif

