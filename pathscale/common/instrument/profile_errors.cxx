/*
 * Copyright (C) 2007 PathScale, LLC.  All Rights Reserved.
 */

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


// ====================================================================
// ====================================================================
//
// Module: profile_errors.cxx
// $Revision$
// $Date$
// $Author$
// $Source$
//
// Revision history:
//  24-Jul-98 - Original Version
//
// Description:
//
// ====================================================================
// ====================================================================

#include <stdio.h>
#include <stdlib.h>

#include "profile_errors.h"

void profile_error(const char *fmt, char *fname)
{
    fprintf (stderr, "%s: ", "libinstr: ");
    fprintf(stderr, fmt, fname);
    fputc('\n', stderr);
    fflush(stderr);
    exit(1);
} 

void profile_warn(const char *fmt, char *fname)
{
    fprintf (stderr, "%s: ", "libinstr: ");
    fprintf(stderr, fmt, fname);
    fputc('\n', stderr);
    fflush(stderr);
    return;
}
