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
 * Module: cwh_preg.h
 * $Revision$
 * $Date$
 * $Author$
 * $Source$
 *
 * Revision history:
 *  dd-mmm-95 - Original Version
 *
 * Description: Exports preg mangement functions.
 *
 * ====================================================================
 * ====================================================================
 */

#ifndef CIR_WH_PREG_INCLUDED
#define CIR_WH_PREG_INCLUDED

#ifdef _KEEP_RCS_ID
static char *rcs_id = "$Source$ $Revision$";
#endif /* _KEEP_RCS_ID */


/* PREG details, returned when allocated */

typedef struct {
  TY_IDX  preg_ty;
  ST * preg_st;
  PREG_NUM preg;
} PREG_det ;

extern void fe_preg_init (void) ;
extern PREG_det cwh_preg_next_preg (TYPE_ID    btype,
				    const char     * name,
				    WN       * home_wn ) ;

extern WN * cwh_preg_temp_save(const char * name, WN  * expr ) ;

#endif /* CIR_WH_PREG_INCLUDED */

