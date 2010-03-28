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

#ifndef __pmp_atomic_h
#define __pmp_atomic_h

#include <stdint.h>
typedef void *voidptr_t;

#define PMP_INLINE_ATOMICS

#if defined __i386__ || defined __x86_64__
#include "atomic-x86.h"
#elif defined __mips__
#include "atomic-mips.h"
#else
#error "libopenmp is not supported in this architecture"
#include "atomic-generic.h"
#endif

#ifdef PMP_INLINE_ATOMICS

static inline void __pmp_atomic_add32 (volatile int32_t *p, int32_t addend)
{
  __pmp_inline_atomic_add32(p, addend);
}

static inline int32_t __pmp_atomic_xadd32 (volatile int32_t *p, int32_t addend)
{
  return __pmp_inline_atomic_xadd32(p, addend);
}

static inline int32_t __pmp_atomic_xchg32 (volatile int32_t *p, 
                                           int32_t new_value)
{
  return __pmp_inline_atomic_xchg32(p, new_value);
}

static inline int32_t __pmp_atomic_cmpxchg32 (volatile int32_t *p,
			                      int32_t old_value,
                                              int32_t new_value)
{
  return __pmp_inline_atomic_cmpxchg32(p, old_value, new_value);
}

static inline void *__pmp_atomic_cmpxchgptr (volatile voidptr_t *p,
                                             void *old_value, void *new_value)
{
  return __pmp_inline_atomic_cmpxchgptr(p, old_value, new_value);
}

#if __WORDSIZE == 64
static inline int64_t __pmp_atomic_xadd64 (volatile int64_t *p, int64_t addend)
{
  return __pmp_inline_atomic_xadd64(p, addend);
}

static inline void __pmp_atomic_add64 (volatile int64_t *p, int64_t addend)
{
  __pmp_inline_atomic_add64(p, addend);
}

static inline int64_t __pmp_atomic_cmpxchg64 (volatile int64_t *p,
			                      int64_t old_value,
                                              int64_t new_value)
{
  return __pmp_inline_atomic_cmpxchg64(p, old_value, new_value);
}
#endif

static inline void __pmp_memory_fence ()
{
  __pmp_inline_memory_fence ();
}

#else
extern void __pmp_atomic_add32 (volatile int32_t *p, int32_t addend);
extern int32_t __pmp_atomic_xadd32 (volatile int32_t *p, int32_t addend);
extern int32_t __pmp_atomic_xchg32 (volatile int32_t *p, int32_t new_value);
extern int32_t __pmp_atomic_cmpxchg32 (volatile int32_t *p,
				       int32_t old_value, int32_t new_value);
extern void *__pmp_atomic_cmpxchgptr (volatile voidptr_t *p,
				      void *old_value, void *new_value);
#if __WORDSIZE == 64
extern int64_t __pmp_atomic_xadd64 (volatile int64_t *p, int64_t addend);
extern void __pmp_atomic_add64 (volatile int64_t *p, int64_t addend);
extern int64_t __pmp_atomic_cmpxchg64 (volatile int64_t *p,
				       int64_t old_value, int64_t new_value);
#endif
extern void __pmp_memory_fence ();
#endif

#endif
