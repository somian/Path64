/*
 *  Copyright (C) 2008 PathScale, LLC.  All Rights Reserved.
 */

/*
 *  Copyright (C) 2006, 2007. QLogic Corporation. All Rights Reserved.
 */

/*
 * Copyright 2002, 2003, 2004, 2005, 2006 PathScale, Inc.  All Rights Reserved.
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

/* values of options */

#define UNDEFINED	-1	/* for any undefined option */

#define ENDIAN_BIG	0
#define ENDIAN_LITTLE	1

#define EXTENDED_ANSI	0
#define KR_ANSI		1
#define STRICT_ANSI	2
#define POSIX		3

#define CALL_SHARED	0
#define NON_SHARED	1
#define DSO_SHARED	2
#define RELOCATABLE	4

#define NORMAL_MP	0
#define CRAY_MP		1

typedef enum {
  M_CPU,
  M_ARCH,
  M_TUNE,
} m_flag;
  
typedef enum {
  ABI_RAG32,
  ABI_N32,
  ABI_64,
  ABI_I32,
  ABI_I64,
  ABI_IA32
} ABI;
typedef enum {
  ISA_NONE      = 0,
  ISA_MIPS1     = 1,
  ISA_MIPS2     = 2,
  ISA_MIPS3     = 3,
  ISA_MIPS4     = 4,
  ISA_MIPS5     = 5,
  ISA_MIPS6     = 6,
  ISA_MIPSX     = 9,
  ISA_MIPS64    = 10,
  ISA_IA641     = 11,
  ISA_IA32      = 12,
  ISA_X8664     = 13,
} ISA;

extern boolean debug;		/* debugging turned on */

extern boolean nostdinc;	/* no standard include directory */

extern char *help_pattern;	/* pattern string for help file */

extern int inline_t;            /* toggle for inline options */

#ifdef KEY
/* Before front-end: UNDEFINED.  After front-end: TRUE if inliner will be run.
   Bug 11325. */
extern int run_inline;
#endif

extern boolean dashdash_flag;   /* when you see -- set this flag to
				   indicate the end of the options */

extern boolean read_stdin;	/* read stdin for input */

extern boolean  xpg_flag;

extern int default_olevel;	/* default optimization level */

extern int ofast;		/* Ofast has been set */

extern int ffast_math_prescan;	// Bug 14302: ffast_math set in prescan

extern int instrumentation_invoked;	/* Instrument whirl */

extern boolean ftz_crt;		/* add flush-to-zero crt */

#ifdef KEY
extern char *f90_module_dir;	/* value of -module option */	// bug 4210
extern int malloc_algorithm;
#endif

/* return whether has been toggled yet */
extern boolean is_toggled (int obj);

/* set obj to value; allow many toggles; last toggle is final value */
extern void toggle (int *obj, int value);

/* Options for configuring the target: */

/* Verify that the target selection is consistent and set defaults: */
extern void Check_Target ( void );

/* process -F option (different for Fortran and C++) */
extern void dash_F_option(void);

/* untoggle the object, so it can be re-toggled later */
extern void untoggle (int *obj, int value);

/* save value in string */
extern void save_name (char **obj, char *value);

/* do action associated with flag */
extern void opt_action (int optflag);

/* return whether to mark this flag as unseen */
extern boolean flag_is_superceded (int optflag);

#ifdef KEY /* bug 4260 */
extern void check_convert_name(char *name);
#endif /* KEY bug 4260 */

/* check if there is a -- and _XPG is set */
extern void check_dashdash ( void );

/* set options for DSM options */
extern void set_dsm_options (void);

extern void Process_Mp (void);
extern void Process_Cray_Mp (void);

extern void print_file_path (char *, int);	/* print path to named file */

extern int subverbose ;

extern char *target_cpu;
