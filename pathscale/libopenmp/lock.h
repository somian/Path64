/*
 * Copyright (C) 2007. PathScale, LLC.  All Rights Reserved.
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

#ifndef __pmp_lock_h
#define __pmp_lock_h

#include <pthread.h>
#include "atomic.h"
#include "team.h"
#include "thread.h"
#include "types.h"
#include "utils.h"

#define PMP_SYNC_UNLOCKED (-1)		/* not a valid global_id */
#define PMP_LOCK_SPIN 1

static inline void __pmp_lock_init (pmp_lock_t *lock)
{
  /* The mutex is initialized unconditionally to avoid the need to
   * access the params (problematic early in initialization). */
  lock->sync = PMP_SYNC_UNLOCKED;
  if (pthread_mutex_init(&lock->mutex, NULL) != 0) {
    __pmp_fatal("cannot initialize lock mutex\n");
  }
}

static inline void __pmp_lock (pmp_global_id_t global_id, pmp_lock_t *lock)
{
  /* DO NOT INSERT PMP_DEBUG STATEMENTS IN HERE (MAY CAUSE NESTED LOCKS) */
  if (__pmp_get_param()->lock_spin == 0) {
    pthread_mutex_lock(&lock->mutex);
  }
  else {
    int32_t sync;
    sync = __pmp_atomic_cmpxchg32(&lock->sync, PMP_SYNC_UNLOCKED, global_id);
    assert(sync != global_id);
  
    while (sync != PMP_SYNC_UNLOCKED) {
      /* The inner loop attempts to reduce the cost of polling on the lock
       * by using a plain read. When the read succeeds, another atomic op
       * is used to attempt to acquire the lock (which of course might miss).
       * A downside to this approach is an increased risk of starvation
       * for an unlucky thread. The inner loop currently loops until the
       * sync location is unlocked. An alternate implementation might bound
       * the number of iterations to choose a ratio between reads and atomic
       * ops. It is not clear to me as to which approach is best. */
      do {
        /* USER LEVEL SPIN LOOP */
        __pmp_yield();
        sync = lock->sync;
        assert(sync != global_id);
      } while (sync != PMP_SYNC_UNLOCKED);
      sync = __pmp_atomic_cmpxchg32(&lock->sync,PMP_SYNC_UNLOCKED,global_id);
      assert(sync != global_id);
    } 
  }
}

static inline int __pmp_trylock (pmp_global_id_t global_id, pmp_lock_t *lock)
{
  /* DO NOT INSERT PMP_DEBUG STATEMENTS IN HERE (MAY CAUSE NESTED LOCKS) */
  if (__pmp_get_param()->lock_spin == 0) {
    return (pthread_mutex_trylock(&lock->mutex) == 0);
  }
  else {
    /* NOTE: this is a non-nested lock. If the lock is already held by
     * this thread, __pmp_trylock returns false indicating that the lock
     * cannot be relocked. */
    int32_t sync;
    sync = __pmp_atomic_cmpxchg32(&lock->sync, PMP_SYNC_UNLOCKED, global_id);
    return (sync == PMP_SYNC_UNLOCKED);
  }
}

static inline void __pmp_unlock (pmp_global_id_t global_id, pmp_lock_t *lock)
{
  /* DO NOT INSERT PMP_DEBUG STATEMENTS IN HERE (MAY CAUSE NESTED LOCKS) */
  if (__pmp_get_param()->lock_spin == 0) {
    pthread_mutex_unlock(&lock->mutex);
  }
  else {
    int32_t sync;
    sync = __pmp_atomic_cmpxchg32(&lock->sync, global_id, PMP_SYNC_UNLOCKED);
    assert(sync == global_id);
  }
}

static inline void __pmp_lock_destroy (pmp_lock_t *lock)
{
  /* NOTE: allow locks to be destoyed even if they are currently locked. */
  if (__pmp_get_param()->lock_spin == 0) {
    pthread_mutex_destroy(&lock->mutex);
  }
}

static inline void __pmp_nest_lock_init (pmp_nest_lock_t *lock)
{
  /* The mutex is initialized unconditionally to avoid the need to
   * access the params (problematic early in initialization). */
  pthread_mutexattr_t attr;
  lock->sync = PMP_SYNC_UNLOCKED;
  lock->nest = 0;
  if (pthread_mutexattr_init(&attr) != 0) {
    __pmp_fatal("cannot initialize pthread mutex attribute for nested lock\n");
  }
#if defined(BUILD_OS_DARWIN)
  /* Google search says the "_NP" stands for "nonportable", some GNU
   * Linux implementations have only that form, but others define
   * the symbols with and without "_NP" to be the same */
  if (pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE) != 0)
#else /* defined(BUILD_OS_DARWIN) */
  if (pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE_NP) != 0)
#endif /* defined(BUILD_OS_DARWIN) */
  {
    __pmp_fatal("cannot set pthread mutex attribute for nested lock\n");
  }
  if (pthread_mutex_init(&lock->mutex, &attr) != 0) {
    __pmp_fatal("cannot initialize nested lock mutex\n");
  }
  if (pthread_mutexattr_destroy(&attr) != 0) {
    __pmp_fatal("cannot initialize pthread mutex attribute for nested lock\n");
  }
}

static inline void __pmp_nest_lock (pmp_global_id_t global_id,
                                    pmp_nest_lock_t *lock)
{
  if (__pmp_get_param()->lock_spin == 0) {
    pthread_mutex_lock(&lock->mutex);
    lock->nest++;
  }
  else {
    int32_t sync;
    sync = __pmp_atomic_cmpxchg32(&lock->sync, PMP_SYNC_UNLOCKED, global_id);
  
    if (sync != global_id) {
      while (sync != PMP_SYNC_UNLOCKED) {
        /* The inner loop attempts to reduce the cost of polling on the lock
         * by using a plain read. When the read succeeds, another atomic op
         * is used to attempt to acquire the lock (which of course might miss).
         * A downside to this approach is an increased risk of starvation
         * for an unlucky thread. The inner loop currently loops until the
         * sync location is unlocked. An alternate implementation might bound
         * the number of iterations to choose a ratio between reads and atomic
         * ops. It is not clear to me as to which approach is best. */
        do {
          /* USER LEVEL SPIN LOOP */
          __pmp_yield();
          sync = lock->sync;
          assert(sync != global_id);
        } while (sync != PMP_SYNC_UNLOCKED);
        sync = __pmp_atomic_cmpxchg32(&lock->sync,PMP_SYNC_UNLOCKED,global_id);
        assert(sync != global_id);
      } 
    }
    assert(lock->nest == 0 || sync == global_id);
    lock->nest++;
  }
}

static inline int __pmp_nest_trylock (pmp_global_id_t global_id,
                                      pmp_nest_lock_t *lock)
{
  if (__pmp_get_param()->lock_spin == 0) {
    if (pthread_mutex_trylock(&lock->mutex) == 0) {
      lock->nest++;
      return lock->nest;
    }
    else {
      return 0;
    }
  }
  else {
    int32_t sync;
    sync = __pmp_atomic_cmpxchg32(&lock->sync, PMP_SYNC_UNLOCKED, global_id);
    if (sync == PMP_SYNC_UNLOCKED || sync == global_id) {
      assert(sync == global_id || lock->nest == 0);
      lock->nest++;
      return lock->nest;
    }
    else {
      return 0;
    }
  }
}

static inline void __pmp_nest_unlock (pmp_global_id_t global_id,
                                      pmp_nest_lock_t *lock)
{
  if (__pmp_get_param()->lock_spin == 0) {
    assert(lock->nest > 0);
    lock->nest--;
    if (lock->nest == 0) {
      pthread_mutex_unlock(&lock->mutex);
    }
  }
  else {
    int32_t sync;
    assert(lock->nest > 0 && lock->sync == global_id);
    lock->nest--;
    if (lock->nest == 0) {
      sync = __pmp_atomic_cmpxchg32(&lock->sync, global_id, PMP_SYNC_UNLOCKED);
      assert(sync == global_id);
    }
  }
}

static inline void __pmp_nest_lock_destroy (pmp_nest_lock_t *lock)
{
  /* NOTE: allow locks to be destroyed even if they are currently locked. */
  if (__pmp_get_param()->lock_spin == 0) {
    pthread_mutex_destroy(&lock->mutex);
  }
}

#endif
