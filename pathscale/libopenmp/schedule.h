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

#ifndef __pmp_schedule_h
#define __pmp_schedule_h

#define PMP_SCHED_STATIC	1
#define PMP_SCHED_STATIC_EVEN	2
#define PMP_SCHED_DYNAMIC	3
#define PMP_SCHED_GUIDED	4
#define PMP_SCHED_RUNTIME	5

#define PMP_SCHED_ORDERED_STATIC	33
#define PMP_SCHED_ORDERED_STATIC_EVEN	34
#define PMP_SCHED_ORDERED_DYNAMIC	35
#define PMP_SCHED_ORDERED_GUIDED	36
#define PMP_SCHED_ORDERED_RUNTIME	37

#define PMP_SCHED_ORDERED_OFFSET (PMP_SCHED_ORDERED_STATIC - PMP_SCHED_STATIC)

/* The value of PMP_GUIDED_CHUNK_DIVISOR is used to divide down the chunk
 * size assigned by the algorithm (and then subjected to the user-specified
 * minimum). A value of 1 gives the biggest possible chunks and the
 * fewest number of calls into the loop scheduler. Larger values will
 * result in smaller chunks giving more opportunities for the dynamic
 * guided scheduler to assign work balancing out variation between loop
 * iterations, at the expense of more calls into the loop scheduler. 
 * With a value of PMP_GUIDED_CHUNK_DIVISOR equal to 1, the first thread will
 * get 1/n'th of the iterations (for a team of n). If these iterations
 * happen to be particularly expensive then this thread will be the
 * critical path through the loop. For this reason, PMP_GUIDED_CHUNK_DIVISOR
 * should be a small integer value greater than 1 and 2 is highly
 * recommended. The default value can be over-ridden using the
 * OMP_GUIDED_CHUNK_DIVISOR environment variable. */

#define PMP_GUIDED_CHUNK_DIVISOR 2

/* This is another parameter that can be used to make the GUIDED algorithm
 * more dynamic in its allocation of chunks to threads. This just limits
 * the maximum size of an allocated chunk. If large enough the performance
 * overhead of entering the scheduler to allocate the chunk will be minimal.
 * The best value of this will be found by measurement across the few
 * benchmarks that depend on guided scheduling. The default value can be
 * over-ridden using the OMP_GUIDED_CHUNK_MAX environment variable. */

#define PMP_GUIDED_CHUNK_MAX 300

#endif
