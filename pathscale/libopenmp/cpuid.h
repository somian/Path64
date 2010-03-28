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
