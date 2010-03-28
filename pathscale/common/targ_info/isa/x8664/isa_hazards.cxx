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
//  Generate ISA hazard information
//  
//  Generate ISA hazard information
//
//  Note:  A "hazard" is a conflict between instructions which may
//  cause unexpected behavior if the instructions appear at a forbidden
//  distance in the instruction stream.  That is, the hardware does not
//  enforce the required separation, and the software is responsible
//  for avoiding such situations.  Most are avoided via the resource
//  mechanism.  This file handles special cases.
//
///////////////////////////////////////////////////////////////////////

//  $Revision$
//  $Date$
//  $Author$
//  $Source$


#include <stddef.h>
#include "topcode.h"
#include "targ_isa_subset.h"
#include "isa_hazards_gen.h"


main()
{
  ISA_HAZARD
    result,
    operand,
    errata;

  ISA_Hazards_Begin( "x8664" );

  result = Hazard_Create("result");
  operand = Hazard_Create("operand");
  errata = Hazard_Create("errata");

  ISA_Hazards_End();
  return 0;
}
