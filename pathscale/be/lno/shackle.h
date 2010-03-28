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


// This file is really -*- C++ -*-

#ifndef shackle_INCLUDED
#define shackle_INCLUDED "shackle.h"

#include "wn.h"

extern void
SHACKLE_Phase (WN *func_nd);

// What's the minimum block size for a dim that's been shackled?
#define SHACKLED_DIM_MIN_SIZE       5
#define MAX_SHACKLE_DIM_OF_MIN_SIZE 1


#define FOR_CHILDREN(wn,chld,childcnt)                                  \
{                                                                       \
  INT32 childcnt;                                                       \
  WN   *chld;                                                           \
  WN   *__temp_wn;                                                      \
                                                                        \
  if (WN_opcode (wn) == OPC_BLOCK)                                      \
    __temp_wn = WN_first (wn);                                          \
  else if (WN_kid_count (wn) > 0)                                       \
    __temp_wn = WN_kid (wn, 0);                                         \
  else                                                                  \
    __temp_wn = NULL;                                                   \
  for (childcnt = 0; __temp_wn != (WN *) NULL; childcnt++) {            \
    chld = __temp_wn;                                                   \
    if (WN_opcode ((wn)) == OPC_BLOCK)                                  \
      __temp_wn = WN_next (chld);                                       \
    else                                                                \
      __temp_wn = (((childcnt)+1) == WN_kid_count(wn)) ?  (WN *) NULL : \
        WN_kid (wn, (childcnt) + 1);

#define END_CHILDREN                                                    \
  }                                                                     \
}  

class SHACKLE_INFO {
private:
  const ST    *_st;             // Symbol table entry being tracked
  BOOL         _st_is_reshaped; // is the underlying st reshaped?
  INT32        _ndim;           // Number of dimensions of array
  BOOL        *_is_shackled;    // Should a given dim be shackled?
  INT32       *_shackle_sizes;  // If so, what is the shackle size?
  // In the old symbol table, ARB_IDX is a pointer to struct arb,
  // In the new symbol table, ARB_IDX is an index pointing to the
  // first index representing the first dimension of the array
  ARB_HANDLE   _array_bounds;   // Array bound information
  MEM_POOL    *_pool;
  TYPE_ID      _type_to_give;
  WN         **_loop_stmts;
  UINT32       _next_loop_count;
  WN         **_cached_lb_expr_st;
  WN         **_cached_ub_expr_st;
public:
  SHACKLE_INFO(const ST *, WN *, MEM_POOL *, TYPE_ID, 
	       BOOL inquire_info=FALSE);
  void Set_Loop_Stmt (INT32 i, WN *stmt) {
    _loop_stmts[i] = stmt;
  }
  BOOL Is_Reshaped() {
    return _st_is_reshaped;
  }
  void Set_Reshaped(BOOL value) {
    _st_is_reshaped = value;
  }
  WN *Get_Loop_Stmt (INT32 i) {
    return _loop_stmts[i];
  }
  TYPE_ID Loop_Type() {
    return _type_to_give;
  }
  WN *Loop();
  INT32 Ndim() {
    return _ndim;
  }
  const ST * Symbol() {
    return _st;
  }
  INT32 Shackle_Dim_Size(const INT32 i) {
    return _shackle_sizes[i];
  }
  void Set_Shackle_Dim_Size (const INT32 i, const INT32 val) {
    _shackle_sizes[i] = val;
  }
  INT32 Ndim_Shackled();
  BOOL Is_Dim_Shackled(const INT32 i) {
    return _is_shackled[i];
  }
  void Set_Dim_Shackled(const INT32 i, BOOL value) {
    _is_shackled[i] = value;
  }
  BOOL Is_Const_Lower(INT32 i) {
    return ARB_const_lbnd(_array_bounds[i]);
  }
  mINT64 Const_Lower(INT32 i) {
    return ARB_lbnd_val(_array_bounds[i]);
  }
  WN *   Expr_Lower (INT32 i) {
    WN *tmp = LWN_Copy_Tree (WN_kid0 (_cached_lb_expr_st[i]));
    WN *fake_unroll[2];
    fake_unroll[0] = WN_kid0 (_cached_lb_expr_st[i]);
    fake_unroll[1] = tmp;
    Unrolled_DU_Update (fake_unroll, 2, 0);
    return tmp;
  }
  
  BOOL Is_Const_Upper(INT32 i) {
    return ARB_const_ubnd(_array_bounds[i]);
  }
  mINT64 Const_Upper(INT32 i) {
    return ARB_ubnd_val(_array_bounds[i]);
  }
  WN *     Expr_Upper(INT32 i) {
    WN *tmp = LWN_Copy_Tree (WN_kid0 (_cached_ub_expr_st[i]));
    WN *fake_unroll[2];
    fake_unroll[0] = WN_kid0 (_cached_ub_expr_st[i]);
    fake_unroll[1] = tmp;
    Unrolled_DU_Update (fake_unroll, 2, 0);
    return tmp;
  }
};

#endif
