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

#ifndef __pmp_affinity_h
#define __pmp_affinity_h

#if defined(BUILD_OS_DARWIN)
#include <unistd.h>
#else /* defined(BUILD_OS_DARWIN) */
#include <linux/unistd.h>
#endif /* defined(BUILD_OS_DARWIN) */
#ifndef __USE_GNU
#define __USE_GNU
#endif
#include <sched.h>

#ifndef CPU_ZERO
/* For distributions that do not provide these in <sched.h>
 * Modern distributions do provide them, and the definitions are
 * quite different to those below. */
typedef unsigned long cpu_set_t;
#define CPU_SET_SIZE (sizeof(cpu_set_t))
#define CPU_ZERO(c) ((*c) = 0)
#define CPU_SET(i,c) ((*c) |= (1L << (i)))
#define CPU_CLR(i,c) ((*c) &= ~(1L << (i)))
#define CPU_ISSET(i,c) (((*c) & (1L << (i))) != 0)
#endif

extern int __pmp_set_affinity (int cpu);
extern int __pmp_get_affinity (cpu_set_t *cpus);
extern int __pmp_has_affinity ();

#endif
