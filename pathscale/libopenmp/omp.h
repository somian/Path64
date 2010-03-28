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

#ifndef __pmp_omp_h
#define __pmp_omp_h

#ifdef __cplusplus
extern "C" {
#endif

extern void omp_set_num_threads (int nthreads);
extern int omp_get_num_threads (void);
extern int omp_get_max_threads (void);
extern int omp_get_thread_num (void);
extern int omp_get_num_procs (void);
extern int omp_in_parallel (void);
extern void omp_set_dynamic (int nthreads);
extern int omp_get_dynamic (void);
extern void omp_set_nested (int nested);
extern int omp_get_nested (void);

typedef void *omp_lock_t;
extern void omp_init_lock (omp_lock_t *lock);
extern void omp_destroy_lock (omp_lock_t *lock);
extern void omp_set_lock (omp_lock_t *lock);
extern void omp_unset_lock (omp_lock_t *lock);
extern int omp_test_lock (omp_lock_t *lock);

typedef void *omp_nest_lock_t;
extern void omp_init_nest_lock (omp_nest_lock_t *lock);
extern void omp_destroy_nest_lock (omp_nest_lock_t *lock);
extern void omp_set_nest_lock (omp_nest_lock_t *lock);
extern void omp_unset_nest_lock (omp_nest_lock_t *lock);
extern int omp_test_nest_lock (omp_nest_lock_t *lock);

extern double omp_get_wtime (void);
extern double omp_get_wtick (void);

#ifdef __cplusplus
}
#endif

#endif /* __pmp_omp_h */
