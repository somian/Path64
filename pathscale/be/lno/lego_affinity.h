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
//
// ============================================================================
// ============================================================================
//
// Model: lego_opts.h
// 
// Description:
//
// Data structures and routines to determine the affinity of loop nest
//
// ============================================================================
// ============================================================================

#ifndef lego_affinity_INCLUDED
#define lego_affinity_INCLUDED "lego_affinity.h"


extern void Whack_Do_Loops (WN* func_nd);


class LEGO_INFO;

class LEGO_UGS {
  ACCESS_VECTOR* _av;  // access vector for this ugs dimension
                       // pointer to the first ref that comes along

  DISTRIBUTE_TYPE _dtype; 
  INT32          _loop_pos; // the loop pos in Coeff() of AV
//  INT32          _stride;
//  INT32          _offset;
  INT32          _min_offset;
  INT32          _max_offset;
  STACK<WN*>     _array_refs;
  STACK<INT>     _ref_dims;  // point to the dimension of each _array_ref
  
public:

  LEGO_UGS (WN* wn_array, DISTRIBUTE_TYPE dtype, ACCESS_VECTOR *av, INT32 dim, INT32 loop_pos);
  ~LEGO_UGS(){};

  BOOL           Add_Ref(WN* wn_array, DISTRIBUTE_TYPE dtype, ACCESS_VECTOR *av, INT32 dim, INT32 loop_pos);
  INT32          Compute_Offset();
  INT32          Get_Min_Offset() { return _min_offset; }
  INT32          Get_Max_Offset() { return _max_offset; }
  STACK<WN*> &   Get_Array_Refs() { return _array_refs; }
  STACK<INT> &   Get_Ref_Dims() { return _ref_dims; }
  INT32          Stride() { return _av->Loop_Coeff(_loop_pos); }
  DISTRIBUTE_TYPE Dis_Type() { return _dtype; }
};

typedef STACK<LEGO_UGS*> LEGO_UGS_STACK;

class LEGO_AFFINITY {
  LEGO_UGS_STACK _block;
  LEGO_UGS_STACK _cyclic;
  LEGO_UGS_STACK _block_cyclic;

public:

  LEGO_AFFINITY();
  ~LEGO_AFFINITY();

  void Add_Ref(WN* wn_array,   // Add wn_array to the LEGO_AFFINITY stack
	       DISTRIBUTE_TYPE dtype, 
	       ACCESS_VECTOR* av,     
	       INT32 dim, 
	       INT32 loop_pos);  

  void Pick_Affinity(LEGO_INFO *dli); // Pick the best affinity for dli

};

#endif





