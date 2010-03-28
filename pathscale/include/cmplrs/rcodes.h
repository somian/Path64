/*
 * Copyright (C) 2007 Pathscale, LLC.  All Rights Reserved.
 */

/*
 * Copyright (C) 2006. QLogic Corporation. All Rights Reserved.
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



/*
 * Return Codes used by the compiler phases,
 * used to communicate between the driver and its children.
 */

#ifndef __RCODES_H
#define __RCODES_H

#ifdef INCLUDE_ID
static char *rcodes_h_id="$Header$";
#endif

/* 
 * Ucode return codes
 */
#define EXEC_UMERGE	255	/* call umerge */

/*
 * Mongoose return codes
 */

#define RC_OKAY			0	/* executed successfully */
#define RC_INTERNAL_ERROR	1	/* a compiler error */
#define RC_USER_ERROR		2	/* a user error */
#define RC_NORECOVER_USER_ERROR	4	/* a user error can't recover from */
#define RC_UNIMPLEMENTED_ERROR	8	/* uses an unimplemented feature */
#define RC_NEED_INLINER		16	/* flag that we need the inliner */
#define RC_SYSTEM_ERROR		32	/* a O/S or environment error */
#define RC_GCC_ERROR		33	/* gcc's error number */
#ifdef KEY
#define RC_GCC_INTERNAL_ERROR	34	/* gcc internal error */
#endif
#define RC_OVERFLOW_ERROR	64	/* an overflow error; try recompiling */
#ifdef KEY
#define RC_RTL_MISSING_ERROR	127	/* environment error causing a missing
                           		   runtime library */
#endif

#endif
