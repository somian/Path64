/*
  Copyright (c) 2006, 2007. QLogic Corporation.  All rights reserved.

  Unpublished -- rights reserved under the copyright laws of the United
  States. USE OF A COPYRIGHT NOTICE DOES NOT IMPLY PUBLICATION OR
  DISCLOSURE. THIS SOFTWARE CONTAINS CONFIDENTIAL INFORMATION AND TRADE
  SECRETS OF QLOGIC CORPORATION. USE, DISCLOSURE, OR REPRODUCTION IS
  PROHIBITED WITHOUT THE PRIOR EXPRESS WRITTEN PERMISSION OF QLOGIC,
  CORPORATION.

  U.S. Government Restricted Rights:
  The Software is a "commercial item," as that term is defined at 48
  C.F.R. 2.101 (OCT 1995), consisting of "commercial computer software"
  and "commercial computer software documentation," as such terms are used
  in 48 C.F.R. 12.212 (SEPT 1995).  Consistent with 48 C.F.R. 12.212 and
  48 C.F.R. 227-7202-1 through 227-7202-4 (JUNE 1995), all U.S. Government
  End Users acquire the Software with only those rights set forth in the
  accompanying license agreement. QLogic Corporation, 26650 Aliso Viejo 
  Parkway, Aliso Viejo, CA 92656.
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
