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



#ifndef SGI_iterator_adapter_h_INCLUDED
#define SGI_iterator_adapter_h_INCLUDED

#include <iterator>

namespace SGI {
  
template <class Iterator> 
class const_iterator {
protected:
  Iterator current;

public:
  typedef typename iterator_traits<Iterator>::iterator_category iterator_category;
  typedef typename iterator_traits<Iterator>::value_type value_type;
  typedef typename iterator_traits<Iterator>::difference_type difference_type;
  typedef typename iterator_traits<Iterator>::pointer pointer;
  typedef const iterator_traits<Iterator>::value_type& reference;
  typedef const iterator_traits<Iterator>::value_type& const_reference;
  typedef Iterator iterator_type;
  typedef const_iterator<Iterator> self;

  const_iterator() {}
  explicit const_iterator(iterator_type x) : current(x) {}

  const_iterator(const self& x) : current(x.current) {}
  template <class Iter>
  const_iterator(const const_iterator<Iter>& x) : current(x.current) {}
    
  iterator_type base() const { return current; }
  const_reference operator*() const {
    return *current; 
  }
  pointer operator->() const { return &(operator*()); }

  self& operator++() {
    ++current;
    return *this;
  }
  self operator++(int) {
    self tmp = *this;
    ++current;
    return tmp;
  }
  self& operator--() {
    --current;
    return *this;
  }
  self operator--(int) {
    self tmp = *this;
    --current;
    return tmp;
  }

  self operator+(difference_type n) const {
    return self(current + n);
  }
  self operator-(difference_type n) const {
    return self(current - n);
  }
  difference_type operator-(self n) const {
    return difference_type(current - n.current);
  }
  self& operator+=(difference_type n) {
    current += n;
    return *this;
  }
  self& operator-=(difference_type n) {
    current -= n;
    return *this;
  }
  const_reference operator[](difference_type n) const { return *(*this + n); }  
}; 
 
} // Close namespace SGI

#endif /* SGI_iterator_adapter_h_INCLUDE */

// Local Variables:
// mode:C++
// End:
