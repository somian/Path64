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


#ifndef w2cf_parentize_INCLUDED
#define w2cf_parentize_INCLUDED
/* ====================================================================
 * ====================================================================
 *
 * Module: w2cf_parentize.h
 * $Revision$
 * $Date$
 * $Author$
 * $Source$
 *
 * Revision history:
 *  10-Sept-96 - Original Version
 *
 * Description:
 *  
 *    The W2CF_Parent_Map is defined to be WN_MAP_UNDEFINED in 
 *    w2cf_parentize.c, and must be set by the user of this module
 *    before using any of the utilities (macros or subroutines).
 *
 * ====================================================================
 * ====================================================================
 */

extern WN_MAP W2CF_Parent_Map;


inline const WN *W2CF_Get_Parent(const WN *wn)
{
   return (const WN *)WN_MAP_Get(W2CF_Parent_Map, wn);
} /* W2CF_Get_Parent */

inline void W2CF_Set_Parent(WN *wn, const WN *p)
{
   WN_MAP_Set(W2CF_Parent_Map, wn, (void *)p);
} /* W2CF_Set_Parent */

extern void W2CF_Parentize(const WN* wn);


#endif /* w2cf_parentize */
