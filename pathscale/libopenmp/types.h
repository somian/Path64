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

#ifndef __pmp_types_h
#define __pmp_types_h

#include <signal.h>
#include <stdint.h>
#include <pthread.h>
#include <sys/types.h>

#ifdef SLES8
#define PMP_NO_NPTL
/* #define PMP_NO_TLS */	/* MD: TEMPORARY HACK DUE TO BUG 5441 */
#endif

#define PMP_USE_PTHREAD_SIGNALS	/* MD: this is required for linuxthreads
				 * compatibility. Note that linuxthreads is
				 * used on 2.6 kernels when libpthreads is
                                 * statically linked into the application. */

#define PMP_NO_TLS		/* MD: TEMPORARY HACK DUE TO BUG 5441 */

#ifndef PMP_NO_TLS
#define PMP_TLS_THREAD
#define PMP_TLS_LOCAL_ID
#define PMP_TLS_GLOBAL_ID
#define PMP_TLS_TEAM
#endif

#define PMP_MAX_THREADS 256
#define PMP_MAX_CPUS 256
#define PMP_MAX_IDS PMP_MAX_THREADS

#if defined __mips__
#define PMP_PARAM_CACHE 1
#else
#define PMP_PARAM_CACHE 0
#endif

typedef int bool;
#define false 0
#define true 1

typedef int pmp_global_id_t;
typedef int pmp_local_id_t;
typedef int pmp_loop_id_t;

typedef int32_t pmp_thread_status_t;
#define PMP_THREAD_UNALLOCATED 0
#define PMP_THREAD_WAITING 1
#define PMP_THREAD_WORKING 2

typedef int32_t pmp_sync_status_t;
#define PMP_SYNC_IDLE 0
#define PMP_SYNC_BLOCKED 1
#define PMP_SYNC_UNBLOCKED 2

#if __WORDSIZE == 64
typedef int64_t pmp_profile_count_t;
#else
typedef int32_t pmp_profile_count_t;
#endif

typedef int32_t pmp_profile_index_t;
#define PMP_PROFILE_OMPC_FORK 0
#define PMP_PROFILE_OMPC_CAN_FORK 1
#define PMP_PROFILE_OMPC_IN_PARALLEL 2
#define PMP_PROFILE_OMPC_GET_THREAD_NUM 3
#define PMP_PROFILE_OMPC_GET_LOCAL_THREAD_NUM 4
#define PMP_PROFILE_OMPC_CRITICAL 5
#define PMP_PROFILE_OMPC_END_CRITICAL 6
#define PMP_PROFILE_OMPC_STATIC_INIT_8 7
#define PMP_PROFILE_OMPC_STATIC_INIT_4 8
#define PMP_PROFILE_OMPC_STATIC_FINI 9
#define PMP_PROFILE_OMPC_SCHEDULER_INIT_8 10
#define PMP_PROFILE_OMPC_SCHEDULE_NEXT_8 11
#define PMP_PROFILE_OMPC_SCHEDULER_INIT_4 12
#define PMP_PROFILE_OMPC_SCHEDULE_NEXT_4 13
#define PMP_PROFILE_OMPC_ORDERED 14
#define PMP_PROFILE_OMPC_END_ORDERED 15
#define PMP_PROFILE_OMPC_MASTER 16
#define PMP_PROFILE_OMPC_END_MASTER 17
#define PMP_PROFILE_OMPC_SERIALIZED_PARALLEL 18
#define PMP_PROFILE_OMPC_END_SERIALIZED_PARALLEL 19
#define PMP_PROFILE_OMPC_BARRIER 20
#define PMP_PROFILE_OMPC_SINGLE 21
#define PMP_PROFILE_OMPC_END_SINGLE 22
#define PMP_PROFILE_OMPC_GET_THDPRV 23
#define PMP_PROFILE_OMPC_COPYIN_THDPRV 24
#define PMP_PROFILE_OMPC_COPYPRIVATE 25
#define PMP_PROFILE_OMPC_FLUSH 26
#define PMP_PROFILE_OMP_SET_NUM_THREADS 27
#define PMP_PROFILE_OMP_GET_NUM_THREADS 28
#define PMP_PROFILE_OMP_GET_MAX_THREADS 29
#define PMP_PROFILE_OMP_GET_THREAD_NUM 30
#define PMP_PROFILE_OMP_GET_NUM_PROCS 31
#define PMP_PROFILE_OMP_IN_PARALLEL 32
#define PMP_PROFILE_OMP_SET_DYNAMIC 33
#define PMP_PROFILE_OMP_GET_DYNAMIC 34
#define PMP_PROFILE_OMP_SET_NESTED 35
#define PMP_PROFILE_OMP_GET_NESTED 36
#define PMP_PROFILE_OMP_GET_WTIME 37
#define PMP_PROFILE_OMP_GET_WTICK 38
#define PMP_PROFILE_OMP_INIT_LOCK 39
#define PMP_PROFILE_OMP_DESTROY_LOCK 40
#define PMP_PROFILE_OMP_SET_LOCK 41
#define PMP_PROFILE_OMP_UNSET_LOCK 42
#define PMP_PROFILE_OMP_TEST_LOCK 43
#define PMP_PROFILE_OMP_INIT_NEST_LOCK 44
#define PMP_PROFILE_OMP_DESTROY_NEST_LOCK 45
#define PMP_PROFILE_OMP_SET_NEST_LOCK 46
#define PMP_PROFILE_OMP_UNSET_NEST_LOCK 47
#define PMP_PROFILE_OMP_TEST_NEST_LOCK 48
#define PMP_PROFILE_UNIMPLEMENTED 49
#define PMP_PROFILE_THREAD_DESCHEDULE 50
#define PMP_PROFILE_THREAD_RESCHEDULE 51
#define PMP_PROFILE_SCHED_STATIC 52
#define PMP_PROFILE_SCHED_STATIC_EVEN 53
#define PMP_PROFILE_SCHED_DYNAMIC 54
#define PMP_PROFILE_SCHED_GUIDED 55
#define PMP_PROFILE_SCHED_ORDERED_STATIC 56
#define PMP_PROFILE_SCHED_ORDERED_STATIC_EVEN 57
#define PMP_PROFILE_SCHED_ORDERED_DYNAMIC 58
#define PMP_PROFILE_SCHED_ORDERED_GUIDED 59
#define PMP_PROFILE_NUM 60

#define PMP_L2_ALIGNED 64

/* workfunc_t is the entry-point interface for a new OpenMP thread */
typedef void (*workfunc_t)(int global_id, void *fp);

/* forward definitions */
typedef struct pmp_lock_t pmp_lock_t;
typedef struct pmp_nest_lock_t pmp_nest_lock_t;
typedef struct pmp_idstack_t pmp_idstack_t;
typedef struct pmp_param_t pmp_param_t;
typedef struct pmp_loop_t pmp_loop_t;
typedef struct pmp_thread_t pmp_thread_t;
typedef struct pmp_team_t pmp_team_t;
typedef struct pmp_manager_t pmp_manager_t;
typedef struct pmp_profile_t pmp_profile_t;

struct pmp_lock_t
{
  volatile int32_t sync;
  pthread_mutex_t mutex;
} __attribute__((aligned(PMP_L2_ALIGNED)));

struct pmp_nest_lock_t
{
  volatile int32_t sync;
  volatile int32_t nest;
  pthread_mutex_t mutex;
} __attribute__((aligned(PMP_L2_ALIGNED)));

struct pmp_idstack_t
{
  int num_ids;
  int stack[PMP_MAX_IDS];
} __attribute__((aligned(PMP_L2_ALIGNED)));

struct pmp_param_t
{
  /* all of these uint8_t fields are boolean flags (packed to save space) */
  uint8_t initialized;
  uint8_t check;
  uint8_t disabled;
  uint8_t dynamic_threads;
  uint8_t enable_affinity;
  uint8_t global_affinity;
  uint8_t nested_forks;		// can be modified by omp_set_nested
  uint8_t serial_outline;
  uint8_t silent;
  uint8_t static_fair;

  int debug_type;
  int default_team_size;	// can be modified by omp_set_num_threads
  int initial_team_size;
  int max_threads;
  int machine_num_cpus;
  int openmp_num_cpus;
  int runtime_chunk;
  int runtime_schedule;
  int thread_spin;
  int lock_spin;
  int guided_chunk_divisor;
  int cpu_stride;
  int cpu_offset;

  double wtick;
  int64_t guided_chunk_max;
  const char *omp_stack_size;
  size_t thread_stack_size;
  size_t thread_guard_size;
  int *openmp_to_machine_cpu_map;
  int *thread_to_cpu_map;
} __attribute__((aligned(PMP_L2_ALIGNED)));

struct pmp_loop_t
{
  /* NOTE: thread-local and contentious fields are in separate cache lines */

  int sched __attribute__((aligned(PMP_L2_ALIGNED)));
  int32_t check_last;
  int64_t lower;
  int64_t upper;
  int64_t inc;
  int64_t min_chunk;
  int32_t *check_loop;
  volatile int64_t chunk;

  volatile pmp_loop_id_t loop_id __attribute__((aligned(PMP_L2_ALIGNED)));
  volatile int looping_threads;
  volatile int64_t now_serving;
  volatile int64_t current;
};

struct pmp_team_t
{
  /* NOTE: thread-local and contentious fields are in separate cache lines */

  int team_size __attribute__((aligned(PMP_L2_ALIGNED)));
  pmp_thread_t **members;	/* master is members[0] */
  pmp_loop_t *loops;
  workfunc_t work;
  void *fp;
  void *copyprivate_src;

  volatile int32_t working_threads __attribute__((aligned(PMP_L2_ALIGNED)));
  volatile int32_t barrier_count;
  volatile int32_t single_count;
  volatile int32_t loop_count;
};

struct pmp_thread_t
{
  /* NOTE: thread-local and contentious fields are in separate cache lines */

  pmp_thread_t *creator __attribute__((aligned(PMP_L2_ALIGNED)));
  pmp_param_t *param;
  void *guard_page;
  pmp_global_id_t global_id;
  pthread_t pthread_id;
  pid_t tid;
  int cpu;
  int32_t single_count;
  int32_t loop_count;
  int serialized_parallel;
  int nesting_depth;

  int64_t iteration __attribute__((aligned(PMP_L2_ALIGNED)));;
  int64_t ticket_number;
  pmp_loop_t *loop;

  pmp_local_id_t local_id __attribute__((aligned(PMP_L2_ALIGNED)));
  pmp_team_t *team;

  volatile pmp_sync_status_t sync __attribute__((aligned(PMP_L2_ALIGNED)));
};

struct pmp_manager_t
{
  /* NOTE: thread-local and contentious fields are in separate cache lines */

  bool initialized __attribute__((aligned(PMP_L2_ALIGNED)));
  pthread_key_t thread_key;
  pmp_team_t *teams;		/* indexed by team number */
  pmp_thread_t *threads;	/* indexed by thread number */
  pmp_param_t *params;		/* indexed by CPU number */

  volatile int32_t waiting_threads __attribute__((aligned(PMP_L2_ALIGNED)));
  volatile int32_t allocated_threads;

  pmp_lock_t pthread_create_lock;
  pmp_lock_t idlock;
  pmp_idstack_t idstack;

  pthread_attr_t pthread_attr;
  sigset_t mask_original;
  sigset_t mask_block_sigpmp;
  sigset_t mask_unblock_sigpmp;
};

struct pmp_profile_t
{
  bool enabled;
  volatile pmp_profile_count_t sample_count[PMP_PROFILE_NUM];
  volatile pmp_profile_count_t iteration_count[PMP_MAX_THREADS];
} __attribute__((aligned(PMP_L2_ALIGNED)));

extern pmp_param_t __pmp_param;
extern pmp_manager_t __pmp_manager;
extern pmp_profile_t __pmp_profile;

#endif
