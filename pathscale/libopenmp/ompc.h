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

#ifndef __pmp_ompc_h
#define __pmp_ompc_h

#include "types.h"

/* This file defines the interface used by the compiler */

/* Thread functions */
extern void __ompc_fork (int nthreads, workfunc_t work, void *fp);
extern int __ompc_can_fork (void);
extern int __ompc_in_parallel (void);
extern void __ompc_serialized_parallel (void);
extern void __ompc_end_serialized_parallel (void);
extern void __ompc_barrier (void);
extern int __ompc_get_thread_num (void);
extern int __ompc_get_local_thread_num (void);
extern int __ompc_single (int global_id);
extern void __ompc_end_single (int global_id);
extern void __ompc_get_thdprv (void ***thdprv, int64_t size,
                               void *data, int global_id);
extern void __ompc_copyin_thdprv (int n, ...);
extern void __ompc_copyprivate (int lock, void *src, 
                                void (*copy)(void*, void*));
extern void __ompc_flush (void);

/* Statically scheduled loop functions */
extern void __ompc_static_init_8 (int global_id, int sched, int64_t *lower,
                                  int64_t *upper, int64_t *stride, int64_t inc,
                                  int64_t chunk);
extern void __ompc_static_init_4 (int global_id, int sched, int *lower,
                                  int *upper, int *stride, int inc, int chunk);
extern void __ompc_static_fini (void);

/* Dyamically scheduled loop functions */
extern void __ompc_scheduler_init_8 (int global_id, int sched, int64_t lower,
                                     int64_t upper, int64_t inc, 
                                     int64_t chunk);
extern int __ompc_schedule_next_8 (int global_id, int64_t *lower,
                                   int64_t *upper, int64_t *inc);
extern void __ompc_scheduler_init_4 (int global_id, int sched, 
                                     int lower, int upper,
                                     int inc, int chunk);
extern int __ompc_schedule_next_4 (int global_id, int *lower, int *upper,
                                   int *inc);

/* Other scheduling related functions */
extern void __ompc_ordered (int global_id);
extern void __ompc_end_ordered (int global_id);
extern int __ompc_master (int global_id);
extern void __ompc_end_master (int global_id);

/* Lock functions */
extern void __ompc_critical (int global_id, pmp_lock_t **lock);
extern void __ompc_end_critical (int global_id, pmp_lock_t **lock);

/* Variables to support APO */
extern volatile int __ompc_sug_numthreads;
extern volatile int __ompc_cur_numthreads;
extern volatile int __ompc_max_numthreads;
#endif
