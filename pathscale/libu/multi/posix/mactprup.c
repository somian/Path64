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


#pragma ident "@(#) libu/multi/posix/mactprup.c	92.1	06/25/99 14:35:10"

#include "macdefs.h"
#include <stdio.h>
#include <stdlib.h>
#include <thread.h>

#if defined(ASSEMBLY)
#   undef ASSEMBLY
#endif
#include "mactdefs.h"


/*
 * Shared semaphore for all the routines in this module.
 */
static int              prup_sem = 0;


/*========================================
 * ISELFSCH_
 *
 * Do "ivar = (jvar = ivar) + 1", single-threaded.
 */
int
iselfsch_(ivar)
int                    *ivar;
{
    int                 jvar;

    _mact_psem(&prup_sem);                                              /*CRIT*/
    {                                                                   /*CRIT*/
        jvar   = *ivar;                                                 /*CRIT*/
        *ivar += 1;                                                     /*CRIT*/
    }                                                                   /*CRIT*/
    _mact_vsem(&prup_sem);                                              /*CRIT*/

    return(jvar);
}


/*========================================
 * ISELFADD_
 *
 * Do "ivar = (jvar = ivar) + ivalue", single-threaded.
 */
int
iselfadd_(ivar, ivalue)
int                    *ivar;
int                    *ivalue;
{
    int                 jvar;

    _mact_psem(&prup_sem);                                              /*CRIT*/
    {                                                                   /*CRIT*/
        jvar   = *ivar;                                                 /*CRIT*/
        *ivar += *ivalue;                                               /*CRIT*/
    }                                                                   /*CRIT*/
    _mact_vsem(&prup_sem);                                              /*CRIT*/

    return(jvar);
}


/*========================================
 * ICRITADD_
 *
 * Do "ivar = ivar + ivalue", single-threaded.
 */
void
icritadd_(ivar, ivalue)
int                    *ivar;
int                    *ivalue;
{

    _mact_psem(&prup_sem);                                              /*CRIT*/
    {                                                                   /*CRIT*/
        *ivar += *ivalue;                                               /*CRIT*/
    }                                                                   /*CRIT*/
    _mact_vsem(&prup_sem);                                              /*CRIT*/

    return;
}


/*========================================
 * ISELFMUL_
 *
 * Do "ivar = (jvar = ivar) * ivalue", single-threaded.
 */
int
iselfmul_(ivar, ivalue)
int                    *ivar;
int                    *ivalue;
{
    int                 jvar;

    _mact_psem(&prup_sem);                                              /*CRIT*/
    {                                                                   /*CRIT*/
        jvar   = *ivar;                                                 /*CRIT*/
        *ivar *= *ivalue;                                               /*CRIT*/
    }                                                                   /*CRIT*/
    _mact_vsem(&prup_sem);                                              /*CRIT*/

    return(jvar);
}


/*========================================
 * ICRITMUL_
 *
 * Do "ivar = ivar * ivalue", single-threaded.
 */
void
icritmul_(ivar, ivalue)
int                    *ivar;
int                    *ivalue;
{

    _mact_psem(&prup_sem);                                              /*CRIT*/
    {                                                                   /*CRIT*/
        *ivar *= *ivalue;                                               /*CRIT*/
    }                                                                   /*CRIT*/
    _mact_vsem(&prup_sem);                                              /*CRIT*/

    return;
}


/*========================================
 * XSELFADD_
 *
 * Do "xvar = (var = xvar) + xvalue", single-threaded.
 */
float
xselfadd_(xvar, xvalue)
float                  *xvar;
float                  *xvalue;
{
    float               var;

    _mact_psem(&prup_sem);                                              /*CRIT*/
    {                                                                   /*CRIT*/
        var    = *xvar;                                                 /*CRIT*/
        *xvar += *xvalue;                                               /*CRIT*/
    }                                                                   /*CRIT*/
    _mact_vsem(&prup_sem);                                              /*CRIT*/

    return(var);
}


/*========================================
 * XCRITADD_
 *
 * Do "xvar = xvar + xvalue", single-threaded.
 */
void
xcritadd_(xvar, xvalue)
float                  *xvar;
float                  *xvalue;
{

    _mact_psem(&prup_sem);                                              /*CRIT*/
    {                                                                   /*CRIT*/
        *xvar += *xvalue;                                               /*CRIT*/
    }                                                                   /*CRIT*/
    _mact_vsem(&prup_sem);                                              /*CRIT*/

    return;
}


/*========================================
 * XSELFMUL_
 *
 * Do "xvar = (var = xvar) * xvalue", single-threaded.
 */
float
xselfmul_(xvar, xvalue)
float                  *xvar;
float                  *xvalue;
{
    float               var;

    _mact_psem(&prup_sem);                                              /*CRIT*/
    {                                                                   /*CRIT*/
        var    = *xvar;                                                 /*CRIT*/
        *xvar *= *xvalue;                                               /*CRIT*/
    }                                                                   /*CRIT*/
    _mact_vsem(&prup_sem);                                              /*CRIT*/

    return(var);
}


/*========================================
 * XCRITMUL_
 *
 * Do "xvar = xvar * xvalue", single-threaded.
 */
void
xcritmul_(xvar, xvalue)
float                  *xvar;
float                  *xvalue;
{

    _mact_psem(&prup_sem);                                              /*CRIT*/
    {                                                                   /*CRIT*/
        *xvar *= *xvalue;                                               /*CRIT*/
    }                                                                   /*CRIT*/
    _mact_vsem(&prup_sem);                                              /*CRIT*/

    return;
}
