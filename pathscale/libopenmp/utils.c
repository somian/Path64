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

#include "affinity.h"
#include "lock.h"
#include "manager.h"
#include "schedule.h"
#include "stack.h"
#include "thread.h"
#include "types.h"
#include "utils.h"
#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>

static const char *__pmp_getenv_prefix = "PSC_";
#define PMP_GETENV_ALLOW_NO_PREFIX 1
#define PMP_GETENV_WARN_NO_PREFIX  1

pmp_param_t __pmp_param = { false };
pmp_profile_t __pmp_profile = { false };
pmp_lock_t __pmp_io_lock;

volatile int __ompc_sug_numthreads;
volatile int __ompc_cur_numthreads;
volatile int __ompc_max_numthreads;

/* It is sometimes useful to redirect these for testing or debugging */
#define PMP_FILE_DEBUG		stderr
#define PMP_FILE_WARNING	stderr
#define PMP_FILE_FATAL		stderr
#define PMP_FILE_PROFILE	stderr
#define PMP_FILE_FLUSH		0

static const char *__pmp_profile_string[] = {
  "__ompc_fork",
  "__ompc_can_fork",
  "__ompc_in_parallel",
  "__ompc_get_thread_num",
  "__ompc_get_local_thread_num",
  "__ompc_critical",
  "__ompc_end_critical",
  "__ompc_static_init_8",
  "__ompc_static_init_4",
  "__ompc_static_fini",
  "__ompc_scheduler_init_8",
  "__ompc_schedule_next_8",
  "__ompc_scheduler_init_4",
  "__ompc_schedule_next_4",
  "__ompc_ordered",
  "__ompc_end_ordered",
  "__ompc_master",
  "__ompc_end_master",
  "__ompc_serialized_parallel",
  "__ompc_end_serialized_parallel",
  "__ompc_barrier",
  "__ompc_single",
  "__ompc_end_single",
  "__ompc_get_thdprv",
  "__ompc_copyin_thdprv",
  "__ompc_copyprivate",
  "__ompc_flush",
  "omp_set_num_threads",
  "omp_get_num_threads",
  "omp_get_max_threads",
  "omp_get_thread_num",
  "omp_get_num_procs",
  "omp_in_parallel",
  "omp_set_dynamic",
  "omp_get_dynamic",
  "omp_set_nested",
  "omp_get_nested",
  "omp_get_wtime",
  "omp_get_wtick",
  "omp_init_lock",
  "omp_destroy_lock",
  "omp_set_lock",
  "omp_unset_lock",
  "omp_test_lock",
  "omp_init_nest_lock",
  "omp_destroy_nest_lock",
  "omp_set_nest_lock",
  "omp_unset_nest_lock",
  "omp_test_nest_lock",
  "unimplemented",
  "deschedule",
  "reschedule",
  "sched_static",
  "sched_static_even",
  "sched_dynamic",
  "sched_guided",
  "sched_ordered_static",
  "sched_ordered_static_even",
  "sched_ordered_dynamic",
  "sched_ordered_guided"
};

/* __stack_size_divisor is a back-door mechanism to allow the OpenMP
 * library to influence the stack sizing of the initial thread of a FORTRAN
 * program. The __set_stack_size_limit routine in libfoobar/stacksize.c 
 * defines this variable with global scope as 1 (at load-time). If the OpenMP 
 * library is present then libopenmp/utils.c will change this value to 
 * the number of CPUs, and this happens at static constructor
 * time. Subsequently, __set_stack_size_limit is called by main and divides 
 * the auto stack size heuristic by this value. Thus, the stack size is 
 * unchanged for serial programs (with no OpenMP library) and divided by 
 * the number of CPUs for parallel programs (with OpenMP library).
 * Note that __stack_size_divisor is defined here as a weak symbol in
 * case libfoobar is not present, such as for an OpenMP C/C++ program.
 * If this weak symbol is used then its value of 0 can be used to detect
 * the absence of a FORTRAN MAIN__ entry-point. In that case, the code
 * for automatically sizing the FORTRAN stack is not used under the
 * assumption that this is not a FORTRAN code - this is the same assumption
 * as in the serial case.
 */

int __stack_size_divisor __attribute__((weak)) = 0;

static inline void __pmp_io_set_lock (void)
{
  if (__pmp_manager.initialized) {
    __pmp_lock(__pmp_get_current_global_id(), &__pmp_io_lock);
  }
}

static inline void __pmp_io_unset_lock (void)
{
  if (__pmp_manager.initialized) {
    __pmp_unlock(__pmp_get_current_global_id(), &__pmp_io_lock);
  }
}

#ifdef PMP_DEBUG
void __pmp_debug (int debug_type, const char *format, ...)
{
  /* Always uses uncached param to avoid initalization ordering problems */
  pmp_param_t *param = &__pmp_param;
  if (!param->silent &&
      ((param->debug_type == PMP_DEBUG_ALL) ||
       (param->debug_type & debug_type) != 0)) {
    pmp_thread_t *t = __pmp_manager.initialized ? __pmp_get_current_thread() :
                                                  NULL;
    va_list arg;
    va_start(arg, format);
    __pmp_io_set_lock();
    fprintf(PMP_FILE_DEBUG, "** OpenMP debug, type %d, thread %d: ", 
            debug_type, (t == NULL) ? 0 : (int) t->global_id);
    vfprintf(PMP_FILE_DEBUG, format, arg);
    if (PMP_FILE_FLUSH) {
      fflush(PMP_FILE_DEBUG);
    }
    __pmp_io_unset_lock();
    va_end(arg);
  }
}
#endif

void __pmp_warning (const char *format, ...)
{
  /* Always uses uncached param to avoid initalization ordering problems */
  pmp_param_t *param = &__pmp_param;
  if (!param->silent) {
    va_list arg;
    va_start(arg, format);
    __pmp_io_set_lock();
    fprintf(PMP_FILE_WARNING, "** OpenMP warning: ");
    vfprintf(PMP_FILE_WARNING, format, arg);
    if (PMP_FILE_FLUSH) {
      fflush(PMP_FILE_WARNING);
    }
    __pmp_io_unset_lock();
    va_end(arg);
  }
}

void __pmp_fatal (const char *format, ...)
{
  va_list arg;
  va_start(arg, format);
  __pmp_io_set_lock();
  fprintf(PMP_FILE_FATAL, "** OpenMP fatal error: ");
  vfprintf(PMP_FILE_FATAL, format, arg);
  fprintf(PMP_FILE_FATAL, "** OpenMP run aborted due to fatal error\n");
    if (PMP_FILE_FLUSH) {
      fflush(PMP_FILE_FATAL);
    }
  __pmp_io_unset_lock();
  va_end(arg);
  exit(EXIT_FAILURE);
}

static void __pmp_profile_init (void)
{
  assert(sizeof(__pmp_profile_string)/sizeof(const char *) == PMP_PROFILE_NUM);
  memset(&__pmp_profile, 0, sizeof(__pmp_profile));
}

void __pmp_profile_enable (void)
{
  __pmp_profile.enabled = true;
}

void __pmp_profile_disable (void)
{
  __pmp_profile.enabled = false;
}

void __pmp_profile_dump (void)
{
  volatile pmp_profile_count_t *sc = __pmp_profile.sample_count;
  volatile pmp_profile_count_t *ic = __pmp_profile.iteration_count;
  int i;

  __pmp_io_set_lock();
  for (i = 0; i < PMP_PROFILE_NUM; i++) {
    if (sc[i] != 0) {
      const char *s = __pmp_profile_string[i];
      fprintf(PMP_FILE_PROFILE, "** OpenMP profile: %s = %ld\n", 
              s, (long) sc[i]);
    }
  }
  for (i = 0; i < PMP_MAX_THREADS; i++) {
    if (ic[i] != 0) {
      fprintf(PMP_FILE_PROFILE,
              "** OpenMP profile: global_id = %d, iterations = %ld\n",
              i, (long) ic[i]);
    }
  }
  if (PMP_FILE_FLUSH) {
    fflush(PMP_FILE_PROFILE);
  }
  __pmp_io_unset_lock();
}

void __pmp_profile_reset (void)
{
  bool enabled = __pmp_profile.enabled;
  memset(&__pmp_profile, 0, sizeof(__pmp_profile));
  __pmp_profile.enabled = enabled;
}

static char *__pmp_getenv (const char *name)
{
  char *fullname = (char *) malloc(strlen(name) + 
                                   strlen(__pmp_getenv_prefix) + 1);
  char *result;
  if (fullname == NULL) {
    __pmp_fatal("out of heap\n");
  }
  strcpy(fullname, __pmp_getenv_prefix);
  strcat(fullname, name);
  result = getenv(fullname);
  if (result == NULL && PMP_GETENV_ALLOW_NO_PREFIX) {
    result = getenv(name);
    if (result != NULL && PMP_GETENV_WARN_NO_PREFIX) {
      __pmp_warning("%s is deprecated, please use %s\n", name, fullname);
    } 
  }
  free(fullname);
  return result;
}

static void __pmp_param_init (void)
{
  memset(&__pmp_param, 0, sizeof(__pmp_param));

  __pmp_param.check = false;
  __pmp_param.disabled = false;
  __pmp_param.dynamic_threads = false;
  __pmp_param.enable_affinity = true;
  __pmp_param.global_affinity = true;
  __pmp_param.nested_forks = false;
  __pmp_param.serial_outline = false;
  __pmp_param.silent = false;
  __pmp_param.static_fair = false;

  __pmp_param.debug_type = PMP_DEBUG_NONE;
  __pmp_param.default_team_size = 1;
  __pmp_param.initial_team_size = 1;
  __pmp_param.max_threads = PMP_MAX_THREADS;
  __pmp_param.machine_num_cpus = 1;
  __pmp_param.openmp_num_cpus = 1;
  __pmp_param.runtime_chunk = 1;
  __pmp_param.runtime_schedule = PMP_SCHED_STATIC_EVEN;
  __pmp_param.thread_spin = PMP_THREAD_SPIN;
  __pmp_param.lock_spin = PMP_LOCK_SPIN;
  __pmp_param.guided_chunk_divisor = PMP_GUIDED_CHUNK_DIVISOR;
  __pmp_param.cpu_stride = 0;
  __pmp_param.cpu_offset = 0;

  __pmp_param.wtick = 0.0;
  __pmp_param.guided_chunk_max = PMP_GUIDED_CHUNK_MAX;
  __pmp_param.omp_stack_size = NULL;
  __pmp_param.thread_stack_size = PMP_DEFAULT_STACK_SIZE;
  __pmp_param.thread_guard_size = PMP_DEFAULT_GUARD_SIZE;
  __pmp_param.openmp_to_machine_cpu_map = NULL;
  __pmp_param.thread_to_cpu_map = NULL;

  __pmp_param.initialized = true;

  __ompc_sug_numthreads = 1;
  __ompc_cur_numthreads = 1;
  __ompc_max_numthreads = 1;
}

static void __pmp_init_wtick (void)
{
  int64_t diff = LONG_LONG_MAX;
  int64_t last;
  int countdown;
  struct timeval time;
  gettimeofday(&time, NULL);
  last = (time.tv_sec * 1000000) + time.tv_usec;
  countdown = 10;
  while (countdown > 0) {
    int64_t now;
    do {
      gettimeofday(&time, NULL);
      now = (time.tv_sec * 1000000) + time.tv_usec;
    } while (now <= last);
    if (now - last < diff) {
      diff = now - last;
      countdown = 10;		/* restart the countdown */
    }
    else {
      countdown--;
    }
  }
  __pmp_param.wtick = ((double) diff) / 1000000.0;
}

static void __pmp_init_silent (void)
{
  if (__pmp_getenv("OMP_SILENT") != NULL) {
    __pmp_param.silent = true;
  }
}

static void __pmp_init_check (void)
{
  if (__pmp_getenv("OMP_CHECK") != NULL) {
#ifdef PMP_CHECK
    __pmp_param.check = true;
#else
    __pmp_warning("checking is not supported by this library\n");
#endif
  }
}

static void __pmp_init_debug (void)
{
  char *env;
  if ((env = __pmp_getenv("OMP_DEBUG")) != NULL) {
#ifdef PMP_DEBUG
    if (*env == 0) {
      __pmp_param.debug_type = PMP_DEBUG_ALL;
    }
    else {
      __pmp_param.debug_type = strtol(env, NULL, 0);
    }
#else
    __pmp_warning("debug is not supported by this library\n");
#endif
  }
}

static void __pmp_init_cpus (void)
{
  cpu_set_t cpus;
  int *openmp_to_machine_cpu_map;
  int i;
  int use_all_cpus = 0;
  char *env;
  void *result;

  if ((env = __pmp_getenv("OMP_AFFINITY_INHERITANCE")) != NULL) {
    if (strcasecmp(env, "true") == 0) {
      use_all_cpus = 0;
      __pmp_debug(PMP_DEBUG_INFO, "affinity inheritance is enabled\n");
    }
    else if (strcasecmp(env, "false") == 0) {
      use_all_cpus = 1;
      __pmp_debug(PMP_DEBUG_INFO, "affinity inheritance is disabled\n");
    }
    else {
      __pmp_warning("ignoring invalid setting for "
                    "PSC_OMP_AFFINITY_INHERITANCE\n");
    }
  }

  if (posix_memalign(&result, PMP_L2_ALIGNED,
                     PMP_MAX_CPUS * sizeof(int)) != 0) {
    __pmp_fatal("failed to allocate CPU mapping array\n");
  }
  openmp_to_machine_cpu_map = (int *) result;
  for (i = 0; i < PMP_MAX_CPUS; i++) {
    openmp_to_machine_cpu_map[i] = -1;
  }
  __pmp_param.openmp_to_machine_cpu_map = openmp_to_machine_cpu_map;

#if defined(BUILD_OS_DARWIN)
  __pmp_param.machine_num_cpus =
    MAX(1, get_sysctl_int(SYSCTL_NPROCESSORS_ONLN));
#else /* defined(BUILD_OS_DARWIN) */
  __pmp_param.machine_num_cpus = MAX(1, sysconf(_SC_NPROCESSORS_ONLN));
#endif /* defined(BUILD_OS_DARWIN) */

  if (use_all_cpus) {
    ;
  }
  else if (__pmp_get_affinity(&cpus) == 0) {
    int openmp_cpu = 0;
    for (i = 0; i < __pmp_param.machine_num_cpus; i++) {
      if (CPU_ISSET(i, &cpus)) {
        openmp_to_machine_cpu_map[openmp_cpu] = i;
        openmp_cpu++;
      }
    }
    if (openmp_cpu == 0) {
      __pmp_warning("current affinity has no CPUs, default to all CPUs\n");
      use_all_cpus = 1;
    }
    else {
      __pmp_param.openmp_num_cpus = openmp_cpu;
    }
  }
  else {
    __pmp_warning("failed to get current affinity, default to all CPUs\n");
    use_all_cpus = 1;
  }

  if (use_all_cpus) {
    __pmp_param.openmp_num_cpus = __pmp_param.machine_num_cpus;
    for (i = 0; i < __pmp_param.openmp_num_cpus; i++) {
      openmp_to_machine_cpu_map[i] = i;
    }
  }

  for (i = 0; i < __pmp_param.openmp_num_cpus; i++) {
    __pmp_debug(PMP_DEBUG_INFO, "OpenMP CPU %d maps to machine CPU %d\n",
                i, openmp_to_machine_cpu_map[i]);
  }
}

static void __pmp_init_disabled (void)
{
  if (__pmp_getenv("OMP_DISABLED") != NULL) {
    __pmp_warning("OpenMP library is disabled\n");
    __pmp_param.disabled = true;
  }
}

static void __pmp_init_default_team_size (void)
{
  char *env;
  if (!__pmp_param.disabled) {
    __pmp_param.default_team_size = __pmp_param.openmp_num_cpus;
    if ((env = getenv("OMP_NUM_THREADS")) != NULL) {
      __pmp_param.default_team_size = atoi(env);
    }
    if (__pmp_param.default_team_size < 1) {
      __pmp_warning("OMP_NUM_THREADS must be at least 1\n");
      __pmp_param.default_team_size = 1;
    }
    if (__pmp_param.default_team_size > PMP_MAX_THREADS) {
      __pmp_warning("OMP_NUM_THREADS is limited to a maximum of %d threads\n",
                    PMP_MAX_THREADS);
      __pmp_param.default_team_size = PMP_MAX_THREADS;
    }
    __pmp_debug(PMP_DEBUG_INFO,
                "total number of CPU cores in machine is %d\n",
                __pmp_param.machine_num_cpus);
    __pmp_debug(PMP_DEBUG_INFO,
                "total number of CPU cores for OpenMP is %d\n",
                __pmp_param.openmp_num_cpus);
    __pmp_debug(PMP_DEBUG_INFO,
                "default team size is %d thread(s)\n",
                __pmp_param.default_team_size);

    __pmp_param.initial_team_size = __pmp_param.default_team_size;
    __ompc_sug_numthreads = __pmp_param.default_team_size;
    __ompc_max_numthreads = __pmp_param.default_team_size;
  }
}

static void __pmp_init_dynamic (void)
{
  char *env;
  if ((env = getenv("OMP_DYNAMIC")) != NULL) {
    if (strcasecmp(env, "true") == 0) {
      __pmp_param.dynamic_threads = false;
      __pmp_warning("dynamic thread adjustment not available "
                    "(ignored OMP_DYNAMIC)\n");
    }
    else if (strcasecmp(env, "false") == 0) {
      __pmp_param.dynamic_threads = false;
      __pmp_debug(PMP_DEBUG_INFO, "dynamic thread extension is disabled\n");
    }
    else {
      __pmp_warning("ignoring invalid setting for OMP_DYNAMIC\n");
    }
  }
}

static void __pmp_init_enable_affinity (void)
{
  char *env;
  if (__pmp_param.default_team_size == 1) {
    __pmp_debug(PMP_DEBUG_INFO, 
                "OpenMP affinity is disabled by default for 1 thread\n");
    __pmp_param.enable_affinity = false;
  }
  if ((env = __pmp_getenv("OMP_AFFINITY")) != NULL) {
    if (strcasecmp(env, "true") == 0) {
      __pmp_param.enable_affinity = true;
      __pmp_debug(PMP_DEBUG_INFO,
                  "OpenMP affinity is enabled by PSC_OMP_AFFINITY\n");
    }
    else if (strcasecmp(env, "false") == 0) {
      __pmp_param.enable_affinity = false;
      __pmp_debug(PMP_DEBUG_INFO,
                  "OpenMP affinity is disabled by PSC_OMP_AFFINITY\n");
    }
    else {
      __pmp_warning("ignoring invalid setting for PSC_OMP_AFFINITY\n");
    }
  }
  if (__pmp_param.enable_affinity) {
    if ((env = __pmp_getenv("OMP_AFFINITY_GLOBAL")) != NULL) {
      if (strcasecmp(env, "true") == 0) {
        __pmp_param.global_affinity = true;
        __pmp_debug(PMP_DEBUG_INFO,
                    "affinity mapping is by global id\n");
      }
      else if (strcasecmp(env, "false") == 0) {
        __pmp_param.global_affinity = false;
        __pmp_debug(PMP_DEBUG_INFO,
                    "affinity mapping is by local id\n");
      }
      else {
        __pmp_warning("ignoring invalid setting for "
                      "PSC_OMP_AFFINITY_GLOBAL\n");
      }
    }
  }
}

static void __pmp_init_nested (void)
{
  char *env;
  if ((env = getenv("OMP_NESTED")) != NULL) {
    if (strcasecmp(env, "true") == 0) {
      __pmp_param.nested_forks = true;
      __pmp_debug(PMP_DEBUG_INFO, "nested forks are enabled\n");
    }
    else if (strcasecmp(env, "false") == 0) {
      __pmp_param.nested_forks = false;
      __pmp_debug(PMP_DEBUG_INFO, "nested forks are disabled\n");
    }
    else {
      __pmp_warning("ignoring invalid setting for OMP_NESTED\n");
    }
  }
}

static void __pmp_init_profile (void)
{
  if (__pmp_getenv("OMP_PROFILE") != NULL) {
#ifdef PMP_PROFILE
    __pmp_profile_enable();
#else
    __pmp_warning("profile is not supported by this library\n");
#endif
  }
}

static void __pmp_init_serial_outline (void)
{
  if (__pmp_getenv("OMP_SERIAL_OUTLINE") != NULL) {
    /* This flag forces the library to fork at the outermost level even if 
       there is only 1 thread. This is useful for testing the out-line code
       generated by the compiler for correctness with 1 thread. Usually,
       the library will not fork at the outermost level if there is only
       1 thread causing the serial code to be executed not the parallel 
       outline code (for performance reasons). */
    __pmp_warning("OMP_SERIAL_OUTLINE is defined, outline code will be used\n"
                  "** instead of serial code for forks with 1 thread\n");
    __pmp_param.serial_outline = true;
  }
}

static void __pmp_init_schedule (void)
{
  char *env;
  if ((env = getenv("OMP_SCHEDULE")) != NULL) {
    char *p = env;
    char *buf = malloc(strlen(p) + 1);
    char *s = buf;
    if (buf == NULL) {
      __pmp_fatal("out of heap\n");
    }
    while (*p != 0 && *p != ',') {
      *s++ = *p++;
    }
    *s = 0;
    if (strcasecmp(buf, "dynamic") == 0) {
      __pmp_param.runtime_schedule = PMP_SCHED_DYNAMIC;
      if (*p != 0) {
        __pmp_param.runtime_chunk = atoi(p+1);
      }
      __pmp_debug(PMP_DEBUG_SCHEDULER,
                  "run-time scheduling is dynamic with chunk size of %d\n",
                 __pmp_param.runtime_chunk);
    }
    else if (strcasecmp(buf, "static") == 0) {
      if (*p == 0) {
        __pmp_param.runtime_schedule = PMP_SCHED_STATIC_EVEN;
        __pmp_debug(PMP_DEBUG_SCHEDULER,
                    "run-time scheduling is static even "
                    "(dynamic chunk size)\n");
      }
      else {
        __pmp_param.runtime_chunk = atoi(p+1);
        __pmp_param.runtime_schedule = PMP_SCHED_STATIC;
        __pmp_debug(PMP_DEBUG_SCHEDULER,
                    "run-time scheduling is static with chunk size of %d\n",
                   __pmp_param.runtime_chunk);
      }
    }
    else if (strcasecmp(buf, "guided") == 0) {
      __pmp_param.runtime_schedule = PMP_SCHED_GUIDED;
      if (*p != 0) {
        __pmp_param.runtime_chunk = atoi(p+1);
      }
      __pmp_debug(PMP_DEBUG_SCHEDULER,
                  "run-time scheduling is guided with minimum chunk size "
                   "of %d\n", __pmp_param.runtime_chunk);
    }
    free(buf);
  }
}

static void __pmp_init_stack_size (void)
{
  long long max_stack;
  int nthreads = __pmp_param.default_team_size;
  __pmp_param.omp_stack_size = __pmp_getenv("OMP_STACK_SIZE");

  /* Bug 14127: Don't attempt to calculate the per-thread stacksize
     for Fortran, because the number of threads (using that same
     stacksize) may increase later. */
  if (__pmp_param.omp_stack_size != NULL) {
    /* This code is used if the user has set the PSC_OMP_STACK_SIZE
       environment variable. */
    int res = __pmp_get_stack_size_limit(__pmp_param.omp_stack_size,
                                         &max_stack, nthreads);
    if (res == 0 && max_stack > 0) {
      __pmp_param.thread_stack_size = (size_t) max_stack;
    }
  }

  __pmp_debug(PMP_DEBUG_INFO,
              "pthread init stack size set to %ld bytes\n", 
              (long) __pmp_param.thread_stack_size);

  /* __pmp_debug(PMP_DEBUG_INFO, "__stack_size_divisor was %d, now %d\n",
                 __stack_size_divisor, nthreads); */

  __stack_size_divisor = nthreads;
}

static void __pmp_init_guard_size (void)
{
  char *omp_guard_size = __pmp_getenv("OMP_GUARD_SIZE");

  if (omp_guard_size != NULL) {
    double guard_size;
    char *end;
  
    guard_size = strtod(omp_guard_size, &end);
    if (errno == ERANGE) {
      __pmp_warning("your requested guard size "
        "of \"%s\" is not well-formed and has been ignored",
        omp_guard_size);
      return;
    }
    switch (tolower(*end)) {
      case 'k':
        guard_size *= 1024;
        break;
      case 'm':
        guard_size *= 1048576;
        break;
      case 'g':
        guard_size *= 1073741824;
        break;
    }
    __pmp_param.thread_guard_size = guard_size;
  }

  __pmp_debug(PMP_DEBUG_INFO,
              "pthread guard size set to %ld bytes\n", 
              (long) __pmp_param.thread_guard_size);
}

static void __pmp_init_thread_spin (void)
{
  char *env;
  if ((env = __pmp_getenv("OMP_THREAD_SPIN")) != NULL) {
    __pmp_param.thread_spin = atoi(env);
  }
  __pmp_debug(PMP_DEBUG_INFO,
              "thread spin count is set to %d\n", __pmp_param.thread_spin);
}

static void __pmp_init_lock_spin (void)
{
  char *env;
  if ((env = __pmp_getenv("OMP_LOCK_SPIN")) != NULL) {
    __pmp_param.lock_spin = (atoi(env) != 0);
  }
  __pmp_debug(PMP_DEBUG_INFO,
              "lock spin count is set to %d\n", __pmp_param.lock_spin);
}

static void __pmp_init_guided_chunk_divisor (void)
{
  char *env;
  if ((env = __pmp_getenv("OMP_GUIDED_CHUNK_DIVISOR")) != NULL) {
    __pmp_param.guided_chunk_divisor = atoi(env);
  }
  if (__pmp_param.guided_chunk_divisor <= 0) {
    __pmp_warning("ignoring invalid setting for "
                  "PSC_OMP_GUIDED_CHUNK_DIVISOR\n");
    __pmp_param.guided_chunk_divisor = PMP_GUIDED_CHUNK_DIVISOR;
  }
  __pmp_debug(PMP_DEBUG_INFO,
              "guided chunk divisor is set to %d\n",
              __pmp_param.guided_chunk_divisor);
}

static void __pmp_init_guided_chunk_max (void)
{
  char *env;
  if ((env = __pmp_getenv("OMP_GUIDED_CHUNK_MAX")) != NULL) {
    __pmp_param.guided_chunk_max = atoi(env);
  }
  if (__pmp_param.guided_chunk_max <= 0) {
    __pmp_warning("ignoring invalid setting for PSC_OMP_GUIDED_CHUNK_MAX\n");
    __pmp_param.guided_chunk_max = PMP_GUIDED_CHUNK_MAX;
  }
  __pmp_debug(PMP_DEBUG_INFO,
              "guided chunk maximum is set to %d\n",
              __pmp_param.guided_chunk_max);
}

static void __pmp_init_static_fair (void)
{
  if (__pmp_getenv("OMP_STATIC_FAIR") != NULL) {
    __pmp_param.static_fair = true;
  }
  __pmp_debug(PMP_DEBUG_INFO,
              "static fair scheduling is %s\n",
              (__pmp_param.static_fair ? "on" : "off"));
}

static void __pmp_init_thread_to_cpu_map (void)
{
  int i;
  int *thread_to_cpu_map;
  int *openmp_to_machine_cpu_map = __pmp_param.openmp_to_machine_cpu_map;
  char *env_map = NULL;
  char *env_stride = NULL;
  char *env_offset = NULL;
  bool mapped = false;
  void *result;

  if (posix_memalign(&result, PMP_L2_ALIGNED,
                     PMP_MAX_THREADS * sizeof(int)) != 0) {
    __pmp_fatal("failed to allocate thread to CPU mapping array\n");
  }
  thread_to_cpu_map = (int *) result;
  __pmp_param.thread_to_cpu_map = thread_to_cpu_map;

  if (__pmp_param.enable_affinity &&
      (env_map = __pmp_getenv("OMP_AFFINITY_MAP")) != NULL) {
    mapped = true;
    char *p = env_map;
    for (i = 0; i < PMP_MAX_THREADS; i++) {
      char *q = p;
      int cpu = strtol(p, &q, 10);
      if (p == q) {
        mapped = false;
        break;
      }
      if (cpu < 0 || cpu >= __pmp_param.openmp_num_cpus) {
        __pmp_warning("CPU %d in PSC_AFFINITY_MAP is out of range\n", cpu);
        mapped = false;
        break;
      }
      thread_to_cpu_map[i] = openmp_to_machine_cpu_map[cpu];
      p = q;
      if (*p == '\0') {
        i++;
        break;
      }
      if (*p != ',') {
        mapped = false;
        break;
      }
      p++;
    }
    if (mapped) {
      int j, k = 0;
      for (j = i; j < PMP_MAX_THREADS; j++) {
        thread_to_cpu_map[j] = thread_to_cpu_map[k];
        k++;
        if (k == i) {
          k = 0;
	}
      }
      __pmp_debug(PMP_DEBUG_INFO, "CPU affinity map is set to %s\n", env_map);

    }
    else {
      __pmp_warning("ignoring invalid setting for PSC_OMP_AFFINITY_MAP\n");
    }
  }

  if (__pmp_param.enable_affinity &&
      (env_stride = __pmp_getenv("OMP_CPU_STRIDE")) != NULL) {
    __pmp_param.cpu_stride = atoi(env_stride);
  }

  if (__pmp_param.enable_affinity &&
      (env_offset = __pmp_getenv("OMP_CPU_OFFSET")) != NULL) {
    __pmp_param.cpu_offset = atoi(env_offset);
  }

  if (mapped) {
    if (env_stride != NULL) {
      __pmp_warning("use PSC_OMP_AFFINITY_MAP, ignoring "
                    "PSC_OMP_CPU_STRIDE\n");
    }
    if (env_offset != NULL) {
      __pmp_warning("use PSC_OMP_AFFINITY_MAP, ignoring "
                    "PSC_OMP_CPU_OFFSET\n");
    }
  }
  else {
    if (__pmp_param.cpu_stride < 0 || 
        __pmp_param.cpu_stride > __pmp_param.openmp_num_cpus) {
      __pmp_warning("ignoring invalid setting for "
                    "PSC_OMP_CPU_STRIDE\n");
      __pmp_param.cpu_stride = 0;
    }
    if (__pmp_param.cpu_offset < 0 || 
        __pmp_param.cpu_offset > __pmp_param.openmp_num_cpus) {
      __pmp_warning("ignoring invalid setting for "
                    "PSC_OMP_CPU_OFFSET\n");
      __pmp_param.cpu_offset = 0;
    }

    if (__pmp_param.enable_affinity) {
      __pmp_debug(PMP_DEBUG_INFO,
                  "CPU stride is set to %d\n",
                  __pmp_param.cpu_stride);
      __pmp_debug(PMP_DEBUG_INFO,
                  "CPU offset is set to %d\n",
                  __pmp_param.cpu_offset);
    }

    for (i = 0; i < PMP_MAX_THREADS; i++) {
      int cpu;
      if (__pmp_param.cpu_stride <= 1) {
        cpu = (i + __pmp_param.cpu_offset) % __pmp_param.openmp_num_cpus;
      }
      else {
        int stride = __pmp_param.cpu_stride;
        int offset = __pmp_param.cpu_offset;
        int num_strides = __pmp_param.openmp_num_cpus / stride;
        cpu = ((i * stride) + ((i / num_strides) % stride) + offset);
        cpu = cpu % __pmp_param.openmp_num_cpus;
      }
      thread_to_cpu_map[i] = openmp_to_machine_cpu_map[cpu];
    }
  }

  if (__pmp_param.enable_affinity) {
    for (i = 0; i < __pmp_param.default_team_size; i++) {
      __pmp_debug(PMP_DEBUG_INFO, "thread %d binds to machine CPU %d\n",
                  i, thread_to_cpu_map[i]);
    }
    for (i = 0; i < PMP_MAX_THREADS; i++) {
      assert(thread_to_cpu_map[i] >= 0 &&
	     thread_to_cpu_map[i] < __pmp_param.machine_num_cpus);
    }
  }
}

static void __pmp_atfork (void)
{
  __pmp_debug(PMP_DEBUG_INFO, "encountered fork in an OpenMP program\n");

  if (__pmp_in_parallel()) {
    __pmp_warning("encountered fork inside an OpenMP parallel section\n");
  }

  __pmp_reinit();
}

static void __pmp_init_atfork (void)
{
  if (pthread_atfork(NULL, NULL, __pmp_atfork) != 0) {
    __pmp_fatal("failed to create pthread_atfork call-back\n");
  }
}

void __pmp_init (void)
{
  if (__pmp_param.initialized) {
    return;
  }

  __cpuid_init();
  __pmp_lock_init(&__pmp_io_lock);
  __pmp_param_init();
  __pmp_profile_init();
  __pmp_init_wtick();
  __pmp_init_silent();
  __pmp_init_debug();
  __pmp_init_cpus();
  __pmp_init_check();
  __pmp_init_disabled();
  __pmp_init_default_team_size();
  __pmp_init_dynamic();
  __pmp_init_enable_affinity();
  __pmp_init_nested();
  __pmp_init_profile();
  __pmp_init_serial_outline();
  __pmp_init_schedule();
  __pmp_init_stack_size();
  __pmp_init_guard_size();
  __pmp_init_thread_spin();
  __pmp_init_lock_spin();
  __pmp_init_guided_chunk_divisor();
  __pmp_init_guided_chunk_max();
  __pmp_init_static_fair();
  __pmp_init_thread_to_cpu_map();
  __pmp_init_atfork();
  __pmp_manager_init();

  __pmp_debug(PMP_DEBUG_INFO, "OpenMP is initialized\n");
}

void __pmp_fini (void)
{
  if (!__pmp_param.initialized) {
    return;
  }
  
  if (__pmp_profile.enabled) {
    __pmp_profile_dump();
  }

  /* The call to destory the io lock is not safe because the other threads
   * may still be trying to output debug information. The correct approach
   * is to terminate all the threads, perform a pthread-level join, and
   * then destroy the lock. This is still TODO. */
  /*__pmp_lock_destroy(&__pmp_io_lock); */

  __pmp_manager_fini();

  __pmp_param.initialized = false;
}

void __pmp_reinit (void)
{
  /* This routine re-initializes the thread manager and any locks
   * used by the library. This is called after a fork so that the OpenMP
   * library is in a sane state for the child process. Note that all
   * pthread mutex structures are supposedly in an invalid state after 
   * the fork and need to be re-initialized. This is done for the locks
   * used by the OpenMP library, however, I do not go chasing down all
   * pthread mutex structures that might have been allocated by the 
   * application using omp_init_lock and omp_init_nest_lock. Also, the
   * case of a fork inside a parallel section might not have the desired
   * semantics, so in this case I give a warning. In general, this is a
   * minimal effort implementation since this is a rare situation. */

  __pmp_lock_init(&__pmp_io_lock);

  __pmp_manager_fini();

  memset(&__pmp_manager, 0, sizeof(pmp_manager_t));

  __pmp_manager_init();

  __pmp_debug(PMP_DEBUG_INFO, "OpenMP is reinitialized\n");
}

#if defined(BUILD_OS_DARWIN)
#include <sys/types.h>
#include <sys/sysctl.h>

/* No memalign available--best we can do is default (16 byte alignment) */
void *darwin_memalign(size_t alignment, size_t size, char *message) {
  (void) alignment;
  void *result = malloc(size);
  if (result == 0)
  {
    __pmp_fatal(message);
  }
  return result;
}

/* Fetch an integer-valued sysctl parameter
 * name		name of parameter in sysctl database
 * return	result, or ~0 if not found
 */
long int
get_sysctl_int(char *name) {
  long int old;
  size_t oldlen = sizeof(old);
  if (sysctlbyname(name, &old, &oldlen, 0, 0)) {
    perror("sysctlbyname");
    return (long int) ~0UL;
    }
  return old;
  }
#endif /* defined(BUILD_OS_DARWIN) */
 
