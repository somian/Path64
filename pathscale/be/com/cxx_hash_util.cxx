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


// -*-C++-*-
// ====================================================================
// ====================================================================
//
// Module: cxx_hash_util.cxx
// $Revision$
// $Date$
// $Author$
// $Source$
//
// Revision history:
//  07-Dec-95 - Merged user-hash version from IPA
//
// Description:
//
// Non-template support for template hash map implementations in
// cxx_hash.h/cxx_hash.cxx.
//
// ====================================================================
// ====================================================================

#ifdef _KEEP_RCS_ID
#define cxx_hash_util_CXX      "cxx_hash_util.cxx"
static char *rcs_id = cxx_hash_util_CXX" $Revision$";
#endif /* _KEEP_RCS_ID */

#include "defs.h"
#include "erglob.h"
#include "cxx_hash.h"

// ====================================================================
// ====================================================================
//
// String hash table support:
//
// Borrows from Bob Fraley's hash map STL implementation...
//
// ====================================================================
// ====================================================================

// Utility table for hashing:
static HASH Init_Hashing_Table();
static HASH Hashing_Table[256];
static HASH _dummy_var = Init_Hashing_Table();

// Initialize a "random" table for hashing function:
static HASH
Init_Hashing_Table ( void )
{
  // randomize(); //
  unsigned i;
  for ( i = 0; i < 256; i++ ) {
    Hashing_Table[i] = i | (i << 8);   // >>> i for orig algorithm <<<
  }
  for ( i = 0; i < 256; ++i ) {
    HASH r = (rand() >> 5) & 255;
    HASH xchg = Hashing_Table[i];
    Hashing_Table[i] = Hashing_Table[r];
    Hashing_Table[r] = xchg;
  }
  return 1;
}

// Modified hash function:
HASH
String_Hash::operator() ( const char *  k ) const
{
  HASH res = 0;

  while ( *k ) {
    res = Hashing_Table[(res & 255) ^ *k++] ^ (res << 3);
  }
  return res ;
}

