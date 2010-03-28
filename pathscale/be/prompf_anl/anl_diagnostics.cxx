/*

  Copyright (C) 2000, 2001 Silicon Graphics, Inc.  All Rights Reserved.

   Path64 is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

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


/* -*-Mode: c++;-*- (Tell emacs to use c++ mode) */

#include <errno.h>		    /* for sys_errlist */
#include <stdio.h>

#include "defs.h"            // Basic types, such as INT32

#include "anl_diagnostics.h"


void 
ANL_DIAGNOSTICS::Warning(const char *msg)
{
   fprintf(_diag_file, "Warning: %s\n", msg);
} // ANL_DIAGNOSTICS::Warning


void 
ANL_DIAGNOSTICS::Error(const char *msg)
{
   fprintf(_diag_file, "Error: %s\n", msg);
   _error_condition = TRUE;
} // ANL_DIAGNOSTICS::Error


void 
ANL_DIAGNOSTICS::Fatal(const char *msg)
{
   fprintf(_diag_file, "Fatal: %s\n", msg);
   exit(1);
} // ANL_DIAGNOSTICS::Fatal

