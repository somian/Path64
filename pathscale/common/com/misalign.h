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


#ifndef misalign_INCLUDED
#define misalign_INCLUDED
#ifdef __cplusplus
extern "C" {
#endif

/* ====================================================================
 * ====================================================================
 *
 * Module: misalign.h
 * $Revision$
 * $Date$
 * $Author$
 * $Source$
 *
 * Revision history:
 *  30-Apr-94 - Original Version
 *
 * Description:
 *
 * Declare an enumerated type and global option variables required by
 * the clients of both irbmem.h and memmodel.h, since we can't seem to
 * get away with an incomplete enumerated type.
 *
 * ====================================================================
 * ====================================================================
 */


#ifdef _KEEP_RCS_ID
static char *misalign_rcs_id = "$Source$ $Revision$";
#endif /* _KEEP_RCS_ID */

/* Misalignment models -- see memmodel.h for global defaults: */
typedef enum {
  MIS_NOSPEC,	/* No misalignment choice specification */
  MIS_NONE,	/* Assume aligned until proven otherwise */
  MIS_CAST,	/* Base alignment on type, trust casts */
  MIS_TYPED	/* Base alignment on type, ignore casts */
} MISALIGNMENT;

/* Misalignment models -- set up in memmodel.c:Configure_Alignment. */
extern MISALIGNMENT Misalignment_Mem;	/* Load/store misalignment */
extern MISALIGNMENT Misalignment_Parm;	/* Parameter misalignment */
extern MISALIGNMENT Misalignment_Agg;	/* Aggregate misalignment */

#ifdef __cplusplus
}
#endif
#endif /* misalign_INCLUDED */
