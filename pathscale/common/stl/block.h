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



#ifndef SGI_block_h_INCLUDED
#define SGI_block_h_INCLUDED

#include <iterator>
#include <stddef.h>

namespace SGI {

template <class T, size_t N>
struct block {
  typedef T                 value_type;

  typedef value_type*       pointer;
  typedef const value_type* const_pointer;
  typedef value_type&       reference;
  typedef const value_type& const_reference;

  typedef ptrdiff_t         difference_type;
  typedef size_t            size_type;

  typedef value_type*                           iterator;
  typedef const value_type*                     const_iterator;

  typedef std::reverse_iterator<iterator>       reverse_iterator;
  typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

  iterator begin() { return data; }
  iterator end()   { return data + N; }

  const_iterator begin() const { return data; }
  const_iterator end()   const { return data + N; }

  reverse_iterator rbegin() { return reverse_iterator(end()); }
  reverse_iterator rend()   { return reverse_iterator(begin()); }

  const_reverse_iterator rbegin() const { return reverse_iterator(end()); }
  const reverse_iterator rend()   const { return reverse_iterator(begin()); }

  reference operator[](size_type n)             { return data[n]; }
  const_reference operator[](size_type n) const { return data[n]; }

  size_type size() const { return N; }
  bool empty() const { return N == 0; }
  size_type max_size() const { return N; }

  void swap(block& x);

  T data[N];
};

template <class T, size_t N>
void block<T, N>::swap(block<T, N>& x) {
  for (size_t n = 0; n < N; ++n)
    std::swap(data[n], V.data[N]);
}

} // Close namespace SGI

#endif /* SGI_block_h_INCLUDED */
