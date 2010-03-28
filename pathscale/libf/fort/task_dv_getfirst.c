/*

  Copyright (C) 2000, 2001, Silicon Graphics, Inc.  All Rights Reserved.

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



#pragma ident "@(#) libf/fort/task_dv_getfirst.c	92.1	06/24/99 10:18:36"
#include <liberrno.h>
#include <stddef.h>

/*
 *  Error handler for an array-allocation/pointer-association
 *  error when tasking is on.
 *  This entry is called by the cf90 compiler only.
 *
 *  Input Arguments:
 *    file     - File name in which error occurred.
 *    line     - Line number in file.
 *    variable - variable name of allocatable array or Fortran pointer.
 */

void
_TASK_DV_GETFIRST_ERROR(
        char    *variable,      /* allocatable array or Fortran pointer */
        char    *file,          /* Fortran routine containing error */
        int     *lineno)        /* Line number in Fortran routine */
{
        (void) _lerror(_LELVL_ABORT, FENGFLCL, variable, file, *lineno);
        return;
}
