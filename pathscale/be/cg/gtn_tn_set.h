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


/* =======================================================================
 * =======================================================================
 *
 *  Module: gtn_tn_set.h
 *  $Revision$
 *  $Date$
 *  $Author$
 *  $Source$
 *
 *  Description:
 *  ============
 *
 *  Functions to translate from GTN_SETs (sets over the universe of
 *  globally live TNs) into TN_SETs (sets over the universe of all TNs).
 *
 *  Reserved prefix:
 *  ================
 *
 *      GTN_TN_SET
 *
 *
 *  Exported functions:
 *  ===================
 *
 *
 *      VOID GTN_TN_SET_Print(
 *          GTN_SET     *gtn_set,
 *          FILE        *file
 *      )
 *
 *          Print the given 'gtn_set' to 'file' as a TN_SET (with the
 *          numbers in the TN universe instead of in the GTN universe.)
 *
 *
 * =======================================================================
 * =======================================================================
 */

#ifndef GTN_TN_SET_INCLUDED
#define GTN_TN_SET_INCLUDED

#include "gtn_set.h"

extern void
GTN_TN_SET_Print(
  GTN_SET *gtn_set,
  FILE    *file
);

#endif
