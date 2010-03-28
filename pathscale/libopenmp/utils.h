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

#ifndef __pmp_utils_h
#define __pmp_utils_h

#include "atomic.h"
#include "cpuid.h"
#include "types.h"
#include <assert.h>

#define MAX(a,b) (((a) > (b)) ? (a) : (b))
#define MIN(a,b) (((a) < (b)) ? (a) : (b))

#define PMP_DEBUG_NONE 0x00
#define PMP_DEBUG_ALL 0x01
#define PMP_DEBUG_CALLS 0x02
#define PMP_DEBUG_INFO 0x04
#define PMP_DEBUG_THREAD 0x08
#define PMP_DEBUG_TEAM 0x10
#define PMP_DEBUG_SCHEDULER 0x20
#define PMP_DEBUG_LOOPS 0x40
#define PMP_DEBUG_LOCKS 0x80

#ifdef PMP_DEBUG
extern void __pmp_debug (int level, const char *format, ...);
#else
#define __pmp_debug(x, ...)
#endif

extern void __pmp_warning (const char *format, ...);
extern void __pmp_fatal (const char *format, ...);

extern void __pmp_init (void) __attribute__((constructor));
extern void __pmp_fini (void) __attribute__((destructor));
extern void __pmp_reinit (void);

#ifdef PMP_PROFILE
static inline void __pmp_sample (pmp_profile_index_t index)
{
  if (__pmp_profile.enabled) {
    assert(index >= 0 && index < PMP_PROFILE_NUM);
#if __WORDSIZE == 64
    __pmp_atomic_add64(&__pmp_profile.sample_count[index], 1);
#else
    __pmp_atomic_add32(&__pmp_profile.sample_count[index], 1);
#endif
  }
}

static inline void __pmp_profile_iterations (pmp_global_id_t global_id,
                                             int64_t iterations)
{
  if (__pmp_profile.enabled && iterations != 0) {
    assert(global_id >= 0 && global_id < PMP_MAX_THREADS);
#if __WORDSIZE == 64
    __pmp_atomic_add64(&__pmp_profile.iteration_count[global_id], iterations);
#else
    __pmp_atomic_add32(&__pmp_profile.iteration_count[global_id], iterations);
#endif
  }
}

#else
#define __pmp_sample(x)
#define __pmp_profile_iterations(x,y)
#endif

extern void __pmp_profile_enable (void);
extern void __pmp_profile_disable (void);
extern void __pmp_profile_dump (void);
extern void __pmp_profile_reset (void);

#if defined(BUILD_OS_DARWIN)
/* Pass this to get_sysctl_int() to get the equivalent of
 * sysconf(_SC_NPROCESSORS_ONLN); (hw.ncpu, hw.activecpu, hw.availcpu,
 * and hw.logicalcpu are also available, so might need to change this
 * definition of any of those is closer to the meaning of _SC_NPROCESSORS_ONLN)
 */
#define SYSCTL_NPROCESSORS_ONLN "hw.physicalcpu"
extern void *darwin_memalign(size_t, size_t, char *);
extern long int get_sysctl_int(char *);
#endif /* defined(BUILD_OS_DARWIN) */

#endif
