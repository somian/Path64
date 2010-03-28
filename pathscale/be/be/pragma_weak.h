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
 *
 * Module: pragma_weak.c
 * $Revision$
 * $Date$
 * $Author$
 * $Source$
 *
 * Revision history:
 *  4-Jan-96 - Original Version
 *
 * Description:
 *  Header for code linked in with be.so that uses routines defined in
 *  another dso. This header should be the last in the include list
 *  so that the external declaration preceeds it.
 *
 * ====================================================================
 */

#pragma weak Assign_alias_id
#pragma weak Copy_alias_info
#pragma weak Get_symbol_info_for_cvt_io
#pragma weak Points_to
#pragma weak Points_to_copy
#pragma weak Valid_alias
#pragma weak Aliased
