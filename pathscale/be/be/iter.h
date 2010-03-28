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
 * Module: iter.h
 * $Revision$
 * $Date$
 * $Author$
 * $Source$
 *
 * Revision history:
 *  03-MAR-96 - Original Version
 *
 * Description:
 *  header for back-end driver iterators
 *
 * ====================================================================
 * ====================================================================
 */
#ifndef iter_INCLUDED
#define iter_INCLUDED

#ifdef _KEEP_RCS_ID
static char *be_utilrcs_id = "$Source$ $Revision$";
#endif /* _KEEP_RCS_ID */

#ifdef __cplusplus
extern "C" {
#endif

/* PU loop iterator macros */
#define PU_INFO_ITER_stack_size	10
#define PU_INFO_ITER_current(x)		((x)->current_pu)
#define PU_INFO_ITER_sp(x)		((x)->sp)
#define PU_INFO_ITER_stack(x)		((x)->stack[(x)->sp])
#define PU_INFO_ITER_is_top_level(x)	((x)->current_pu == NULL || \
					 (x)->sp == 0)

/* PU loop iterator */
typedef struct pu_info_iter {
  PU_Info *current_pu;
  PU_Info *stack[PU_INFO_ITER_stack_size];
  INT32 sp;
} PU_INFO_ITER;

/* PU loop iterator prototypes */
void Pu_Init(PU_INFO_ITER *, PU_Info *);
BOOL Pu_While(PU_INFO_ITER *);
void Pu_Next(PU_INFO_ITER *);

#ifdef __cplusplus
}
#endif

#endif /* be_iter_INCLUDED */
