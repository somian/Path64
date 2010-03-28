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


#ifndef targ_ctrl_INCLUDED
#define targ_ctrl_INCLUDED
#ifdef __cplusplus
extern "C" {
#endif

/* ====================================================================
 * ====================================================================
 *
 * Module: targ_ctrl.h
 * $Revision$
 * $Date$
 * $Author$
 * $Source$
 *
 * Description:
 *
 * This file contains target-specific control information.  It is
 * included in controls.c and should not be visible elsewhere.
 *
 * ====================================================================
 * ====================================================================
 */


#ifdef _KEEP_RCS_ID
static char *targ_ctrl_rcs_id = "$Source$ $Revision$";
#endif /* _KEEP_RCS_ID */

static STR_LIST Targ_1 = {"X8664", NULL};

#define Possible_Targets Targ_1
#define TARG_FIRST_DEF 0
#define TARG_SECOND_DEF 0

#ifdef __cplusplus
}
#endif
#endif /* targ_ctrl_INCLUDED */
