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


#ifndef config_host_INCLUDED
#define config_host_INCLUDED
#ifdef __cplusplus
extern "C" {
#endif

/* ====================================================================
 * ====================================================================
 *
 * Module: config_host.h
 * $Revision$
 * $Date$
 * $Author$
 * $Source$
 *
 * Revision history:
 *  15-Jun-91 - Original Version
 *
 * Description:
 *
 * This file defines general configuration parameters which are
 * specific to the compiler's host machine and system.
 *
 * See com/config.h for a detailed description of configuration
 * parameters, where they are declared, and how they are processed.
 *
 * See also:
 *	TARGET/com/defs.h -- Ubiquitous configuration parameters, e.g.
 *		basic type definitions appropriate to the host system.
 *	TARGET/com/targ_const.h -- Manipulation of target constants, as
 *		appropriate on the host system.
 *
 * ====================================================================
 * ====================================================================
 */


#ifdef _KEEP_RCS_ID
static char *config_host_rcs_id = "$Source$ $Revision$";
#endif /* _KEEP_RCS_ID */

/* What is the byte sex of the host?  Note that the variable
 * Host_Byte_Sex is set based on this definition in config_host.c.
 */
#if !(defined(linux) || defined(BUILD_OS_DARWIN))
#define HOST_IS_BIG_ENDIAN	1
#define HOST_IS_LITTLE_ENDIAN	0
#else
#define HOST_IS_BIG_ENDIAN	0
#define HOST_IS_LITTLE_ENDIAN	1
#endif

/* Does the host (compiler) support quad-precision floating point? */
#if !defined(HOST_SUPPORTS_QUAD_FLOAT)
#if defined(_COMPILER_VERSION) && (_COMPILER_VERSION >= 400) && _SGIAPI || (defined(__GNUC__) && defined(TARG_MIPS))
#define HOST_SUPPORTS_QUAD_FLOAT 1
#else
#define HOST_SUPPORTS_QUAD_FLOAT 0
#endif
#endif

/* Configuration prior to flag processing: */
extern void Preconfigure_Host (void);

/* Configuration after flag processing: */
extern void Configure_Host (void);

/* Reconfiguration for each source file: */
extern void Configure_Source_Host ( char *filename );

#ifdef __cplusplus
}
#endif
#endif /* config_host_INCLUDED */
