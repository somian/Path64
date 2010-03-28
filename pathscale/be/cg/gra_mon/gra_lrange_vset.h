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

// LRANGE sets represented as vectors
/////////////////////////////////////
//
//  Description:
//
//      When an LRANGE_SET would be too sparse, this might be just the thing.
//      This is a very specialized and simple data structure and is probably
//      only useful for GRA_INTERFERE.  In praticular, it provides no
//      protection against adding the same element twice -- this is handled by
//      GRA_INTEFERE.

//  $Revision$
//  $Date$
//  $Author$
//  $Source$


#ifndef GRA_LRANGE_VSET_INCLUDED
#define GRA_LRANGE_VSET_INCLUDED

#ifndef GRA_LRANGE_VSET_RCS_ID
#define GRA_LRANGE_VSET_RCS_ID
#ifdef _KEEP_RCS_ID
static char *gra_lrange_vset_rcs_id = "$Source$ $Revision$";
#endif
#endif

#include "defs.h"
#include "gra_lrange.h"


// A set of LRANGEs impleneted as a dynamically created vector of pointers
// to LRANGEs.  
class LRANGE_VSET {
friend class LRANGE_VSET_ITER;
friend LRANGE_VSET *LRANGE_VSET_Create( LRANGE** vec, size_t count, MEM_POOL *pool );
  size_t  count;    // Number of elements
  LRANGE* vec[1];   // Here they are
public:
  LRANGE_VSET(void) {}
  ~LRANGE_VSET(void) {}

  // access functions
  INT Count(void)		{ return count; }

  // non-inlined member functions
  void Delete_Element( LRANGE* lrange );
  void Replace_Element( LRANGE* old_lr, LRANGE* new_lr);
};

extern LRANGE_VSET* LRANGE_VSET_Create( LRANGE** vec, size_t count, MEM_POOL *pool );

// An iterator type used to loop over the LRANGEs in a LRANGE_VSET.
class LRANGE_VSET_ITER {
  LRANGE** vec;         // The vector of LRANGEs
  INT32    current;     // Current index.  Counted down to -1
public:
  void Init(LRANGE_VSET *vset)	{ vec = vset->vec; current = vset->count-1; }
  BOOL Done(void)		{ return current < 0; }
  LRANGE *Current(void)		{ return vec[current]; }
  void Step(void)		{ --current; }
};


#endif
