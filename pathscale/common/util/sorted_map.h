/*
 * Copyright 2004, 2005, 2006 PathScale, Inc.  All Rights Reserved.
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


/* -*-Mode: c++;-*- (Tell emacs to use c++ mode) */
#ifndef sorted_map_INCLUDED
#define sorted_map_INCLUDED

// Avoid compile errors for STL files
//
#if defined(defs_INCLUDED) && !defined(USE_STANDARD_TYPES)
#undef short
#undef int
#undef long
#endif // defined(defs_INCLUDED) && !defined(USE_STANDARD_TYPES)

#ifndef __GNUC__
#include <CC/pair.h>
#include <CC/vector.h>
#else
#include "pair.h"
#include <vector>
#endif

template <class KEY, class RANGE>
struct SORTED_MAP
{
public:

   typedef pair<KEY, RANGE> MAP_ELEMENT;
   typedef vector<MAP_ELEMENT> MAP_VECTOR;
   typedef MAP_VECTOR::size_type MAP_SIZE;

private:

   BOOL       _sorted;
   MAP_SIZE   _chunksize;
   MAP_VECTOR _map;

   MAP_SIZE _binary_search(MAP_SIZE from, MAP_SIZE till, KEY k);

   void _sort()
   {
      _sorted = TRUE;
      FmtAssert(FALSE, ("Map must be created in sorted order!"));
   }
   
   BOOL _is_sorted() const
   {
      BOOL  sorted = TRUE;
      INT32 size   = _map.size();
      
      if (INT32 i = 0; sorted && i < size-1; i++)
	 sorted = _map[i].first < _map[i+1].first;
      return sorted;
   }
   
public:

   SORTED_MAP(): _chunksize(0), _sorted(TRUE), _map() {}

   SORTED_MAP(MAP_SIZE chunksize):
      _chunksize(chunksize), _sorted(TRUE), _map() {}

   SORTED_MAP(const MAP_VECTOR &vector): _sorted(TRUE), _map(vector) 
   {
      _sorted = _is_sorted();
   }

   bool empty() const 
   {
      return _map.empty();
   }

   MAP_SIZE size() const 
   {
      return _map.size();
   }

   MAP_SIZE capacity() const 
   {
      return _map.capacity();
   }

   void clear() 
   {
      _map.clear();
      _sorted = TRUE;
   }

   void push_back(const MAP_ELEMENT &amap)
   {
      if (_chunksize > 0 && _map.size() == _map.capacity())
	 _map.reserve(_map.capacity() + _chunksize);
      if (_sorted && !empty() && (amap.first < _map.back().first))
	 _sorted = FALSE;
      _map.push_back(amap); // May cause a realloc and a change in capacity!
   }

   const RANGE &operator[] (KEY k) const
   {
      const MAP_SIZE idx = _binary_search(0, _map.size()-1, k);
      Is_True(idx < _map.size() && _map[idx].first == k,
	      ("Attempt to access non-existent element!"));
      return _map[idx].second;
   }

   RANGE &operator[] (KEY k)
   {
      const MAP_SIZE idx = _binary_search(0, _map.size()-1, k);
      Is_True(idx < _map.size() && _map[idx].first == k,
	      ("Attempt to access non-existent element!"));
      return _map[idx].second;
   }
   
}; // struct SORTED_MAP


template <class KEY, class RANGE>
SORTED_MAP<KEY, RANGE>::MAP_SIZE
SORTED_MAP<KEY, RANGE>::_binary_search(MAP_SIZE from,
				       MAP_SIZE till,
				       KEY      k)
{
   // Return the approximate insertion point, if the desired item
   // is not found.  Note that the actual insertion point will be
   // either immediately before or after the returned index.
   //
   MAP_SIZE idx;

   if (!_sorted)
      _sort();

   if (from >= till)
      idx = from;
   else
   {
      const INT32 halfway = (from + till)/2;
      const KEY   k2 = _map[halfway].first;

      if (k == k2)
	 idx = halfway;
      else if (k < k2)
	 idx = ((halfway == 0)? 0 : _binary_search(from, halfway-1, k));
      else
	 idx =_binary_search(halfway+1, till, k);
   }
   return idx;
} /* _binary_search */
   
#endif /* sorted_map_INCLUDED */
