/*

  Copyright (C) 2000, 2001 Silicon Graphics, Inc.  All Rights Reserved.

  This program is free software; you can redistribute it and/or modify it
  under the terms of version 2 of the GNU General Public License as
  published by the Free Software Foundation.

  This program is distributed in the hope that it would be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  

  Further, this software is distributed without any warranty that it is
  free of the rightful claim of any third person regarding infringement 
  or the like.  Any license provided herein, whether implied or 
  otherwise, applies only to this software file.  Patent licenses, if 
  any, provided herein do not apply to combinations of this program with 
  other software, or any other product whatsoever.  

  You should have received a copy of the GNU General Public License along
  with this program; if not, write the Free Software Foundation, Inc., 59
  Temple Place - Suite 330, Boston MA 02111-1307, USA.

  Contact information:  Silicon Graphics, Inc., 1600 Amphitheatre Pky,
  Mountain View, CA 94043, or:

  http://www.sgi.com

  For further information regarding this notice, see:

  http://oss.sgi.com/projects/GenInfo/NoticeExplan

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
