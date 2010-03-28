/*
 * Copyright (C) 2007. PathScale, LLC. All Rights Reserved.
 */
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
