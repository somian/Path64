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


#ifndef HB_CFLOW_H_INCLUDED
#define HB_CFLOW_H_INCLUDED
// This file contains interface routines between CFLOW and hyperblock formation
// These routines are mainly to deal with what canb happen when blocks are merged or deleted.


/////////////////////////////////////
//  Replace block old_bb with block new_bb, keeping hyperblock strcutures
//  up to date.
/////////////////////////////////////
extern void HB_CFLOW_Replace_Block(BB *old_bb, BB *new_bb);

////////////////////////////////////////////////////////////////
//
// Test to make sure that merging two BBs would not break the hyperblock data structures
//
//////////////////////////////////////////////////////////////////
BOOL HB_CFLOW_Can_Merge_BBs(BB *bb1, BB *bb2);

////////////////////////////////////////////////////////////////
//
// Remove a block from the hyperblocks list
//
////////////////////////////////////////////////////////////////
extern void HB_CFLOW_Remove_Block(BB *bb);

//
// Also defined in hb.h
//
extern void Setup_HB_bb_map(void);

#endif
