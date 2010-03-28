/*
 * Copyright (C) 2008 PathScale, LLC.  All Rights Reserved.
 */

/*
 * Copyright (C) 2006, 2007. QLogic Corporation. All Rights Reserved.
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

extern char *source_file;
extern boolean multiple_source_files;

extern char *outfile;		/* outfile for -o */
extern char *prof_file;        /*  executable file for prof to work upon */
extern char *fb_file;		/* feedback file for -fb_create */
extern char *internal_fb_file;	/* feedback file for -fb */
extern char *opt_file;		/* feedback file for -fb_opt */
extern char *fb_xdir;		/* dir where pixie emits dso's */
extern char *fb_cdir;		/* dir where pixie emits count files */
extern char *fb_phase;         /* phase for -fb_phase */
extern char *fb_type;          /* type for -fb_type */
#ifdef KEY
extern char *coco_setfile;	/* setfile for -fcoco */
#endif

#ifdef TARG_X8664
extern int mregparm;
extern boolean msseregparm;
#endif

extern char *ldpath_for_pixie;  /* Tell pixie where to find ld */
extern int saved_argc;		/* original argc */
extern char **saved_argv;	/* original argv */
extern char *command_line;	/* original command line */

extern boolean keep_mp;		/* keep pfa/pca file */
extern boolean keep_list;	/* keep pfa/pca listing */
extern boolean keep_listing;	/* keep listing file */
extern boolean keep_cif;	/* keep cif file */
extern boolean auto_parallelize;	/* invoke lno:ap=1 */
extern boolean Gen_feedback;	/* Generate pixified binary for spec */
extern boolean Use_feedback;	/* Use feedback for spec */
extern boolean Disable_open_mp; /* Disable the recognition of open mp */
extern boolean Disable_old_mp;  /* Disable the recognition old style mp */
extern char roundoff;		/* roundoff level for pfa */
extern boolean O3_flag;		/* -O3 has been specified */
extern boolean nocpp_flag;	/* don't invoke cpp */
extern boolean use_cpp;		/* Use cpp instead of ftpp for F90 */
extern boolean expand_ftpp_macros; /* fully macro-expand in ftpp */
extern int     fortran_line_length;	/* Line length for fixed form fortran */

extern char *ld_library_path;	/* env. variable LD_LIBRARY_PATH */
extern char *ld_libraryn32_path;   /* env. variable LD_LIBRARYN32_PATH */

extern char *orig_program_name; /* actual name passed in to argv[0] */

boolean dump_outfile_to_stdout;	// for "-o -"


/* call once before running compiler */
extern void init_phase_info (void);

/* run dsm_prelink */
extern void run_dsm_prelink(void);

/* run ld only */
extern void run_ld (void);
extern void run_ar (void);
extern void run_pixie (void);
extern void run_prof (void);

/* run whole compiler */
extern void run_compiler (int argc, char *argv[]);

/* return fixed-up file name */
extern char *fix_name_by_lang (char *name);

/* save original command line */
extern void save_command_line(int, char **);
extern void set_current_arg_pos(int n);
extern int check_for_saved_option(char *s);
extern void cancel_saved_arg(int count);
extern void add_minus_c_option(void);

/* save user options for ipl */
extern void save_ipl_commands (void);
extern char *dirname(char *const s);

int quote_shell_arg (char *p, char *buf);

#ifdef KEY
// Get the system GCC's major version number.
extern int get_gcc_major_version(void);
extern void init_frontend_phase_names(int, int);
#endif

#define PASS1 0
#define PASS2 1



