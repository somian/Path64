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
 *  Module: annotations.c
 *  $Revision$
 *  $Date$
 *  $Author$
 *  $Source$
 *
 *  Description:
 *  ============
 *
 *  Module to create and access annotations that can be attached to 
 *  any arbitrary data structure.
 *
 *  See annotations.h for description.
 *
 * =======================================================================
 * =======================================================================
 */

#include "defs.h"
#include "errors.h"
#include "mempool.h"
#include "cgir.h"
#include "annotations.h"


extern ANNOTATION *
ANNOT_Add (
  ANNOTATION *annot_list, 
  ANNOTATION_KIND kind, 
  void *information,
  MEM_POOL *pool)
{
  ANNOTATION *new_a = TYPE_MEM_POOL_ALLOC (ANNOTATION, pool);
  ANNOTATION *list, *next;

  ANNOT_next(new_a) = NULL;
  ANNOT_info(new_a) = information;
  ANNOT_kind(new_a) = kind;

  /* The new annotation is added at the end of the annotations list.
   * This maintains the order in which annotations are added. Some 
   * clients care about this.
   */
  for (list = annot_list; list != NULL; list = next) {
    next = ANNOT_next(list);
    if (next == NULL) {
      ANNOT_next(list) = new_a;
      break;
    }
  }
  if (annot_list == NULL) annot_list = new_a;
  return annot_list;

}

extern ANNOTATION *
ANNOT_Unlink (
  ANNOTATION *annot_list,
  ANNOTATION *this1)
{
  ANNOTATION *list, *next;

  if ( annot_list == this1 ) {
    return ANNOT_next(annot_list);
  }

  for (list = annot_list; list != NULL; list = next) {
    next = ANNOT_next(list);
    if ( next == this1 ) {
      ANNOT_next(list) = ANNOT_next(next);
      break;
    }
  }

  return annot_list;
}

extern ANNOTATION *
ANNOT_Get (ANNOTATION *list, ANNOTATION_KIND kind)
{
  ANNOTATION_KIND cur_kind;

  while (list != NULL) {
    cur_kind = ANNOT_kind(list);
    if (cur_kind == kind) {
      break;
    }
    list = ANNOT_next(list);
  }
  return list;
}

