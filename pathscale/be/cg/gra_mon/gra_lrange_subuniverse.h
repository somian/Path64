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

/*                      Subuniverses of Live Ranges
 *                      ===========================
 *
 *  Description:
 *
 *      We use LRANGE_SETs to represent interference.  Live ranges can
 *      only conflict if they are (a) in the same region and (b) need a
 *      register from the same register class.  We'll take advantage of
 *      this fact to make our interference sets denser.  This module
 *      implements multiple dense LRANGE<=>INT32 mappings to be used as
 *      support for LRANGE subuniverses.  These subuniverses are
 *      NON-INTERSECTING in the current implementation.
 *
 *      Count returns the number of elements in the subuniverse.  
 *      Since the numbering is guaranteed to be dense, this can be used in
 *      conjunction with Nth_Lrange to iteration over the elements:
 *
 *              for ( i = sub->Count() - 1; i >= 0; --i) {
 *                  LRANGE *lrange = sub->Nth_Lrange(i);
 *                  .. lrange is now a live range in subuniverse
 *              }
 */


/*
 *  $Revision$
 *  $Date$
 *  $Author$
 *  $Source$
 */


#ifndef LRANGE_SUBUNIVERSE_INCLUDED
#define LRANGE_SUBUNIVERSE_INCLUDED

#ifndef LRANGE_SUBUNIVERSE_RCS_ID
#define LRANGE_SUBUNIVERSE_RCS_ID
#ifdef _KEEP_RCS_ID
static char *lrange_subuniverse_rcs_id = "$Source$ $Revision$";
#endif
#endif

#include "defs.h"
#include "errors.h"
#include "gra_lrange.h"

class LRANGE_SET_SUBUNIVERSE {
friend class LRANGE_SUB_MGR;
  LRANGE** lranges;	// Maps integers to LRANGEs
  INT32 count;		// Number of elements currently in the subuniverse
  INT32 alloc_size;     // How many it can hold before it must be reallocated
public:
  LRANGE_SET_SUBUNIVERSE(void) {}
  ~LRANGE_SET_SUBUNIVERSE(void) {}

  LRANGE** Lranges(void)	{ return lranges; }
  INT32 Count(void)		{ return count; }
  INT32 Alloc_Size(void)	{ return alloc_size; }
  INT32 Member_Count(void)	{ return alloc_size; }

  void Add(LRANGE* lrange);
  LRANGE *Nth_Lrange(INT i);
};

// use shorter name in gra's code
class LRANGE_SUBUNIVERSE: public LRANGE_SET_SUBUNIVERSE {
};

class LRANGE_SUB_MGR {
friend class LRANGE_SET_SUBUNIVERSE;
  MEM_POOL pool;		// where to allocate stuff
  BOOL pool_initialized;
public:
  LRANGE_SUB_MGR(void) { pool_initialized = FALSE; }
  ~LRANGE_SUB_MGR(void) {}

  void Initialize(void);
  void Finalize(void);
  LRANGE_SUBUNIVERSE *Create(INT32 initial_size);
};

extern LRANGE_SUB_MGR lrange_sub_mgr;

#endif
