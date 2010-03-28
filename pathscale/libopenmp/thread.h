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

#ifndef __pmp_thread_h
#define __pmp_thread_h

#include "types.h"
#include <assert.h>
#include <signal.h>
#include <sys/types.h>

#define PMP_DEFAULT_STACK_SIZE (32*1024*1024)	/* for C/C++ not Fortran */

/* If the user specifies a stack, the hard max limit is applied to keep
 * the value in a reasonable range. If the library uses a default stack
 * size, a soft max limit (equal or less than the hard max) is applied.
 * This can be used to keep the default size in check, while giving
 * more freedom to the user. This is used on MIPS 32-bit to prevent
 * stack failures or warnings in the default case. */

#if __WORDSIZE == 64
#define PMP_STACK_HARD_MAX 0x100000000L /* 4GB should be enough for anybody */
#else
#define PMP_STACK_HARD_MAX 0x40000000L  /* and 1GB under 32-bit compilation */
#endif

#if defined __mips__ && __WORDSIZE == 32
#define PMP_STACK_SOFT_MAX 0x04000000L  /* 64MB soft max on 32-bit MIPS */
#else
#define PMP_STACK_SOFT_MAX PMP_STACK_HARD_MAX
#endif

#if __WORDSIZE == 64
#define PMP_DEFAULT_GUARD_SIZE (32*1024*1024)	/* 32MB guard on 64-bit */
#else
#define PMP_DEFAULT_GUARD_SIZE (0)		/*   no guard on 32-bit */
#endif

#if defined __mips__
#define PMP_THREAD_SPIN 10000
#else
#define PMP_THREAD_SPIN 100
#endif

/*
  Signal used to wake up threads, must
  not clash with other signals used by
  pthreads
*/
#if defined(BUILD_OS_DARWIN)
/* Darwin doesn't provide Posix Real Time signal extension */
#define SIGPMP SIGUSR1
#else /* defined(BUILD_OS_DARWIN) */
#define SIGPMP (SIGRTMIN+4)
#endif /* defined(BUILD_OS_DARWIN) */

extern void __pmp_thread_init (pmp_thread_t *thread,
                               pmp_global_id_t global_id);
extern void __pmp_thread_create_main (void);
extern void __pmp_thread_create (pmp_thread_t *thread);
extern int __pmp_thread_acquire (int nthreads);
extern void __pmp_thread_assign (pmp_team_t *team, pmp_thread_t *master,
                                 int nthreads);

#ifndef PMP_NO_TLS
#ifdef PMP_TLS_THREAD
extern __thread pmp_thread_t *__pmp_tls_current_thread;
#endif
#ifdef PMP_TLS_LOCAL_ID
extern __thread pmp_local_id_t __pmp_tls_current_local_id;
#endif
#ifdef PMP_TLS_GLOBAL_ID
extern __thread pmp_global_id_t __pmp_tls_current_global_id;
#endif
#ifdef PMP_TLS_TEAM
extern __thread pmp_team_t *__pmp_tls_current_team;
#endif
#endif

static inline pmp_thread_t *__pmp_get_current_thread (void)
{
#if (defined PMP_NO_TLS) || (!defined PMP_TLS_THREAD)
  pmp_thread_t *result;
  if (pthread_self() == __pmp_manager.threads[0].pthread_id) {
    result = &__pmp_manager.threads[0];
  }
  else {
    result = (pmp_thread_t *) pthread_getspecific(__pmp_manager.thread_key);
  }
  assert(result != NULL);
  return result;
#else
  return __pmp_tls_current_thread;
#endif
}

static inline pmp_thread_t *__pmp_get_main_thread (void)
{
  return &__pmp_manager.threads[0];
}

static inline pmp_thread_t *__pmp_get_thread (pmp_global_id_t i)
{
  assert(i < __pmp_manager.allocated_threads);
  return &__pmp_manager.threads[i];
}

static inline pmp_local_id_t __pmp_get_current_local_id (void)
{
#if (defined PMP_NO_TLS) || (!defined PMP_TLS_LOCAL_ID)
  return __pmp_get_current_thread()->local_id;
#else
  return __pmp_tls_current_local_id;
#endif
}

static inline pmp_global_id_t __pmp_get_current_global_id (void)
{
#if (defined PMP_NO_TLS) || (!defined PMP_TLS_GLOBAL_ID)
  return __pmp_get_current_thread()->global_id;
#else
  return __pmp_tls_current_global_id;
#endif
}

static inline int __pmp_in_parallel (void)
{
  pmp_thread_t *thread = __pmp_get_current_thread();
  return (thread->team != NULL) || (thread->serialized_parallel > 0);
}

static inline pmp_param_t *__pmp_get_param (void)
{
  pmp_param_t *param = __pmp_get_current_thread()->param;
  assert(param != NULL);
  return param;
}

static inline void __pmp_yield (void)
{
#ifndef __mips__
  sched_yield();
#endif
}

#endif
