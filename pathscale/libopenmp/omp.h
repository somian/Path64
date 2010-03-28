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
