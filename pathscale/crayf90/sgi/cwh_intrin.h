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
 * Module: cwh_intrin.h
 * $Revision$
 * $Date$
 * $Author$
 * $Source$
 *
 * Revision history:
 *  dd-mmm-95 - Original Version
 *
 * Description:
 *
 * ====================================================================
 * ====================================================================
 */

#ifndef CWH_INTRIN_INCLUDED
#define CWH_INTRIN_INCLUDED

#ifdef _KEEP_RCS_ID
static char *rcs_id = "$Source$ $Revision$";
#endif /* _KEEP_RCS_ID */


extern WN * cwh_intrin_call(INTRINSIC intr, INT16 numargs, WN ** k, WN **sz, BOOL *v ,TYPE_ID bt ) ;
extern WN * cwh_intrin_op(INTRINSIC intr, INT16 numargs, WN ** k, WN **sz, BOOL *v ,TYPE_ID bt ) ;

extern WN * cwh_intrin_wrap_ref_parm(WN *wa, TY_IDX  ty) ;
extern WN * cwh_intrin_wrap_char_parm(WN *wa, WN *sz ) ;
extern WN * cwh_intrin_wrap_value_parm(WN *wn) ;
extern ST * cwh_intrin_make_intrinsic_symbol(const char *name, TYPE_ID ty);

#endif /* CWH_INTRIN_INCLUDED */

