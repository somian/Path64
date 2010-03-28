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

#ifndef __cpuid_h
#define __cpuid_h

#if defined __i386__ || defined __x86_64__

typedef struct cpuid_info_t
{
  int has_cpuid;
  int max_eax_basic;
  int max_eax_ext;
  int has_x87;
  int has_mmx;
  int has_sse;
  int has_sse2;
  int has_sse3;
  int has_htt;
  int cmp_legacy;
  int num_cores;
} cpuid_info_t;

extern cpuid_info_t __cpuid_info;

static inline int __cpuid_has_cpuid ()  { return __cpuid_info.has_cpuid; }
static inline int __cpuid_has_x87 ()    { return __cpuid_info.has_x87; }
static inline int __cpuid_has_mmx ()    { return __cpuid_info.has_mmx; }
static inline int __cpuid_has_sse ()    { return __cpuid_info.has_sse; }
static inline int __cpuid_has_sse2 ()   { return __cpuid_info.has_sse2; }
static inline int __cpuid_has_sse3 ()   { return __cpuid_info.has_sse3; }
static inline int __cpuid_has_htt ()    { return __cpuid_info.has_htt; }
static inline int __cpuid_cmp_legacy () { return __cpuid_info.cmp_legacy; }
static inline int __cpuid_num_cores ()  { return __cpuid_info.num_cores; }

#endif

extern void __cpuid_init ();

#endif
