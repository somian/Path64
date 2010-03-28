/*
 *  Copyright (C) 2008 PathScale, LLC.  All Rights Reserved.
 */

/*
 * Copyright 2004 PathScale, Inc.  All Rights Reserved.
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


#include "defs.h"
#include "errors.h"
#include "config_targ.h"
#include "config_platform.h"

void Fill_Align_Initialize_Parameters(INT *L1_sz, INT *L2_sz, INT *pg_sz)
{
  INT L1_cache_line_sz;
  INT L2_cache_line_sz;

  switch (Target) {

    case TARGET_mips5kf:
      L1_cache_line_sz = 32;
      L2_cache_line_sz = 64;
      break;

    case TARGET_twc9a:
      L1_cache_line_sz = 64;
      L2_cache_line_sz = 64;
      break;

    default:
      L1_cache_line_sz = 32;
      L2_cache_line_sz = 128;
      break;
  }
  
  *L1_sz = L1_cache_line_sz;
  *L2_sz = L2_cache_line_sz;
  *pg_sz = 16384;
}

