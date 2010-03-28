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


// This may look like C code, but it is really -*- C++ -*-
//

#ifndef _ara_INCLUDED
#define _ara_INCLUDED

#include "ara_region.h"
#include "ara_loop.h"

// Build the ARA_Loop_Info tree for the program
extern void Perform_ARA_and_Parallelization(PU_Info* current_pu, WN* func_nd);

// Walk the wn tree and process each loop to set the ARA_LOOP_INFO.
extern void ARA_Initialize_Loops(WN* wn, ARA_LOOP_INFO *parent_info);

// Walk the ARA_LOOP_Info tree to compute the array region info
extern void ARA_Walk_Loops(ARA_LOOP_INFO *root_info);

// Cleanup connections of ARA to program tree 
extern void ARA_Cleanup(WN* func_nd);

// Initilize mem pool
extern void ARA_Initialize();

#endif
