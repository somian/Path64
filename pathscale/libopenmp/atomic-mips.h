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

#ifndef __pmp_atomic_mips_h
#define __pmp_atomic_mips_h

#include <stdint.h>

#define FEATURE_REPEAT_LL

static inline void __pmp_inline_atomic_add32 (volatile int32_t *p,
                                              int32_t addend)
{
  int32_t tmp;
  asm volatile (".set push\n"
                "\t.set noreorder\n"
                "\t.set nomacro\n"
                "0:\n"
                "\tll %0, (%2)\n"
#ifdef FEATURE_REPEAT_LL
                "\tll %0, (%2)\n"
#endif
                "\tadd %0, %0, %1\n"
                "\tsc %0, (%2)\n" 
                "\tbeq %0, 0, 0b\n"
                "\tnop\n"
                "\t.set pop" :
                "=&r" (tmp) : "r" (addend), "r" (p) : "memory");
}

static inline int32_t __pmp_inline_atomic_xadd32 (volatile int32_t *p,
                                                  int32_t addend)
{
  int32_t tmp, result;
  asm volatile (".set push\n"
                "\t.set noreorder\n"
                "\t.set nomacro\n"
                "0:\n"
                "\tll %1, (%3)\n"
#ifdef FEATURE_REPEAT_LL
                "\tll %1, (%3)\n"
#endif
                "\tadd %0, %1, %2\n"
                "\tsc %0, (%3)\n" 
                "\tbeq %0, 0, 0b\n"
                "\tnop\n"
                "\t.set pop" :
                "=&r" (tmp), "=&r" (result) : "r" (addend), "r" (p) : 
                "memory");
  return result;
}

static inline int32_t __pmp_inline_atomic_xchg32 (volatile int32_t *p, 
                                                  int32_t new_value)
{
  int32_t tmp, result;
  asm volatile (".set push\n"
                "\t.set noreorder\n"
                "\t.set nomacro\n"
                "0:\n"
                "\tll %1, (%3)\n"
#ifdef FEATURE_REPEAT_LL
                "\tll %1, (%3)\n"
#endif
                "\tor %0, $0, %2\n"
                "\tsc %0, (%3)\n" 
                "\tbeq %0, 0, 0b\n"
                "\tnop\n"
                "\t.set pop" :
                "=&r" (tmp), "=&r" (result), "+&r" (new_value) : 
                "r" (p) : "memory");
  return result;
}

static inline int32_t __pmp_inline_atomic_cmpxchg32 (volatile int32_t *p, 
                                                     int32_t old_value,
                                                     int32_t new_value)
{
  int32_t tmp, result;
  asm volatile (".set push\n"
                "\t.set noreorder\n"
                "\t.set nomacro\n"
                "0:\n"
                "\tll %1, (%4)\n"
#ifdef FEATURE_REPEAT_LL
                "\tll %1, (%4)\n"
#endif
                "\tbne %1, %3, 1f\n"
                "\tor %0, $0, %2\n"
                "\tsc %0, (%4)\n" 
                "\tbeq %0, 0, 0b\n"
                "\tnop\n"
                "1:\n"
                "\t.set pop" :
                "=&r" (tmp), "=&r" (result), "+&r" (new_value) : 
                "r" (old_value), "r" (p) : 
                "memory");
  return result;
}

#if __WORDSIZE == 32
static inline void *__pmp_inline_atomic_cmpxchgptr (volatile voidptr_t *p, 
                                                    void *old_value,
                                                    void *new_value)
{
  void *result, *tmp;
  asm volatile (".set push\n"
                "\t.set noreorder\n"
                "\t.set nomacro\n"
                "0:\n"
                "\tll %1, (%4)\n"
#ifdef FEATURE_REPEAT_LL
                "\tll %1, (%4)\n"
#endif
                "\tbne %1, %3, 1f\n"
                "\tor %0, $0, %2\n"
                "\tsc %0, (%4)\n" 
                "\tbeq %0, 0, 0b\n"
                "\tnop\n"
                "1:\n"
                "\t.set pop" :
                "=&r" (tmp), "=&r" (result), "+&r" (new_value) : 
                "r" (old_value), "r" (p) : 
                "memory");
  return result;
}
#endif

#if __WORDSIZE == 64
static inline void *__pmp_inline_atomic_cmpxchgptr (volatile voidptr_t *p, 
                                                    void *old_value,
                                                    void *new_value)
{
  void *result, *tmp;
  asm volatile (".set push\n"
                "\t.set noreorder\n"
                "\t.set nomacro\n"
                "0:\n"
                "\tlld %1, (%4)\n"
#ifdef FEATURE_REPEAT_LL
                "\tlld %1, (%4)\n"
#endif
                "\tbne %1, %3, 1f\n"
                "\tor %0, $0, %2\n"
                "\tscd %0, (%4)\n" 
                "\tbeq %0, 0, 0b\n"
                "\tnop\n"
                "1:\n"
                "\t.set pop" :
                "=&r" (tmp), "=&r" (result), "+&r" (new_value) : 
                "r" (old_value), "r" (p) : 
                "memory");
  return result;
}
#endif

#if __WORDSIZE == 64
static inline int64_t __pmp_inline_atomic_xadd64 (volatile int64_t *p,
                                                  int64_t addend)
{
  int64_t tmp, result;
  asm volatile (".set push\n"
                "\t.set noreorder\n"
                "\t.set nomacro\n"
                "0:\n"
                "\tlld %1, (%3)\n"
#ifdef FEATURE_REPEAT_LL
                "\tlld %1, (%3)\n"
#endif
                "\tdadd %0, %1, %2\n"
                "\tscd %0, (%3)\n" 
                "\tbeq %0, 0, 0b\n"
                "\tnop\n"
                "\t.set pop" :
                "=&r" (tmp), "=&r" (result) : "r" (addend), "r" (p) :
                "memory");
  return result;
}
#endif

#if __WORDSIZE == 64
static inline void __pmp_inline_atomic_add64 (volatile int64_t *p,
                                              int64_t addend)
{
  int64_t tmp;
  asm volatile (".set push\n"
                "\t.set noreorder\n"
                "\t.set nomacro\n"
                "0:\n"
                "\tlld %0, (%1)\n"
#ifdef FEATURE_REPEAT_LL
                "\tlld %0, (%1)\n"
#endif
                "\tdadd %0, %0, %2\n"
                "\tscd %0, (%1)\n" 
                "\tbeq %0, 0, 0b\n"
                "\tnop\n"
                "\t.set pop" :
                "=&r" (tmp) : "r" (p), "r" (addend) : "memory");
}
#endif

#if __WORDSIZE == 64
static inline int64_t __pmp_inline_atomic_cmpxchg64 (volatile int64_t *p, 
                                                     int64_t old_value,
                                                     int64_t new_value)
{
  int64_t tmp, result;
  asm volatile (".set push\n"
                "\t.set noreorder\n"
                "\t.set nomacro\n"
                "0:\n"
                "\tlld %1, (%4)\n"
#ifdef FEATURE_REPEAT_LL
                "\tlld %1, (%4)\n"
#endif
                "\tbne %1, %3, 1f\n"
                "\tor %0, $0, %2\n"
                "\tscd %0, (%4)\n" 
                "\tbeq %0, 0, 0b\n"
                "\tnop\n"
                "1:\n"
                "\t.set pop" :
                "=&r" (tmp), "=&r" (result), "+&r" (new_value) : 
                "r" (old_value), "r" (p) : 
                "memory");
  return result;
}
#endif

static inline void __pmp_inline_memory_fence ()
{
  asm volatile ("sync" : : : "memory");
}

#endif
