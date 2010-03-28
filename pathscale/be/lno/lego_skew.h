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

/**
***  Exported types and functions:
***
***	void Lego_Skew_Indices(WN* wn_tree)
***
***	  If all of the reshaped arrays in a loop have a constant offset
***	  "k", skew the loop indices to remove the constant offset.  
**/

class LEGO_SKEW {
private: 
  ACCESS_VECTOR* _av; 
  WN* _array; 
  INT _dim; 
  INT _count; 
public: 
  ACCESS_VECTOR* Av() { return _av; }
  WN* Array() { return _array; }
  INT Dim() { return _dim; } 
  INT Count() { return _count; }  
  void Increment() { _count++; }
  LEGO_SKEW(ACCESS_VECTOR* av, WN* array, INT dim, INT count) 
    : _av(av), _array(array), _dim(dim), _count(count) {} 
  ~LEGO_SKEW() {}  
}; 

extern void Lego_Skew_Indices(WN* wn_tree); 

