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
 *  Module: note.c
 *  $Revision$
 *  $Date$
 *  $Author$
 *  $Source$
 *
 *  Revision comments:
 *
 *  8-Mar-1994 - Initial version
 *
 *  Description:
 *  ============
 *
 *  Compiler to user note implementation.
 *
 * =======================================================================
 * =======================================================================
 */


#include "defs.h"
#include "config.h"
#include "tracing.h"
#include "erglob.h"

#include "tn.h"
#include "bb.h"
#include "op.h"

#include "note.h"

/* =======================================================================
 *
 *  NOTE_Add_To_BB
 *
 *  See interface description.
 *
 * =======================================================================
 */
void
NOTE_Add_To_BB(
  BB *bb,
  NOTE_HANDLER handler,
  NOTE_INFO   *info
)
{
  NOTE *comment = TYPE_MEM_POOL_ALLOC(NOTE,&MEM_pu_pool);
  NOTE_handler(comment) = handler;
  NOTE_info(comment) = info;
  BB_Add_Annotation (bb, ANNOT_NOTE, comment);
}      


/* =======================================================================
 *
 *  NOTE_BB_Act
 *
 *  See interface description.
 *
 * =======================================================================
 */
void
NOTE_BB_Act(
    BB          *bb,
    NOTE_ACTION  action,
    FILE        *file
)
{
  ANNOTATION *ant;

  for (ant = ANNOT_First (BB_annotations(bb), ANNOT_NOTE);
       ant != NULL;
       ant = ANNOT_Next (ant, ANNOT_NOTE))
  {
    NOTE_ACT(ANNOT_note(ant), action, file);
  }
}


/* =======================================================================
 *
 *  NOTE_Retrieve_Note_For_Handler
 *
 *  See interface description.
 *
 * =======================================================================
 */
NOTE_INFO *
NOTE_Retrieve_Note_For_Handler(
    BB          *bb,
    NOTE_HANDLER  handler
)
{
  ANNOTATION *ant;

  for (ant = ANNOT_First (BB_annotations(bb), ANNOT_NOTE);
       ant != NULL;
       ant = ANNOT_Next (ant, ANNOT_NOTE))
  {
    if (NOTE_handler(ANNOT_note(ant)) == handler)
      return NOTE_info(ANNOT_note(ant));
  }
  return NULL;
}
