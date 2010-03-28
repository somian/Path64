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


// This file is really -*- C++ -*-
#ifndef shackle_if_INCLUDED
#define shackle_if_INCLUDED "shackle_if.h"


BOOL
analyze_stmts_in_func_for_if (WN *func_nd);
void
Convert_Do_Loops_Conditionals(WN *func_nd);
void      
shackle_if_init(MEM_POOL *);
void      
shackle_if_finalize();

enum SHACKLE_BOUND {
  SHACKLE_LOWER,
  SHACKLE_UPPER,
  SHACKLE_BOTH
};


#endif
