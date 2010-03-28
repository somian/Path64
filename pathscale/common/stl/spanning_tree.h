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



#ifndef SGI_spanning_tree_h_INCLUDED
#define SGI_spanning_tree_h_INCLUDED

#include "bucket_container.h"
#include "container_with_membership.h"

#include <stdio.h>

namespace SGI {

template <class ClusterIterator,
          class OutputIterator,
          class Compare,
          class DestinationFunction,
          class Sizetype>
OutputIterator minimum_spanning_tree(// const
                                     ClusterIterator initial,
                                     OutputIterator result,
                                     Compare comp,
                                     DestinationFunction destination,
				     Sizetype size)
{
  typedef iterator_traits<ClusterIterator>::value_type::iterator edge_iterator;
  typedef iterator_traits<edge_iterator>::value_type heap_element;
  typedef heap_with_membership<heap_element,
                               DestinationFunction,
                               Compare> heap_type;
  heap_type heap(size, destination, comp);  
  typename heap_type::insert_iterator inserter(heap); 
  heap.remove(initial);
  copy(initial->begin(), initial->end(), inserter);
  while (!heap.empty()) {
    initial = destination(heap.top());
    *result++ = heap.top();
    heap.pop();
    copy(initial->begin(), initial->end(), inserter);
  }
  return result;
}

} // Close namespace SGI

#endif /* SGI_spanning_tree_h_INCLUDE */

// Local Variables:
// mode:C++
// End:
