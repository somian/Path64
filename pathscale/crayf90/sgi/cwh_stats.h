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
 * Module: cwh_stats.h
 * $Revision$
 * $Date$
 * $Author$
 * $Source$
 *
 * Revision history:
 *  dd-mmm-95 - Original Version
 *
 * Description: prototypes for entry points into cwh_stats.c - the 
 *              counter functions & printing.
 *
 * ====================================================================
 * ====================================================================
 */

#ifndef CWH_STATS_INCLUDED
#define CWH_STATS_INCLUDED

#ifdef _KEEP_RCS_ID
static char *rcs_id = "$Source$ $Revision$";
#endif /* _KEEP_RCS_ID */

/* 
 * counter defines for types - measure number of times New_TY is called
 * mostly, though other actions are counted too.
 *
 * Note c_POINTER and c_F90_NOPTR require Make_Pointer_Type 
 * in stab_util.c to be modified... ie: add
 *
 *  BUMP_TY_COUNTER(c_POINTER);
 *  at New_TY()
 *
 * and
 *
 *   if (!TY_is_f90_pointer(t)) { 
 *     ....
 *   } else 
 *       BUMP_TY_COUNTER(c_F90_NOPTR);
*/


enum type_counter { 
  c_TY_ARRAY = 0,
  c_TY_POINTER = 1,
  c_TY_f90_POINTER = 2,
  c_TY_PROC = 3 ,
  c_TY_MISC = 4,
  c_TY_STRUCT = 5,
  c_TY_COPY = 6,
  c_TY_UNIQ_POINTER = 7,
  c_TY_DTYPE =  8,
  c_TY_MATCH_ARRAY =  9,
  c_TY_MATCH_DOPE =  10,
  c_TY_MATCH =  11,
  c_TY_CLEARED = 12,
  c_TY_REUSED = 13,
  c_TY_F90_NOPTR = 14,
  c_TY_LAST  = 15 
};

extern int cwh_stat_ty_c[c_TY_LAST];


#ifdef _DEBUG 
#define BUMP_TY_COUNTER(i) cwh_stat_ty_c[i] ++ ; 
#else
#define BUMP_TY_COUNTER(i) 
#endif



#endif /* CWH_STATS_INCLUDED */
