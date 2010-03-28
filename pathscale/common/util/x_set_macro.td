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
 * Module: x_set_macro.td
 * $Revision$
 * $Date$
 * $Author$
 * $Source$
 *
 * Revision history:
 *  05-05-93 - Original Version
 *
 * Description:
 *
 *      Template for macro type definition of sets of objects.  This is
 *      processed by common/util/gen_x_set into a particular instantiation
 *      for a particular base type.
 *
 * ====================================================================
 * ====================================================================
 */

BEGIN


#ifndef #uset#_TD_INCLUDED
#define #uset#_TD_INCLUDED

struct bs;
typedef struct bs #uset#;

#endif
