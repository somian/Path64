/*
 * Copyright 2003, 2004, 2005, 2006 PathScale, Inc.  All Rights Reserved.
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


#ifndef ipo_parent_INCLUDED
#define ipo_parent_INCLUDED

#include "defs.h"
#include "wn.h"

/*------------------------------------------------------------------------*/
/* parent pointers for all nodes makes it possible to replace one node    */
/* with another tree node, with ease                                      */
/*------------------------------------------------------------------------*/
extern void WN_Set_Parent (WN* wn, const WN* parent_wn, WN_MAP Parent_Map, 
			   WN_MAP_TAB *map_tab);

extern WN* WN_Get_Parent (const WN* wn, WN_MAP Parent_Map,
			  WN_MAP_TAB *map_tab);

extern void WN_Parentize (WN* wn, WN_MAP Parent_Map,
			  WN_MAP_TAB* map_tab);

extern BOOL WN_Check_Parentize (const WN* wn, WN_MAP Parent_Map,
				WN_MAP_TAB* map_tab);

#endif

