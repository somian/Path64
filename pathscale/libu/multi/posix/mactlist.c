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


#pragma ident "@(#) libu/multi/posix/mactlist.c	92.1	06/25/99 14:35:10"

#include "macdefs.h"
#include <stdio.h>
#include <stdlib.h>
#include <thread.h>
#include <malloc.h>
#include "mactdefs.h"


/*========================================
 * TSKSTAT
 *
 * Record information about all existing tasks while holding the
 * macrotask descriptor free list semaphore, yielding a coherent
 * view of the current situation.  The only oddity here is that
 * if we're unable to acquire memory to hold the descriptions,
 * we return -1 for the task count.
 */
static void
tskstat(num_tasks_addr, info_paddr)
int                    *num_tasks_addr; /* Where to put the number of tasks  */
unsigned int          **info_paddr;     /* Where to put the information      */
{
    unsigned int       *info,           /* Static info about the tasks       */
                       *mtids,          /* Mtids pointer into info[]         */
                       *states,         /* States pointer into info[]        */
                       *stids,          /* Stids pointer into info[]         */
                       *utids,          /* Utids pointer into info[]         */
                       *wobjs,          /* Wobjs pointer into info[]         */
                       *wcounts;        /* Wcounts pointer into info[]       */
    int                 num_tasks;      /* Estimated number of tasks         */
    unsigned int        mh_idx;         /* Index into _mact_hash[]           */
    volatile mact_task_t
                       *mt;             /* Ptr to some task's descriptor     */
    volatile mact_task_t
                       *mt_w;           /* Ptr to some waitee task's desc    */
    mact_lock_t        *ml_w;           /* Ptr to some waitee lock's desc    */
    mact_event_t       *me_w;           /* Ptr to some waitee event's desc   */
    mact_barrier_t     *mb_w;           /* Ptr to some waitee barrier's desc */

    _mact_psem(&_mact_free_sem);                                        /*CRIT*/
    {                                                                   /*CRIT*/
                                                                        /*CRIT*/
#       if defined(DEBUG)                                               /*CRIT*/
            mt_dump_();                                                 /*CRIT*/
#       endif                                                           /*CRIT*/
                                                                        /*CRIT*/
        /*                                */                            /*CRIT*/
        /* Get space for the information. */                            /*CRIT*/
        /*                                */                            /*CRIT*/
        if ((num_tasks = _mact_total_count - _mact_free_count) <= 0     /*CRIT*/
           ) {                                                          /*CRIT*/
            *num_tasks_addr = 0;                                        /*CRIT*/
            goto release_semaphore;  /* At critsec end */               /*CRIT*/
        }                                                               /*CRIT*/
                                                                        /*CRIT*/
        if (   (info = (unsigned int *)                                 /*CRIT*/
                       malloc(6 * num_tasks * sizeof(unsigned int))     /*CRIT*/
               )                                                        /*CRIT*/
            == NULL                                                     /*CRIT*/
           ) {                                                          /*CRIT*/
            *num_tasks_addr = -1;                                       /*CRIT*/
            goto release_semaphore;  /* At critsec end */               /*CRIT*/
        }                                                               /*CRIT*/
                                                                        /*CRIT*/
        /*                          */                                  /*CRIT*/
        /* Fill in the information. */                                  /*CRIT*/
        /*                          */                                  /*CRIT*/
        mtids   = &info[0];                                             /*CRIT*/
        states  = &info[1 * num_tasks];                                 /*CRIT*/
        stids   = &info[2 * num_tasks];                                 /*CRIT*/
        utids   = &info[3 * num_tasks];                                 /*CRIT*/
        wobjs   = &info[4 * num_tasks];                                 /*CRIT*/
        wcounts = &info[5 * num_tasks];                                 /*CRIT*/
        for (mh_idx = 0; mh_idx < MTHASH_SIZE; mh_idx++) {              /*CRIT*/
            for (mt = _mact_hash[mh_idx];                               /*CRIT*/
                 mt != NULL;                                            /*CRIT*/
                 mt = mt->mt_link                                       /*CRIT*/
                ) {                                                     /*CRIT*/
                *mtids++       = mt->mt_mtid;                           /*CRIT*/
                *states++      = (unsigned int) mt->mt_state;           /*CRIT*/
                *stids++       = (unsigned int) mt->mt_stid;            /*CRIT*/
                *utids++       = mt->mt_utid;                           /*CRIT*/
                if (mt->mt_state      == TS_Running) {                  /*CRIT*/
                    *wobjs++   = (unsigned int) mt->mt_stid;            /*CRIT*/
                    *wcounts++ = 0;                                     /*CRIT*/
                }                                                       /*CRIT*/
                else if (mt->mt_state == TS_Lock_Wait) {                /*CRIT*/
                    ml_w               = (mact_lock_t *)                /*CRIT*/
                                         mt->mt_wait_obj;               /*CRIT*/
                    *wobjs++   = (unsigned int) ml_w;                   /*CRIT*/
                    *wcounts++ = ml_w->ml_wcount;                       /*CRIT*/
                }                                                       /*CRIT*/
                else if (mt->mt_state == TS_Event_Wait) {               /*CRIT*/
                    me_w               = (mact_event_t *)               /*CRIT*/
                                         mt->mt_wait_obj;               /*CRIT*/
                    *wobjs++   = (unsigned int) me_w;                   /*CRIT*/
                    *wcounts++ = me_w->me_wcount;                       /*CRIT*/
                }                                                       /*CRIT*/
                else if (mt->mt_state == TS_Bar_Wait) {                 /*CRIT*/
                    mb_w               = (mact_barrier_t *)             /*CRIT*/
                                         mt->mt_wait_obj;               /*CRIT*/
                    *wobjs++   = (unsigned int) mb_w;                   /*CRIT*/
                    *wcounts++ = mb_w->mb_ocount - mb_w->mb_ncount;     /*CRIT*/
                }                                                       /*CRIT*/
                else if (mt->mt_state == TS_Task_Wait) {                /*CRIT*/
                    mt_w               = (mact_task_t *)                /*CRIT*/
                                         mt->mt_wait_obj;               /*CRIT*/
                    *wobjs++   = (unsigned int) mt_w->mt_mtid;          /*CRIT*/
                    *wcounts++ = mt_w->mt_wait_count;                   /*CRIT*/
                }                                                       /*CRIT*/
                else {                                                  /*CRIT*/
                    *wobjs++   = 0;                                     /*CRIT*/
                    *wcounts++ = 0;                                     /*CRIT*/
                }                                                       /*CRIT*/
            }                                                           /*CRIT*/
        }                                                               /*CRIT*/
        *num_tasks_addr = (mtids - info);                               /*CRIT*/
        *info_paddr     = &info[0];                                     /*CRIT*/
    }                                                                   /*CRIT*/
                                                                        /*CRIT*/
    /*                                          */                      /*CRIT*/
    /* Common goto target for various failures. */                      /*CRIT*/
    /*                                          */                      /*CRIT*/
release_semaphore:                                                      /*CRIT*/
    _mact_vsem(&_mact_free_sem);                                        /*CRIT*/

    /*
     * Done.
     */
    return;
}


/*========================================
 * TSKLIST
 *
 * List the tasks.
 */
void
tsklist_(void)
{
    unsigned int       *info,           /* Static info about the tasks */
                       *mtids,          /* Mtids pointer into info[]   */
                       *states,         /* States pointer into info[]  */
                       *stids,          /* Stids pointer into info[]   */
                       *utids,          /* Utids pointer into info[]   */
                       *wobjs,          /* Wobjs pointer into info[]   */
                       *wcounts;        /* Wcounts pointer into info[] */
    int                 num_tasks;      /* Estimated number of tasks   */

    extern void        _MACT_DO_TSKLIST /* I/O routine, in Fortran-90  */
                       (int          *,
                        unsigned int *,
                        unsigned int *,
                        unsigned int *,
                        unsigned int *,
                        unsigned int *,
                        unsigned int *
                       );

    /*
     * Grab the information.
     */
    tskstat(&num_tasks, &info);
    if (num_tasks < 0) {
        perror("Could not get memory for TSKLIST listing");
    }

    /*
     * Report it.
     */
    _MACT_DO_TSKLIST(&num_tasks,
                     &info[0],
                     &info[1 * num_tasks],
                     &info[2 * num_tasks],
                     &info[3 * num_tasks],
                     &info[4 * num_tasks],
                     &info[5 * num_tasks]
                    );

    /*
     * Release the memory we grabbed.
     */
    if (num_tasks > 0) {
        free((void *) info);
    }

    /*
     * Done.
     */
    return;
}
