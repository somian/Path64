//-*-c++-*-
// ====================================================================
// ====================================================================
//
// Module: opt_config.h
// $Revision$
// $Date$
// $Author$
// $Source$
//
// ====================================================================
//
// Copyright (C) 2000, 2001 Silicon Graphics, Inc.  All Rights Reserved.
//
/*
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
//
// ====================================================================
// ====================================================================


#ifndef opt_config_INCLUDED
#define opt_config_INCLUDED             "opt_config.h"
#ifdef _KEEP_RCS_ID
static char *opt_configrcs_id = 	opt_config_INCLUDED"$Revision$";
#endif /* _KEEP_RCS_ID */

/* The -WOPT: group option flag variables are now in common/com: */
#include "config_wopt.h"

/*********************************************************************
 ***
 *** Anything that should not be seen outside of opt_config.c should
 *** be declared in this section.
 ***
 *********************************************************************
 */
#ifdef OPT_INTERNAL


/* used in yyy.c */

#define MAX_STACK_SIZE                          1024

#endif  /* OPT_INTERNAL */
/*********************************************************************
 ***
 *** Anything that may be seen outside of opt_config.c should
 *** be declared in this section.
 ***
 *********************************************************************
 */

#define STR_NONE "none"

#define INPUT_PATH_LENGTH 512

/* #define USE_LARGE_CODEREP 1 comment out to flush out any problem */

/* Default file	extensions: */
#define	IRB_FILE_EXTENSION ".B"	/* Binary WHIRL IR file */
#define	IRA_FILE_EXTENSION ".ir"/* Ascii  WHIRL IR file */
#define	ERR_FILE_EXTENSION ".e"	/* Error file */
#define	LST_FILE_EXTENSION ".l"	/* Listing file */
#define	TRC_FILE_EXTENSION ".t"	/* Trace file */
#define	ASM_FILE_EXTENSION ".s"	/* Assembly code file */
#define	OBJ_FILE_EXTENSION ".o"	/* Relocatable object file */
#define DSTDUMP_FILE_EXTENSION ".be.dst" /* DST dump-file extension */

/* The maximum profile level -- should probably be defined in a global
 * header file, but isn't.
 */
enum {MAX_PROFILE_LEVEL = 0 };

/* constants used by optimizer now */
enum { CFG_BB_TAB_SIZE = 10 };
enum { CFG_LAB_HASH_SIZE = 10 };
enum { CFG_ALTENTRY_TAB_SIZE = 10 };
enum { CFG_EARLYEXIT_TAB_SIZE = 10 };
enum { VAR_PHI_HASH_SIZE = 256 };
enum { CODE_HTABLE_SIZE = 9113 };	/* should be prime */
enum { CODE_ITABLE_SIZE = 1619 };	/* should be prime */
enum { EMITTER_COLOR_TAB_SIZE = 10 };
enum { RVI_CTAB_SIZE = 521 };		/* should be prime */
enum { LFTR_HASH_SIZE = 50 };		

#endif  /* opt_config_INCLUDED */

