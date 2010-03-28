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
 *  Module: comment.cxx
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
 *  Support for arbitrary textual comments.
 *
 * =======================================================================
 * =======================================================================
 */

#include "defs.h"
#include "config.h"
#include "errors.h"
#include "config_asm.h"

#include "mempool.h"
#include "strtab.h"
#include "cgir.h"
#include "note.h"

#include "comment.h"

typedef struct note_info {
  STR_IDX text;
} NOTE_INFO;


#define NOTE_INFO_text(info)	((info)->text)


/* =======================================================================
 *
 *  COMMENT_Handler
 *
 *  Handler for comments.
 *
 * =======================================================================
 */
static void
COMMENT_Handler(
  NOTE_ACTION action,
  NOTE_INFO  *info,
  FILE       *file
)
{
  switch (action) {
  case NOTE_PRINT_TO_FILE:
    fprintf(file, "%s %s\n", ASM_CMNT_LINE, Index_To_Str(NOTE_INFO_text(info)));
    break;
  case NOTE_PRINT_HANDLER_NAME_TO_FILE:
    fprintf(file, "COMMENT_Handler");
    break;
  case NOTE_PRINT_TO_ANL_FILE:
    /* ignore for now */
    break;
  }
}


/* =======================================================================
 *
 *  COMMENT_Add
 *
 *  See interface description.
 *  Store string idx rather than string itself cause
 *  strtab can be realloc'ed.
 *
 * =======================================================================
 */
void
COMMENT_Add(
  BB *bb,
  STR_IDX text
)
{
  NOTE_INFO *info = TYPE_MEM_POOL_ALLOC(NOTE_INFO, &MEM_pu_nz_pool);
  NOTE_INFO_text(info) = text;
  NOTE_Add_To_BB(bb, COMMENT_Handler, info);
}
