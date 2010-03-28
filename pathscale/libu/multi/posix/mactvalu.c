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


#pragma ident "@(#) libu/multi/posix/mactvalu.c	92.1	06/25/99 14:35:10"

#include "macdefs.h"
#include <stdio.h>
#include <stdlib.h>
#include <thread.h>
#include "mactdefs.h"


/*========================================
 * TSKVALUE
 *
 * What's my user task identifier?
 */
void
tskvalue_(val_ptr)
unsigned int *val_ptr;                  /* Where to put the result        */
{
    volatile mact_task_t
                       *my_mt;          /* Ptr to my macrotask descriptor */

    /*
     * Find my descriptor, and give back whatever is stored
     * there.  If I have no descriptor, give back 0 (zero).
     */
    if ((my_mt = _mact_find_self()) == NULL) {
        *val_ptr = 0;
    }
    else {
        *val_ptr = my_mt->mt_utid;
    }

    /*
     * Done.
     */
    return;
}
