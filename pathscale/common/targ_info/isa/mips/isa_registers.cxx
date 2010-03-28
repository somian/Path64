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
//  Generate ISA registers information
///////////////////////////////////////


//  $Revision: 1.4 $
//  $Date: 05/03/02 15:27:56-08:00 $
//  $Author: bos@eng-24.internal.keyresearch.com $
//  $Source: common/targ_info/isa/mips/SCCS/s.isa_registers.cxx $

#include <stddef.h>
#include "isa_registers_gen.h"
#include "targ_isa_subset.h"


static const char *int_reg_names[32] = {
    NULL,   NULL,   NULL,   NULL,   NULL,   NULL,   NULL,   NULL,
    NULL,   NULL,   NULL,   NULL,   NULL,   NULL,   NULL,   NULL,
    NULL,   NULL,   NULL,   NULL,   NULL,   NULL,   NULL,   NULL,
    NULL,   NULL,   NULL,   NULL,   "$gp",  "$sp",  NULL,   NULL,
};

static int ISA_Mask(ISA_SUBSET isa)
{
  return 1 << (int)isa;
}


static int All_ISA_Mask(void)
{
  int i;
  int mask = 0;
  for (i = ISA_SUBSET_MIN; i <= ISA_SUBSET_MAX; ++i) {
    mask |= 1 << i;
  }
  return mask;
}


static int Range_ISA_Mask(ISA_SUBSET min_isa, ISA_SUBSET max_isa)
{
  int i;
  int mask = 0;
  for (i = (int)min_isa; i <= (int)max_isa; ++i) {
    mask |= 1 << i;
  }
  return mask;
}


main (int argc, char** argv)
{
  ISA_Registers_Begin("MIPS");

  ISA_REGISTER_CLASS rc_integer = ISA_Register_Class_Create("integer", 64, true, false);
  ISA_REGISTER_CLASS rc_float = ISA_Register_Class_Create("float", 64, true, false);

  ISA_REGISTER_CLASS rc_fcc = ISA_Register_Class_Create("fcc", 1, false, false);

  ISA_REGISTER_CLASS rc_hilo = ISA_Register_Class_Create("hilo", 128, false, false);

  ISA_Register_Set(rc_integer, 0, 31, "$%u", int_reg_names, All_ISA_Mask());
  ISA_Register_Set(rc_float, 0, 31, "$f%u", NULL, All_ISA_Mask());
  ISA_Register_Set(rc_fcc, 0, 7, "$fcc%u", NULL, All_ISA_Mask());
  ISA_Register_Set(rc_hilo, 0, 0, "$hilo", NULL, All_ISA_Mask());

  ISA_Registers_End();
}
