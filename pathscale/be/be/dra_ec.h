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


// ====================================================================
// ====================================================================
//
// Module: dra_ec.h
//
// Revision history:
//  25-Aug-96: Original Version
//
// Description: External interface for error checking of reshaped 
//              array portions passed as function arguments.
// 
// ====================================================================
// ====================================================================


// Global variables that contain high-level types for the
// run-time structure used in error-checking of reshaped
// array portions apssed as function arguments. These are
// defined in be/be/dra_ec.cxx and also used in be/lno/lego_ec.cxx
//

extern TY_IDX DRA_EC_struct_ptr_ty;


// Field offsets for the structure used in run-time error checking
// of reshaped array portions passed as function arguments
//
#define CART_array_base      0
#define CART_ndims           8
#define CART_element_size   16
#define CART_index          24
