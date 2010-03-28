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
