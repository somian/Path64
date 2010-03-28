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


#ifndef err_host_INCLUDED
#define err_host_INCLUDED
#ifdef __cplusplus
extern "C" {
#endif

/* ====================================================================
 * ====================================================================
 *
 * Module: err_host.h (Microcode compiler version)
 * $Revision$
 * $Date$
 * $Author$
 * $Source$
 *
 * Revision history:
 *  07-Sep-89 - Original Version
 *  01-Feb-91 - Copied for TP/Muse
 *
 * Description:
 *
 * Interface definition for the host program-specific portions of the
 * error handling package in util/errors.c.  Contains:
 *
 *  1)	Literal #defines for host program-specific phase IDs.
 *
 *  2)	Literal #defines for host program-specific parameter kinds.
 *
 *  3)	Prototypes for host program-specific error reporting functions.
 *
 *  4)	Definition of the Signal_Cleanup routine.
 *
 * This module should only be included by the host program-specific
 * erxxx.h files, errors.c, and err_host.c.  See also the
 * initialization of the error descriptor table in err_host.tab.
 *
 * ====================================================================
 * ====================================================================
 */


#ifdef _KEEP_RCS_ID
static char *err_host_rcs_id = "$Source$ $Revision$";
#endif /* _KEEP_RCS_ID */

/* Define any host program-specific phase literals not defined in
 * errors.h (EP_phase):
 */
/*	none for microcode compiler	*/
#ifndef MONGOOSE_BE
#define EP_DRIVER	(EP_PREDEFINED+1)	/* Compiler driver */
#define EP_IR_BUILDER	(EP_PREDEFINED+2)	/* SGIR builder */
#define EP_IR_READER	(EP_PREDEFINED+3)	/* SGIR reader */
#define EP_GLOB_OPT	(EP_PREDEFINED+4)	/* Global optimizer */
#define EP_GRA		(EP_PREDEFINED+5)	/* Global allocator */
#define EP_TARGET	(EP_PREDEFINED+6)	/* Target-specific */
#define EP_LAST		(EP_PREDEFINED+6)
#endif

/* Define any host program-specific error parameter kinds not defined
 * in errors.h (ET_kind):
 */
#define ET_LAST		ET_PREDEFINED+1

/* Format host program-specific error message parameters.  The caller
 * is expected to copy the formatted string out of the result before
 * calling this routine again.
 */
extern const char *Host_Format_Parm (
  INT kind,	/* One of the parameter types in errors.h or above */
  MEM_PTR parm	/* A message parameter passed by the reporter */
);

/* Do any necessary host program-specific cleanup before aborting due
 * to a signal:
 */
extern void Signal_Cleanup (
  INT sig	/* The signal ID */
);

#ifdef __cplusplus
}
#endif
#endif /* err_host_INCLUDED */
