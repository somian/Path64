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


//
//  Tail duplication module of hyperblock formation.
//
/////////////////////////////////////
//
//  Algorithm description:
//
//  Duplicate blocks in order to remove side entrances to the hyperblock.
//  Blocks will only be duplicated once, regardless of how many side
//  entrances may reach them.
//
//  Ultimately, we'd like to be a little smarter about how we tail duplicate
//  so as to avoid unnecessary impacts on the I-cache.  For now, however,
//  it is not yet clear how best to do this so we are going to tail duplicate
//  unconditionally.
//
/////////////////////////////////////
//
//  Externally visible routines:
//
//	BOOL HB_Tail_Duplicate(HB* hb, BB_MAP duplicate,
//                             BOOL post_tail_duplication)
//	  Perform tail duplication on each hyperblock.
//
/////////////////////////////////////

#ifndef HB_TAIL_DUPLICATION_H_INCLUDED
#define HB_TAIL_DUPLICATION_H_INCLUDED

#include "hb.h"

extern BOOL HB_Tail_Duplicate(HB* hb, BB_MAP duplicate,
	                      HB_bb_list& duplicate_bbs,
                              BOOL post_tail_duplication);

#endif





