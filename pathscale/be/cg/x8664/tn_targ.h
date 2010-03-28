/*
 * Copyright 2003, 2004, 2005, 2006 PathScale, Inc.  All Rights Reserved.
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


/* ====================================================================
 * ====================================================================
 *
 *  TN utility routines which include target dependencies.
 *
 *  THIS FILE IS ONLY TO BE INCLUDE BY ../tn.h!!!!
 *
 * ====================================================================
 * ====================================================================
 */

//
// Generate TN to contain a general predicate value
//

#include "targ_sim.h"

inline TN*
Gen_Predicate_TN()
{
  FmtAssert( false, ("No predicate TN for x86.") );
  return Gen_Register_TN(ISA_REGISTER_CLASS_integer, 4);
}

//
// No fcc registers for this arch.
//
inline BOOL TN_is_fcc_register (const TN *tn)
{
  return FALSE;
}

inline TN* Rflags_TN()
{
  return Build_Dedicated_TN( ISA_REGISTER_CLASS_rflags, REGISTER_MIN, 0 );
}

inline TN* Rip_TN()
{
  return Build_Dedicated_TN( ISA_REGISTER_CLASS_rip, REGISTER_MIN, 0 );
}

inline TN* X87_cw_TN()
{
  return Build_Dedicated_TN( ISA_REGISTER_CLASS_x87_cw, REGISTER_MIN, 0 );
}

inline TN* Ebx_TN()
{
  return Build_Dedicated_TN( ISA_REGISTER_CLASS_integer, RBX, 4 );
}
