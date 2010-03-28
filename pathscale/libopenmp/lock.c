/*
 * Copyright (C) 2007. PathScale, LLC. All Rights Reserved.
 */
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

#include <stdlib.h>
#include "atomic.h"
#include "ompc.h"
#include "lock.h"

/* NOTE: allocated state for critical locks is never recovered. */

void __ompc_critical (int global_id, pmp_lock_t **lock)
{
  pmp_lock_t *l = *lock;

  __pmp_debug(PMP_DEBUG_CALLS, "__ompc_critical global_id=%d, lock=%p\n",
              global_id, lock);
  __pmp_sample(PMP_PROFILE_OMPC_CRITICAL);
  __pmp_memory_fence();

  if (l == NULL) {
    void *l_new;
    void *l_old;
    void *result;
#if defined(BUILD_OS_DARWIN)
    result = darwin_memalign(PMP_L2_ALIGNED, sizeof(pmp_lock_t),
      "cannot allocate critical lock\n");
#else /* defined(BUILD_OS_DARWIN) */
    if (posix_memalign(&result, PMP_L2_ALIGNED, sizeof(pmp_lock_t)) != 0) {
      __pmp_fatal("cannot allocate critical lock\n");
    }
#endif /* defined(BUILD_OS_DARWIN) */
    l = (pmp_lock_t *) result;
    l_new = (void **) result;
    __pmp_lock_init(l);
    l_old = __pmp_atomic_cmpxchgptr((volatile voidptr_t *) lock, NULL, l_new);
    if (l_old != NULL) {
      /* This thread lost the race, delete my lock and use the existing lock */
      free(l);
      l = (pmp_lock_t *) l_old;
    }
  }

  __pmp_lock(global_id, l);
}

void __ompc_end_critical (int global_id, pmp_lock_t **lock)
{
  pmp_lock_t *l = *lock;
  assert(l != NULL);
  __pmp_unlock(global_id, l);
  __pmp_memory_fence();
  __pmp_debug(PMP_DEBUG_CALLS, "__ompc_end_critical global_id=%d, lock=%p\n", 
              global_id, lock);
  __pmp_sample(PMP_PROFILE_OMPC_END_CRITICAL);
}
