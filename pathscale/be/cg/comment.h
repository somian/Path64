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


/* =======================================================================
 * =======================================================================
 *
 *  Module: comment.h
 *  $Revision$
 *  $Date$
 *  $Author$
 *  $Source$
 *
 *  Revision comments:
 *
 *  21-Jan-1998 - Initial version
 *
 *  Description:
 *  ============
 *
 *  Add arbitrary comments to a basic block.
 *
 *  Exported functions:
 *  ===================
 *
 *      void COMMENT_Add(
 *        BB   *bb,
 *	  STR_IDX  text
 *      )
 *
 *          Add comment <text> to <bb>.
 *
 * =======================================================================
 * =======================================================================
 */

#ifndef COMMENT_INCLUDED

#ifdef _KEEP_RCS_ID
static const char swp_note_rcs_id[] = "$Source$ $Revision$";
#endif /* _KEEP_RCS_ID */

extern void COMMENT_Add(BB *bb, STR_IDX text);

#endif
