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


/* This is the data file for the libI77_s.a library */
#include <cmplrs/fio.h>
#include "fmt.h"

unit           *f77units = 0;	/* unit table *//* sjc #1963 11Dec87 */
int             mxunit = 0;	/* size of unit table */
flag            f77init = 0;	/* 0 on entry, 1 after initializations */
unit           *f77curunit = 0;	/* current unit for single-process I/O */
XINT             icnum = 0, icpos = 0;
char           *icptr = 0, *icend = 0;
int             space_assigned = 0;
ftnint		errluno;
