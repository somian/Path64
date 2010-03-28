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
 * Module: config_platform.h
 * $Revision$
 * $Date$
 * $Author$
 * $Source$
 *
 * Revision history:
 *  11-May-96 - Original Version
 *
 * Description:
 *
 * Definitions of the various platforms supported by the compiler.
 * This package is used to configure certain platform-specific options
 * like the -Ofast option set and cache configuration for LNO.
 *
 * This is separate from config_targ.h because it has a very small set
 * of clients, whereas config_targ.h is included in config.h and hence
 * everywhere.
 *
 * NOTE:  There is an outstanding bug, PV 378171, to base this
 * functionality on an external configuration file.
 *
 * WARNING:  This header should be usable by the driver, so it should
 * be clean of special compiler types.
 *
 * ====================================================================
 * ====================================================================
 */

#ifndef config_platform_INCLUDED
#define config_platform_INCLUDED

#ifdef _KEEP_RCS_ID
static char *config_platform_rcs_id = "$Source$ $Revision$";
#endif /* _KEEP_RCS_ID */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* What are the supported platforms? */
typedef enum {
  IP0,          /* Unknown processor */
  IP_END	/* End of platform list */
} PLATFORM;

/* What are the supported platforms (for driver): */
typedef enum {
  PROC_NONE,
  PROC_OPTERON
} PROCESSOR;

extern PLATFORM Platform;	/* Target platform */

/* How should we set the options? */
typedef struct {
  PLATFORM id;		/* Which IP? */
  char *name;		/* "ipxx" */
  char *pname;		/* "r10000" */
  PROCESSOR processor;	/* PROC_RxK */
  /* Add fields here for values of options which vary per processor */
  char *nickname;	/* "Shiva" */
} PLATFORM_OPTIONS;

#define POPTS_id(p)		((p)->id)
#define POPTS_name(p)		((p)->name)
#define POPTS_pname(p)		((p)->pname)
#define POPTS_processor(p)	((p)->processor)
#define POPTS_nickname(p)	((p)->nickname)

extern PLATFORM_OPTIONS * Get_Platform_Options ( char *name );

#ifdef __cplusplus
}
#endif /* __cplusplus */
    
#endif /* config_platform_INCLUDED */
