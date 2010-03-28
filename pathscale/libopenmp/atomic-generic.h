/*
  Copyright (c) 2007. QLogic Corporation.  All rights reserved.

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

#ifndef __pmp_atomic_generic_h
#define __pmp_atomic_generic_h

#include <stdint.h>

/* These are generic versions and do not provide any guarantee
 * atomicity. They are a porting aid only, and are not sufficient
 * for an implementation using multiple threads. */

static inline void __pmp_inline_atomic_add32 (volatile int32_t *p, 
                                              int32_t addend)
{
  *p += addend;
}

static inline int32_t __pmp_inline_atomic_xadd32 (volatile int32_t *p,
                                                  int32_t addend)
{
  int32_t result = *p;
  *p += addend;
  return result;
}

static inline int32_t __pmp_inline_atomic_xchg32 (volatile int32_t *p, 
                                                  int32_t new_value)
{
  int32_t value = *p;
  *p = new_value;
  return value;
}

static inline int32_t __pmp_inline_atomic_cmpxchg32 (volatile int32_t *p, 
                                                     int32_t old_value,
                                                     int32_t new_value)
{
  int32_t value = *p;
  if (old_value == value) {
    *p = new_value;
   }
  return value;
}

static inline void *__pmp_inline_atomic_cmpxchgptr  (volatile voidptr_t *p, 
                                                     void *old_value,
                                                     void *new_value)
{
  void *value = *p;
  if (old_value == value) {
    *p = new_value;
  }
  return value;
}

#if __WORDSIZE == 64
static inline int64_t __pmp_inline_atomic_xadd64 (volatile int64_t *p,
                                                  int64_t addend)
{
  int64_t result = *p;
  *p += addend;
  return result;
}
#endif

#if __WORDSIZE == 64
static inline void __pmp_inline_atomic_add64 (volatile int64_t *p,
                                              int64_t addend)
{
  *p += addend;
}
#endif

#if __WORDSIZE == 64
static inline int64_t __pmp_inline_atomic_cmpxchg64 (volatile int64_t *p, 
                                                     int64_t old_value,
                                                     int64_t new_value)
{
  int64_t value = *p;
  if (old_value == value) {
    *p = new_value;
  }
  return value;
}
#endif

static inline void __pmp_inline_memory_fence ()
{
}

#endif
