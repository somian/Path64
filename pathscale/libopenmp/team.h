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
