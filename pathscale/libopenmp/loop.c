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

#include <assert.h>
#include <stdlib.h>
#include "atomic.h"
#include "loop.h"
#include "team.h"
#include "thread.h"
#include "utils.h"

/* The sole purpose of the mask PMP_MASK_LOOP_ID is to ensure that 
 * loop id values are non-negative and hence distinct from PMP_LOOP_UNUSED. */

#define PMP_LOOP_UNUSED (-1)		/* not a valid loop id */
#define PMP_MAX_CHECK_WARNINGS 1000

#ifdef PMP_CHECK
static int __pmp_check_warnings = 0;

void __pmp_start_check (pmp_loop_t *loop)
{
  int64_t size;
  if (!__pmp_get_param()->check) {
    return;
  }
  assert(loop->check_loop == NULL);
  size = (loop->upper - loop->lower) / loop->inc + 1;
  if ((loop->check_loop = (int32_t *) calloc(size, sizeof(int32_t))) == NULL) {
    __pmp_fatal("unable to allocate check array of %" SCNd64 " entries\n", 
                (long long) size);
  }
  loop->check_last = 0;
}

void __pmp_loop_check (pmp_loop_t *loop, int64_t value)
{
  int64_t index;
  if (!__pmp_get_param()->check) {
    return;
  }
  assert(loop->check_loop != NULL);
  assert((loop->inc >= 0) ? 
         (value >= loop->lower && value <= loop->upper) :
         (value >= loop->upper && value <= loop->lower));
  assert((value - loop->lower) % loop->inc == 0);
  index = (value - loop->lower) / loop->inc;
  __pmp_atomic_add32(&loop->check_loop[index], 1);
}

void __pmp_last_check (pmp_loop_t *loop)
{
  if (!__pmp_get_param()->check) {
    return;
  }
  __pmp_atomic_add32(&loop->check_last, 1);
}

static void __pmp_warn_check (pmp_loop_t *loop,
                              int64_t first_index, int64_t last_index,
                              int64_t first_value, int64_t last_value,
                              int64_t size, int64_t team_size, int check)
{
  if (__pmp_check_warnings < PMP_MAX_CHECK_WARNINGS) {
    if (first_index == last_index) {
      __pmp_warning("__pmp_stop_check: iteration %" SCNd64 " of %" SCNd64 " "
                    "(loop value %" SCNd64 ") has count of %d\n", 
                    (long long) first_index, (long long) size, 
                    (long long) first_value, check);
    }
    else {
      __pmp_warning("__pmp_stop_check: iterations %" SCNd64 " to %" SCNd64 " of %" SCNd64 " "
                    "(loop values %" SCNd64 " to %" SCNd64 ") have count of %d\n", 
                    (long long) first_index, (long long) last_index,
                    (long long) size, 
                    (long long) first_value, (long long) last_value,
                    check);
    }
    __pmp_warning("__pmp_stop_check: loop has team_size=%d, sched=%d, "
                  " lower=%" SCNd64 ", upper=%" SCNd64 ", inc=%" SCNd64 " chunk=%" SCNd64 "\n",
                  team_size, loop->sched, 
                  (long long) loop->lower, (long long) loop->upper, 
                  (long long) loop->inc, (long long) loop->chunk);
  }
  else if (__pmp_check_warnings == PMP_MAX_CHECK_WARNINGS) {
    __pmp_warning("__pmp_stop_check: too many warnings, "
                  "ignoring any more\n");
  }
  __pmp_check_warnings++;
}

void __pmp_stop_check (pmp_loop_t *loop, pmp_thread_t *thread)
{
  int team_size = __pmp_get_team_size(thread->team);
  int64_t size;
  int64_t value;
  int64_t i;
  bool warn = false;
  int64_t warn_first = 0;
  int warn_check = 0;
  if (!__pmp_get_param()->check) {
    return;
  }
  assert(loop->check_loop != NULL);
  size = (loop->upper - loop->lower) / loop->inc + 1;
  value = loop->lower;
  for (i = 0; i < size; i++) {
    if (loop->check_loop[i] != 1) {
      if (!warn) {
        warn = true;
        warn_first = i;
        warn_check = loop->check_loop[i];
      }
      else if (warn_check != loop->check_loop[i]) {
        __pmp_warn_check(loop, warn_first, i - 1,
                         loop->lower + (warn_first * loop->inc),
                         loop->lower + ((i - 1) * loop->inc),
                         size, team_size, warn_check);
        warn_first = i;
        warn_check = loop->check_loop[i];
      }
    }
    else if (warn) {
      __pmp_warn_check(loop, warn_first, i - 1,
                       loop->lower + (warn_first * loop->inc),
                       loop->lower + ((i - 1) * loop->inc),
                       size, team_size, warn_check);
      warn = false;
    }
    value += loop->inc;
  }
  if (warn) {
    __pmp_warn_check(loop, warn_first, i - 1,
                     loop->lower + (warn_first * loop->inc),
                     loop->lower + ((i - 1) * loop->inc),
                     size, team_size, warn_check);
  }
  if (loop->check_last != 1) {
    if (__pmp_check_warnings < PMP_MAX_CHECK_WARNINGS) {
      __pmp_warning("__pmp_last_check: last iteration has count of %d\n",
                    loop->check_last);
    }
    else if (__pmp_check_warnings == PMP_MAX_CHECK_WARNINGS) {
      __pmp_warning("__pmp_stop_check: too many warnings, "
                    "ignoring any more\n");
    }
    __pmp_check_warnings++;
  }
  free(loop->check_loop);
  loop->check_loop = NULL;
}
#endif

void __pmp_loop_init (pmp_loop_t *loop)
{
  loop->loop_id = PMP_LOOP_UNUSED;		/* not a valid loop id */
  loop->check_last = 0;
  loop->check_loop = NULL;
}

void __pmp_loop_alloc (pmp_thread_t *thread, int sched,
                       int64_t lower, int64_t upper, 
                       int64_t inc, int64_t chunk, int64_t min_chunk)
{
  pmp_team_t *team = thread->team;
  int32_t thread_count = thread->loop_count;
  pmp_loop_id_t loop_id = thread_count & PMP_MASK_LOOP_ID;
  int loop_index = thread_count & PMP_MASK_LOOP_INDEX;
  int32_t team_count;
  int team_size;
  pmp_loop_t *loop;

  assert(thread->loop == NULL);

  if (team == NULL) {
    /* This thread is not in a team: schedule as if in a team of size one
     * and reuse the first loop structure for the (unused) team associated
     * with this thread. */
    team_count = thread_count;
    team_size = 1;
    loop = __pmp_manager.teams[thread->global_id].loops;
  }
  else {
    team_count = __pmp_atomic_cmpxchg32(&team->loop_count,
                                        thread_count, thread_count + 1);
    team_size = team->team_size;
    loop = team->loops + loop_index;
  }

  thread->loop_count = thread_count + 1;
  if (thread_count == team_count) {
    /* This is the first thread to hit this loop */
    pmp_loop_id_t old_loop_id;

    assert(loop->loop_id != loop_id);

    while (loop->loop_id != PMP_LOOP_UNUSED) {
      /* USER LEVEL SPIN LOOP */
      __pmp_yield();
    }

    loop->looping_threads = team_size;
    loop->now_serving = lower;
    loop->sched = sched;
    loop->lower = lower;
    loop->upper = upper;
    loop->inc = inc;
    loop->chunk = chunk;
    loop->current = lower;
    loop->min_chunk = min_chunk;

    __pmp_start_check(loop);

    /* Must be last assignment, note that atomic orders memory operations */
    old_loop_id = __pmp_atomic_cmpxchg32(&loop->loop_id, PMP_LOOP_UNUSED, 
                                         loop_id);
    assert(old_loop_id == PMP_LOOP_UNUSED);

    __pmp_debug(PMP_DEBUG_LOOPS, "__pmp_loop_alloc: thread global_id %d "
                "allocated loop_id %d\n", thread->global_id, loop_id);
  }
  else {
    /* This is not the first thread to hit this loop */

    while (loop->loop_id != loop_id) {
      /* USER LEVEL SPIN LOOP */
      __pmp_yield();
    }

    __pmp_debug(PMP_DEBUG_LOOPS, "__pmp_loop_alloc: thread global_id %d "
                "acquired loop_id %d\n", thread->global_id, loop_id);
  }
  thread->loop = loop;
}


void __pmp_loop_free (pmp_thread_t *thread)
{
  pmp_loop_t *loop = thread->loop;
  int32_t count;
  int32_t loop_id;
  loop_id = (thread->loop_count - 1) & PMP_MASK_LOOP_ID;
  assert(loop->loop_id == loop_id);
  count = __pmp_atomic_xadd32(&loop->looping_threads, -1);
  assert(count >= 1);
  if (count == 1) {
    /* This is the last thread to free the loop */
    __pmp_stop_check(loop, thread);
    loop->loop_id = PMP_LOOP_UNUSED;
    __pmp_debug(PMP_DEBUG_LOOPS, "__pmp_loop_free: thread global_id %d "
                "deallocated loop_id %d\n", thread->global_id, loop_id);
  }
  else {
    __pmp_debug(PMP_DEBUG_LOOPS, "__pmp_loop_free: thread global_id %d "
                "released loop_id %d\n", thread->global_id, loop_id);
  }
  thread->loop = NULL;
}
