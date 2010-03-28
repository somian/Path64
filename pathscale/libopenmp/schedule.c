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

#include "loop.h"
#include "ompc.h"
#include "schedule.h"
#include "team.h"
#include "thread.h"
#include "utils.h"
#include <assert.h>

/* MIN and MAX that take account of loop direction: */
#define LOOPMIN(i,a,b) ((i >= 0) ? MIN(a,b) : MAX(a,b))
#define LOOPMAX(i,a,b) ((i >= 0) ? MAX(a,b) : MIN(a,b))

static inline void __pmp_scheduler_sample (int sched)
{
#ifdef PMP_PROFILE
  switch (sched) {
    case PMP_SCHED_STATIC: {
      __pmp_sample(PMP_PROFILE_SCHED_STATIC);
      break;
    }
    case PMP_SCHED_STATIC_EVEN: {
      __pmp_sample(PMP_PROFILE_SCHED_STATIC_EVEN);
      break;
    }
    case PMP_SCHED_DYNAMIC: {
      __pmp_sample(PMP_PROFILE_SCHED_DYNAMIC);
      break;
    }
    case PMP_SCHED_GUIDED: {
      __pmp_sample(PMP_PROFILE_SCHED_GUIDED);
      break;
    }
    case PMP_SCHED_ORDERED_STATIC: {
      __pmp_sample(PMP_PROFILE_SCHED_ORDERED_STATIC);
      break;
    }
    case PMP_SCHED_ORDERED_STATIC_EVEN: {
      __pmp_sample(PMP_PROFILE_SCHED_ORDERED_STATIC_EVEN);
      break;
    }
    case PMP_SCHED_ORDERED_DYNAMIC: {
      __pmp_sample(PMP_PROFILE_SCHED_ORDERED_DYNAMIC);
      break;
    }
    case PMP_SCHED_ORDERED_GUIDED: {
      __pmp_sample(PMP_PROFILE_SCHED_ORDERED_GUIDED);
      break;
    }
    default: {
        __pmp_fatal("unknown dynamic scheduling type %d\n", sched);
      break;
    }
  }
#endif
}

static inline void __pmp_loop_analyser (pmp_thread_t *thread, int sched,
                                        pmp_global_id_t global_id,
                                        pmp_local_id_t local_id,
                                        int64_t loop_lower, int64_t loop_upper,
                                        int64_t my_lower, int64_t my_upper,
                                        int64_t inc, int64_t chunk, 
                                        int64_t stride)
{
  pmp_loop_t *loop;
  bool allocated;

  __pmp_debug(PMP_DEBUG_LOOPS, "__pmp_loop_analyser: sched=%d, global_id=%d, "
              "local_id=%d, loop_lower=%" SCNd64 ", loop_upper=%" SCNd64 ", "
              "my_lower=%" SCNd64 ", my_upper=%" SCNd64 ", inc=%" SCNd64 ", chunk=%" SCNd64 ", "
              "stride=%" SCNd64 "\n",
              sched, global_id, local_id,
              (long long) loop_lower, (long long) loop_upper,
              (long long) my_lower, (long long) my_upper,
              (long long) inc, (long long) chunk,
              (long long) stride);
  if (!__pmp_profile.enabled && !__pmp_get_param()->check) {
    return;
  }

  /* NOTE: set chunk=0 and stride=0 for a non-strided loop. They will 
   * then be auto-sized to use the inner loop for the required iterations
   * from my_lower to my_upper (inclusive). The outer loop will run
   * only once. */
  if (chunk == 0 && stride == 0) {
    chunk = (my_upper - my_lower) / inc + 1;
  }

  assert(inc != 0 && chunk != 0);
  assert((inc > 0 && stride >= 0) || (inc < 0 && stride <= 0));

  allocated = false;

#ifdef PMP_CHECK
  if (thread->loop == NULL) {
    /* For statically scheduled loops, allocate a loop to hold check data */
    __pmp_loop_alloc(thread, sched, loop_lower, loop_upper, inc, chunk, chunk);
    allocated = true;
  }
#endif

  /* NOTE: filter out cases where the loop contains no iterations */
  if ((inc >= 0) ? (my_lower <= my_upper) : (my_lower >= my_upper)) {

    assert((inc >= 0) ? (loop_lower <= my_lower && 
                         my_lower <= my_upper && 
                         my_upper <= loop_upper)
                      : (loop_upper <= my_upper &&
                         my_upper <= my_lower &&
                         my_lower <= loop_lower));

    loop = thread->loop;

#if (defined PMP_PROFILE) || (defined PMP_CHECK)
    if (inc >= 0) {
      int64_t count = 0;
      int64_t outer = my_lower;
      while (outer <= loop_upper) {
        int64_t inner = outer;
        int64_t i;
        for (i = 0; i < chunk && inner <= my_upper; i++) {
          __pmp_loop_check(loop, inner);
          count++;
          inner += inc;
        }
        if (inner > loop_upper) {
          __pmp_last_check(loop);
        }
        if (stride == 0) {
          break;
	}
        else {
          outer += stride;
          my_upper = MIN(my_upper + stride, loop_upper);
	}
      }
      __pmp_profile_iterations(global_id, count);
    }
    else {
      int64_t count = 0;
      int64_t outer = my_lower;
      while (outer >= loop_upper) {
        int64_t inner = outer;
        int64_t i;
        for (i = 0; i < chunk && inner >= my_upper; i++) {
          __pmp_loop_check(loop, inner);
          count++;
          inner += inc;
        }
        if (inner < loop_upper) {
          __pmp_last_check(loop);
        }
        if (stride == 0) {
          break;
	}
        else {
          outer += stride;
          my_upper = MAX(my_upper + stride, loop_upper);
	}
      }
      __pmp_profile_iterations(global_id, count);
    }
#endif
  }

#ifdef PMP_CHECK
  if (allocated) {
    /* For statically scheduled loops, deallocate the loop */
    __pmp_loop_free(thread);
  }
#endif
}

static inline void __pmp_static_init (int global_id, int sched,
                                      int64_t *lowerp, int64_t *upperp, 
                                      int64_t *stridep,
                                      int64_t inc, int64_t chunk)
{
  /* NOTE: chunk parameter is undefined/unused for static even scheduling */

  pmp_thread_t *thread = __pmp_get_thread(global_id);
  int team_size = __pmp_get_team_size(thread->team);
  int64_t loop_lower = *lowerp;
  int64_t loop_upper = *upperp;
  int64_t lower;
  int64_t upper;
  
  assert(team_size > 0);

  if (team_size == 1) {
    *stridep = (inc > 0) ? (loop_upper - loop_lower + 1) : 
                           (loop_upper - loop_lower - 1);
  }
  else {
    pmp_local_id_t local_id = thread->local_id;
    int64_t stride;
    switch (sched) {
      case PMP_SCHED_STATIC_EVEN: {
        int64_t size = (loop_upper - loop_lower) / inc + 1;
        assert(size >= 0);
        if (!__pmp_get_param()->static_fair) {
          /* The size is divided by the team_size and rounded up to give
           * the chunk size. Chunks of this size are assigned to threads
           * in increased local_id order. If the division was not exact
           * then the last thread will have fewer iterations, and possibly
           * none at all. */
          chunk = (size + team_size - 1) / team_size;
          lower = loop_lower + (local_id * chunk * inc);
	}
        else {
          /* The size is divided by the team_size and rounded down to 
           * give the chunk. Each thread will have at least this many
           * iterations. If the division was not exact then the remainder
           * iterations are scheduled across the threads in increasing
           * thread order. Note that the difference between the minimum
           * and maximum number of iterations assigned to the threads
           * across the team is at most 1. The maximum number of iterations
           * assigned to a thread (the worst case path through the schedule)
           * is the same as for default behavior. */
          int64_t remainder;
          int64_t index;
          chunk = size / team_size;
          remainder = size - (chunk * team_size);
          index = MIN(local_id, remainder) * (chunk + 1);
          if (local_id > remainder) {
            index += (local_id - remainder) * chunk;
	  }
          lower = loop_lower + (index * inc);
          chunk += (local_id < remainder);
	}

        if ((inc >= 0) ? (lower <= loop_upper) : (lower >= loop_upper)) {
          upper = LOOPMIN(inc, lower + (chunk - 1) * inc, loop_upper);
          stride = size * inc;
        }
        else {
          /* If the entire set of iterations falls out of the loop bounds
           * then arrange for a non-iterating loop which will not trigger
           * the LASTPRIVATE check made by the compiler. This means that
           * the final value of the loop induction variable must not exceed
           * the loop upper bound. */
          lower = loop_lower - inc;
          upper = lower - inc;
          stride = inc;
        }
        __pmp_loop_analyser(thread, sched, global_id, local_id,
                            loop_lower, loop_upper,
                            lower, upper, inc, chunk, stride);
        break;
      }
      case PMP_SCHED_STATIC: {
        stride = chunk * inc;
        lower = loop_lower + (local_id * stride);
        if ((inc >= 0) ? (lower <= loop_upper) : (lower >= loop_upper)) {
          upper = LOOPMIN(inc, lower + (chunk - 1) * inc, loop_upper);
          stride *= team_size;
        }
        else {
          /* If the entire set of iterations falls out of the loop bounds
           * then arrange for a non-iterating loop which will not trigger
           * the LASTPRIVATE check made by the compiler. This means that
           * the final value of the loop induction variable must not exceed
           * the loop upper bound. */
          lower = loop_lower - inc;
          upper = lower - inc;
          stride = inc;
        }
        __pmp_loop_analyser(thread, sched, global_id, local_id,
                            loop_lower, loop_upper,
                            lower, upper, inc, chunk, stride);
        break;
      }
      default: {
        __pmp_fatal("unknown static scheduling type %d\n", sched);
        stride = 0;
        lower = loop_lower;
        upper = loop_upper;
      }
    }
    *lowerp = lower;
    *upperp = upper;
    *stridep = stride;
  }
  __pmp_scheduler_sample(sched);
}

static inline void __pmp_scheduler_init (int global_id, int sched,
                                         int64_t lower, int64_t upper,
                                         int64_t inc, int64_t chunk)
{
  /* NOTE: chunk parameter is undefined/unused for static even scheduling */

  pmp_thread_t *thread = __pmp_get_thread(global_id);
  pmp_param_t *param = __pmp_get_param();
  int64_t min_chunk = MAX(1, chunk);

  if (sched == PMP_SCHED_RUNTIME || sched == PMP_SCHED_ORDERED_RUNTIME) {
    int old = sched;
    sched = param->runtime_schedule;
    chunk = param->runtime_chunk;
    if (old == PMP_SCHED_ORDERED_RUNTIME) {
      sched += PMP_SCHED_ORDERED_OFFSET;
    } 
  }

  if (sched == PMP_SCHED_GUIDED || sched == PMP_SCHED_ORDERED_GUIDED) {
    /* The initial chunk is loop trip count spread over the number of 
     * threads (the division is rounded up) */
    int team_size = __pmp_get_team_size(thread->team);
    int64_t divisor = team_size * param->guided_chunk_divisor;
    chunk = (upper - lower + divisor) / divisor;
    chunk = MIN(param->guided_chunk_max, chunk);
    chunk = MAX(min_chunk, chunk);
  }

  __pmp_scheduler_sample(sched);

  assert(inc != 0 && chunk != 0 && min_chunk != 0);

  __pmp_loop_alloc(thread, sched, lower, upper, inc, chunk, min_chunk);

  thread->iteration = 0;
}

static inline int __pmp_schedule_next (int global_id, int64_t *lowerp,
                                       int64_t *upperp, int64_t *incp)
{
  pmp_thread_t *thread = __pmp_get_thread(global_id);
  int team_size = __pmp_get_team_size(thread->team);
  int64_t iteration = thread->iteration;
  pmp_local_id_t local_id = thread->local_id;
  pmp_loop_t *loop = thread->loop;

  assert(loop != NULL);
  assert(local_id < team_size);

  if (team_size == 1) {
    if (iteration == 0) {
      *lowerp = loop->lower;
      *upperp = loop->upper;
      *incp = loop->inc;
      thread->ticket_number = loop->lower;
      thread->iteration = 1;
      __pmp_loop_analyser(thread, loop->sched, global_id, local_id,
                          loop->lower, loop->upper,
                          *lowerp, *upperp, *incp, 0, 0);
      return 1;
    }
    else {
      assert(iteration == 1);
      __pmp_loop_free(thread);
      return 0;
    }
  }
  else {
    int     sched = loop->sched;
    int64_t lower = loop->lower;
    int64_t upper = loop->upper;
    int64_t inc   = loop->inc;
    int64_t chunk = loop->chunk;
    switch (sched) {
      case PMP_SCHED_STATIC:
      case PMP_SCHED_ORDERED_STATIC: {
        /* NOTE: setting a small value of chunk causes (unnecessary) iteration
         * through this code. If the chunk is ignored, the code degenerates
         * into the static even case (which is the default). */
        int64_t size = (upper - lower) / inc + 1;
        int64_t size_per_thread = ((size - 1) / team_size + 1) * inc;
        int64_t thread_lower = lower + (local_id * size_per_thread);
        int64_t thread_upper = thread_lower + size_per_thread - inc;
        int64_t this_lower = thread_lower + (iteration * chunk * inc);
        int64_t this_upper = this_lower + (chunk - 1) * inc;
        thread_upper = LOOPMIN(inc, thread_upper, upper);
        this_upper = LOOPMIN(inc, this_upper, thread_upper);
        if ((inc >= 0) ? (this_lower > thread_upper) : 
                         (this_lower < thread_upper)) {
          __pmp_loop_free(thread);
          return 0;
	}
        else {
          *incp = inc;
          *lowerp = this_lower;
          *upperp = this_upper;
          thread->ticket_number = this_lower;
          thread->iteration++;
          __pmp_loop_analyser(thread, sched, global_id, local_id, lower, upper,
                              *lowerp, *upperp, *incp, 0, 0);
          return 1;
	}
        /* NOT REACHED */
        break;
      }
      case PMP_SCHED_STATIC_EVEN:
      case PMP_SCHED_ORDERED_STATIC_EVEN: {
        if (iteration == 0) {
          int64_t size = (upper - lower) / inc + 1;
          int64_t thread_lower;
          int64_t thread_upper;
          if (!__pmp_get_param()->static_fair) {
            int64_t size_per_thread = ((size - 1) / team_size + 1) * inc;
            thread_lower = lower + (local_id * size_per_thread);
            thread_upper = thread_lower + size_per_thread - inc;
	  }
          else {
            int64_t chunk = size / team_size;
            int64_t remainder = size - (chunk * team_size);
            int64_t index = MIN(local_id, remainder) * (chunk + 1);
            if (local_id > remainder) {
              index += (local_id - remainder) * chunk;
            }
            thread_lower = lower + (index * inc);
            chunk += (local_id < remainder);
            thread_upper = thread_lower + (chunk - 1) * inc;
	  }
          thread_upper = LOOPMIN(inc, thread_upper, upper);
          if ((inc >= 0) ? (thread_lower > thread_upper) : 
                           (thread_lower < thread_upper)) {
            __pmp_loop_free(thread);
            return 0;
	  }
	  else {
            *incp = inc;
            *lowerp = thread_lower;
            *upperp = thread_upper;
            thread->ticket_number = thread_lower;
            thread->iteration++;
            __pmp_loop_analyser(thread, sched, global_id, local_id,
                                lower, upper, *lowerp, *upperp, 
                                *incp, 0, 0);
            return 1;
	  }
	}
        else {
          assert(iteration == 1);
          __pmp_loop_free(thread);
	  return 0;
	}
        /* NOT REACHED */
        break;
      }
      case PMP_SCHED_DYNAMIC:
      case PMP_SCHED_ORDERED_DYNAMIC: {
        int64_t stride = inc * chunk;
#if __WORDSIZE == 64
        int64_t current = __pmp_atomic_xadd64(&loop->current, stride);
#else
        /* TODO: the atomic xadd64 is a problem for 32-bit compilation */
        /*       the workaround below is just to do a 32-bit atomic add */
        int64_t current;
        current = (int64_t) __pmp_atomic_xadd32((int32_t *) &loop->current,
                                                (int32_t) stride);
#endif
        if ((inc >= 0) ? (current > upper) : (current < upper)) {
          __pmp_loop_free(thread);
          return 0;
	}
	else {
          *incp = inc;
          *lowerp = current;
          *upperp = *lowerp + stride - inc;
          *upperp = LOOPMIN(inc, upper, *upperp);
          thread->ticket_number = current;
          thread->iteration++;
          __pmp_loop_analyser(thread, sched, global_id, local_id, lower, upper,
                              *lowerp, *upperp, *incp, 0, 0);
          return 1;
	}
        /* NOT REACHED */
        break;
      }
      case PMP_SCHED_GUIDED:
      case PMP_SCHED_ORDERED_GUIDED: {
        /* NOTE: guided scheduling uses a heuristic to choose a good
         * chunk size to divide up the remaining iterations amongst
         * the team (subject to a minimum). An exact implementation of 
         * this would require a lock on the loop data. However, the
         * heuristic can be approximated using (possibly) stale values 
         * and this should be good enough. The value of "remaining"
         * is monotonically decreasing. The worst that could happen
         * is that an update to loop->chunk is lost slightly unbalancing
         * the distribution. The most important point is that loop->current
         * is maintained atomically. */
        /* UPDATE: if cmpxchg64 is available then this is used to protect
         * the update of loop->chunk. This is fairly cunning, and makes
         * the chunk update more accurate in this case! */
        int64_t min_chunk = loop->min_chunk;
        int64_t remaining = upper - loop->current + 1;		 /* estimate */
        int64_t my_chunk = MAX(min_chunk, MIN(chunk, remaining));/* estimate */
        int64_t stride = inc * my_chunk;
#if __WORDSIZE == 64
        int64_t current = __pmp_atomic_xadd64(&loop->current, stride);
#else
        /* TODO: the atomic xadd64 is a problem for 32-bit compilation */
        /*       the workaround below is just to do a 32-bit atomic add */
        int64_t current = __pmp_atomic_xadd32((int32_t *) &loop->current,
                                              (int32_t) stride);
#endif
        assert(stride != 0);
        __pmp_debug(PMP_DEBUG_LOOPS, "__pmp_schedule_next: global_id=%d, "
                    "remaining=%d, my_chunk=%d, stride=%d, current=%d\n",
                    global_id, remaining, my_chunk, stride, current);

        if ((inc >= 0) ? (current > upper) : (current < upper)) {
          __pmp_loop_free(thread);
          return 0;
	}
	else {
          pmp_param_t *param = __pmp_get_param();
          int64_t my_upper = LOOPMIN(inc, upper, current + stride - inc);
          int64_t new_chunk;
          int64_t divisor;
          remaining = upper - my_upper;				/* estimate */
          divisor = team_size * param->guided_chunk_divisor;
          new_chunk = (remaining + divisor - 1) / divisor;
          new_chunk = MIN(param->guided_chunk_max, new_chunk);
          new_chunk = MAX(min_chunk, new_chunk);
#if __WORDSIZE == 64
          (void) __pmp_atomic_cmpxchg64(&loop->chunk, chunk, new_chunk);
#else
          loop->chunk = new_chunk;				/* estimate */
#endif
          *incp = inc;
          *lowerp = current;
          *upperp = my_upper;
          thread->ticket_number = current;
          thread->iteration++;
          __pmp_loop_analyser(thread, sched, global_id, local_id, lower, upper,
                              *lowerp, *upperp, *incp, 0, 0);
          return 1;
	}
        /* NOT REACHED */
        break;
      }
      default: {
        __pmp_fatal("unknown dynamic scheduling type %d\n", sched);
        break;
      }
    }
    /* NOT REACHED */
    assert(0);
    __pmp_loop_free(thread);
    return 0;
  }
  /* NOT REACHED */
}

void __ompc_static_init_8 (int global_id, int sched, int64_t *lower,
                           int64_t *upper, int64_t *stride, int64_t inc,
                           int64_t chunk)
{
  __pmp_debug(PMP_DEBUG_CALLS, "__ompc_static_init_8 global_id=%d with "
              "sched=%d, lower=%" SCNd64 ", upper=%" SCNd64 ", inc=%" SCNd64 ", chunk=%" SCNd64 "\n",
              global_id, sched, (long long) *lower, (long long) *upper,
              (long long) inc, (long long) chunk);
  __pmp_sample(PMP_PROFILE_OMPC_STATIC_INIT_8);
  __pmp_static_init(global_id, sched, lower, upper, stride, inc, chunk);
  __pmp_debug(PMP_DEBUG_CALLS, "__ompc_static_init_8 global_id=%d returns "
              "lower=%" SCNd64 ", upper=%" SCNd64 ", stride=%" SCNd64 "\n",
              global_id, (long long) *lower, (long long) *upper, 
              (long long) *stride);
}

void __ompc_static_init_4 (int global_id, int sched, int *lower,
                           int *upper, int *stride, int inc, int chunk)
{
  int64_t l = (int64_t) *lower;
  int64_t u = (int64_t) *upper;
  int64_t s;

  __pmp_debug(PMP_DEBUG_CALLS, "__ompc_static_init_4 global_id=%d with "
              "sched=%d, lower=%d, upper=%d, inc=%d, chunk=%d\n",
              global_id, sched, *lower, *upper, inc, chunk);

  __pmp_sample(PMP_PROFILE_OMPC_STATIC_INIT_4);

  __pmp_static_init(global_id, sched, &l, &u, &s, inc, chunk);
  *lower = (int) l;
  *upper = (int) u;
  *stride = (int) s;

  __pmp_debug(PMP_DEBUG_CALLS, "__ompc_static_init_4 global_id=%d returns "
              "lower=%d, upper=%d, stride=%d\n",
              global_id, *lower, *upper, *stride);
}

void __ompc_static_fini (void)
{
  __pmp_debug(PMP_DEBUG_CALLS, "__ompc_static_fini\n");
  __pmp_sample(PMP_PROFILE_OMPC_STATIC_FINI);
  /* no work here, NOTE: does not appear to be called by compiler anyway */
}

void __ompc_scheduler_init_8 (int global_id, int sched, int64_t lower,
                              int64_t upper, int64_t inc, int64_t chunk)
{
  __pmp_debug(PMP_DEBUG_CALLS, "__ompc_scheduler_init_8 global_id=%d with "
              "sched=%d, lower=%" SCNd64 ", upper=%" SCNd64 ", inc=%" SCNd64 ", chunk=%" SCNd64 "\n",
              global_id, sched,
              (long long) lower, (long long) upper,
              (long long) inc, (long long) chunk);

  __pmp_sample(PMP_PROFILE_OMPC_SCHEDULER_INIT_8);

  __pmp_scheduler_init(global_id, sched, lower, upper, inc, chunk);
}

int __ompc_schedule_next_8 (int global_id, int64_t *lower,
                            int64_t *upper, int64_t *inc)
{
  int result;

  __pmp_sample(PMP_PROFILE_OMPC_SCHEDULE_NEXT_8);

  result = __pmp_schedule_next(global_id, lower, upper, inc);

  if (result == 0) {
    __pmp_debug(PMP_DEBUG_CALLS, "__ompc_schedule_next_8 global_id=%d returns "
                "result=0\n", global_id);
  }
  else {
    __pmp_debug(PMP_DEBUG_CALLS, "__ompc_schedule_next_8 global_id=%d returns "
                "lower=%" SCNd64 ", upper=%" SCNd64 ", inc=%" SCNd64 ", result=%d\n",
                global_id, (long long) *lower, (long long) *upper, 
                (long long) *inc, result);
  }
  return result;
}

void __ompc_scheduler_init_4 (int global_id, int sched, int lower, int upper,
                              int inc, int chunk)
{
  __pmp_debug(PMP_DEBUG_CALLS, "__ompc_scheduler_init_4 global_id=%d with "
              "sched=%d, lower=%d, upper=%d, inc=%d, chunk=%d\n",
              global_id, sched, lower, upper, inc, chunk);

  __pmp_sample(PMP_PROFILE_OMPC_SCHEDULER_INIT_4);

  __pmp_scheduler_init(global_id, sched, (int64_t) lower, (int64_t) upper,
                       (int64_t) inc, (int64_t) chunk);
}

int __ompc_schedule_next_4 (int global_id, int *lower, int *upper,
                            int *inc)
{
  int result;
  int64_t l;
  int64_t u;
  int64_t i;

  __pmp_sample(PMP_PROFILE_OMPC_SCHEDULE_NEXT_4);

  result = __pmp_schedule_next(global_id, &l, &u, &i);

  *lower = (int) l;
  *upper = (int) u;
  *inc = (int) i;

  if (result == 0) {
    __pmp_debug(PMP_DEBUG_CALLS, "__ompc_schedule_next_4 global_id=%d returns "
                "result=0\n", global_id);
  }
  else {
    __pmp_debug(PMP_DEBUG_CALLS, "__ompc_schedule_next_4 global_id=%d returns "
                "lower=%d, upper=%d, inc=%d, result=%d\n",
                global_id, *lower, *upper, *inc, result);
  }

  return result;
}

void __ompc_ordered (int global_id)
{
  pmp_thread_t *thread = __pmp_get_thread(global_id);

  if (__pmp_get_team_size(thread->team) > 1) {
    pmp_loop_t *loop = thread->loop;
    int64_t ticket_number = thread->ticket_number;
    int64_t now_serving;

    __pmp_debug(PMP_DEBUG_CALLS, "__ompc_ordered: global_id=%d\n", global_id);
    __pmp_sample(PMP_PROFILE_OMPC_ORDERED);

    if (loop == NULL || loop->sched <= PMP_SCHED_ORDERED_OFFSET) {
      __pmp_warning("ordered directives must be used inside ordered "
                    "OpenMP loops\n");
      return;
    }

    assert(loop != NULL);

    now_serving = loop->now_serving;

    if (now_serving != ticket_number) {
      if ((loop->inc >= 0) ? (now_serving > ticket_number) :
                             (now_serving < ticket_number)) {
        __pmp_warning("ordered OpenMP loop may result in program deadlock\n");
        __pmp_warning("maybe due to multiple ordered directives "
                      "in a loop iteration\n");
      }
      while (loop->now_serving != ticket_number) {
        /* USER LEVEL SPIN LOOP */
        __pmp_yield();
      }
    }

    __pmp_debug(PMP_DEBUG_LOOPS, "__ompc_ordered: now serving global_id=%d "
                " ticket_number=%" SCNd64 "\n", global_id, (long long) ticket_number);
  }

  __pmp_memory_fence();
}

void __ompc_end_ordered (int global_id)
{
  pmp_thread_t *thread = __pmp_get_thread(global_id);

  __pmp_memory_fence();

  if (__pmp_get_team_size(thread->team) > 1) {
    pmp_loop_t *loop = thread->loop;
    int64_t ticket_number = thread->ticket_number;

    __pmp_debug(PMP_DEBUG_CALLS, "__ompc_end_ordered: global_id=%d\n",
                global_id);
    __pmp_sample(PMP_PROFILE_OMPC_END_ORDERED);

    if (loop == NULL || loop->sched <= PMP_SCHED_ORDERED_OFFSET) {
      __pmp_warning("ordered directives must be used inside ordered "
                    "OpenMP loops\n");
      return;
    }

    assert(loop != NULL);
    assert(loop->now_serving == ticket_number);

    __pmp_debug(PMP_DEBUG_LOOPS, "__ompc_ordered: stop serving global_id=%d "
                " ticket_number=%" SCNd64 "\n", global_id, (long long) ticket_number);

    loop->now_serving += loop->inc;
    thread->ticket_number = ticket_number + loop->inc;
  }
}
