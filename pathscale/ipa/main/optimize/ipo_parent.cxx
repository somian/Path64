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


#include "defs.h"
#include "wn.h"
#include "opcode.h"


/*------------------------------------------------------------------------*/
/* parent pointers for all nodes makes it possible to replace one node    */
/* with another tree node, with ease                                      */
/*------------------------------------------------------------------------*/
extern void WN_Set_Parent (WN* wn, const WN* parent_wn, WN_MAP Parent_Map,
			   WN_MAP_TAB *map_tab) 
{
  IPA_WN_MAP_Set(map_tab, Parent_Map, wn,(void *) parent_wn);
}

/*------------------------------------------------------------------------*/
/* get the parent of wn                                                   */
/*------------------------------------------------------------------------*/
extern WN* WN_Get_Parent (const WN* wn, WN_MAP Parent_Map,
			  WN_MAP_TAB *map_tab)
{
  WN* parent_wn = (WN*) IPA_WN_MAP_Get(map_tab, Parent_Map,(WN*)wn);
  return parent_wn;
}

/*------------------------------------------------------------------------*/
/* set the parents in the side array                                      */
/*------------------------------------------------------------------------*/
extern void WN_Parentize (WN* wn, WN_MAP Parent_Map, WN_MAP_TAB *map_tab) 
{
  if (!OPCODE_is_leaf (WN_opcode (wn))) { 
    if (WN_operator(wn) == OPR_BLOCK) {
      WN* kid = WN_first (wn);
      while (kid) {
        WN_Set_Parent (kid, wn, Parent_Map, map_tab);
        WN_Parentize (kid, Parent_Map, map_tab);
        kid = WN_next (kid);
      }
    }
    else {
      INT kidno;
      WN* kid;
      for (kidno=0; kidno<WN_kid_count(wn); kidno++) {
        kid = WN_kid (wn, kidno);
        if (kid) { 
          WN_Set_Parent (kid, wn, Parent_Map, map_tab);
          WN_Parentize (kid, Parent_Map, map_tab);
        }
      }
    }
  }
}


/*------------------------------------------------------------------------*/
/* Given a tree, check that its parent pointers are consistent.           */
/* Return TRUE if ok, FALSE otherwise.                                    */
/*------------------------------------------------------------------------*/
BOOL WN_Check_Parentize (const WN* wn, WN_MAP Parent_Map, WN_MAP_TAB *map_tab)
{
  BOOL isok = TRUE;
  if (!OPCODE_is_leaf (WN_opcode (wn))) { 
    if (WN_operator(wn) == OPR_BLOCK) {
      WN* kid = WN_first (wn);
      while (kid) {
        Is_True ((WN_Get_Parent (kid, Parent_Map, map_tab) == wn),
                 ("CheckParentize Error: kid 0x%x, parent 0x%x\n", kid, wn));
        isok &= (WN_Get_Parent (kid, Parent_Map,map_tab) == wn);
        if (!isok) break;
        isok &= WN_Check_Parentize (kid, Parent_Map, map_tab);
        if (!isok) break;
        kid = WN_next (kid);
      }
    }
    else {
      INT kidno;
      WN* kid;
      for (kidno=0; kidno<WN_kid_count(wn); kidno++) {
        kid = WN_kid (wn, kidno);
        if (kid) { 
          Is_True ((WN_Get_Parent (kid, Parent_Map, map_tab) == wn),
                   ("CheckParentize Error: kid 0x%x, parent 0x%x\n", kid, wn));
          isok &= (WN_Get_Parent (kid, Parent_Map, map_tab) == wn);
          if (!isok) break;
          isok &= WN_Check_Parentize (kid, Parent_Map, map_tab);
          if (!isok) break;
        }
      }
    }
  }
  return isok;
}








