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
#ifndef LANG_DEFS_H
#include "lang_defs.h"
#endif

extern int show_version;	/* show the compiler version */
extern boolean show_copyright;	/* show the compiler copy right */
extern boolean dump_version;	/* dump the compiler version */
extern boolean show_flag;	/* show what driver does */
extern boolean execute_flag;	/* execute phases */
extern boolean time_flag;	/* give time info */
extern boolean run_m4;		/* run m4 on each ratfor file */
extern boolean prelink_flag;	/* C++ only:  run the prelinker before ld */
extern boolean quiet_flag;	/* g++: suppress timing information */
extern boolean show_search_path; /* show the directory search path */
extern boolean show_defaults;   /* show the default compiler options */

extern const char compiler_version[];

/* run a phase of the compiler */
extern void run_phase (phases_t, char *, string_list_t *); 

/* exec another program, putting result in output.
 * This is simple version of full run_phase. */
extern void run_simple_program (char *name, char **argv, char *output);

/* Handler () is used for catching signals.  */
extern void handler (int sig);

/* set signal handler */ 
extern void catch_signals (void);

/* exit from driver */
extern void do_exit (int code)
     __attribute__ ((noreturn));

/* get gcc version string, e.g. "2.7.2".
 * put numeric result in passed param (int), if not null. */
extern const char *get_gcc_version (int *, int);
