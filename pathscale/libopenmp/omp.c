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

#include "defalias.h"
#include "lock.h"
#include "omp.h"
#include "ompc.h"
#include "team.h"
#include "thread.h"
#include "types.h"
#include "utils.h"
#include <assert.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>

/* 
 * The functions ending in _ are FORTRAN routines/functions that pass
 * parameters by reference. The functions ending in __ are FORTRAN
 * functions/routines that are accessed without the intrinsics being
 * defined (e.g. if -mp is not used).
 */

static void __pmp_set_num_threads (int nthreads)
{
  /* accesses __pmp_param and any/all cached pmp_param_t structures */
  int default_team_size = MIN(MAX(1, nthreads), PMP_MAX_THREADS);
  pmp_param_t *param = __pmp_get_param();
  if (!param->disabled && param->default_team_size != nthreads) {
    __pmp_param.default_team_size = default_team_size;
    __ompc_sug_numthreads = default_team_size;
    __ompc_max_numthreads = default_team_size;
    if (PMP_PARAM_CACHE) {
      /* propagate default_team_size to param caches */
      int cpus = param->machine_num_cpus;
      int i;
      for (i = 0; i < cpus; i++) {
        __pmp_manager.params[i].default_team_size = default_team_size;
      }
    }
  }
}

static void __pmp_set_nested (int nested)
{
  /* accesses __pmp_param and any/all cached pmp_param_t structures */
  __pmp_param.nested_forks = (nested != 0);
  if (PMP_PARAM_CACHE) {
    /* propagate default_team_size to param caches */
    int cpus = __pmp_get_param()->machine_num_cpus;
    int i;
    for (i = 0; i < cpus; i++) {
      __pmp_manager.params[i].nested_forks = (nested != 0);
    }
  }
}

void omp_set_num_threads (int nthreads)
{
  __pmp_set_num_threads(nthreads);
  __pmp_debug(PMP_DEBUG_CALLS, "omp_set_num_threads to %d\n",
              __pmp_get_param()->default_team_size);
  __pmp_sample(PMP_PROFILE_OMP_SET_NUM_THREADS);
}

void omp_set_num_threads_ (int *nthreads)
{
  __pmp_set_num_threads(*nthreads);
  __pmp_debug(PMP_DEBUG_CALLS, "omp_set_num_threads_ to %d\n",
              __pmp_get_param()->default_team_size);
  __pmp_sample(PMP_PROFILE_OMP_SET_NUM_THREADS);
}

#if defined(BUILD_OS_DARWIN)
void omp_set_num_threads__(int *nthreads) { omp_set_num_threads_(nthreads); }
#else /* defined(BUILD_OS_DARWIN) */
defalias(omp_set_num_threads_, omp_set_num_threads__);
#endif /* defined(BUILD_OS_DARWIN) */

int omp_get_num_threads (void)
{
  int team_size = __pmp_get_current_team_size();
  __pmp_debug(PMP_DEBUG_CALLS, "omp_get_num_threads returns %d\n", team_size);
  __pmp_sample(PMP_PROFILE_OMP_GET_NUM_THREADS);
  return team_size;
}

#if defined(BUILD_OS_DARWIN)
void omp_get_num_threads_() { omp_get_num_threads(); }
void omp_get_num_threads__() { omp_get_num_threads(); }
#else /* defined(BUILD_OS_DARWIN) */
defalias(omp_get_num_threads, omp_get_num_threads_);
defalias(omp_get_num_threads_, omp_get_num_threads__);
#endif /* defined(BUILD_OS_DARWIN) */

int omp_get_max_threads (void)
{
  pmp_param_t *param = __pmp_get_param();
  int max_threads = param->disabled ? 1 : param->default_team_size;
  __pmp_debug(PMP_DEBUG_CALLS, "omp_get_max_threads returns %d\n",
              max_threads);
  __pmp_sample(PMP_PROFILE_OMP_GET_MAX_THREADS);
  return max_threads;
}

#if defined(BUILD_OS_DARWIN)
int omp_get_max_threads_() { return omp_get_max_threads(); }
int omp_get_max_threads__() { return omp_get_max_threads(); }
#else /* defined(BUILD_OS_DARWIN) */
defalias(omp_get_max_threads, omp_get_max_threads_);
defalias(omp_get_max_threads_, omp_get_max_threads__);
#endif /* defined(BUILD_OS_DARWIN) */

int omp_get_thread_num (void)
{
  int local_id = __pmp_get_current_local_id();
  assert(local_id != -1);
  __pmp_debug(PMP_DEBUG_CALLS, "omp_get_thread_num returns %d\n", local_id);
  __pmp_sample(PMP_PROFILE_OMP_GET_THREAD_NUM);
  return local_id;
}

#if defined(BUILD_OS_DARWIN)
int omp_get_thread_num_() { return omp_get_thread_num(); }
int omp_get_thread_num__() { return omp_get_thread_num(); }
#else /* defined(BUILD_OS_DARWIN) */
defalias(omp_get_thread_num, omp_get_thread_num_);
defalias(omp_get_thread_num_, omp_get_thread_num__);
#endif /* defined(BUILD_OS_DARWIN) */

int omp_get_num_procs (void)
{
  int num_cpus = __pmp_get_param()->openmp_num_cpus;
  __pmp_debug(PMP_DEBUG_CALLS, "omp_get_num_procs returns %d\n", num_cpus);
  __pmp_sample(PMP_PROFILE_OMP_GET_NUM_PROCS);
  return num_cpus;
}

#if defined(BUILD_OS_DARWIN)
int omp_get_num_procs_() { return omp_get_num_procs(); }
int omp_get_num_procs__() { return omp_get_num_procs(); }
#else /* defined(BUILD_OS_DARWIN) */
defalias(omp_get_num_procs, omp_get_num_procs_);
defalias(omp_get_num_procs_, omp_get_num_procs__);
#endif /* defined(BUILD_OS_DARWIN) */

int omp_in_parallel (void)
{
  int in_parallel = __pmp_in_parallel();
  __pmp_debug(PMP_DEBUG_CALLS, "omp_in_parallel returns %d\n", in_parallel);
  __pmp_sample(PMP_PROFILE_OMP_IN_PARALLEL);
  return in_parallel;
}

#if defined(BUILD_OS_DARWIN)
int omp_in_parallel_() { return omp_in_parallel(); }
int omp_in_parallel__() { return omp_in_parallel(); }
#else /* defined(BUILD_OS_DARWIN) */
defalias(omp_in_parallel, omp_in_parallel_);
defalias(omp_in_parallel_, omp_in_parallel__);
#endif /* defined(BUILD_OS_DARWIN) */

void omp_set_dynamic (int dynamic)
{
  __pmp_debug(PMP_DEBUG_CALLS, "omp_set_dynamic to %d\n", dynamic);
  if (dynamic != 0) {
    __pmp_warning("dynamic thread adjustment not available "
                  "(ignored omp_set_dynamic)\n");
  }
  __pmp_sample(PMP_PROFILE_OMP_SET_DYNAMIC);
}

void omp_set_dynamic_ (int *dynamic)
{
  __pmp_debug(PMP_DEBUG_CALLS, "omp_set_dynamic_ to %d\n", *dynamic);
  if (dynamic != 0) {
    __pmp_warning("dynamic thread adjustment not available "
                  "(ignored omp_set_dynamic)\n");
  }
  __pmp_sample(PMP_PROFILE_OMP_SET_DYNAMIC);
}

#if defined(BUILD_OS_DARWIN)
void omp_set_dynamic__(int *dynamic) { omp_set_dynamic_(dynamic); }
#else /* defined(BUILD_OS_DARWIN) */
defalias(omp_set_dynamic_, omp_set_dynamic__);
#endif /* defined(BUILD_OS_DARWIN) */

int omp_get_dynamic (void)
{
  __pmp_debug(PMP_DEBUG_CALLS, "omp_get_dynamic\n");
  __pmp_sample(PMP_PROFILE_OMP_GET_DYNAMIC);
  return __pmp_get_param()->dynamic_threads;
}

#if defined(BUILD_OS_DARWIN)
int omp_get_dynamic_() { return omp_get_dynamic(); }
int omp_get_dynamic__() { return omp_get_dynamic(); }
#else /* defined(BUILD_OS_DARWIN) */
defalias(omp_get_dynamic, omp_get_dynamic_);
defalias(omp_get_dynamic_, omp_get_dynamic__);
#endif /* defined(BUILD_OS_DARWIN) */

void omp_set_nested (int nested)
{
  __pmp_debug(PMP_DEBUG_CALLS, "omp_set_nested to %d\n", nested);
  __pmp_set_nested(nested);
  __pmp_sample(PMP_PROFILE_OMP_SET_NESTED);
}

void omp_set_nested_ (int *nested)
{
  __pmp_debug(PMP_DEBUG_CALLS, "omp_set_nested_ to %d\n", *nested);
  __pmp_set_nested(*nested);
  __pmp_sample(PMP_PROFILE_OMP_SET_NESTED);
}

#if defined(BUILD_OS_DARWIN)
void omp_set_nested__(int *nested) { omp_set_nested_(nested); }
#else /* defined(BUILD_OS_DARWIN) */
defalias(omp_set_nested_, omp_set_nested__);
#endif /* defined(BUILD_OS_DARWIN) */

int omp_get_nested (void)
{
  __pmp_debug(PMP_DEBUG_CALLS, "omp_get_nested\n");
  __pmp_sample(PMP_PROFILE_OMP_GET_NESTED);
  return __pmp_get_param()->nested_forks;
}

#if defined(BUILD_OS_DARWIN)
int omp_get_nested_() { return omp_get_nested(); }
int omp_get_nested__() { return omp_get_nested(); }
#else /* defined(BUILD_OS_DARWIN) */
defalias(omp_get_nested, omp_get_nested_);
defalias(omp_get_nested_, omp_get_nested__);
#endif /* defined(BUILD_OS_DARWIN) */

/* locks - assumes that *lock is writable */

void omp_init_lock (omp_lock_t *lock)
{
  pmp_lock_t *l;
  void *result;
  __pmp_debug(PMP_DEBUG_CALLS, "omp_init_lock with lock %p\n", lock);
  __pmp_sample(PMP_PROFILE_OMP_INIT_LOCK);
#if defined(BUILD_OS_DARWIN)
  result = darwin_memalign(PMP_L2_ALIGNED, sizeof(pmp_lock_t),
    "cannot allocate lock\n");
#else /* defined(BUILD_OS_DARWIN) */
  if (posix_memalign(&result, PMP_L2_ALIGNED, sizeof(pmp_lock_t)) != 0) {
    __pmp_fatal("cannot allocate lock state\n");
  }
#endif /* defined(BUILD_OS_DARWIN) */
  l = (pmp_lock_t *) result;
  __pmp_lock_init(l);
  *lock = l;
}

#if defined(BUILD_OS_DARWIN)
void omp_init_lock_(omp_lock_t *lock) { omp_init_lock(lock); }
void omp_init_lock__(omp_lock_t *lock) { omp_init_lock(lock); }
#else /* defined(BUILD_OS_DARWIN) */
defalias(omp_init_lock, omp_init_lock_);
defalias(omp_init_lock_, omp_init_lock__);
#endif /* defined(BUILD_OS_DARWIN) */

void omp_destroy_lock (omp_lock_t *lock)
{
  __pmp_debug(PMP_DEBUG_CALLS, "omp_destroy_lock with lock %p\n", lock);
  __pmp_sample(PMP_PROFILE_OMP_DESTROY_LOCK);
  __pmp_lock_destroy(*lock);
}

#if defined(BUILD_OS_DARWIN)
void omp_destroy_lock_(omp_lock_t *lock) { omp_destroy_lock(lock); }
void omp_destroy_lock__(omp_lock_t *lock) { omp_destroy_lock(lock); }
#else /* defined(BUILD_OS_DARWIN) */
defalias(omp_destroy_lock, omp_destroy_lock_);
defalias(omp_destroy_lock_, omp_destroy_lock__);
#endif /* defined(BUILD_OS_DARWIN) */

void omp_set_lock (omp_lock_t *lock)
{
  __pmp_debug(PMP_DEBUG_CALLS, "omp_set_lock with lock %p\n", lock);
  __pmp_sample(PMP_PROFILE_OMP_SET_LOCK);
  __pmp_lock(__pmp_get_current_global_id(), *lock);
}

#if defined(BUILD_OS_DARWIN)
void omp_set_lock_(omp_lock_t *lock) { omp_set_lock(lock); }
void omp_set_lock__(omp_lock_t *lock) { omp_set_lock(lock); }
#else /* defined(BUILD_OS_DARWIN) */
defalias(omp_set_lock, omp_set_lock_);
defalias(omp_set_lock_, omp_set_lock__);
#endif /* defined(BUILD_OS_DARWIN) */

void omp_unset_lock (omp_lock_t *lock)
{
  __pmp_unlock(__pmp_get_current_global_id(), *lock);
  __pmp_debug(PMP_DEBUG_CALLS, "omp_unset_lock with lock %p\n", lock);
  __pmp_sample(PMP_PROFILE_OMP_UNSET_LOCK);
}

#if defined(BUILD_OS_DARWIN)
void omp_unset_lock_(omp_lock_t *lock) { omp_unset_lock(lock); }
void omp_unset_lock__(omp_lock_t *lock) { omp_unset_lock(lock); }
#else /* defined(BUILD_OS_DARWIN) */
defalias(omp_unset_lock, omp_unset_lock_);
defalias(omp_unset_lock_, omp_unset_lock__);
#endif /* defined(BUILD_OS_DARWIN) */

int omp_test_lock (omp_lock_t *lock)
{
  int success = __pmp_trylock(__pmp_get_current_global_id(), *lock);
  __pmp_debug(PMP_DEBUG_CALLS, "omp_test_lock with lock %p returns %d\n",
              lock, success);
  __pmp_sample(PMP_PROFILE_OMP_TEST_LOCK);
  return success;
}

#if defined(BUILD_OS_DARWIN)
void omp_test_lock_(omp_lock_t *lock) { omp_test_lock(lock); }
void omp_test_lock__(omp_lock_t *lock) { omp_test_lock(lock); }
#else /* defined(BUILD_OS_DARWIN) */
defalias(omp_test_lock, omp_test_lock_);
defalias(omp_test_lock_, omp_test_lock__);
#endif /* defined(BUILD_OS_DARWIN) */

/* nested locks - assumes that *lock is writable */

void omp_init_nest_lock (omp_nest_lock_t *lock)
{
  pmp_nest_lock_t *l;
  void *result;
  __pmp_debug(PMP_DEBUG_CALLS, "omp_init_nest_lock with lock %p\n", lock);
  __pmp_sample(PMP_PROFILE_OMP_INIT_NEST_LOCK);
#if defined(BUILD_OS_DARWIN)
  result = darwin_memalign(PMP_L2_ALIGNED, sizeof(pmp_nest_lock_t),
    "cannot allocate lock\n");
#else /* defined(BUILD_OS_DARWIN) */
  if (posix_memalign(&result, PMP_L2_ALIGNED, 
                     sizeof(pmp_nest_lock_t)) != 0) {
    __pmp_fatal("cannot allocate nested lock state\n");
  }
#endif /* defined(BUILD_OS_DARWIN) */
  l = (pmp_nest_lock_t *) result;
  __pmp_nest_lock_init(l);
  *lock = l;
}

#if defined(BUILD_OS_DARWIN)
void omp_init_nest_lock_(omp_nest_lock_t *lock) { omp_init_nest_lock(lock); }
void omp_init_nest_lock__(omp_nest_lock_t *lock) {omp_init_nest_lock(lock); }
#else /* defined(BUILD_OS_DARWIN) */
defalias(omp_init_nest_lock, omp_init_nest_lock_);
defalias(omp_init_nest_lock_, omp_init_nest_lock__);
#endif /* defined(BUILD_OS_DARWIN) */

void omp_destroy_nest_lock (omp_nest_lock_t *lock)
{
  __pmp_debug(PMP_DEBUG_CALLS, "omp_destroy_nest_lock with lock %p\n", lock);
  __pmp_sample(PMP_PROFILE_OMP_DESTROY_NEST_LOCK);
  __pmp_nest_lock_destroy(*lock);
}

#if defined(BUILD_OS_DARWIN)
void omp_destroy_nest_lock_(omp_nest_lock_t *lock) { omp_destroy_nest_lock(lock); }
void omp_destroy_nest_lock__(omp_nest_lock_t *lock) { omp_destroy_nest_lock(lock); }
#else /* defined(BUILD_OS_DARWIN) */
defalias(omp_destroy_nest_lock, omp_destroy_nest_lock_);
defalias(omp_destroy_nest_lock_, omp_destroy_nest_lock__);
#endif /* defined(BUILD_OS_DARWIN) */

void omp_set_nest_lock (omp_nest_lock_t *lock)
{
  __pmp_debug(PMP_DEBUG_CALLS, "omp_set_nest_lock with lock %p\n", lock);
  __pmp_sample(PMP_PROFILE_OMP_SET_NEST_LOCK);
  __pmp_nest_lock(__pmp_get_current_global_id(), *lock);
}

#if defined(BUILD_OS_DARWIN)
void omp_set_nest_lock_(omp_nest_lock_t *lock) { omp_set_nest_lock(lock); }
void omp_set_nest_lock__(omp_nest_lock_t *lock) { omp_set_nest_lock(lock); }
#else /* defined(BUILD_OS_DARWIN) */
defalias(omp_set_nest_lock, omp_set_nest_lock_);
defalias(omp_set_nest_lock_, omp_set_nest_lock__);
#endif /* defined(BUILD_OS_DARWIN) */

void omp_unset_nest_lock (omp_nest_lock_t *lock)
{
  __pmp_nest_unlock(__pmp_get_current_global_id(), *lock);
  __pmp_debug(PMP_DEBUG_CALLS, "omp_unset_nest_lock with lock %p\n", lock);
  __pmp_sample(PMP_PROFILE_OMP_UNSET_NEST_LOCK);
}

#if defined(BUILD_OS_DARWIN)
void omp_unset_nest_lock_(omp_nest_lock_t *lock) { omp_unset_nest_lock(lock); }
void omp_unset_nest_lock__(omp_nest_lock_t *lock) { omp_unset_nest_lock(lock); }
#else /* defined(BUILD_OS_DARWIN) */
defalias(omp_unset_nest_lock, omp_unset_nest_lock_);
defalias(omp_unset_nest_lock_, omp_unset_nest_lock__);
#endif /* defined(BUILD_OS_DARWIN) */

int omp_test_nest_lock (omp_nest_lock_t *lock)
{
  int success = __pmp_nest_trylock(__pmp_get_current_global_id(), *lock);
  __pmp_debug(PMP_DEBUG_CALLS, "omp_test_nest_lock with lock %p returns %d\n",
              lock, success);
  __pmp_sample(PMP_PROFILE_OMP_TEST_NEST_LOCK);
  return success;

}

#if defined(BUILD_OS_DARWIN)
int omp_test_nest_lock_(omp_nest_lock_t *lock) { return omp_test_nest_lock(lock); }
int omp_test_nest_lock__(omp_nest_lock_t *lock) { return omp_test_nest_lock(lock); }
#else /* defined(BUILD_OS_DARWIN) */
defalias(omp_test_nest_lock, omp_test_nest_lock_);
defalias(omp_test_nest_lock_, omp_test_nest_lock__);
#endif /* defined(BUILD_OS_DARWIN) */

/* timing */

double omp_get_wtime (void)
{
  struct timeval time;
  double t;
  gettimeofday(&time, NULL);
  t = ((double) time.tv_sec) + ((double) time.tv_usec / 1000000.0);
  __pmp_debug(PMP_DEBUG_CALLS, "omp_get_wtime returns wtime = %e\n", t);
  __pmp_sample(PMP_PROFILE_OMP_GET_WTIME);
  return t;
}

#if defined(BUILD_OS_DARWIN)
double omp_get_wtime_() { return omp_get_wtime(); }
double omp_get_wtime__() { return omp_get_wtime(); }
#else /* defined(BUILD_OS_DARWIN) */
defalias(omp_get_wtime, omp_get_wtime_);
defalias(omp_get_wtime_, omp_get_wtime__);
#endif /* defined(BUILD_OS_DARWIN) */

double omp_get_wtick (void)
{
  double t = __pmp_get_param()->wtick;
  __pmp_debug(PMP_DEBUG_CALLS, "omp_get_wtick returns wtick = %e\n", t);
  __pmp_sample(PMP_PROFILE_OMP_GET_WTICK);
  return t;
}

#if defined(BUILD_OS_DARWIN)
double omp_get_wtick_() { return omp_get_wtick(); }
double omp_get_wtick__() { return omp_get_wtick(); }
#else /* defined(BUILD_OS_DARWIN) */
defalias(omp_get_wtick, omp_get_wtick_);
defalias(omp_get_wtick_, omp_get_wtick__);
#endif /* defined(BUILD_OS_DARWIN) */
