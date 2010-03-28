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


//
// Generate instruction decoding information.
/////////////////////////////////////
/////////////////////////////////////
//
//  $Revision$
//  $Date$
//  $Author$
//  $Source$

#include "topcode.h"
#include "isa_decode_gen.h"
#include "targ_isa_bundle.h"
 
main()
{
  ISA_Decode_Begin("x8664");

  STATE unit = Create_Unit_State("unit", 0, 3);
  Initial_State(unit);

  ISA_Decode_End();
  return 0;
}
