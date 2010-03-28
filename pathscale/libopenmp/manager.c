/*
 * Copyright (C) 2007, 2008 PathScale, LLC.  All Rights Reserved.
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

#include "atomic.h"
#include "idstack.h"
#include "lock.h"
#include "team.h"
#include "thread.h"
#include "types.h"
#include "utils.h"
#include <assert.h>
#include <dirent.h>	/* bizarre header trick to find PTHREAD_STACK_MIN */
#include <errno.h>
#if defined(BUILD_OS_DARWIN)
#include <unistd.h>
#include <limits.h>
#else /* defined(BUILD_OS_DARWIN) */
#include <linux/unistd.h>
#endif /* defined(BUILD_OS_DARWIN) */
#include <stdlib.h>
#include <string.h>
#include <sys/syscall.h>

static const bool STACK_SIZE_LIMIT_WITH_PTHREAD_CREATE = 0;

pmp_manager_t __pmp_manager __attribute__ ((aligned(PMP_L2_ALIGNED))) =
{
  false
};

static void __pmp_signal_dummy (int sig)
{
  ;
}

static void *__pmp_thread_dummy (void *arg)
{
  return NULL;
}

static size_t __pmp_get_pthread_stack_size_limit (void)
{
  pmp_param_t *param = &__pmp_param;
  size_t stack_size = PTHREAD_STACK_MIN;
  size_t stack_max = (param->omp_stack_size == NULL) ?
                     PMP_STACK_SOFT_MAX :   /* soft max for default */
                     PMP_STACK_HARD_MAX;    /* hard max for explicit stack */
  pthread_attr_t pthread_attr;

  if (pthread_attr_init(&pthread_attr) != 0) {
    __pmp_fatal("failed to initialize pthread attribute structure\n");
  }

  while ((stack_size << 1) <= stack_max &&
	 pthread_attr_setstacksize(&pthread_attr, stack_size << 1) == 0) {
    __pmp_debug(PMP_DEBUG_INFO, "pthread attr stack size of %ld bytes is OK\n",
                stack_size << 1);
    stack_size <<= 1;    
  }

  if (STACK_SIZE_LIMIT_WITH_PTHREAD_CREATE) {
    while (stack_size >= PTHREAD_STACK_MIN &&
  	 pthread_attr_setstacksize(&pthread_attr, stack_size) == 0) {
      pthread_t pthread_id;
      int result = pthread_create(&pthread_id, &pthread_attr,
  				__pmp_thread_dummy, NULL);
      if (result == 0) {
        __pmp_debug(PMP_DEBUG_INFO, "pthread stack size of %ld bytes is OK\n",
                    stack_size);
        void *value;
        pthread_join(pthread_id, &value);
        break;
      }
      else if (result == ENOMEM) {
        stack_size >>= 1;
      }
      else {
        /* other error case, give up on loop */
        break;
      }
    }
  }

  if (pthread_attr_destroy(&pthread_attr) != 0) {
    __pmp_fatal("failed to destroy pthread attribute structure\n");
  }

  __pmp_debug(PMP_DEBUG_INFO, "maximum pthread stack size is %ld bytes\n",
              stack_size);
  return stack_size;
}

void __pmp_manager_create_main_thread (void)
{
  int global_id;
  global_id = __pmp_atomic_xadd32(&__pmp_manager.allocated_threads, 1);
  assert(global_id == 0);
  __pmp_thread_create_main();
}

int __pmp_manager_create_more_threads (int nthreads)
{
  int count;

  for (count = 0; count < nthreads; count++) {
    int global_id = __pmp_atomic_xadd32(&__pmp_manager.allocated_threads, 1);
    assert(global_id != 0);

    if (global_id >= PMP_MAX_THREADS) {
      __pmp_atomic_xadd32(&__pmp_manager.allocated_threads, -1);
      break;
    }

    __pmp_thread_create(__pmp_get_thread(global_id));
  }

  return count;
}

void __pmp_manager_init (void)
{
  int i;
  struct sigaction act;
  void *result;
  sigset_t emptyset;
  size_t stack_min = PTHREAD_STACK_MIN;
  size_t stack_max = __pmp_get_pthread_stack_size_limit();
  size_t stack_default = -1;
  pmp_param_t *param = &__pmp_param;
  int cpus = param->machine_num_cpus;

  assert(!__pmp_manager.initialized);

  assert((sizeof(pmp_team_t) % PMP_L2_ALIGNED) == 0);
  assert((sizeof(pmp_thread_t) % PMP_L2_ALIGNED) == 0);

  if (pthread_key_create(&__pmp_manager.thread_key, NULL) != 0) {
    __pmp_fatal("cannot create pthread key\n");
  }

  /* Allocate and initialize team array */
#if defined(BUILD_OS_DARWIN)
  result = darwin_memalign(PMP_L2_ALIGNED,
    sizeof(pmp_team_t) * PMP_MAX_THREADS, "cannot allocate teams array\n");
#else /* defined(BUILD_OS_DARWIN) */
  if (posix_memalign(&result, PMP_L2_ALIGNED, 
                     sizeof(pmp_team_t) * PMP_MAX_THREADS) != 0) {
    __pmp_fatal("cannot allocate teams array\n");
  }
#endif /* defined(BUILD_OS_DARWIN) */
  memset(result, 0, sizeof(pmp_team_t) * PMP_MAX_THREADS);
  __pmp_manager.teams = (pmp_team_t *) result;

  for (i = 0; i < PMP_MAX_THREADS; i++) {
    __pmp_team_init(__pmp_manager.teams + i);
  }

  /* Allocate and initialize thread array */
#if defined(BUILD_OS_DARWIN)
  result = darwin_memalign(PMP_L2_ALIGNED,
    sizeof(pmp_thread_t) * PMP_MAX_THREADS, "cannot allocate threads array\n");
#else /* defined(BUILD_OS_DARWIN) */
  if (posix_memalign(&result, PMP_L2_ALIGNED,
                     sizeof(pmp_thread_t) * PMP_MAX_THREADS) != 0) {
    __pmp_fatal("cannot allocate threads array\n");
  }
#endif /* defined(BUILD_OS_DARWIN) */
  memset(result, 0, sizeof(pmp_thread_t) * PMP_MAX_THREADS);
  __pmp_manager.threads = (pmp_thread_t *) result;

  for (i = 0; i < PMP_MAX_THREADS; i++) {
    __pmp_thread_init(__pmp_manager.threads + i, i);
  }

  __pmp_manager.params = NULL;
  __pmp_manager.waiting_threads = 0;  
  __pmp_manager.allocated_threads = 0;

  __pmp_idstack_init(&__pmp_manager.idstack);
  __pmp_lock_init(&__pmp_manager.idlock);
  __pmp_lock_init(&__pmp_manager.pthread_create_lock);

  if (pthread_attr_init(&__pmp_manager.pthread_attr) != 0) {
    __pmp_fatal("failed to initialize pthread attribute structure\n");
  }
  if (pthread_attr_setscope(&__pmp_manager.pthread_attr,
                            PTHREAD_SCOPE_SYSTEM) != 0) {
    __pmp_warning("failed to set pthread scheduling scope\n");
  }
  if (pthread_attr_getstacksize(&__pmp_manager.pthread_attr,
                                &stack_default) != 0) {
    __pmp_warning("failed to get pthread default stack size\n");
  }
  if (param->thread_stack_size < stack_min) {
    if (param->omp_stack_size != NULL) {
      /* Only warn if explicit PSC_OMP_STACK_SIZE was given */
      __pmp_warning("requested pthread stack too small, "
                    "using %ld bytes instead\n",
                    (long) stack_min);
    }
    param->thread_stack_size = stack_min;
  }
  if (param->thread_stack_size > stack_max) {
    if (param->omp_stack_size != NULL) {
      /* Only warn if explicit PSC_OMP_STACK_SIZE was given */
      __pmp_warning("requested pthread stack too large, "
                    "using %ld bytes instead\n",
                    (long) stack_max);
    }
    param->thread_stack_size = stack_max;
  }
  if (pthread_attr_setstacksize(&__pmp_manager.pthread_attr,
                                param->thread_stack_size) != 0) {
    __pmp_warning("failed to set pthread stack size to %ld bytes\n",
                  (long) param->thread_stack_size);
    if (stack_default > 0) {
      __pmp_warning("use system default pthread stack size of %ld bytes\n", 
                    (long) stack_default);
      param->thread_stack_size = stack_default;
    }
  }
  __pmp_debug(PMP_DEBUG_THREAD, "pthread stack size is set to %ld bytes\n",
              (long) param->thread_stack_size);

  /* If params are to be cached (per CPU), create the necessary data structures
   * and propagate the current param values. Any updates of the params
   * after here will need explicit code to keep the param cache up to date. */

  if (PMP_PARAM_CACHE) {
    /* Allocate and initialize param array */
    if (posix_memalign(&result, PMP_L2_ALIGNED,
                       sizeof(pmp_param_t) * cpus) != 0) {
      __pmp_fatal("cannot allocate params array\n");
    }
    __pmp_manager.params = (pmp_param_t *) result;
  
    for (i = 0; i < cpus; i++) {
      __pmp_manager.params[i] = __pmp_param;
  
      if (posix_memalign(&result, PMP_L2_ALIGNED,
                         PMP_MAX_CPUS * sizeof(int)) != 0) {
        __pmp_fatal("failed to allocate CPU mapping array\n");
      }
      memcpy(result, __pmp_param.openmp_to_machine_cpu_map,
             PMP_MAX_CPUS * sizeof(int));
  
      __pmp_manager.params[i].openmp_to_machine_cpu_map = (int *) result;
  
      if (posix_memalign(&result, PMP_L2_ALIGNED,
                         PMP_MAX_THREADS * sizeof(int)) != 0) {
        __pmp_fatal("failed to allocate thread to CPU mapping array\n");
      }
      memcpy(result, __pmp_param.thread_to_cpu_map,
             PMP_MAX_THREADS * sizeof(int));
  
      __pmp_manager.params[i].thread_to_cpu_map = (int *) result;
    }
  }
  else {
    __pmp_manager.params = NULL;
  }

  sigemptyset(&__pmp_manager.mask_block_sigpmp);
  sigaddset(&__pmp_manager.mask_block_sigpmp, SIGPMP);
  sigfillset(&__pmp_manager.mask_unblock_sigpmp);
  sigdelset(&__pmp_manager.mask_unblock_sigpmp, SIGPMP);

  /* Do not block the following terminating/core signals: */
  sigdelset(&__pmp_manager.mask_unblock_sigpmp, SIGHUP);
  sigdelset(&__pmp_manager.mask_unblock_sigpmp, SIGINT);
  sigdelset(&__pmp_manager.mask_unblock_sigpmp, SIGQUIT);
  sigdelset(&__pmp_manager.mask_unblock_sigpmp, SIGILL);
  sigdelset(&__pmp_manager.mask_unblock_sigpmp, SIGABRT);
  sigdelset(&__pmp_manager.mask_unblock_sigpmp, SIGFPE);
  sigdelset(&__pmp_manager.mask_unblock_sigpmp, SIGSEGV);
  sigdelset(&__pmp_manager.mask_unblock_sigpmp, SIGPIPE);
  sigdelset(&__pmp_manager.mask_unblock_sigpmp, SIGALRM);
  sigdelset(&__pmp_manager.mask_unblock_sigpmp, SIGTERM);

  /* Retain the original set of signals: */
  sigemptyset(&emptyset);
#ifdef PMP_USE_PTHREAD_SIGNALS
  if (pthread_sigmask(SIG_BLOCK, &emptyset, 
                          &__pmp_manager.mask_original) != 0) {
    __pmp_fatal("pthread_sigmask system call failed\n");
  }
#else
  if (sigprocmask(SIG_BLOCK, &emptyset, &__pmp_manager.mask_original) != 0) {
    __pmp_fatal("sigprocmask system call failed\n");
  }
#endif

  memset(&act, 0, sizeof(act));
  act.sa_handler = &__pmp_signal_dummy;
  if (sigaction(SIGPMP, &act, NULL) != 0) {
    __pmp_fatal("sigaction system call failed\n");
  }

  __pmp_manager_create_main_thread();

  __pmp_manager.initialized = true;	/* before more threads */

  (void) __pmp_manager_create_more_threads(param->default_team_size - 1);

  __pmp_debug(PMP_DEBUG_INFO, "OpenMP thread manager is initialized\n");
}

void __pmp_manager_fini (void)
{
  int i;

  assert(__pmp_manager.initialized);

  // 14865: __pmp_lock_destroy calls __pmp_get_param which accesses
  // __pmp_manager.threads and more, so destroy locks first.
  __pmp_lock_destroy(&__pmp_manager.idlock);
  __pmp_lock_destroy(&__pmp_manager.pthread_create_lock);

  free(__pmp_manager.threads);

  for (i = 0; i < PMP_MAX_THREADS; i++) {
    __pmp_team_fini(__pmp_manager.teams + i);
  }

  free(__pmp_manager.teams);

  if (__pmp_manager.params != NULL) {
    for (i = 0; i < __pmp_param.machine_num_cpus; i++) {
      free(__pmp_manager.params[i].openmp_to_machine_cpu_map);
      free(__pmp_manager.params[i].thread_to_cpu_map);
    }
    free(__pmp_manager.params);
  }

#ifdef PMP_USE_PTHREAD_SIGNALS
  if (pthread_sigmask(SIG_SETMASK, &__pmp_manager.mask_original,
                          NULL) != 0) {
    __pmp_fatal("pthread_sigmask system call failed\n");
  }
#else
  if (sigprocmask(SIG_SETMASK, &__pmp_manager.mask_original, NULL) != 0) {
    __pmp_fatal("sigprocmask system call failed\n");
  }
#endif

  __pmp_manager.initialized = false;

  /* TODO - attempt to clean-up pthreads? They should all be hanging
  * around in __pmp_thread_wait(). */
}
