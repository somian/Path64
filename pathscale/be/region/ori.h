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
 * Module: ori.h
 * $Revision$
 * $Date$
 * $Author$
 * $Source$
 *
 * Description:
 * Olimit Region Insertion (ORI)
 *
 * ====================================================================
 * ====================================================================
 */

#ifndef ori_INCLUDED
#define ori_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

extern WN * Olimit_Region_Insertion (WN *pu_tree, INT32 olimit);

#ifdef __cplusplus
}
#endif

#endif /* ori_INCLUDED */
