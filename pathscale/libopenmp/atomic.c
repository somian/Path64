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

#include "atomic.h"

#ifndef PMP_INLINE_ATOMICS

void __pmp_atomic_add32 (volatile int32_t *p, int32_t addend)
{
  __pmp_inline_atomic_add32(p, addend);
}

int32_t __pmp_atomic_xadd32 (volatile int32_t *p, int32_t addend)
{
  return __pmp_inline_atomic_xadd32(p, addend);
}

int32_t __pmp_atomic_xchg32 (volatile int32_t *p, int32_t new_value)
{
  return __pmp_inline_atomic_xchg32(p, new_value);
}

int32_t __pmp_atomic_cmpxchg32 (volatile int32_t *p,
			        int32_t old_value, int32_t new_value)
{
  return __pmp_inline_atomic_cmpxchg32(p, old_value, new_value);
}

void *__pmp_atomic_cmpxchgptr (volatile voidptr_t *p,
		               void *old_value, void *new_value)
{
  return __pmp_inline_atomic_cmpxchgptr(p, old_value, new_value);
}

#if __WORDSIZE == 64
int64_t __pmp_atomic_xadd64 (volatile int64_t *p, int64_t addend)
{
  return __pmp_inline_atomic_xadd64(p, addend);
}

void __pmp_atomic_add64 (volatile int64_t *p, int64_t addend)
{
  __pmp_inline_atomic_add64(p, addend);
}

int64_t __pmp_atomic_cmpxchg64 (volatile int64_t *p,
			        int64_t old_value, int64_t new_value)
{
  return __pmp_inline_atomic_cmpxchg64(p, old_value, new_value);
}
#endif

void __pmp_memory_fence ()
{
  __pmp_inline_memory_fence ();
}

#endif
