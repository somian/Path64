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


#pragma ident "@(#) libu/multi/posix/mactwait.c	92.1	06/25/99 14:35:10"

#include "macdefs.h"
#include <stdio.h>
#include <stdlib.h>
#include <thread.h>
#include "mactdefs.h"


/*========================================
 * TSKWAIT
 *
 * Wait for a macrotask to complete.
 */
void
tskwait_(tca)
unsigned int tca[];                     /* Task Control Array               */
{
    volatile mact_task_t
                       *mt,             /* Ptr to some macrotask descriptor */
                       *my_mt;          /* Ptr to my macrotask descriptor   */
    unsigned int        utid;           /* User task ID from TCA            */
    int                 do_return;      /* Return after releasing lock(s)?  */

    /*
     * Find the macrotask.
     */
    mt  = (volatile mact_task_t *) tca[1];
    utid = (tca[0] < 3) ? 0 : tca[2];
    if (   mt->mt_state == TS_Nonexistent
        || mt->mt_state == TS_Complete
        || mt->mt_utid  != utid
       ) {
        /*
         * Either no such task, or it's complete -- we're done.
         */
        return;
    }

    /*
     * Lock the free list and check it again.
     */
    do_return = 0;
    _mact_psem(&_mact_free_sem);                                        /*CRIT*/
    {                                                                   /*CRIT*/
        if (   mt->mt_state == TS_Nonexistent                           /*CRIT*/
            || mt->mt_state == TS_Complete                              /*CRIT*/
            || mt->mt_utid  != utid                                     /*CRIT*/
           ) {                                                          /*CRIT*/
            /*                                 */                       /*CRIT*/
            /* It completed since we found it. */                       /*CRIT*/
            /*                                 */                       /*CRIT*/
            do_return = 1;                                              /*CRIT*/
        }                                                               /*CRIT*/
        else {                                                          /*CRIT*/
            _mact_psem(&mt->mt_sem);                                   /*CCRIT*/
            {                                                          /*CCRIT*/
                if (   mt->mt_state == TS_Nonexistent                  /*CCRIT*/
                    || mt->mt_state == TS_Complete                     /*CCRIT*/
                    || mt->mt_utid  != utid                            /*CCRIT*/
                   ) {                                                 /*CCRIT*/
                    /*                                 */              /*CCRIT*/
                    /* It completed since we found it. */              /*CCRIT*/
                    /*                                 */              /*CCRIT*/
                    do_return = 1;                                     /*CCRIT*/
                }                                                      /*CCRIT*/
                else {                                                 /*CCRIT*/
                    /*                                     */          /*CCRIT*/
                    /* Add us to the list of tasks waiting */          /*CCRIT*/
                    /* for this one.                       */          /*CCRIT*/
                    /*                                     */          /*CCRIT*/
                    my_mt               = _mact_find_self();           /*CCRIT*/
                    my_mt->mt_wait_link = mt->mt_waiters;              /*CCRIT*/
                    mt->mt_waiters      = (volatile void *) my_mt;     /*CCRIT*/
                    mt->mt_wait_count++;                               /*CCRIT*/
                    my_mt->mt_wait_obj  = mt;                          /*CCRIT*/
                    my_mt->mt_state     = TS_Task_Wait;                /*CCRIT*/
                }                                                      /*CCRIT*/
            }                                                          /*CCRIT*/
            _mact_vsem(&mt->mt_sem);                                   /*CCRIT*/
        }                                                               /*CRIT*/
    }                                                                   /*CRIT*/
    _mact_vsem(&_mact_free_sem);                                        /*CRIT*/

    /*
     * If the task has already completed, then we're done.
     */
    if (do_return)
        return;

    /*
     * Otherwise, wait for it to complete.  It will change our
     * state changes back to "running" when it completes.
     */
    DEC_RUNNABLE_MACT_COUNT();
    _mact_wait_for_me_to_be_running(my_mt);

    /*
     * Done.
     */
    return;
}
