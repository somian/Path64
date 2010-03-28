/*
  Copyright (c) 2007. QLogic Corporation.  All rights reserved.

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
