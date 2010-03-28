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
