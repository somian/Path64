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


#pragma ident "@(#) libu/multi/posix/mactcutil.c	92.1	06/25/99 14:35:10"

#include "macdefs.h"
#include <stdio.h>
#include <stdlib.h>
#include <thread.h>

#if defined(ASSEMBLY)
#   undef ASSEMBLY
#endif
#include "mactdefs.h"


/*========================================
 * _MACT_WAIT_FOR_ME_TO_BE_RUNNING
 *
 * Wait for a lock to be unlocked, an event to be posted, a
 * barrier to fill, or a task to complete.  Also, identify
 * and report deadlock.
 */
void
_mact_wait_for_me_to_be_running(volatile mact_task_t *my_mt)
{

    /*
     * If nobody is ever going to wake us up, then let's just
     * give up now.
     */
    CHECK_FOR_MACT_DEADLOCK();

    /*
     * When the thing I'm waiting for occurs, the other task
     * involved will change my state to "running".  Wait for
     * that to happen.
     */
    while (my_mt->mt_state != TS_Running)
        ;

    return;
}
