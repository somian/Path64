/*
 * Copyright (C) 2007, 2008. PathScale, LLC. All Rights Reserved.
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

#include "affinity.h"
#include "atomic.h"
#include "idstack.h"
#include "manager.h"
#include "ompc.h"
#include "stack.h"
#include "team.h"
#include "utils.h"

#include <assert.h>
#include <errno.h>
#if defined(BUILD_OS_DARWIN)
#include <unistd.h>
#else /* defined(BUILD_OS_DARWIN) */
#include <linux/unistd.h>
#endif /* defined(BUILD_OS_DARWIN) */
#include <sched.h>
#include <signal.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <unistd.h>

#ifndef PMP_NO_TLS
#ifdef PMP_TLS_THREAD
__thread pmp_thread_t *__pmp_tls_current_thread = NULL;
#endif
#ifdef PMP_TLS_LOCAL_ID
__thread pmp_local_id_t __pmp_tls_current_local_id = -1;
#endif
#ifdef PMP_TLS_GLOBAL_ID
__thread pmp_global_id_t __pmp_tls_current_global_id = -1;
#endif
#ifdef PMP_TLS_TEAM
__thread pmp_team_t *__pmp_tls_current_team = NULL;
#endif
#endif

#ifndef PMP_NO_NPTL
extern pid_t gettid(void);
pid_t gettid(void) { return syscall(SYS_gettid);}
#if defined(BUILD_OS_DARWIN)
int tkill(pid_t tid,int sig) { return syscall(SYS___pthread_kill, tid, sig);}
#else /* defined(BUILD_OS_DARWIN) */
int tkill(pid_t tid,int sig) { return syscall(SYS_tkill, tid, sig);}
#endif /* defined(BUILD_OS_DARWIN) */
#endif

static inline void __pmp_thread_wait (pmp_thread_t *thread)
{
  int32_t sync;
  int thread_spin;
  int i;

  if (thread->sync == PMP_SYNC_UNBLOCKED) {
    __pmp_debug(PMP_DEBUG_THREAD, "thread global_id %d does not block (1)\n",
                thread->global_id);
    thread->sync = PMP_SYNC_IDLE;
    return;
  }

  thread_spin = __pmp_get_param()->thread_spin;

  for (i = 0; i < thread_spin; i++) {
    /* USER LEVEL SPIN LOOP */
    if (thread->sync == PMP_SYNC_UNBLOCKED) {
      __pmp_debug(PMP_DEBUG_THREAD, "thread global_id %d does not block (2)\n",
                  thread->global_id);
      thread->sync = PMP_SYNC_IDLE;
      return;
    }
    __pmp_yield();
  }

  sync = __pmp_atomic_cmpxchg32(&thread->sync, PMP_SYNC_IDLE,
                                PMP_SYNC_BLOCKED);

  if (sync == PMP_SYNC_IDLE) {
    __pmp_debug(PMP_DEBUG_THREAD, "thread global_id %d is waiting\n",
                thread->global_id);

    __pmp_sample(PMP_PROFILE_THREAD_DESCHEDULE);

#ifdef PMP_USE_PTHREAD_SIGNALS
    {
      int sig;
      do {
        sigwait(&__pmp_manager.mask_block_sigpmp, &sig);
      } while (sig != SIGPMP);
    }
#else
    sigsuspend(&__pmp_manager.mask_unblock_sigpmp);
    /* NOTE: it is unfortunate that sigsuspend does not tell us which
     *       signal has been raised. */
#endif
    __pmp_debug(PMP_DEBUG_THREAD, "thread global_id %d is awake\n",
                thread->global_id);
  }
  else {
    __pmp_debug(PMP_DEBUG_THREAD, "thread global_id %d does not block (3)\n",
                thread->global_id);
    thread->sync = PMP_SYNC_IDLE;
  }
}

static inline void __pmp_thread_wake (pmp_thread_t *thread)
{
  int32_t sync = __pmp_atomic_cmpxchg32(&thread->sync, PMP_SYNC_IDLE,
                                        PMP_SYNC_UNBLOCKED);
  assert(sync != PMP_SYNC_UNBLOCKED);

  if (sync == PMP_SYNC_BLOCKED) {
    __pmp_debug(PMP_DEBUG_THREAD, "thread global_id %d is being signaled\n",
                thread->global_id);

    thread->sync = PMP_SYNC_IDLE;

    assert(thread->tid != -1);

    __pmp_sample(PMP_PROFILE_THREAD_RESCHEDULE);
#if (defined PMP_USE_PTHREAD_SIGNALS)
    if (pthread_kill(thread->pthread_id, SIGPMP) != 0) {
      __pmp_fatal("unable to wake thread using pthread_kill\n");
    }
#elif (defined PMP_NO_NPTL)
    if (kill(thread->tid, SIGPMP) != 0) {
      __pmp_fatal("unable to wake thread using kill\n");
    }
#else
    if (tkill(thread->tid, SIGPMP) != 0) {
      __pmp_fatal("unable to wake thread using tkill\n");
    }
#endif
  }
  else {
    __pmp_debug(PMP_DEBUG_THREAD, "thread global_id %d is woken\n",
                thread->global_id);
  }
}

static inline void __pmp_thread_worker_join (pmp_team_t *team)
{
  int32_t count = __pmp_atomic_xadd32(&team->working_threads, -1);
  __pmp_debug(PMP_DEBUG_THREAD, "worker thread joins with count of %d\n", 
              (int) count);
  assert(count >= 1);
  if (count == 1) {
    __pmp_thread_wake(team->members[0]);
  }
}

static inline void __pmp_thread_master_join (pmp_thread_t *master)
{
  pmp_team_t *team = master->team;
  int32_t count;
  int thread_spin = __pmp_get_param()->thread_spin;
  int i;

  /* NOTE: insert a small spin loop here to try to arrange for the master
   *       to arrive just after the last worker thread. If this happens
   *       then we avoid a much more expensive thread synchronization. */

  for (i = 0; i < thread_spin; i++) {
    /* USER LEVEL SPIN LOOP */
    if (team->working_threads == 1) {
      team->working_threads = 0;
      return;
    }
    __pmp_yield();
  }

  count = __pmp_atomic_xadd32(&team->working_threads, -1);
  __pmp_debug(PMP_DEBUG_THREAD, "master thread joins with count of %d\n", 
              (int) count);
  assert(count >= 1);
  if (count > 1) {
    __pmp_thread_wait(master);
  }
}

static inline void __pmp_thread_idle (pmp_thread_t *thread)
{
  thread->single_count = 0;
  thread->loop_count = 0;
  thread->serialized_parallel = 0;
  thread->nesting_depth = 0;
}

static void __pmp_thread_bind (pmp_thread_t *thread)
{
  /* TODO : use dynamic information to bind threads appropriately */

  pmp_param_t *param = __pmp_get_param();
  if (param->enable_affinity) {
    int cpu;
    int index = param->global_affinity ? thread->global_id : thread->local_id;
    assert(index < PMP_MAX_THREADS);
    cpu = param->thread_to_cpu_map[index];
    assert(cpu < param->machine_num_cpus);
    if (thread->cpu != cpu) {
      static bool __pmp_enable_affinity_warning = true;
      int e;
      if (__pmp_manager.params != NULL) {
        thread->param = &__pmp_manager.params[cpu];
      }
      else {
        thread->param = &__pmp_param;
      }
      e = __pmp_set_affinity(cpu);
      __pmp_debug(PMP_DEBUG_THREAD, "__pmp_thread_bind: global_id=%d, "
                  "local_id=%d, CPU=%d, param=%p\n",
                  thread->global_id, thread->local_id, cpu, thread->param);
      if (e != 0 && __pmp_enable_affinity_warning) {
        __pmp_warning("failed to set affinity\n");
        __pmp_warning("maybe the kernel does not support "
                      "affinity system calls\n");
        __pmp_enable_affinity_warning = false;
      }
      thread->cpu = cpu;
    }

    /* TODO: give the thread an opportunity to move to its bound CPU
     * before continuing? Currently just do a __pmp_yield(). It is not
     * clear if this is necessary or sufficient. */
    __pmp_yield();
  }
}

static inline void __pmp_thread_work (pmp_thread_t *thread)
{
  pmp_team_t *team = thread->team;
  assert(team != NULL);

  __pmp_debug(PMP_DEBUG_THREAD, "thread global_id %d, local_id %d, "
              "team at %p has lots of work to do now\n", 
              thread->global_id, thread->local_id, team);

  /* NOTE: the id passed to the work function is the global_id. This is
   * passed back to certain library routines as the global_id parameter. 
   * It can be used to find the thread structure very quickly. */
  team->work(thread->global_id, team->fp);

  __pmp_debug(PMP_DEBUG_THREAD, "thread global_id %d, local_id %d, "
              "team at %p has no more work to do\n", 
              thread->global_id, thread->local_id, team);
}

#ifdef PMP_NO_NPTL
static void __pmp_shared_catch_segv (pmp_thread_t *thread)
{
  static int32_t installing_segv = 0;
  static int32_t installed_segv = 0;

  /* For Linuxthreads this only needs to be done once, since sigaction's are 
   * shared across all of the pthreads. I arrange for it to be set up by the
   * first worker thread that is woken up. This tranfers SEGV catching
   * responsibility from the serial code in libfoobar to libopenmp as
   * soon as parallelism is employed.
   */

  if (__pmp_atomic_cmpxchg32(&installing_segv, 0, 1) == 0) {
    __pmp_debug(PMP_DEBUG_THREAD, "thread global_id %d "
		"is installing the SEGV handler\n", thread->global_id);
    __pmp_catch_segv();
    __pmp_debug(PMP_DEBUG_THREAD, "thread global_id %d "
		"has installed the SEGV handler\n", thread->global_id);
    installed_segv = 1;
  }
  while (installed_segv == 0) {
    /* USER LEVEL SPIN LOCK */
    __pmp_yield();
  }
}
#endif

static void *__pmp_thread_run (void *arg)
{
  pmp_thread_t *thread = (pmp_thread_t *) arg;
  pmp_team_t *team;

  thread->pthread_id = pthread_self();
#ifdef PMP_NO_NPTL
  thread->tid = getpid();
#else
  thread->tid = gettid();
#endif

#ifndef PMP_NO_TLS
#ifdef PMP_TLS_THREAD
  __pmp_tls_current_thread = thread;
#endif
#ifdef PMP_TLS_LOCAL_ID
  __pmp_tls_current_local_id = thread->local_id;
#endif
#ifdef PMP_TLS_GLOBAL_ID
  __pmp_tls_current_global_id = thread->global_id;
#endif
#endif

#ifdef PMP_USE_PTHREAD_SIGNALS
  if (pthread_sigmask(SIG_BLOCK, &__pmp_manager.mask_block_sigpmp, 
                      NULL) != 0) {
    __pmp_fatal("unable to set thread-specific sigmask\n");
  }
#else
  if (sigprocmask(SIG_BLOCK, &__pmp_manager.mask_block_sigpmp, NULL) != 0) {
    __pmp_fatal("unable to set thread-specific sigmask\n");
  }
#endif

  if (pthread_setspecific(__pmp_manager.thread_key, (void *) thread) != 0) {
    __pmp_fatal("unable to set thread-specific data\n");
  }

  __pmp_debug(PMP_DEBUG_THREAD, "thread global_id %d is running\n",
              thread->global_id);

  /* Note that there is no synchronization between the creating thread and
   * the created thread until here. This is the point where the created
   * thread is assigned to do some work. The reason that this is sufficient
   * is because the __pmp_thread_wait/wake mechanism is "protected" by
   * the thread->sync value which has been pre-initialized. If the creator
   * gets to the wake first, then it will just swap in PMP_THREAD_UNBLOCKED
   * and its work is done. If it gets to the wake second, then the created
   * thread must have got there first and this guarantees that the other
   * thread fields will already be initialized by the created thread.
   *
   * With nested forking, there is the possibility that the creator thread
   * will be usurped by another forking thread (there is no lock between
   * creation of a thread and that thread being assigned to do work). This
   * works for the same reason as described above.
   */

  __pmp_thread_wait(thread);		/* wait for first assignment */

#ifdef PMP_NO_NPTL
  __pmp_shared_catch_segv(thread);	/* set up shared segv handler */
#else
  __pmp_catch_segv();			/* set up thread's segv handler */
#endif

  __pmp_thread_bind(thread);		/* bind to the assigned local_id */

  while (1) {

    __pmp_debug(PMP_DEBUG_THREAD,
                "__pmp_thread_run: thread tid=%d, pthread_id=0x%08x "
                "global_id=%d, local_id=%d\n",
                (int) thread->tid, (int) thread->pthread_id,
                (int) thread->global_id, (int) thread->local_id);

    team = thread->team;
    assert(team != NULL);
#ifndef PMP_NO_TLS
#ifdef PMP_TLS_LOCAL_ID
    __pmp_tls_current_local_id = thread->local_id;
#endif
#ifdef PMP_TLS_TEAM
    __pmp_tls_current_team = team;
#endif
#endif
    __pmp_memory_fence();
    __pmp_thread_work(thread);		/* do the work */
    __pmp_thread_worker_join(team);	/* wake up team master */
    __pmp_memory_fence();
    __pmp_thread_idle(thread);		/* thread is now idle */
    __pmp_thread_wait(thread);		/* wait for more work */
    __pmp_thread_bind(thread);		/* update binding */
  }

  /* Currently unreachable */

  __pmp_debug(PMP_DEBUG_THREAD, "thread global_id %d is exiting\n",
              thread->global_id);

  return NULL;
}

void __pmp_thread_init (pmp_thread_t *thread, pmp_global_id_t global_id)
{
  thread->creator = NULL;		/* not created yet */
  thread->param = &__pmp_param;		/* no local params yet */
  thread->guard_page = NULL;		/* no guard page yet */
  thread->global_id = global_id;	/* constant global id */
  thread->pthread_id = (pthread_t) -1;	/* not allocated */
  thread->tid = -1;			/* not allocated */
  thread->cpu = -1;			/* not bound to any CPU */
  thread->single_count = 0;		/* no single hits yet */
  thread->loop_count = 0;		/* no dynamic loops yet */
  thread->serialized_parallel = 0;	/* no serialized parallels yet */
  thread->nesting_depth = 0;		/* no nested parallels yet */

  thread->iteration = 0;		/* no dynamic loops yet */
  thread->ticket_number = 0;		/* no dynamic loops yet */
  thread->loop = NULL;			/* no dynamic loops yet */

  thread->sync = PMP_SYNC_IDLE;		/* no synchronization activity */
  thread->local_id = -1;		/* not assigned to a team */
  thread->team = NULL;			/* not assigned to a team */
}

void __pmp_thread_create_main (void)
{
  pmp_thread_t *thread = __pmp_get_main_thread();
  int global_id;
  global_id = __pmp_idstack_pop(&__pmp_manager.idstack);
  assert(global_id == 0);

  thread->pthread_id = pthread_self();
#ifdef PMP_NO_NPTL
  thread->tid = getpid();
#else
  thread->tid = gettid();
#endif
  thread->local_id = 0;

#ifndef PMP_NO_TLS
#ifdef PMP_TLS_THREAD
  __pmp_tls_current_thread = thread;
#endif
#ifdef PMP_TLS_LOCAL_ID
  __pmp_tls_current_local_id = 0;
#endif
#ifdef PMP_TLS_GLOBAL_ID
  __pmp_tls_current_global_id = thread->global_id;
#endif
#endif

#ifdef PMP_USE_PTHREAD_SIGNALS
  if (pthread_sigmask(SIG_BLOCK, &__pmp_manager.mask_block_sigpmp, 
                      NULL) != 0) {
    __pmp_fatal("unable to set thread-specific sigmask\n");
  }
#else
  if (sigprocmask(SIG_BLOCK, &__pmp_manager.mask_block_sigpmp, NULL) != 0) {
    __pmp_fatal("unable to set thread-specific sigmask\n");
  }
#endif

  if (pthread_setspecific(__pmp_manager.thread_key, (void *) thread) != 0) {
    __pmp_fatal("unable to set thread-specific data\n");
  }

  __pmp_thread_bind(thread);		/* early master bind */

  __pmp_debug(PMP_DEBUG_THREAD, "created main thread global_id %d\n",
              thread->global_id);

  __pmp_debug(PMP_DEBUG_THREAD,
              "__pmp_thread_create_main: tid=%d, pthread_id=0x%08x "
              "global_id=%d, local_id=%d\n",
              (int) thread->tid, (int) thread->pthread_id,
              (int) thread->global_id, (int) thread->local_id);
}

void __pmp_thread_create (pmp_thread_t *thread)
{
  pmp_thread_t *creator = __pmp_get_current_thread();
  pthread_t pthread_id;
  int result;
  pmp_param_t *param = __pmp_get_param();

  thread->creator = creator;

  if (param->thread_guard_size > 0) {
    void *guard;
    /* NOTE: this lock is to give a better chance of the guard page 
     * allocation to immediately follow the pthread stack allocation. */
    __pmp_lock(thread->global_id, &__pmp_manager.pthread_create_lock);

    /* NOTE: it seems that mmap tends to allocate in an upwards direction
       so allocate the guard page first. */
    guard = mmap(0, param->thread_guard_size, PROT_NONE,
#if defined(BUILD_OS_DARWIN)
                 MAP_PRIVATE | MAP_ANON,
#else /* defined(BUILD_OS_DARWIN) */
                 MAP_PRIVATE | MAP_ANONYMOUS,
#endif /* defined(BUILD_OS_DARWIN) */
		 0, 0);
    if (guard == MAP_FAILED) {
      __pmp_warning("unable to allocate a guard page of %ld bytes\n",
                    (long) param->thread_guard_size);
    }
    else {
      __pmp_debug(PMP_DEBUG_THREAD, "guard page allocated at address %p\n",
                  guard);
      thread->guard_page = guard;
    }
  }

  if ((result = pthread_create(&pthread_id, &__pmp_manager.pthread_attr,
			       __pmp_thread_run, thread)) != 0) {
    if (__pmp_manager.allocated_threads > param->initial_team_size) {
      __pmp_warning(
        "pthread_create failed when trying to allocate thread %d\n",
        __pmp_manager.allocated_threads);
      __pmp_warning(
        "note this is more than the initial number of threads (%d)\n",
        param->initial_team_size);
#if defined(BUILD_OS_DARWIN)
      if (sizeof(long) == 4)
#else /* defined(BUILD_OS_DARWIN) */
      if (__WORDSIZE == 32)
#endif /* defined(BUILD_OS_DARWIN) */
      {
	int64_t total_stack = ((int64_t) param->thread_stack_size) *
                              ((int64_t) __pmp_manager.allocated_threads);
        if (total_stack > 0x40000000LL) {
          __pmp_warning(
            "the failure may be due to excessive thread stack size\n");
          __pmp_warning(
            "try using a smaller setting for PSC_OMP_STACK_SIZE\n");
	}
      }
    }
    __pmp_fatal("unable to create thread (result code %d)\n", result);
  }

  if (param->thread_guard_size > 0) {
    __pmp_unlock(thread->global_id, &__pmp_manager.pthread_create_lock);
  }

  __pmp_atomic_xadd32(&__pmp_manager.waiting_threads, 1);

  __pmp_debug(PMP_DEBUG_THREAD, "created thread global_id %d\n", 
              thread->global_id);
}

int __pmp_thread_acquire (int nthreads)
{
  int count = 1;	/* count from 1 to ignore master thread */

  /* NOTE - in the typical case this while construct does not loop */

  while (count < nthreads) {
    int required = nthreads - count;
    int waiting = __pmp_atomic_xadd32(&__pmp_manager.waiting_threads,
                                      -required);
    if (waiting >= required) {
      count += required;
      break;
    }
    else {
      count += waiting;
      required -= waiting;
      __pmp_atomic_xadd32(&__pmp_manager.waiting_threads, required);
      if (__pmp_manager_create_more_threads(required) == 0) {
        break;
      }
    }
  }

  __pmp_atomic_add32(&__ompc_cur_numthreads, count - 1);

  __pmp_debug(PMP_DEBUG_THREAD, "acquired %d out of %d threads\n",
              count, nthreads);

  return count;
}

void __pmp_thread_assign (pmp_team_t *team, pmp_thread_t *master, int nthreads)
{
  pmp_idstack_t *idstack = &__pmp_manager.idstack;
  int i;

  __pmp_lock(master->global_id, &__pmp_manager.idlock);

  for (i = 1; i < nthreads; i++) {
    int global_id = __pmp_idstack_pop(idstack);
    pmp_thread_t *thread = __pmp_manager.threads + global_id;

    __pmp_debug(PMP_DEBUG_THREAD,
                "assigning thread global_id %d to local_id %d "
                "of team at %p\n", 
                thread->global_id, i, team);

    assert(thread->global_id == global_id);
    assert(thread->local_id == -1);
    assert(thread->team == NULL);

    thread->local_id = i;
    thread->team = team;
    team->members[i] = thread;

    __pmp_debug(PMP_DEBUG_THREAD,
                "assigned thread global_id %d to local_id %d "
                "of team at %p\n", 
                thread->global_id, thread->local_id, team);
  }

  __pmp_unlock(master->global_id, &__pmp_manager.idlock);

  for (i = 1; i < nthreads; i++) {
    /* This is pulled outside the idlock because waking a thread typically
     * involves an inter-CPU synchronization which is relatively expensive.
     */
    __pmp_thread_wake(team->members[i]);
  }
}

static void __pmp_thread_release (pmp_team_t *team, pmp_thread_t *master)
{
  pmp_local_id_t old_local_id;
  pmp_idstack_t *idstack = &__pmp_manager.idstack;
  int nworkers = team->team_size - 1;
  int i;

  __pmp_lock(master->global_id, &__pmp_manager.idlock);

  for (i = nworkers; i >= 1; i--) {
    pmp_thread_t *thread = team->members[i];

    assert(thread != master);
    old_local_id = thread->local_id;
    thread->local_id = -1;
    thread->team = NULL;

    __pmp_idstack_push(idstack, thread->global_id);

    __pmp_debug(PMP_DEBUG_THREAD,
                "released thread global_id %d from local_id %d "
                "of team at %p\n", 
                thread->global_id, old_local_id, team);
  }

  __pmp_unlock(master->global_id, &__pmp_manager.idlock);

  __pmp_atomic_add32(&__ompc_cur_numthreads, -nworkers);

  __pmp_atomic_xadd32(&__pmp_manager.waiting_threads, nworkers);
}

static inline void __pmp_thread_fork (pmp_thread_t *master, int nthreads,
                                      workfunc_t work, void *fp)
{
  pmp_team_t *old_team = master->team;
  pmp_local_id_t old_local_id = master->local_id;

  if (nthreads == 0) {
    nthreads = __pmp_get_new_team_size();
  }

  master->local_id = 0;
  master->single_count = 0;
  master->loop_count = 0;
  master->team = __pmp_team_alloc(master, nthreads, work, fp);

#ifndef PMP_NO_TLS
#ifdef PMP_TLS_LOCAL_ID
  __pmp_tls_current_local_id = 0;
#endif
#ifdef PMP_TLS_TEAM
  __pmp_tls_current_team = master->team;
#endif
#endif
 
  __pmp_thread_bind(master);		/* rebind to new local_id */
  __pmp_thread_work(master);
  __pmp_thread_master_join(master);

  __pmp_thread_release(master->team, master);

  __pmp_team_free(master, master->team);

  master->team = old_team;
  master->local_id = old_local_id;
  __pmp_thread_bind(master);		/* rebind to old local_id */

#ifndef PMP_NO_TLS
#ifdef PMP_TLS_LOCAL_ID
  __pmp_tls_current_local_id = old_local_id;
#endif
#ifdef PMP_TLS_TEAM
  __pmp_tls_current_team = master->team;
#endif
#endif
}

static inline void __pmp_thread_barrier (pmp_thread_t *thread)
{
  /* NOTE: the compiler optimizes away OMP barriers in the serial code
   * so there is no need to optimize that case here. The case of a team
   * with just one thread is not so common, so ideally don't optimize 
   * that path either. However, it is currently necessary to check
   * (team != NULL) so one might as well check for the 1-thread team too.
   * The most important case is the n-way barrier where n > 1. */
  pmp_team_t *team = thread->team;
  int team_size = __pmp_get_team_size(team);
  if (team_size > 1) {
    int32_t count = __pmp_atomic_xadd32(&team->barrier_count, -1);
    assert(count > 0);
    __pmp_debug(PMP_DEBUG_THREAD, "thread hits barrier with count of %d\n", 
                (int) count);
    if (count > 1) {
      __pmp_thread_wait(thread);
    }
    else {
      pmp_local_id_t local_id = thread->local_id;
      int i;
      team->barrier_count = team_size;
      for (i = 0; i < team_size; i++) {
        pmp_thread_t *t = team->members[i];
        if (i != local_id) {
          __pmp_thread_wake(t);
        }
      }
    }
  }
}

static inline int __pmp_thread_single (pmp_thread_t *thread)
{
  /* NOTE: the compiler optimizes away OMP singles in the serial code
   * so there is no need to optimize that case here. The case of a team
   * with just one thread is not so common, so don't optimize that path either.
   * The most important case is the n-way single where n > 1. However, it
   * is possible for a thread with no team to call this (e.g. orphaned
   * directive) so in that case one has to check to see if there is a team.
   * TODO: allocate a team of 1 thread in the orphaned case to reduce the
   * amount of special case code throughout the library looking for NULL
   * team pointers. */
  pmp_team_t *team = thread->team;
  if (__pmp_get_team_size(team) == 1) {
    return 1;
  }
  else {
    int32_t thread_count = thread->single_count;
    int32_t team_count = __pmp_atomic_cmpxchg32(&team->single_count,
                                                thread_count, 
                                                thread_count + 1);
    thread->single_count++;
    __pmp_debug(PMP_DEBUG_THREAD, "__pmp_thread_single, local_id %d "
                "thread_count=%d team_count=%d %s\n", 
                thread->local_id, thread_count, team_count, 
                (thread_count == team_count) ? "win" : "lose");
    return (thread_count == team_count);
  }
}

void __ompc_fork (int nthreads, workfunc_t work, void *fp)
{
  pmp_thread_t *master = __pmp_get_current_thread();
  __pmp_debug(PMP_DEBUG_CALLS, "__ompc_fork nthreads=%d, work=%p, fp=%p "
              " (nesting depth = %d)\n",
              nthreads, work, fp, master->nesting_depth);
  __pmp_sample(PMP_PROFILE_OMPC_FORK);
  __pmp_memory_fence();
  __pmp_thread_fork(master, nthreads, work, fp);
  __pmp_memory_fence();
  __pmp_debug(PMP_DEBUG_CALLS, "__ompc_fork completed"
              " (nesting depth = %d)\n",
              master->nesting_depth);
}

int __ompc_can_fork (void)
{
  int team_size = __pmp_get_new_team_size();
  int has_forked = (__pmp_get_main_thread()->nesting_depth > 0);
  pmp_param_t *param = __pmp_get_param();
  int serial_outline = param->serial_outline;
  int disabled = param->disabled;
  int can_fork = (team_size > 1 || has_forked || serial_outline) && !disabled;
  __pmp_debug(PMP_DEBUG_CALLS, "__ompc_can_fork returns %d\n", can_fork);
  __pmp_sample(PMP_PROFILE_OMPC_CAN_FORK);
  return can_fork;
}

int __ompc_in_parallel (void)
{
  pmp_thread_t *thread = __pmp_get_current_thread();
  int in_parallel = (thread->team != NULL) || 
                    (thread->serialized_parallel > 0);
  __pmp_debug(PMP_DEBUG_CALLS, "__ompc_in_parallel returns %d\n", in_parallel);
  __pmp_sample(PMP_PROFILE_OMPC_IN_PARALLEL);
  return in_parallel;
}

void __ompc_serialized_parallel (void)
{
  pmp_thread_t *thread = __pmp_get_current_thread();
  __pmp_debug(PMP_DEBUG_CALLS, "__ompc_serialized_parallel\n");
  __pmp_sample(PMP_PROFILE_OMPC_SERIALIZED_PARALLEL);
  thread->serialized_parallel++;
}

void __ompc_end_serialized_parallel (void)
{
  pmp_thread_t *thread = __pmp_get_current_thread();
  __pmp_debug(PMP_DEBUG_CALLS, "__ompc_end_serialized_parallel\n");
  __pmp_sample(PMP_PROFILE_OMPC_END_SERIALIZED_PARALLEL);
  assert(thread->serialized_parallel > 0);
  thread->serialized_parallel --;
}

void __ompc_barrier (void)
{
  pmp_thread_t *thread = __pmp_get_current_thread();
  __pmp_debug(PMP_DEBUG_CALLS, "__ompc_barrier\n");
  __pmp_sample(PMP_PROFILE_OMPC_BARRIER);
  __pmp_memory_fence();
  __pmp_thread_barrier(thread);
}

int __ompc_get_thread_num (void)
{
  int global_id = __pmp_get_current_global_id();
  __pmp_debug(PMP_DEBUG_CALLS, "__ompc_get_thread_num returns %d\n",global_id);
  __pmp_sample(PMP_PROFILE_OMPC_GET_THREAD_NUM);
  return global_id;
}

int __ompc_get_local_thread_num (void)
{
  int local_id = __pmp_get_current_local_id();
  assert(local_id != -1);
  __pmp_debug(PMP_DEBUG_CALLS, "__ompc_get_local_thread_num returns %d\n",
              local_id);
  __pmp_sample(PMP_PROFILE_OMPC_GET_LOCAL_THREAD_NUM);
  return local_id;
}

int __ompc_single (int global_id)
{
  int result = __pmp_thread_single(__pmp_get_thread(global_id));
  __pmp_debug(PMP_DEBUG_CALLS, "__ompc_single global_id=%d returns %d\n", 
              global_id, result);
  __pmp_sample(PMP_PROFILE_OMPC_SINGLE);
  return result;
}

void __ompc_end_single (int global_id)
{
  __pmp_debug(PMP_DEBUG_CALLS, "__ompc_end_single global_id=%d\n", global_id);
  __pmp_sample(PMP_PROFILE_OMPC_END_SINGLE);
  /* no work here, note that the compiler does call this function */
}


int __ompc_master (int global_id)
{
  int master = (__pmp_get_thread(global_id)->local_id == 0);
  __pmp_debug(PMP_DEBUG_CALLS, "__ompc_master global_id=%d returns %d\n",
              global_id, master);
  __pmp_sample(PMP_PROFILE_OMPC_MASTER);
  return master;
}

void __ompc_end_master (int global_id)
{
  __pmp_debug(PMP_DEBUG_CALLS, "__ompc_end_master global_id=%d\n", global_id);
  __pmp_sample(PMP_PROFILE_OMPC_END_MASTER);
  /* no work here, NOTE: does not appear to be called by compiler anyway */
}

void __ompc_get_thdprv (void ***thdprv, int64_t size,
                        void *data, int global_id)
{
  __pmp_debug(PMP_DEBUG_CALLS, "__ompc_get_thdprv: thdprv=%p, size=%ld, "
              "data=%p, global_id=%d\n", thdprv, (long) size, data, global_id);
  __pmp_sample(PMP_PROFILE_OMPC_GET_THDPRV);

  if (__pmp_get_param()->disabled) {
    void **t = (void **) calloc (1, sizeof(void *));
    if (t == NULL) {
      __pmp_fatal("failed to allocate thread private data\n");
    }
    t[0] = data;
    *thdprv = t;
  }
  else {
    void **t = *thdprv;
    if (t == NULL) {
      /* TODO: can I reduce the size of this array? Note that it is indexed
       * by global_id and global_id's can be arbitrarily assigned to threads
       * in general, so this may be difficult. */
      void *t_new;
      void *t_cur;
      t = (void **) calloc(PMP_MAX_THREADS, sizeof(void *));
      if (t == NULL) {
        __pmp_fatal("failed to allocate thread private data\n");
      }
      t_new = (void *) t;
      t_cur = __pmp_atomic_cmpxchgptr((volatile voidptr_t *) thdprv, 
                                      NULL, t_new);
      if (t_cur != NULL) {
        /* This thread lost the race and another thread has already
         * installed a thdprv array. Simply back out this allocation
         * and use *thdprv. */
        free(t);
        t = (void **) t_cur;
      }
    }
    if (t[global_id] == NULL) {
      /* The OpenMP 2.5 standard says:
       *
       * "Each copy of a threadprivate object is initialized once, in the manner
       * specified by the program, but at an unspecified point in the program
       * prior to the first reference to that copy."
       *
       * Since the initial values live in the statically allocated block of
       * memory passed to our "data" argument, the master thread needs to use
       * a dynamically allocated block, just as the additional threads do, so
       * that it if it changes its copies of the variables before the program
       * enters the first parallel region, those changes have no effect on the
       * copies in the additional threads. Observation shows that the code
       * generator calls __ompc_get_thdprv from the serial portion of the
       * program, for the master thread, before it changes any values.
       *
       * Note the copying is done without synchronization, which is safe only
       * because we're copying statically initialized and subsequently
       * unchanged values: copying from the main thread would require a
       * barrier.
       */
      t[global_id] = (void *) malloc(size);
      if (t[global_id] == NULL) {
        __pmp_fatal("failed to allocate thread private data");
      }
      memcpy(t[global_id], data, size);
    }
  }
}

void __ompc_copyin_thdprv (int n, ...)
{
  pmp_global_id_t global_id;

  __pmp_debug(PMP_DEBUG_CALLS, "__ompc_copyin_thdprv: n=%d\n", n);
  __pmp_sample(PMP_PROFILE_OMPC_COPYIN_THDPRV);

  if (__pmp_get_param()->disabled) {
    return;
  }

  global_id = __pmp_get_current_global_id();

  va_list ap;
  va_start(ap, n);
  while (n > 0) {
    void *dst = va_arg(ap, void*);
    void *src = va_arg(ap, void*);
    int size = va_arg(ap, int);
    if (dst != src) {
      __pmp_debug(PMP_DEBUG_THREAD, "__ompc_copyin_thdprv: global_id=%d "
                  "dst: %p, src: %p, size: %d\n", global_id, dst, src, size);
      memcpy(dst, src, size);
    }
    n -= 3;
  }
  va_end(ap);
}

void __ompc_copyprivate (int lock, void *src, void (*copy)(void*, void*))
{
  pmp_thread_t *thread = __pmp_get_current_thread();
  pmp_team_t *team = thread->team;

  __pmp_debug(PMP_DEBUG_CALLS,"__ompc_copyprivate: lock=%d, src=%p, copy=%p\n",
              lock, src, copy);
  __pmp_sample(PMP_PROFILE_OMPC_COPYPRIVATE);

  if (team != NULL && team->team_size > 1) {
    /* TODO: consider a different implementation without double barriers? */
    if (lock) {
      team->copyprivate_src = src;
    }
    __pmp_thread_barrier(thread);
    if (!lock) {
      copy(team->copyprivate_src, src);
    }
    __pmp_thread_barrier(thread);
  }
}

void __ompc_flush (void)
{
  __pmp_debug(PMP_DEBUG_CALLS, "__ompc_flush\n");
  __pmp_sample(PMP_PROFILE_OMPC_FLUSH);
  /* No work to do here as the architecture provides h/w SMP cache coherency */
}
