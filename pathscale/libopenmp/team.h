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

#ifndef __pmp_team_h
#define __pmp_team_h

#include "types.h"
#include "thread.h"
 
extern void __pmp_team_init (pmp_team_t *team);
extern void __pmp_team_fini (pmp_team_t *team);
extern pmp_team_t *__pmp_team_alloc (pmp_thread_t *master, int nthreads,
                                     workfunc_t work, void *fp);
extern void __pmp_team_free (pmp_thread_t *master, pmp_team_t *team);

static inline pmp_team_t *__pmp_get_current_team (void)
{
  /* NOTE: this will return NULL if there is no current team */
#if (defined PMP_NO_TLS) || (!defined PMP_TLS_TEAM)
  return __pmp_get_current_thread()->team;
#else
  return __pmp_tls_current_team;
#endif
}

static inline int __pmp_get_current_team_size (void)
{
  pmp_team_t *team = __pmp_get_current_team();
  return (team == NULL) ? 1 : team->team_size;
}

static inline int __pmp_get_team_size (pmp_team_t *team)
{
  return (team == NULL) ? 1 : team->team_size;
}

static inline int __pmp_get_new_team_size (void)
{
  pmp_param_t *param = __pmp_get_param();
  if (__pmp_get_current_team() != NULL && !param->nested_forks) {
    return 1;
  }
  else {
    return param->default_team_size;
  }
}

#endif
