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

#ifndef __pmp_atomic_x86_h
#define __pmp_atomic_x86_h

#include <stdint.h>
#include "cpuid.h"

#if defined(BUILD_OS_DARWIN)
#define LOCK_PREFIX "lock/"
#else /* defined(BUILD_OS_DARWIN) */
#define LOCK_PREFIX "lock "
#endif /* defined(BUILD_OS_DARWIN) */

static inline void __pmp_inline_atomic_add32 (volatile int32_t *p,
					      int32_t addend)
{
  /* Equivalent pseudo-code sequence: 
   *   acquire_lock();
   *   *p += addend;
   *   release_lock();
   */

  asm volatile (LOCK_PREFIX "add %1, %0" : 
                "+m" (*p) : 
                "r" (addend) : 
                "memory");
}

static inline int32_t __pmp_inline_atomic_xadd32 (volatile int32_t *p,
                                                  int32_t addend)
{
  /* Equivalent pseudo-code sequence: 
   *   int32_t result;
   *   acquire_lock();
   *   result = *p;
   *   *p += addend;
   *   release_lock();
   *   return result;
   */

  asm volatile (LOCK_PREFIX "xadd %1, %0" : 
                "+m" (*p), "+r" (addend) : : 
                "memory");
  return addend;
}

static inline int32_t __pmp_inline_atomic_xchg32 (volatile int32_t *p, 
                                                  int32_t new_value)
{
  /* Equivalent pseudo-code sequence:
   *   int32_t value;
   *   acquire_lock();
   *   value = *p;
   *   *p = new_value;
   *   release_lock();
   *   return value;
   */

  asm volatile ("xchg %1, %0" : 
                "+m" (*p), "+r" (new_value) : : 
                "memory");
  return new_value;
}

static inline int32_t __pmp_inline_atomic_cmpxchg32 (volatile int32_t *p, 
                                                     int32_t old_value,
                                                     int32_t new_value)
{
  /* Equivalent pseudo-code sequence:
   *   int32_t value;
   *   acquire_lock();
   *   value = *p;
   *   if (old_value == value) {
   *     *p = new_value;
   *   }
   *   release_lock();
   *   return value;
   */

  asm volatile (LOCK_PREFIX "cmpxchg %2, %0" : 
                "+m" (*p), "+a" (old_value) :
                "r" (new_value) : 
                "memory");
  return old_value;
}

static inline void *__pmp_inline_atomic_cmpxchgptr  (volatile voidptr_t *p, 
                                                     void *old_value,
                                                     void *new_value)
{
  /* Equivalent pseudo-code sequence:
   *   void *value;
   *   acquire_lock();
   *   value = *p;
   *   if (old_value == value) {
   *     *p = new_value;
   *   }
   *   release_lock();
   *   return value;
   */

  asm volatile (LOCK_PREFIX "cmpxchg %2, %0" : 
                "+m" (*p), "+a" (old_value) :
                "r" (new_value) : 
                "memory");
  return old_value;
}

#if __WORDSIZE == 64
static inline int64_t __pmp_inline_atomic_xadd64 (volatile int64_t *p,
                                                  int64_t addend)
{
  /* Equivalent pseudo-code sequence: 
   *   int64_t result;
   *   acquire_lock();
   *   result = *p;
   *   *p += addend;
   *   release_lock();
   *   return result;
   */

  asm volatile (LOCK_PREFIX "xadd %1, %0" : 
                "+m" (*p), "+r" (addend) : : 
                "memory");
  return addend;
}
#endif

#if __WORDSIZE == 64
static inline void __pmp_inline_atomic_add64 (volatile int64_t *p,
                                              int64_t addend)
{
  /* Equivalent pseudo-code sequence: 
   *   acquire_lock();
   *   *p += addend;
   *   release_lock();
   */

  asm volatile (LOCK_PREFIX "add %1, %0" : 
                "+m" (*p) : 
                "r" (addend) : 
                "memory");
}
#endif

#if __WORDSIZE == 64
static inline int64_t __pmp_inline_atomic_cmpxchg64 (volatile int64_t *p, 
                                                     int64_t old_value,
                                                     int64_t new_value)
{
  /* Equivalent pseudo-code sequence:
   *   int64_t value;
   *   acquire_lock();
   *   value = *p;
   *   if (old_value == value) {
   *     *p = new_value;
   *   }
   *   release_lock();
   *   return value;
   */

  asm volatile (LOCK_PREFIX "cmpxchg %2, %0" : 
                "+m" (*p), "+a" (old_value) :
                "r" (new_value) : 
                "memory");
  return old_value;
}
#endif

/* Memory fences are used to implement the implicit flushes
 * mandated by the OpenMP specification on certain OpenMP directives.
 * This has been implemented per the 2.0 spec, but I believe the 2.5
 * spec to be essentially the same (just reworded and clarified).
 */

#if __WORDSIZE == 64
static inline void __pmp_inline_memory_fence ()
{
  /* mfence is presumed to exist on all 64-bit processors */
  asm volatile ("mfence" : : : "memory");
}
#else
static inline void __pmp_inline_memory_fence ()
{
  /* dynamic to check to see which fence is required on 32-bit processors */
  if (__cpuid_has_sse2()) {
    asm volatile ("mfence" : : : "memory");
  }
  else if (__cpuid_has_sse()) {
    asm volatile ("sfence" : : : "memory");
  }
}
#endif
 
#endif
