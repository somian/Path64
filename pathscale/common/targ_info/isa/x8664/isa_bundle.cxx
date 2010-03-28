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
//  Generate ISA bundle information
///////////////////////////////////////

//  $Revision$
//  $Date$
//  $Author$
//  $Source$

#include <stddef.h>
#include "topcode.h"
#include "isa_bundle_gen.h"

main()
{
  ISA_EXEC_UNIT_TYPE
    Fetch_Unit;  // Instruction fetch type

  ISA_Bundle_Begin("x8664", 32);

  ISA_Bundle_Pack_Create(ISA_Bundle_Pack_Little_Endian);
  Pack_Slot(0, 0, 0, 24);

  /* ===== Specification for Fetch_Unit Type ===== */
  Fetch_Unit = ISA_Exec_Unit_Type_Create("Fetch_Unit", NULL);
  Instruction_Exec_Unit_Group(Fetch_Unit,
			      TOP_UNDEFINED);

  ISA_Bundle_Type_Create("i", ".i", 1);
  Slot (0, Fetch_Unit);

  ISA_Bundle_End();
  return 0;
}
