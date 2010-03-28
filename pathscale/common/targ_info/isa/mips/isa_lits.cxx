/*
 * Copyright 2009 PathScale, LLC.  All Rights Reserved.
 */

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
// Generate a list of literal classes and values for the ISA.
/////////////////////////////////////////////////////////

#include "isa_lits_gen.h"

main ()
{
  LIT_RANGE r1to32;

  ISA_Lits_Begin();

  r1to32 = ISA_Create_Lit_Range("1..32", 1, 32);

  ISA_Create_Lit_Class("simm16", SIGNED, SignedBitRange(16), LIT_RANGE_END);
  ISA_Create_Lit_Class("uimm16", UNSIGNED, UnsignedBitRange(16), LIT_RANGE_END);
  ISA_Create_Lit_Class("uimm5", UNSIGNED, UnsignedBitRange(5), LIT_RANGE_END);
  ISA_Create_Lit_Class("pcrel16", SIGNED, SignedBitRange(16), LIT_RANGE_END);
  ISA_Create_Lit_Class("pc26", UNSIGNED, UnsignedBitRange(26), LIT_RANGE_END);
  ISA_Create_Lit_Class("len5", UNSIGNED, r1to32, LIT_RANGE_END);

  ISA_Lits_End();
}
