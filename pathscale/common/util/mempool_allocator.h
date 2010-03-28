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

#ifndef mempool_allocator_INCLUDED
#define mempool_allocator_INCLUDED

#ifndef mempool_INCLUDED
#include "mempool.h"
#endif

template <class T>
class mempool_allocator {
private:
  MEM_POOL* pool;
  template <class U> friend class mempool_allocator;

public:
  typedef size_t    size_type;
  typedef ptrdiff_t difference_type;
  typedef T*        pointer;
  typedef const T*  const_pointer;
  typedef T&        reference;
  typedef const T&  const_reference;
  typedef T         value_type;

  template <class U> struct rebind {
    typedef mempool_allocator<U> other;
  };

  mempool_allocator() : pool(Malloc_Mem_Pool) {} // The default value.
  mempool_allocator(MEM_POOL* p) : pool(p) {}
  mempool_allocator(const mempool_allocator& a) : pool(a.pool) {}
  template <class U> mempool_allocator(const mempool_allocator<U>& a)
    : pool(a.pool) {}
  ~mempool_allocator() {}

  pointer address(reference x) const { return &x; }
  const_pointer address(const_reference x) const { return &x; }

  pointer allocate(size_type n, const void* = 0) 
    { return n != 0 ? TYPE_MEM_POOL_ALLOC_N(T, pool, n) : pointer(0); }

  void deallocate(pointer p, size_type /* n */) {
    if (p)
      MEM_POOL_FREE(pool, p);
  }

  size_type max_size() const { return size_t(-1) / sizeof(T); }

  void construct(pointer p, const T& val) { new(p) T(val); }
  void destroy(pointer p) { p->~T(); }
  bool operator!=( class mempool_allocator<T>& p) { return pool != p.pool; }
};

template<>
class mempool_allocator<void> {
public:
  typedef size_t      size_type;
  typedef ptrdiff_t   difference_type;
  typedef void*       pointer;
  typedef const void* const_pointer;
  typedef void        value_type;

  template <class U> struct rebind {
    typedef mempool_allocator<U> other;
  };
};
#endif //  mempool_allocator_INCLUDED
