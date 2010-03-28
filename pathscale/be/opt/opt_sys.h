//-*-c++-*-
// ====================================================================
// ====================================================================
//
// Module: opt_sys.h
// $Revision$
// $Date$
// $Author$
// $Source$
//
// Revision history:
//  09-JAN-94 shin - Original Version
//
// ====================================================================
//
// Copyright (C) 2000, 2001 Silicon Graphics, Inc.  All Rights Reserved.
//
/*
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
//
// ====================================================================
//
// Description:
//
//   Defines all OS dependent function interface
//
// ====================================================================
// ====================================================================


#ifndef opt_sys_INCLUDED
#define opt_sys_INCLUDED "opt_sys.h"
#ifdef _KEEP_RCS_ID
static char *opt_sysrcs_id = opt_sys_INCLUDED"$ $Revision$";
#endif /* _KEEP_RCS_ID */

#define BZERO bzero

//  Return the CPU time in millisecond.
#if defined(BUILD_OS_DARWIN)
/* Darwin/BSD has only 100 ticks per second, so we need to use FP */
#define CLOCK_IN_MS() ((clock_t) (clock() / (CLOCKS_PER_SEC / 1000.0)))
#else /* defined(BUILD_OS_DARWIN) */
#define CLOCK_IN_MS() (clock() / (CLOCKS_PER_SEC / 1000))
#endif /* defined(BUILD_OS_DARWIN) */

#endif
