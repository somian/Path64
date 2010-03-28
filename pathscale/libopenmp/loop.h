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

#ifndef __pmp_loop_h
#define __pmp_loop_h

#include "types.h"

#ifdef PMP_CHECK
/* This is set to 1 loop so that loop checking is performed on all loops */
#define PMP_LOG2_LOOPS		0
#else
/* TODO: tune this value? */
#define PMP_LOG2_LOOPS		4
#endif

#define PMP_MAX_LOOPS		(1 << PMP_LOG2_LOOPS)
#define PMP_MASK_LOOP_ID        0x7FFFFFFF
#define PMP_MASK_LOOP_INDEX	(PMP_MAX_LOOPS - 1)

#ifdef PMP_CHECK
extern void __pmp_start_check (pmp_loop_t *loop);
extern void __pmp_loop_check (pmp_loop_t *loop, int64_t value);
extern void __pmp_last_check (pmp_loop_t *loop);
extern void __pmp_stop_check (pmp_loop_t *loop, pmp_thread_t *thread);
#else
#define __pmp_start_check(x)
#define __pmp_last_check(x)
#define __pmp_loop_check(x,y)
#define __pmp_stop_check(x,y)
#endif

extern void __pmp_loop_init (pmp_loop_t *loop);

extern void __pmp_loop_alloc (pmp_thread_t *thread, int sched,
                              int64_t lower, int64_t upper, int64_t inc,
                              int64_t chunk, int64_t min_chunk);

extern void __pmp_loop_free (pmp_thread_t *thread);

#endif
