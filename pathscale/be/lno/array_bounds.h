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


// -*-C++-*-

/**
***  Exported types and functions:
***
***
***	BOOL Tree_Has_Regions(WN* wn_tree)
***
***	  Returns TRUE if the tree rooted at 'wn_tree' has a REGION
***	  node, FALSE otherwise.
***
***	void Hoist_Messy_Bounds(WN* func_nd);
***
***	  Hoist the messy loop bounds for all SNLs in the tree 'func_nd', 
***	  introducing appropriate guard tests where needed to avoid 
***	  speculation. 
**/

extern BOOL Tree_Has_Regions(WN* wn_tree); 
extern void Hoist_Messy_Bounds(WN* func_nd); 
