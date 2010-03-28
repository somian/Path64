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
 * Module: iter.c
 * $Revision$
 * $Date$
 * $Author$
 * $Source$
 *
 * Revision history:
 *  3-MAR-96 dahl - Original Version
 *
 * Description:
 *	PU iterator for BE driver.
 *
 * ====================================================================
 * ====================================================================
 */

#define rbi_CXX	"iter.c"
#ifdef _KEEP_RCS_ID
static char *rcs_id = iter_C"$Revision$";
#endif /* _KEEP_RCS_ID */

#if ! defined(BUILD_OS_DARWIN)
#include <elf.h>	/* for Elf64_Word */
#endif /* ! defined(BUILD_OS_DARWIN) */
#include "defs.h"
#include "stab.h"
#include "pu_info.h"	/* PU_Info */
#include "iter.h"	/* PU_INFO_ITER */
#include "wn.h"
#include "be_util.h"	/* Reset_Current_PU_Count, Advance_Current_PU_Count */
#include "errors.h"	/* FmtAssert */

/* ==================================================================== */
/* iterator functions for PU loop in back-end				*/
/* ==================================================================== */

static void Pu_Push(PU_INFO_ITER *iter, PU_Info *pu_tree)
{
  PU_INFO_ITER_stack(iter) = pu_tree;
  PU_INFO_ITER_sp(iter)++;
  FmtAssert(PU_INFO_ITER_sp(iter) <= PU_INFO_ITER_stack_size,
	    ("Pu_Push, stack overflow - too many nested procedures"));
}

static PU_Info *Pu_Pop(PU_INFO_ITER *iter)
{
  if (PU_INFO_ITER_sp(iter) == 0) {
    Is_True(0,("Pu_Pop, stack underflow, problem with nested procedure"));
    return NULL;
  }
  PU_INFO_ITER_sp(iter)--;
  return PU_INFO_ITER_stack(iter);
}

void Pu_Init(PU_INFO_ITER *iter, PU_Info *pu_tree)
{
  PU_INFO_ITER_sp(iter) = 0;
  PU_INFO_ITER_current(iter) = pu_tree;
  Reset_Current_PU_Count();
}

BOOL Pu_While(PU_INFO_ITER *iter)
{
  return PU_INFO_ITER_current(iter) != NULL;
}

/* check for child PUs added by MP lowering */
void Pu_Next(PU_INFO_ITER *iter)
{
  if (PU_Info_child(PU_INFO_ITER_current(iter))) {
    Pu_Push(iter, PU_INFO_ITER_current(iter));
    PU_INFO_ITER_current(iter) = PU_Info_child(PU_INFO_ITER_current(iter));
  } else {
    PU_INFO_ITER_current(iter) = PU_Info_next(PU_INFO_ITER_current(iter));
    if (PU_INFO_ITER_current(iter) == NULL && PU_INFO_ITER_sp(iter)) {
      while (PU_INFO_ITER_sp(iter)) {
	PU_INFO_ITER_current(iter) = Pu_Pop(iter);
	if (PU_Info_next(PU_INFO_ITER_current(iter)))
	  break;
      }
      PU_INFO_ITER_current(iter) = PU_Info_next(PU_INFO_ITER_current(iter));
    }
  }
  Advance_Current_PU_Count();
}
