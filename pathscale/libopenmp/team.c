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

#include "loop.h"
#include "team.h"
#include "utils.h"
#include <stdlib.h>
#include <string.h>

void __pmp_team_init (pmp_team_t *team)
{
  void *result;
  int i;

  team->team_size = 0; 		/* no threads assigned to team */
  team->work = NULL;		/* no work to do */
  team->fp = NULL;		/* no fp */
  team->copyprivate_src = NULL;	/* not in the copyprivate mechanism */
  team->working_threads = 0;	/* no threads are working */
  team->barrier_count = 0;	/* no threads are in the barrier */
  team->single_count = 0;	/* no single hits yet */
  team->loop_count = 0;		/* no dynamic loops yet */

#if defined(BUILD_OS_DARWIN)
  result = darwin_memalign(PMP_L2_ALIGNED,
    sizeof(pmp_thread_t *) * PMP_MAX_THREADS,
    "cannot allocate team members array\n");
#else /* defined(BUILD_OS_DARWIN) */
  if (posix_memalign(&result, PMP_L2_ALIGNED, 
                     sizeof(pmp_thread_t *) * PMP_MAX_THREADS) != 0) {
    __pmp_fatal("cannot allocate team members array\n");
  }
#endif /* defined(BUILD_OS_DARWIN) */
  memset(result, 0, sizeof(pmp_thread_t *) * PMP_MAX_THREADS);
  team->members = (pmp_thread_t **) result;

#if defined(BUILD_OS_DARWIN)
  result = darwin_memalign(PMP_L2_ALIGNED, sizeof(pmp_loop_t) * PMP_MAX_LOOPS,
    "cannot allocate team loops array\n");
#else /* defined(BUILD_OS_DARWIN) */
  if (posix_memalign(&result, PMP_L2_ALIGNED,
                     sizeof(pmp_loop_t) * PMP_MAX_LOOPS) != 0) {
    __pmp_fatal("cannot allocate team loops array\n");
  }
#endif /* defined(BUILD_OS_DARWIN) */
  memset(result, 0, sizeof(pmp_loop_t) * PMP_MAX_LOOPS);
  team->loops = (pmp_loop_t *) result;
  
  for (i = 0; i< PMP_MAX_LOOPS; i++) {
    __pmp_loop_init(team->loops + i);
  }
}

void __pmp_team_fini (pmp_team_t *team)
{
  free(team->members);
  free(team->loops);
}

pmp_team_t *__pmp_team_alloc (pmp_thread_t *master, int nthreads,
                              workfunc_t work, void *fp)
{
  pmp_team_t *team = __pmp_manager.teams + master->global_id;

  if (master->nesting_depth > 0) {
    /* This thread's team is currently in use, so must allocate
     * another one. This occurs due to nested parallelism. */
    void *result;
#if defined(BUILD_OS_DARWIN)
    result = darwin_memalign(PMP_L2_ALIGNED, sizeof(pmp_team_t),
      "cannot allocate team\n");
#else /* defined(BUILD_OS_DARWIN) */
    if (posix_memalign(&result, PMP_L2_ALIGNED, sizeof(pmp_team_t)) != 0) {
      __pmp_fatal("cannot allocate team\n");
    }
#endif /* defined(BUILD_OS_DARWIN) */
    memset(result, 0, sizeof(pmp_team_t));
    team = (pmp_team_t *) result;
    __pmp_team_init(team);
  }

  master->nesting_depth++;

  nthreads = __pmp_thread_acquire(nthreads);
  assert(nthreads > 0);
  assert(team->team_size == 0);	/* check that team is not in use! */
  team->team_size = nthreads;
  team->members[0] = master;
  team->work = work;
  team->fp = fp;
  team->working_threads = nthreads;
  team->barrier_count = nthreads;
  team->single_count = 0;
  team->loop_count = 0;

  __pmp_debug(PMP_DEBUG_TEAM, "allocating team of %d thread(s), "
              "master global_id is %d, team is at %p\n", 
              nthreads, master->global_id, team);

  __pmp_thread_assign(team, master, nthreads);

  return team;
}

void __pmp_team_free (pmp_thread_t *master, pmp_team_t *team)
{
  assert(team->working_threads == 0);
  assert(master->nesting_depth > 0);
  master->nesting_depth--;
  team->team_size = 0;
  if (master->nesting_depth > 0) {
    __pmp_team_fini(team);
    free(team);
  }
}
