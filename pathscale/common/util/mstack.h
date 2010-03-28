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


#ifndef stack_INCLUDED
#define stack_INCLUDED
#ifdef __cplusplus
extern "C" {
#endif

/* ====================================================================
 * ====================================================================
 *
 * Module: stack.h
 * $Revision$
 * $Date$
 * $Author$
 * $Source$
 *
 * Revision history:
 *  14-Jun-93 - Original Version
 *
 * Description:  function prototypes for stack.c
 *
 * ====================================================================
 * ====================================================================
 */


#ifdef _KEEP_RCS_ID
static char *stack_rcs_id = "$Source$ $Revision$";
#endif /* _KEEP_RCS_ID */


#if mips

extern int trace_stack ( int prfunc, int prfile );

#else
#if A_UX

extern int stack_lev ( int b );
extern int trace_stack ( int a, int b );

#else

extern int trace_stack ( int a, int b );

#endif /* A_UX */
#endif /* mips */

#ifdef __cplusplus
}
#endif
#endif /* stack_INCLUDED */
