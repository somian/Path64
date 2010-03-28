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
