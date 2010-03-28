/*
 * Copyright (C) 2008. PathScale, LLC. All Rights Reserved.
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cpuid.h"
#include "types.h"

#if defined __i386__ || defined __x86_64__

#define CPUID_DEBUG 0

typedef struct cpuid_results_t
{
  uint32_t eax;
  uint32_t ebx;
  uint32_t ecx;
  uint32_t edx;
} cpuid_results_t;

#define EFLAGS_ID_BIT (1 << 21)

#if __WORDSIZE == 64
typedef uint64_t uintreg_t;
#else
typedef uint32_t uintreg_t;
#endif

#define CPUID_BASIC_AUTHENTICITY	  	0x00000000
#define CPUID_BASIC_VERSION_AND_FEATURES	0x00000001
#define CPUID_BASIC_CACHE_AND_TLB		0x00000002
#define CPUID_BASIC_SERIAL_NUMBER		0x00000003
#define CPUID_BASIC_CACHE_PARAMETERS		0x00000004
#define CPUID_BASIC_MONITOR_LEVEL		0x00000005

#define CPUID_EXT_MAXIMUM			0x80000000
#define CPUID_EXT_SIGNATURE_AND_FEATURES	0x80000001
#define CPUID_EXT_BRAND1			0x80000002
#define CPUID_EXT_BRAND2			0x80000003
#define CPUID_EXT_BRAND3			0x80000004
#define CPUID_EXT_RESERVED1			0x80000005
#define CPUID_EXT_CACHE_PARAMETERS		0x80000006
#define CPUID_EXT_RESERVED2			0x80000007
#define CPUID_EXT_LONG_MODE_ADDRESSES		0x80000008

/* Bit fields for CPUID features from CPUID_BASIC_VERSION_AND_FEATURES */
#define CPUID_EDX_X87_BIT  	0x00000001	/* bit 0 */
#define CPUID_EDX_MMX_BIT  	0x00800000	/* bit 23 */
#define CPUID_EDX_SSE_BIT  	0x02000000	/* bit 25 */
#define CPUID_EDX_SSE2_BIT 	0x04000000	/* bit 26 */
#define CPUID_EDX_HTT_BIT  	0x10000000	/* bit 28 */
#define CPUID_ECX_SSE3_BIT 	0x00000001	/* bit 0 */
#define CPUID_EBX_CORE_MASK	0x00FF0000	/* bits [23:16] */
#define CPUID_EBX_CORE_SHIFT	16

/* Bit fields for CPUID features from CPUID_EXT_SIGNATURE_AND_FEATURES */
#define CPUID_ECX_CMP_LEGACY_BIT 0x00000002	/* bit 2 */

/* Bit fields for CPUID features from CPUID_EXT_LONG_MODE_ADDRESSES */
#define CPUID_ECX_CORE_MASK	0x000000FF	/* bits [7:0] */
#define CPUID_ECX_CORE_SHIFT	0

static uint32_t __cpuid_get_eflags ()
{
#if defined(BUILD_OS_DARWIN)
  /* Assembler and compiler are out of synch unless we use 16 bit type */
  unsigned short int reg;
#else /* defined(BUILD_OS_DARWIN) */
  uintreg_t reg;
#endif /* defined(BUILD_OS_DARWIN) */
  asm ("pushf\n\t"
       "pop %0" :
       "=a" (reg));
  if (CPUID_DEBUG) {
    printf("get eflags returns 0x%08x\n", (uint32_t) reg);
  }
  return (uint32_t) reg;
}

static void __cpuid_set_eflags (uint32_t eflags)
{
#if defined(BUILD_OS_DARWIN)
  /* Assembler and compiler are out of synch unless we use 16 bit type */
  unsigned short int reg = (unsigned short int) eflags;
#else /* defined(BUILD_OS_DARWIN) */
  uintreg_t reg = (uintreg_t) eflags;
#endif /* defined(BUILD_OS_DARWIN) */
  if (CPUID_DEBUG) {
    printf("set eflags to 0x%08x\n", (uint32_t) eflags);
  }
  asm ("push %0\n\t"
       "popf" : :
       "a" (reg));
}

static int __cpuid_check ()
{
  uint32_t eflags = __cpuid_get_eflags();
  __cpuid_set_eflags(eflags ^ EFLAGS_ID_BIT);
  return (eflags & EFLAGS_ID_BIT) != (__cpuid_get_eflags() & EFLAGS_ID_BIT);
}

/* For -m32 -fpic, %ebx stores the GOT pointer, so inlining this function
 * would result in cpuid clobbering this GOT value.
 */
static void __cpuid_asm_cpuid (uint32_t eax, cpuid_results_t *results) __attribute__((__noinline__));
static void __cpuid_asm_cpuid (uint32_t eax, cpuid_results_t *results)
{
#if defined(BUILD_OS_DARWIN)
  __asm __volatile("mov %%esi, %%ebx\n\t"
     "cpuid\n\t"
     "xchg %%esi, %%ebx" :
       "=a" (results->eax),
       "=S" (results->ebx),
       "=c" (results->ecx),
       "=d" (results->edx) :
       "a" (eax) :
       "memory");
#else /* defined(BUILD_OS_DARWIN) */
  asm volatile ("cpuid" :
                "=a" (results->eax),
                "=b" (results->ebx),
                "=c" (results->ecx),
                "=d" (results->edx) :
                "a" (eax) : 
                "memory");
#endif /* defined(BUILD_OS_DARWIN) */
}

static void __cpuid (uint32_t eax, cpuid_results_t *results)
{
  __cpuid_asm_cpuid(eax, results);

  if (CPUID_DEBUG) {
    printf("cpuid eax = 0x%08x return eax = 0x%08x\n", eax, results->eax);
    printf("cpuid eax = 0x%08x return ebx = 0x%08x\n", eax, results->ebx);
    printf("cpuid eax = 0x%08x return ecx = 0x%08x\n", eax, results->ecx);
    printf("cpuid eax = 0x%08x return edx = 0x%08x\n", eax, results->edx);
  }
}

cpuid_info_t __cpuid_info;

void __cpuid_init ()
{
  memset(&__cpuid_info, 0, sizeof(cpuid_info_t));
  __cpuid_info.num_cores = 1;

  /* Flip EFLAGS.ID twice to make the cpuid check idempotent */
  __cpuid_info.has_cpuid = __cpuid_check() && __cpuid_check();

  if (__cpuid_info.has_cpuid) {
    cpuid_results_t cpuid_results;

    __cpuid(CPUID_BASIC_AUTHENTICITY, &cpuid_results);
    __cpuid_info.max_eax_basic = cpuid_results.eax;

    if (CPUID_BASIC_VERSION_AND_FEATURES <= __cpuid_info.max_eax_basic) {
      __cpuid(CPUID_BASIC_VERSION_AND_FEATURES, &cpuid_results);
      __cpuid_info.has_x87  = (cpuid_results.edx & CPUID_EDX_X87_BIT) != 0;
      __cpuid_info.has_mmx  = (cpuid_results.edx & CPUID_EDX_MMX_BIT) != 0;
      __cpuid_info.has_sse  = (cpuid_results.edx & CPUID_EDX_SSE_BIT) != 0;
      __cpuid_info.has_sse2 = (cpuid_results.edx & CPUID_EDX_SSE2_BIT) != 0;
      __cpuid_info.has_sse3 = (cpuid_results.ecx & CPUID_ECX_SSE3_BIT) != 0;
      __cpuid_info.has_htt  = (cpuid_results.edx & CPUID_EDX_HTT_BIT) != 0;
    }

    __cpuid(CPUID_EXT_MAXIMUM, &cpuid_results);
    __cpuid_info.max_eax_ext = cpuid_results.eax;

    if (CPUID_EXT_SIGNATURE_AND_FEATURES <= __cpuid_info.max_eax_ext) {
      __cpuid(CPUID_EXT_SIGNATURE_AND_FEATURES, &cpuid_results);
      __cpuid_info.cmp_legacy = (cpuid_results.edx & CPUID_ECX_CMP_LEGACY_BIT) != 0;
    }

    if (__cpuid_info.has_htt && !__cpuid_info.cmp_legacy) {
      /* This detects the number of (fake) cores (per socket) on Intel HTT */
      if (CPUID_BASIC_VERSION_AND_FEATURES <= __cpuid_info.max_eax_basic) {
        __cpuid(CPUID_BASIC_VERSION_AND_FEATURES, &cpuid_results);
        __cpuid_info.num_cores = ((cpuid_results.ebx & CPUID_EBX_CORE_MASK) 
                                >> CPUID_EBX_CORE_SHIFT);
      }
    }
    else {
      /* This detects the number of (true) cores (per socket) on AMD multi-core */
      if (CPUID_EXT_LONG_MODE_ADDRESSES <= __cpuid_info.max_eax_ext) {
        __cpuid(CPUID_EXT_LONG_MODE_ADDRESSES, &cpuid_results);
        __cpuid_info.num_cores = ((cpuid_results.ecx & CPUID_ECX_CORE_MASK) 
                                >> CPUID_ECX_CORE_SHIFT) + 1;
      }
    }
  }

  if (CPUID_DEBUG) {
    printf("cpuid has_cpuid  = %d\n", __cpuid_has_cpuid());
    printf("cpuid has_x87    = %d\n", __cpuid_has_x87());
    printf("cpuid has_mmx    = %d\n", __cpuid_has_mmx());
    printf("cpuid has_sse    = %d\n", __cpuid_has_sse());
    printf("cpuid has_sse2   = %d\n", __cpuid_has_sse2());
    printf("cpuid has_sse3   = %d\n", __cpuid_has_sse3());
    printf("cpuid has_htt    = %d\n", __cpuid_has_htt());
    printf("cpuid cmp_legacy = %d\n", __cpuid_cmp_legacy());
    printf("cpuid num_cores  = %d\n", __cpuid_num_cores());
  }
}

#ifdef UNIT_TEST
int main ()
{
  __cpuid_init();
  printf("cpuid has_cpuid  = %d\n", __cpuid_has_cpuid());
  printf("cpuid has_x87    = %d\n", __cpuid_has_x87());
  printf("cpuid has_mmx    = %d\n", __cpuid_has_mmx());
  printf("cpuid has_sse    = %d\n", __cpuid_has_sse());
  printf("cpuid has_sse2   = %d\n", __cpuid_has_sse2());
  printf("cpuid has_sse3   = %d\n", __cpuid_has_sse3());
  printf("cpuid has_htt    = %d\n", __cpuid_has_htt());
  printf("cpuid cmp_legacy = %d\n", __cpuid_cmp_legacy());
  printf("cpuid num_cores  = %d\n", __cpuid_num_cores());
  return EXIT_SUCCESS;
}
#endif

#else

void __cpuid_init ()
{
}

#endif
