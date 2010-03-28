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


/* ====================================================================
 * ====================================================================
 *
 * Module: w2cf_parentize.c
 * $Revision$
 * $Date$
 * $Author$
 * $Source$
 *
 * Revision history:
 *  10-Sept-96 - Original Version
 *
 * ====================================================================
 * ====================================================================
 */

#ifdef _KEEP_RCS_ID
static char *rcs_id = "$Source$ $Revision$";
#endif /* _KEEP_RCS_ID */


#include "common_include.h" /* For defs.h, config.h, erglob.h, etc. */
#include "w2cf_parentize.h"


WN_MAP W2CF_Parent_Map = WN_MAP_UNDEFINED;


void 
W2CF_Parentize(const WN* wn)
{
   /* Given a tree, initialize its parent pointers.
    * Override what was there, if anything.
    * Do not update parent pointer of the root node 'wn'.
    * This is copied from be/lno/lwn_util.h!
    */
   if (!OPCODE_is_leaf (WN_opcode (wn)))
   { 
      if (WN_opcode(wn) == OPC_BLOCK)
      {
	 WN *kid = WN_first(wn);
	 while (kid) {
	    W2CF_Set_Parent(kid, wn);
	    W2CF_Parentize(kid);
	    kid = WN_next(kid);
	 }
      }
      else {
	 INT kidno;
	 WN *kid;
	 for (kidno=0; kidno < WN_kid_count(wn); kidno++) {
	    kid = WN_kid (wn, kidno);
	    if (kid) { 
	       W2CF_Set_Parent(kid, wn);
	       W2CF_Parentize(kid);
	    }
	 }
      }
   }
} /* W2FC_Parentize */
