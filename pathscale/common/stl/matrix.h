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


//-*-c++-*-
// =======================================================================
// =======================================================================
//
//  Module: matrix.h
//  $Revision: 1.5 $
//  $Date: 04/12/21 14:57:25-08:00 $
//  $Author: bos@eng-25.internal.keyresearch.com $
//  $Source: /home/bos/bk/kpro64-pending/common/stl/SCCS/s.matrix.h $
//
//  Revision comments:
//
//  01-May-1999 - Initial version
//
//  Description:
//  ------------
//
//     A very simple matrix implementation on top of vectors,
//     where each element can be accessed through an application
//     operator that takes row and column numbers as indices.
//
//     The matrix elements are ordered in row-major order, and traversal
//     through iterators will wrap around to next/previous rows.  A
//     forward traversal from pos(5,0) to pos(6,0) will start at column 0
//     of row 5 and end at column 0 of row 6.  A reverse traversal from
//     rpos(5,cols()-1) to rpos(4,cols()-1) will start at the largest
//     column of row 5 and end at the largest column of row 4.  Both 
//     of these traversals will visit the same elements, but in reverse
//     orders.
//
//     Currently, a bare minimum of a matrix implementation, which
//     can be improved upon by users as the need arises.
//
//     Whatever file includes this, must also have the following includes:
//
//        #include <vector>
//
//     and a definition of macro Is_True and type INT. 
//
// ====================================================================
// ====================================================================

#include <memory>
#include <vector>

template <class T, class _ALLOC = std::allocator<T> >
class MATRIX
{
private:

  INT                    _rows;
  INT                    _cols;
  std::vector<T, _ALLOC> _v;

  INT _d2_to_d1(INT at_row, INT at_col) const 
  {
    Is_True(at_row >= 0 && at_row < _rows && at_col >= 0 && at_col < _cols,
	    ("Invalid idx (%d,%d) for MATRIX(%d,%d)!",
	     at_row, at_col, _rows, _cols));
    return at_row*_cols + at_col;
  }

  INT _it_offset(INT at_row, INT at_col) const
  {
    // Offset from first element in matrix to given position.
    //
    return _d2_to_d1(at_row, at_col);
  }

  INT _rit_offset(INT at_row, INT at_col) const
  {
    // Offset from last element in matrix to given position.
    //
    return _rows*_cols - _d2_to_d1(at_row, at_col) - 1;
  }

public:

  typedef typename std::vector<T, _ALLOC>::reference        reference;
  typedef typename std::vector<T, _ALLOC>::const_reference  const_reference;
  typedef typename std::vector<T, _ALLOC>::allocator_type   allocator_type;
  typedef typename std::vector<T, _ALLOC>::iterator         iterator;
  typedef typename std::vector<T, _ALLOC>::const_iterator   const_iterator;
  typedef typename std::vector<T, _ALLOC>::reverse_iterator reverse_iterator;
  typedef typename std::vector<T, _ALLOC>::const_reverse_iterator 
                                           const_reverse_iterator;

  MATRIX(INT         rows,
	 INT         cols, 
	 const T&      value,
	 const _ALLOC &alloc = _ALLOC()):
    _rows(rows), _cols(cols), _v(rows*cols, value, alloc) {}
  
  reference operator() (INT i, INT j) 
  {
    return _v[_d2_to_d1(i,j)];
  }

  const_reference operator() (INT i, INT j) const
  {
    return _v[_d2_to_d1(i,j)];
  }

  INT rows() const {return _rows;}
  INT cols() const {return _cols;}

  iterator       begin() {return _v.begin();}
  iterator       end() {return _v.end();}
  const_iterator begin() const {return _v.begin();}
  const_iterator end() const {return _v.end();}

  reverse_iterator       rbegin() {return _v.rbegin();}
  reverse_iterator       rend()  {return _v.rend();}
  const_reverse_iterator rbegin() const {return _v.rbegin();}
  const_reverse_iterator rend()  const {return _v.rend();}

  iterator       pos(INT i, INT j) {return begin() + _it_offset(i,j);}
  const_iterator pos(INT i, INT j) const {return begin() + _it_offset(i,j);}

  reverse_iterator rpos(INT i, INT j)
  {
    return rbegin() + _rit_offset(i,j);
  }
  const_reverse_iterator rpos(INT i, INT j) const
  {
    return rbegin() + _rit_offset(i,j);
  }
}; // MATRIX

