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

#include <stdio.h>
#include <sys/syscall.h>
#include <unistd.h>

#include "affinity.h"
#include "thread.h"
#include "types.h"
#include "utils.h"

#ifndef __NR_sched_setaffinity
/* for distributions that do not provide these in <sys/syscall.h> */
#if defined __x86_64__
#define __NR_sched_setaffinity	203
#endif
#if defined __i386__
#define __NR_sched_setaffinity	241
#endif
#endif

#ifndef __NR_sched_getaffinity
/* for distributions that do not provide these in <sys/syscall.h> */
#if defined __x86_64__
#define __NR_sched_getaffinity	204
#endif
#if defined __i386__
#define __NR_sched_getaffinity	242
#endif
#endif

typedef enum affinity_style_t {
  AFFINITY_UNKNOWN,
  AFFINITY_UNSUPPORTED,
  AFFINITY_2P,
  AFFINITY_3P
} pmp_affinity_style_t;

int __pmp_getset_affinity (int nr, pid_t pid, unsigned int len, 
                           cpu_set_t *cpus)
                           
{
  static pmp_affinity_style_t __pmp_affinity_style = AFFINITY_UNKNOWN;
  int e = -1;

  if (__pmp_affinity_style == AFFINITY_UNKNOWN ||
      __pmp_affinity_style == AFFINITY_3P) {
    e = syscall(nr, 0, len, cpus);
    /* __pmp_debug(PMP_DEBUG_INFO,
     *             "__pmp_getset_affinity nr=%d 3P returns %d\n",
     *             nr, e);
     */
    if (e >= 0) {
      __pmp_affinity_style = AFFINITY_3P;
      e = 0;
    }
  }

  if (__pmp_affinity_style == AFFINITY_UNKNOWN ||
      __pmp_affinity_style == AFFINITY_2P) {
    e = syscall(nr, 0, cpus);
    /* __pmp_debug(PMP_DEBUG_INFO, 
     *             "__pmp_getset_affinity nr=%d 2P returns %d\n",
     *             nr, e);
     */
    if (e >= 0) {
      __pmp_affinity_style = AFFINITY_2P;
      e = 0;
    }
  }

  if (__pmp_affinity_style == AFFINITY_UNKNOWN) {
    __pmp_affinity_style = AFFINITY_UNSUPPORTED;
  }

  return e;
}

int __pmp_set_affinity (int cpu)
{
  cpu_set_t cpus;
  CPU_ZERO(&cpus);
  CPU_SET(cpu, &cpus);
  if (cpu < __pmp_get_param()->machine_num_cpus) {
    return __pmp_getset_affinity(__NR_sched_setaffinity, 0,
                                 sizeof(cpus), &cpus);
  }
  else {
    return -1;
  }
}

int __pmp_get_affinity (cpu_set_t *cpus)
{
  return __pmp_getset_affinity(__NR_sched_getaffinity, 0,
                               sizeof(*cpus), cpus);
}

int __pmp_has_affinity ()
{
  cpu_set_t cpus;
  CPU_ZERO(&cpus);
  int e = __pmp_getset_affinity(__NR_sched_getaffinity, 0, 
                                sizeof(cpus), &cpus);
  if (e == 0) {
    int cpu;
    for (cpu = 0; cpu < __pmp_get_param()->machine_num_cpus; cpu++) {
      if (!CPU_ISSET(cpu, &cpus)) {
        /* The thread cannot be scheduled on a CPU so it has affinity */
        e = 1;
        break;
      }
    }
  }
  return e;
}
