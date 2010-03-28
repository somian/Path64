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


// ====================================================================
// ====================================================================
//
// Module: ipa_feedback.cxx
//
// Revision history:
//  03-Sep-97 - Original Version
//
// Description:
//
// Create an output file(s) containing useful information derived during 
// various phases of Interprocedural Analysis. These files contain very
// helpful IPA-derived info such as: 
//
//    o  global variable and function names whose access can be optimized 
//       via compiler directives (e.g. #pragma section_gp, #pragma hidden,
//       #pragma internal, #pragma inline, #pragma noinline ).
//
//    o  names of functions that IPA analysis found to be 'dead' ( but not
//       due to inlining ). Such functions are never called or they are
//       called within other functions which are in turn never called, etc
//
//    o  names of global variables that IPA analysis found to be unused,
//       e.g. written but not read, etc.
//       
// ====================================================================
// ====================================================================


#include "defs.h"
#include "cxx_memory.h"
#include "cxx_hash.h"
#include "erglob.h"
#include "glob.h"
#include "mempool.h"
#include "tracing.h"
#include "strtab.h"
#include "stab.h"
#include "ipa_feedback.h"

extern IPA_FEEDBACK_STRINGS *IPA_Fbk_Strings;

// Wrapper routine to allow C programs in ipa/common to access string hashing
extern "C" INT32 C_Emit_id_string( const char *name );


// ====================================================================
//
// Hash table for strings in the feedback string table.
//
// ====================================================================

// We avoid all duplicates:
typedef class USER_HASH_TABLE < char*, INT32, String_Hash, String_Equal >
	IPA_FEEDBACK_HASH_TBL;
IPA_FEEDBACK_HASH_TBL *IPA_Feedback_Hash_Tbl;

// ====================================================================
//
// IPA_FEEDBACK_STRINGS::IPA_FEEDBACK_STRINGS
// 
// Constructor: Initialize to reflect a new string pool 
//
// ====================================================================

IPA_FEEDBACK_STRINGS::IPA_FEEDBACK_STRINGS ( MEM_POOL* str_pool )
{
	_str_pool_max  =  FBK_STR_POOL_INIT_SIZE;
	_str_pool_size = 0;
	_str_pool      = str_pool;
	_str_pool_base = MEM_POOL_Alloc ( _str_pool,
				sizeof(char)*FBK_STR_POOL_INIT_SIZE );
	 if( _str_pool_base == NULL ) {
		 ErrMsg ( EC_No_Mem, "IPA Feedback string pool unavail" );
	 }
}

// ====================================================================
//
// IPA_FEEDBACK_HASHES::IPA_FEEDBACK_HASHES
// 
// Constructor: Initialize to reflect a string hash table
//
// ====================================================================

IPA_FEEDBACK_HASHES::IPA_FEEDBACK_HASHES ( MEM_POOL* str_hash_tbl )
{
	// Initialize the string hash table:

	IPA_Feedback_Hash_Tbl = 
            CXX_NEW ( IPA_FEEDBACK_HASH_TBL ( FBK_HASH_TAB_SIZE, str_hash_tbl ),
                      str_hash_tbl );
}

// ====================================================================
//
// IPA_FEEDBACK_STRINGS::Add_string
// 
// Add a string to an IPA feedback string pool. The string has been  
// previously checked for duplication.
//
// ====================================================================

INT32
IPA_FEEDBACK_STRINGS::Add_id_string ( const char *s )
{
  INT32 len = strlen(s);
  INT32 loc;

  // Expand the string pool if required

  if ( _str_pool_size == 0 ) _str_pool_size = 1;
  if ( _str_pool_max <= _str_pool_size + len+1 ) {
	_str_pool_base = MEM_POOL_Realloc ( _str_pool, _str_pool_base, 
           sizeof(char)*_str_pool_max, 
           sizeof(char)*(_str_pool_size + len+1+FBK_STR_POOL_INIT_SIZE) );
	if ( _str_pool_base == NULL ) {
		ErrMsg ( EC_No_Mem, "IPA Feedback string pool realloc fails " );
	}
	_str_pool_max = _str_pool_size + len+1+FBK_STR_POOL_INIT_SIZE;
  }

  // Add the new string:
  strcpy ( ((char *)_str_pool_base)+_str_pool_size, s );
  loc = _str_pool_size;
  _str_pool_size += len+1;

  return loc;
}

// ====================================================================
//
// IPA_FEEDBACK_STRINGS::Emit_id_string
//
// Emit a name to an IPA feedback string pool and returns its index.
// A hash table (above) is used to avoid any duplication.
//
// WARNING:  The hash table implementation used stores the string
// pointer as the key rather than the string itself.  So the name
// entered must not be a string which will be overwritten before being
// done with the hash table.
//
// ====================================================================

INT32
IPA_FEEDBACK_STRINGS::Emit_id_string ( const char *name )
{
  // check 1st for duplicate and recycle index if found
  INT32 index = IPA_Feedback_Hash_Tbl->Find ( (char *)name );

  if ( index == 0 ) {
    // no duplicate found - add a new string
    index = IPA_FEEDBACK_STRINGS::Add_id_string ( (char *)name );
    IPA_Feedback_Hash_Tbl->Enter ( (char *)name, index );
    return index;
  }
  else {
    return -index;
  }
}

// ====================================================================
//
// C_Emit_id_string
//
// A wrapper routine to allow the C programs in /common portion of ipa
// source tree to call the string hashing mechanisms to eliminate dups.
//
// WARNING:  The hash table implementation used stores the string
// pointer as the key rather than the string itself.  So the name
// entered must not be a string which will be overwritten before being
// done with the hash table. Separate phases of IPA do free up memory so
// this is a problem if trying to extend hash tables/string pools across
// phases.
//
// ====================================================================
#ifdef TODO

INT32
C_Emit_id_string ( const char *name )
{
  return ( IPA_Fbk_Strings->Emit_id_string( name ) );
}

#endif
