/*
  Copyright (c) 2006, 2007. QLogic Corporation.  All rights reserved.

   Path64 is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by
   the Free Software Foundation version 3

   Path64 is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
   or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
   License for more details.

   You should have received a copy of the GNU General Public License
   along with Path64; see the file COPYING.  If not, write to the Free
   Software Foundation, 51 Franklin Street, Fifth Floor, Boston, MA
   02110-1301, USA.
*/

/* Copyright (c) 2005, 2006. PathScale, Inc.  All Rights Reserved. */

#ifndef __idstack_h
#define __idstack_h

#include <assert.h>
#include "types.h"
#include "lock.h"
#include "utils.h"

static inline void __pmp_idstack_init (pmp_idstack_t *idstack)
{
  int i;
  idstack->num_ids = PMP_MAX_IDS;
  for (i = 0; i < PMP_MAX_IDS; i++) {
    idstack->stack[PMP_MAX_IDS - i - 1] = i;
  }
}

static inline int __pmp_idstack_pop (pmp_idstack_t *idstack)
{
  int num_ids = idstack->num_ids;
  assert(num_ids > 0);
  idstack->num_ids = num_ids - 1;
  return idstack->stack[num_ids - 1];
}

static inline void __pmp_idstack_push (pmp_idstack_t *idstack, int id)
{
  int num_ids = idstack->num_ids;
  assert(num_ids < PMP_MAX_IDS);
  idstack->stack[num_ids] = id;
  idstack->num_ids = num_ids + 1;
}

#endif
