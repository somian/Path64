/*

  Copyright (C) 2000, 2001 Silicon Graphics, Inc.  All Rights Reserved.

   Path64 is free software; you can redistribute it and/or modify it
   under the terms of the GNU Lesser General Public License as published by
   the Free Software Foundation version 2.1

   Path64 is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
   or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
   License for more details.

   You should have received a copy of the GNU General Public License
   along with Path64; see the file COPYING.  If not, write to the Free
   Software Foundation, 51 Franklin Street, Fifth Floor, Boston, MA
   02110-1301, USA.

   Special thanks goes to SGI for their continued support to open source

*/


#pragma ident "@(#) libu/multi/posix/mactdata.c	92.1	06/25/99 14:35:10"

#include "macdefs.h"
#include <stdio.h>
#include <thread.h>
#include "mactdefs.h"


/*
 * Per-task information.  We start with 10 descriptors, all free.
 */
static mact_task_t      initial_tasks[] =
                        {
                            { 1, TS_Running, 1, 0, NULL, NULL, NULL, 0,
                              NULL, 0, NULL,
                              NULL,
                              NULL, 0,
                              { 0 }
                            },
                            { 0, TS_Nonexistent, 0, 0, NULL, NULL, NULL, 0,
                              NULL, 0, NULL,
                              NULL,
                              &initial_tasks[2], 0,
                              { 0 }
                            },
                            { 0, TS_Nonexistent, 0, 0, NULL, NULL, NULL, 0,
                              NULL, 0, NULL,
                              NULL,
                              &initial_tasks[3], 0,
                              { 0 }
                            },
                            { 0, TS_Nonexistent, 0, 0, NULL, NULL, NULL, 0,
                              NULL, 0, NULL,
                              NULL,
                              &initial_tasks[4], 0,
                              { 0 }
                            },
                            { 0, TS_Nonexistent, 0, 0, NULL, NULL, NULL, 0,
                              NULL, 0, NULL,
                              NULL,
                              &initial_tasks[5], 0,
                              { 0 }
                            },
                            { 0, TS_Nonexistent, 0, 0, NULL, NULL, NULL, 0,
                              NULL, 0, NULL,
                              NULL,
                              &initial_tasks[6], 0,
                              { 0 }
                            },
                            { 0, TS_Nonexistent, 0, 0, NULL, NULL, NULL, 0,
                              NULL, 0, NULL,
                              NULL,
                              &initial_tasks[7], 0,
                              { 0 }
                            },
                            { 0, TS_Nonexistent, 0, 0, NULL, NULL, NULL, 0,
                              NULL, 0, NULL,
                              NULL,
                              &initial_tasks[8], 0,
                              { 0 }
                            },
                            { 0, TS_Nonexistent, 0, 0, NULL, NULL, NULL, 0,
                              NULL, 0, NULL,
                              NULL,
                              &initial_tasks[9], 0,
                              { 0 }
                            },
                            { 0, TS_Nonexistent, 0, 0, NULL, NULL, NULL, 0,
                              NULL, 0, NULL,
                              NULL,
                              NULL, 0,
                              { 0 }
                            },
                        };

volatile mact_task_t   *_mact_tasks             = &initial_tasks[0];
volatile mact_task_t   *_mact_free              = &initial_tasks[1];
volatile int            _mact_free_sem          = 0;
volatile mact_task_t   *_mact_hash[MTHASH_SIZE] = { NULL,
                                                  &initial_tasks[0],
                                                  NULL,
                                                };
volatile int            _mact_hash_sem          = 0;


/*
 * Total number of macrotask descriptors we have, and the number
 * of free ones.
 */
volatile unsigned int   _mact_total_count
                        =   sizeof(initial_tasks)
                          / sizeof(initial_tasks[0]);
volatile unsigned int   _mact_free_count
                        =   (  sizeof(initial_tasks)
                             / sizeof(initial_tasks[0])
                            )
                          - 1;


/*
 * Total number of runnable macrotasks there are, and a semaphore
 * to protect access.
 */
volatile unsigned int   _mact_runnable_tasks = 1;
volatile int            _mact_runnable_tasks_sem = 0;


/*
 * Macrotask stack sizes, and the associated semaphore.
 */
volatile size_t         _mact_stack_size     = 0;
volatile int            _mact_stack_size_sem = 0;
