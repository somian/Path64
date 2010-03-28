/*
Copyright 2004, 2005, 2006 PathScale Inc.  All Rights Reserved.

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



#pragma ident "@(#) libf/fio/fnum.c	92.2	08/02/99 10:38:18"
 

#include <stdio.h>
#include <foreign.h>
#include <errno.h>
#include <liberrno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "fio.h"

#ifdef KEY /* Bug 1683 */
/* Don't pollute the Fortran namespace */

int pathf90_fnum(int *u)

#else

extern int fnum_(int *u);

int fnum_(int *u)

#endif /* KEY Bug 1683 */
{
        int             n, retval;
        unum_t          unum; 
        unit            *cup;
        struct stat     buf;
        struct fiostate cfs;
        int     stat;
        register int    errf;           /* ERR processing flag  */

        unum    = *u;
        retval  = -1;

        if (unum != 0 && unum != 5 && unum != 6)
          return retval;

        STMT_BEGIN(unum, 0, T_INQU, NULL, &cfs, cup);
                                                                                                                                                             
        if (cup == NULL && !GOOD_UNUM(unum))
                _ferr(&cfs, FEIVUNIT, unum);    /* invalid unit number */
                                                                                                                                                             
        if (cup == NULL) 
                 cup     = _imp_open(    &cfs,
                                         SEQ,
                                         FMT,
                                         unum,
                                         errf,
                                         &stat);

        retval      = fileno ( cup->ufp.std );
        STMT_END(cup, T_INQU, NULL, &cfs);      /* unlock the unit */
        return retval;
}

