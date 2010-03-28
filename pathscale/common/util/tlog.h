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
 * Module: tlog.h
 * $Revision$
 * $Date$
 * $Author$
 * $Source$
 *
 * Revision history:
 *  14-Sep-95 - Original Version
 *
 * Description:  function prototypes for tlog.c which generates
 *               transformation log (t-log)
 *
 * ====================================================================
 * ====================================================================
 */

#ifndef tlog_INCLUDED
#define tlog_INCLUDED
#ifdef __cplusplus
extern "C" {
#endif

#ifdef _KEEP_RCS_ID
static char *tlog_rcs_id = "$Source$ $Revision$";
#endif /* _KEEP_RCS_ID */

#include <sys/types.h>

/* 
   interface for generating one tlog record phase_name, trans_name, and
   keyword has to be one word input_string, output_string, and
   aux_info_string cannot have " { " or " } " but can have white-space,
   etc.
*/

extern void Generate_Tlog(
  const char*   phase_name,	/* e.g. "IPA", "LNO" */
  const char*   trans_name,	/* e.g. "scalar_renaming" */
  SRCPOS        srcpos, 	/* source position where this */
				/* transformation occurs */
  const char*   keyword,	/* one word to distinguish this */
				/* transformation. can be null pointer */
  const char*   input_string,
  const char*   output_string,
  const char*   aux_info_string
);
#ifdef __cplusplus
}
#endif

#endif /* tlog_INCLUDED */


